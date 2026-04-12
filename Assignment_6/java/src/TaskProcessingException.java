package matrix;

/**
 * Custom checked exception thrown when a worker encounters an unrecoverable
 * error while processing a MatrixTask.
 *
 * Using a custom exception type (rather than a generic RuntimeException) makes
 * error handling explicit and allows callers to distinguish task-level failures
 * from other unexpected errors.
 */
public class TaskProcessingException extends Exception {

    private final int taskId;
    private final int rowIndex;

    public TaskProcessingException(int taskId, int rowIndex, String message, Throwable cause) {
        super(String.format("Task %d (row %d) failed: %s", taskId, rowIndex, message), cause);
        this.taskId   = taskId;
        this.rowIndex = rowIndex;
    }

    public TaskProcessingException(int taskId, int rowIndex, String message) {
        super(String.format("Task %d (row %d) failed: %s", taskId, rowIndex, message));
        this.taskId   = taskId;
        this.rowIndex = rowIndex;
    }

    public int getTaskId()   { return taskId; }
    public int getRowIndex() { return rowIndex; }
}
