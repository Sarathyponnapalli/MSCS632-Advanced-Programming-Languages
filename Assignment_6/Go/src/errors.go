package main

import "fmt"

// TaskProcessingError is a custom error type that carries task context.
// In Go, error handling is explicit: functions return an error value that
// callers must check. This type satisfies the built-in error interface by
// implementing the Error() string method.
type TaskProcessingError struct {
	TaskID   int
	RowIndex int
	Reason   string
}

func (e *TaskProcessingError) Error() string {
	return fmt.Sprintf("task %d (row %d) failed: %s", e.TaskID, e.RowIndex, e.Reason)
}

// newTaskError is a convenience constructor.
func newTaskError(task MatrixTask, reason string) *TaskProcessingError {
	return &TaskProcessingError{
		TaskID:   task.TaskID,
		RowIndex: task.RowIndex,
		Reason:   reason,
	}
}
