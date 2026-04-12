package matrix;

import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Logger;

/**
 * Worker thread that pulls MatrixTask objects from the SharedTaskQueue,
 * computes the corresponding row of the result matrix, and stores it in
 * the ResultStore.
 *
 * Concurrency technique demonstrated:
 *   - ReentrantLock guards the per-worker statistics (tasksProcessed,
 *     errorsEncountered) so they can be read safely from the main thread
 *     after the executor shuts down.
 *
 * Exception handling:
 *   - InterruptedException (getTask, Thread.sleep): interrupt flag is always
 *     restored before exiting so the JVM can shut down cleanly.
 *   - TaskProcessingException: thrown when computation or dimension mismatch
 *     is detected; caught per-task so one failure does not kill the worker.
 *   - Unexpected RuntimeException: also caught per-task and wrapped for logging.
 */
public class MatrixWorker implements Runnable {

    private static final Logger LOGGER = Logger.getLogger(MatrixWorker.class.getName());

    // Simulated processing delay range (ms) - represents computational work
    private static final int MIN_DELAY_MS = 10;
    private static final int MAX_DELAY_MS = 50;

    private final int workerId;
    private final SharedTaskQueue taskQueue;
    private final ResultStore resultStore;

    // ReentrantLock protects the mutable stat counters
    private final ReentrantLock statsLock = new ReentrantLock();
    private int tasksProcessed    = 0;
    private int errorsEncountered = 0;

    public MatrixWorker(int workerId, SharedTaskQueue taskQueue, ResultStore resultStore) {
        this.workerId    = workerId;
        this.taskQueue   = taskQueue;
        this.resultStore = resultStore;
    }

    @Override
    public void run() {
        String threadName = Thread.currentThread().getName();
        LOGGER.info(String.format("[Worker-%d | %s] Started", workerId, threadName));

        while (true) {
            MatrixTask task;

            // --- Retrieve next task from shared queue ---
            try {
                task = taskQueue.getTask();
            } catch (InterruptedException e) {
                // Always restore interrupted status so higher-level code can see it
                Thread.currentThread().interrupt();
                LOGGER.warning(String.format(
                    "[Worker-%d] Interrupted while waiting for a task. Exiting cleanly.", workerId));
                break;
            }

            // null == poison pill: queue is drained and marked done
            if (task == null) {
                LOGGER.info(String.format(
                    "[Worker-%d] No more tasks. Processed %d row(s), %d error(s). Exiting.",
                    workerId, getTasksProcessed(), getErrorsEncountered()));
                break;
            }

            // --- Process the task ---
            LOGGER.info(String.format("[Worker-%d] Processing %s", workerId, task));
            try {
                double[] resultRow = computeRow(task);
                resultStore.storeRow(task.getRowIndex(), resultRow);
                incrementProcessed();
                LOGGER.info(String.format("[Worker-%d] Completed %s", workerId, task));

            } catch (InterruptedException e) {
                // Thread.sleep inside computeRow was interrupted - must restore flag
                Thread.currentThread().interrupt();
                incrementErrors();
                LOGGER.warning(String.format(
                    "[Worker-%d] Interrupted during computation of %s. Exiting.", workerId, task));
                break;   // exit the loop; interrupt means shutdown is requested

            } catch (TaskProcessingException e) {
                // Known, typed failure - log with full context
                incrementErrors();
                LOGGER.severe(String.format(
                    "[Worker-%d] TaskProcessingException: %s", workerId, e.getMessage()));
                // Worker continues - one bad row does not stop the others

            } catch (Exception e) {
                // Unexpected error - wrap and log, then continue
                incrementErrors();
                LOGGER.severe(String.format(
                    "[Worker-%d] Unexpected error processing %s: %s [%s]",
                    workerId, task, e.getMessage(), e.getClass().getSimpleName()));
            }
        }

        LOGGER.info(String.format("[Worker-%d | %s] Finished", workerId, threadName));
    }

    /**
     * Compute one row of C = A x B.
     * Simulates processing delay to represent real computational work.
     *
     * @param task the row task
     * @return the computed row vector
     * @throws InterruptedException      if Thread.sleep is interrupted
     * @throws TaskProcessingException   if matrix dimensions are inconsistent
     */
    private double[] computeRow(MatrixTask task)
            throws InterruptedException, TaskProcessingException {

        double[]   rowA    = task.getRowA();
        double[][] matrixB = task.getMatrixB();

        // Dimension guard - throw our custom exception for a bad task
        if (rowA == null || matrixB == null || matrixB.length == 0) {
            throw new TaskProcessingException(
                task.getTaskId(), task.getRowIndex(),
                "null or empty input data");
        }
        if (rowA.length != matrixB.length) {
            throw new TaskProcessingException(
                task.getTaskId(), task.getRowIndex(),
                String.format("dimension mismatch: rowA.length=%d but matrixB.rows=%d",
                    rowA.length, matrixB.length));
        }

        int      cols      = matrixB[0].length;
        double[] rowResult = new double[cols];

        // Dot product: result[j] = sum(A[row][k] * B[k][j])
        for (int j = 0; j < cols; j++) {
            double sum = 0.0;
            for (int k = 0; k < rowA.length; k++) {
                sum += rowA[k] * matrixB[k][j];
            }
            rowResult[j] = sum;
        }

        // Simulated processing delay - InterruptedException propagates to run()
        int delay = MIN_DELAY_MS + (int)(Math.random() * (MAX_DELAY_MS - MIN_DELAY_MS));
        Thread.sleep(delay);

        return rowResult;
    }

    // --- Thread-safe stat helpers (ReentrantLock) ---

    private void incrementProcessed() {
        statsLock.lock();
        try { tasksProcessed++; }
        finally { statsLock.unlock(); }
    }

    private void incrementErrors() {
        statsLock.lock();
        try { errorsEncountered++; }
        finally { statsLock.unlock(); }
    }

    public int getWorkerId() { return workerId; }

    public int getTasksProcessed() {
        statsLock.lock();
        try { return tasksProcessed; }
        finally { statsLock.unlock(); }
    }

    public int getErrorsEncountered() {
        statsLock.lock();
        try { return errorsEncountered; }
        finally { statsLock.unlock(); }
    }
}
