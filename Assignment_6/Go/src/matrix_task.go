package main

import "fmt"

// MatrixTask represents a single row-computation job.
// It is immutable once created, so no synchronization is needed to read it
// from multiple goroutines.
type MatrixTask struct {
	TaskID   int
	RowIndex int
	RowA     []float64
	MatrixB  [][]float64
}

func (t MatrixTask) String() string {
	return fmt.Sprintf("MatrixTask[id=%d, row=%d]", t.TaskID, t.RowIndex)
}
