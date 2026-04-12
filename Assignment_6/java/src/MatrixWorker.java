package matrix;

import java.util.logging.Logger;

/**
 * Worker thread that pulls MatrixTask objects from the SharedTaskQueue,
 * computes the corresponding row of the result matrix, and stores it in
 * the ResultStore.
 *
 * Exception handling:
 *   - InterruptedException: restores the interrupt flag and exits cleanly.
 *   - Any runtime error during dot-product computation is caught so one bad
 *     task doesn't kill the whole worker thread.
 */
public class MatrixWorker implements Runnable {

    private static final Logger LOGGER = Logger.getLogger(MatrixWorker.class.getName());

    // Simulated processing delay range (ms) — represents computational work
    private static final int MIN_DELAY_MS = 10;
    private static final int MAX_DELAY_MS = 50;

    private final int workerId;
    private final SharedTaskQueue taskQueue;
    private final ResultStore resultStore;

    // Stats tracked per worker
    private int tasksProcessed = 0;
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

            // ── Retrieve next task ────────────────────────────────────────────
            try {
                task = taskQueue.getTask();
            } catch (InterruptedException e) {
                // Restore the interrupted status and exit gracefully
                Thread.currentThread().interrupt();
                LOGGER.warning(String.format(
                    "[Worker-%d] Interrupted while waiting for task. Exiting.", workerId));
                break;
            }

            // null is the shutdown signal (queue is drained and marked done)
            if (task == null) {
                LOGGER.info(String.format(
                    "[Worker-%d] No more tasks. Processed %d rows, %d error(s). Exiting.",
                    workerId, tasksProcessed, errorsEncountered));
                break;
            }

            // ── Process the task ─────────────────────────────────────────────
            LOGGER.fine(() -> String.format("[Worker-%d] Processing %s", workerId, task));
            try {
                double[] resultRow = computeRow(task);
                resultStore.storeRow(task.getRowIndex(), resultRow);
                tasksProcessed++;

            } catch (Exception e) {
                // Catches arithmetic errors, null dereference, etc.
                errorsEncountered++;
                LOGGER.severe(String.format(
                    "[Worker-%d] ERROR processing %s: %s", workerId, task, e.getMessage()));
                // Worker continues to next task — one bad task doesn't stop others
            }
        }

        LOGGER.info(String.format("[Worker-%d | %s] Finished", workerId, threadName));
    }

    /**
     * Compute one row of C = A × B.
     * Simulates processing delay to represent real computational work.
     *
     * @param task the row task
     * @return the computed row vector
     * @throws InterruptedException if the simulated delay is interrupted
     */
    private double[] computeRow(MatrixTask task) throws InterruptedException {
        double[]   rowA    = task.getRowA();
        double[][] matrixB = task.getMatrixB();
        int        cols    = matrixB[0].length;
        double[]   rowResult = new double[cols];

        // Dot product: result[j] = sum(A[row][k] * B[k][j])
        for (int j = 0; j < cols; j++) {
            double sum = 0.0;
            for (int k = 0; k < rowA.length; k++) {
                sum += rowA[k] * matrixB[k][j];
            }
            rowResult[j] = sum;
        }

        // Simulate processing delay
        int delay = MIN_DELAY_MS + (int)(Math.random() * (MAX_DELAY_MS - MIN_DELAY_MS));
        Thread.sleep(delay);

        return rowResult;
    }

    // ── Getters for reporting ─────────────────────────────────────────────────
    public int getWorkerId()         { return workerId; }
    public int getTasksProcessed()   { return tasksProcessed; }
    public int getErrorsEncountered(){ return errorsEncountered; }
}
