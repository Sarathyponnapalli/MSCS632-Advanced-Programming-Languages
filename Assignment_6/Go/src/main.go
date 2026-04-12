// Parallel Matrix Multiplication System -- Go
//
// Architecture:
//   main goroutine (producer)
//       |-- sends MatrixTask --> taskCh (buffered channel = shared queue)
//       |                           |
//       |               Worker goroutines (x NUM_WORKERS)
//       |                   |-- read taskCh (blocked until task available)
//       |                   |-- call computeRow()
//       |                   |-- call resultStore.StoreRow()
//       |                   |-- send errors  --> errCh
//       |                   |-- send stats   --> statsCh
//       |
//       |-- closes taskCh (signals workers to finish)
//       |-- collects errors from errCh
//       |-- waits for all workers via statsCh
//       |-- calls resultStore.WriteToFile()
//
// Concurrency model:
//   - Channels replace explicit locks for the task queue (Go idiom:
//     "Do not communicate by sharing memory; share memory by communicating.")
//   - sync.Mutex in ResultStore protects the shared result matrix.
//   - sync.WaitGroup is NOT used here; instead statsCh serves as both a
//     completion signal and a stats carrier (idiomatic Go).
//   - defer is used throughout for guaranteed resource cleanup.

package main

import (
	"fmt"
	"log"
	"math"
	"math/rand"
	"os"
	"time"
)

// --- Configuration ---
const (
	rowsA      = 8
	inner      = 6
	colsB      = 7
	numWorkers = 4
	queueCap   = 16 // buffered channel capacity (backpressure)
	outFile    = "result_matrix.txt"
)

func main() {
	// Configure logger to include timestamps
	log.SetFlags(log.Ltime | log.Lmicroseconds)
	log.SetOutput(os.Stdout)

	log.Println("====================================================")
	log.Println("  Parallel Matrix Multiplication System -- Go")
	log.Println("====================================================")
	log.Printf("Matrix A: %dx%d   Matrix B: %dx%d   Workers: %d",
		rowsA, inner, inner, colsB, numWorkers)

	// 1. Generate random matrices
	matrixA := generateMatrix(rowsA, inner)
	matrixB := generateMatrix(inner, colsB)
	printMatrix("Matrix A", matrixA)
	printMatrix("Matrix B", matrixB)

	// 2. Create shared resources
	//    taskCh is a BUFFERED channel -- it acts as the shared task queue.
	//    A buffered channel of capacity queueCap lets the producer enqueue
	//    up to queueCap tasks without blocking, providing backpressure when full.
	taskCh  := make(chan MatrixTask, queueCap)
	errCh   := make(chan error, rowsA)       // collect errors from all workers
	statsCh := make(chan WorkerStats, numWorkers) // one stat report per worker

	resultStore := NewResultStore(rowsA, colsB)

	// 3. Launch worker goroutines
	//    go func() launches a goroutine -- lightweight, managed by the Go scheduler.
	//    Each worker receives the same shared channels and resultStore pointer.
	for i := 1; i <= numWorkers; i++ {
		go RunWorker(i, taskCh, resultStore, errCh, statsCh)
		log.Printf("Launched Worker-%d", i)
	}

	// 4. Producer: send one task per row of A into taskCh
	startTime := time.Now()
	log.Println("--- Enqueueing tasks ---")

	for row := 0; row < rowsA; row++ {
		task := MatrixTask{
			TaskID:   row,
			RowIndex: row,
			RowA:     matrixA[row],
			MatrixB:  matrixB,
		}
		// Sending to a buffered channel blocks only when full (backpressure).
		// This is the Go equivalent of SharedTaskQueue.addTask().
		taskCh <- task
		log.Printf("Enqueued %s", task)
	}

	// Closing the channel is the Go "poison pill":
	// all goroutines ranging over taskCh will exit their loop once
	// the channel is drained and closed.
	close(taskCh)
	log.Println("--- taskCh closed: workers will exit after draining ---")

	// 5. Collect errors from errCh (non-blocking drain via select)
	//    We close errCh after all workers have reported stats, then drain it.

	// 6. Wait for all workers to finish by reading numWorkers stats
	log.Println("--- Waiting for all workers to finish ---")
	var totalErrors int
	var totalTasks  int

	for i := 0; i < numWorkers; i++ {
		stats := <-statsCh // blocks until a worker sends its final report
		log.Printf("Worker-%d summary: %d task(s) completed, %d error(s)",
			stats.WorkerID, stats.TasksComplete, stats.Errors)
		totalTasks  += stats.TasksComplete
		totalErrors += stats.Errors
	}

	// Now safe to close errCh and drain any buffered errors
	close(errCh)
	for err := range errCh {
		log.Printf("[ERROR collected] %v", err)
	}

	elapsed := time.Since(startTime)
	log.Printf("--- All workers done | rows: %d/%d | errors: %d | time: %v ---",
		resultStore.CompletedRows(), rowsA, totalErrors, elapsed.Round(time.Millisecond))

	// 7. Display result and verify against serial computation
	printMatrix("Result Matrix C = A x B", resultStore.GetResult())

	expected := serialMultiply(matrixA, matrixB)
	if matricesEqual(resultStore.GetResult(), expected, 1e-9) {
		log.Println("Verification vs serial result: CORRECT")
	} else {
		log.Println("Verification vs serial result: MISMATCH -- check for race conditions!")
	}

	// 8. Write result to file
	//    Check error returned from WriteToFile -- Go error handling convention
	if err := resultStore.WriteToFile(outFile); err != nil {
		log.Printf("[ERROR] Failed to write output file: %v", err)
	} else {
		log.Printf("Result written to %q", outFile)
	}

	log.Println("====================================================")
	log.Printf("  Complete. Tasks: %d  Errors: %d  Time: %v",
		totalTasks, totalErrors, elapsed.Round(time.Millisecond))
	log.Println("====================================================")
}

// --- Helpers ---

// generateMatrix creates a rows x cols matrix of random float64 values in [0,10).
func generateMatrix(rows, cols int) [][]float64 {
	rng := rand.New(rand.NewSource(42))
	m := make([][]float64, rows)
	for i := range m {
		m[i] = make([]float64, cols)
		for j := range m[i] {
			m[i][j] = math.Round(rng.Float64()*100) / 10.0
		}
	}
	return m
}

// serialMultiply computes C = A x B sequentially for verification.
func serialMultiply(A, B [][]float64) [][]float64 {
	n, p, m := len(A), len(B), len(B[0])
	C := make([][]float64, n)
	for i := range C {
		C[i] = make([]float64, m)
		for j := 0; j < m; j++ {
			for k := 0; k < p; k++ {
				C[i][j] += A[i][k] * B[k][j]
			}
		}
	}
	return C
}

// matricesEqual compares two matrices element-wise within a tolerance.
func matricesEqual(A, B [][]float64, tol float64) bool {
	for i := range A {
		for j := range A[i] {
			if math.Abs(A[i][j]-B[i][j]) > tol {
				return false
			}
		}
	}
	return true
}

// printMatrix logs a formatted matrix to stdout.
func printMatrix(label string, m [][]float64) {
	log.Printf("--- %s (%dx%d) ---", label, len(m), len(m[0]))
	for _, row := range m {
		line := "  "
		for _, v := range row {
			line += fmt.Sprintf("%7.2f ", v)
		}
		log.Print(line)
	}
}
