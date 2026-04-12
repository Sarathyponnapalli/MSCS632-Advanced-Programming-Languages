package main

import (
	"fmt"
	"log"
	"math/rand"
	"time"
)

// WorkerStats holds per-worker counters reported at shutdown.
type WorkerStats struct {
	WorkerID      int
	TasksComplete int
	Errors        int
}

// RunWorker is the goroutine function for each worker.
//
// Go concurrency model:
//   - taskCh  (receive-only <-chan) is the channel acting as the shared task queue.
//     Channels are concurrency-safe by design; no extra locking is needed.
//   - errCh   (send-only chan<-) collects per-task errors back to the main goroutine.
//   - statsCh (send-only chan<-) sends final WorkerStats when this worker exits.
//
// The worker loops, receiving tasks from taskCh until the channel is CLOSED
// by the producer. Channel close is Go's idiomatic "poison pill":
//   range over a channel exits cleanly when it is closed and drained.
//
// Error handling (Go style):
//   Go has no try-catch. Errors are explicit return values. Every fallible
//   call is checked. Errors are forwarded on errCh so one bad task does not
//   stop the worker — it just logs and moves on.
func RunWorker(
	workerID int,
	taskCh <-chan MatrixTask,
	resultStore *ResultStore,
	errCh chan<- error,
	statsCh chan<- WorkerStats,
) {
	log.Printf("[Worker-%d] Started", workerID)

	stats := WorkerStats{WorkerID: workerID}

	// range over channel: blocks on each receive, exits when channel is closed
	for task := range taskCh {
		log.Printf("[Worker-%d] Processing %s", workerID, task)

		rowResult, err := computeRow(task)
		if err != nil {
			stats.Errors++
			errCh <- err // forward to main's error collector
			log.Printf("[Worker-%d] ERROR processing %s: %v", workerID, task, err)
			continue // worker keeps going — one bad task does not kill it
		}

		resultStore.StoreRow(task.RowIndex, rowResult)
		stats.TasksComplete++
		log.Printf("[Worker-%d] Completed %s", workerID, task)
	}

	log.Printf("[Worker-%d] Finished -- %d task(s) completed, %d error(s)",
		workerID, stats.TasksComplete, stats.Errors)

	statsCh <- stats // signal main that this worker is done
}

// computeRow calculates one row of C = A x B.
// Returns (rowResult, nil) on success, or (nil, error) on failure.
// Go convention: always check the returned error before using the result.
func computeRow(task MatrixTask) ([]float64, error) {
	rowA := task.RowA
	matB := task.MatrixB

	// Explicit input validation - return errors, never panic on bad input
	if len(rowA) == 0 || len(matB) == 0 {
		return nil, newTaskError(task, "nil or empty input data")
	}
	if len(rowA) != len(matB) {
		return nil, newTaskError(task,
			fmt.Sprintf("dimension mismatch: len(rowA)=%d but len(matB)=%d",
				len(rowA), len(matB)))
	}

	cols := len(matB[0])
	rowResult := make([]float64, cols)

	// Dot product: result[j] = sum(A[row][k] * B[k][j])
	for j := 0; j < cols; j++ {
		var sum float64
		for k, aVal := range rowA {
			sum += aVal * matB[k][j]
		}
		rowResult[j] = sum
	}

	// Simulated processing delay (10-50 ms) to represent computational work
	delay := time.Duration(10+rand.Intn(40)) * time.Millisecond
	time.Sleep(delay)

	return rowResult, nil
}
