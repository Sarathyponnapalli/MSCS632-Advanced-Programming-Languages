package main

import (
	"fmt"
	"log"
	"os"
	"sync"
)

// ResultStore is the shared output resource.
// It holds the computed rows of the result matrix and can flush them to disk.
//
// Synchronization: a sync.Mutex guards the completedRows counter and the
// result slice so multiple goroutines can call StoreRow concurrently without
// a data race. Individual row slots are safe because each row index is owned
// by exactly one worker.
type ResultStore struct {
	mu            sync.Mutex
	result        [][]float64
	completedRows int
	rows          int
	cols          int
}

// NewResultStore allocates the result matrix.
func NewResultStore(rows, cols int) *ResultStore {
	result := make([][]float64, rows)
	for i := range result {
		result[i] = make([]float64, cols)
	}
	return &ResultStore{result: result, rows: rows, cols: cols}
}

// StoreRow writes one computed row into the shared result matrix.
// Safe to call from multiple goroutines.
func (rs *ResultStore) StoreRow(rowIndex int, rowData []float64) {
	rs.mu.Lock()
	defer rs.mu.Unlock() // defer ensures the lock is always released

	rs.result[rowIndex] = rowData
	rs.completedRows++
	log.Printf("[ResultStore] Stored row %d (%d/%d complete)",
		rowIndex, rs.completedRows, rs.rows)
}

// GetResult returns the assembled result matrix.
// Call only after all workers have finished.
func (rs *ResultStore) GetResult() [][]float64 {
	rs.mu.Lock()
	defer rs.mu.Unlock()
	return rs.result
}

// CompletedRows returns how many rows have been stored so far.
func (rs *ResultStore) CompletedRows() int {
	rs.mu.Lock()
	defer rs.mu.Unlock()
	return rs.completedRows
}

// WriteToFile writes the result matrix to a plain-text file.
// Go error handling: every file operation is checked; defer closes the file
// even if an error occurs partway through writing.
func (rs *ResultStore) WriteToFile(filename string) error {
	rs.mu.Lock()
	defer rs.mu.Unlock()

	// os.Create can fail (permissions, disk full, etc.) - check the error
	f, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("WriteToFile: could not create %q: %w", filename, err)
	}
	defer func() {
		// defer guarantees Close() is called even if Write() panics or errors
		if cerr := f.Close(); cerr != nil {
			log.Printf("[ResultStore] WARNING: failed to close %q: %v", filename, cerr)
		}
	}()

	_, err = fmt.Fprintln(f, "=== Matrix Multiplication Result ===")
	if err != nil {
		return fmt.Errorf("WriteToFile: write header: %w", err)
	}

	for _, row := range rs.result {
		for j, v := range row {
			if j > 0 {
				_, err = fmt.Fprint(f, "  ")
			}
			_, err = fmt.Fprintf(f, "%10.2f", v)
			if err != nil {
				return fmt.Errorf("WriteToFile: write value: %w", err)
			}
		}
		_, err = fmt.Fprintln(f)
		if err != nil {
			return fmt.Errorf("WriteToFile: write newline: %w", err)
		}
	}

	log.Printf("[ResultStore] Result matrix written to %q", filename)
	return nil
}
