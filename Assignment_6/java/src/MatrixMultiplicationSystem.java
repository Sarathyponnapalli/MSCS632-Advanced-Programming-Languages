package matrix;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.logging.ConsoleHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

/**
 * Parallel Matrix Multiplication System (Java)
 *
 * Architecture:
 * Producer (main thread) --> SharedTaskQueue --> Worker threads
 * |
 * ResultStore --> output file
 *
 * Concurrency model:
 * - SharedTaskQueue uses synchronized + wait/notifyAll
 * - Worker threads managed by a fixed-size ExecutorService
 * - ResultStore uses synchronized writes (one row per worker)
 * - Clean shutdown via markDone() poison-pill pattern
 */
public class MatrixMultiplicationSystem {

    private static final Logger LOGGER = Logger.getLogger(MatrixMultiplicationSystem.class.getName());

    // --- Configuration ---
    private static final int ROWS_A = 8; // rows in matrix A
    private static final int INNER = 6; // cols A == rows B
    private static final int COLS_B = 7; // cols in matrix B
    private static final int NUM_WORKERS = 4; // worker thread count
    private static final int QUEUE_CAP = 16; // bounded queue capacity
    private static final String OUT_FILE = "result_matrix.txt";

    public static void main(String[] args) {
        configureLogging();
        LOGGER.info("====================================================");
        LOGGER.info("   Parallel Matrix Multiplication System -- Java   ");
        LOGGER.info("====================================================");
        LOGGER.info(String.format("Matrix A: %dx%d   Matrix B: %dx%d   Workers: %d",
                ROWS_A, INNER, INNER, COLS_B, NUM_WORKERS));

        // 1. Generate random matrices
        double[][] matrixA = generateMatrix(ROWS_A, INNER);
        double[][] matrixB = generateMatrix(INNER, COLS_B);
        printMatrix("Matrix A", matrixA);
        printMatrix("Matrix B", matrixB);

        // 2. Create shared resources
        SharedTaskQueue taskQueue = new SharedTaskQueue(QUEUE_CAP);
        ResultStore resultStore = new ResultStore(ROWS_A, COLS_B);

        // 3. Create and start worker threads via ExecutorService
        ExecutorService executor = Executors.newFixedThreadPool(NUM_WORKERS);
        List<MatrixWorker> workers = new ArrayList<>();

        for (int i = 0; i < NUM_WORKERS; i++) {
            MatrixWorker worker = new MatrixWorker(i + 1, taskQueue, resultStore);
            workers.add(worker);
            executor.submit(worker);
            LOGGER.info("Submitted Worker-" + (i + 1) + " to executor");
        }

        // 4. Producer: enqueue one task per row of A
        long startTime = System.currentTimeMillis();
        LOGGER.info("--- Enqueueing tasks ---");

        for (int row = 0; row < ROWS_A; row++) {
            MatrixTask task = new MatrixTask(row, row, matrixA[row], matrixB);
            try {
                taskQueue.addTask(task);
                LOGGER.info("Enqueued " + task);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                LOGGER.severe("Producer interrupted while enqueueing tasks. Aborting.");
                executor.shutdownNow();
                return;
            }
        }

        // Signal workers: no more tasks coming
        taskQueue.markDone();
        LOGGER.info("--- All tasks enqueued. Queue marked done. ---");

        // 5. Graceful shutdown: wait for all workers to finish
        executor.shutdown();
        try {
            boolean finished = executor.awaitTermination(60, TimeUnit.SECONDS);
            if (!finished) {
                LOGGER.warning("Timeout waiting for workers. Forcing shutdown.");
                executor.shutdownNow();
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            LOGGER.severe("Main thread interrupted during shutdown: " + e.getMessage());
            executor.shutdownNow();
        }

        long elapsed = System.currentTimeMillis() - startTime;

        // 6. Report worker stats
        LOGGER.info("--- Worker Summary ---");
        int totalErrors = 0;
        for (MatrixWorker w : workers) {
            LOGGER.info(String.format("Worker-%d: %d tasks processed, %d error(s)",
                    w.getWorkerId(), w.getTasksProcessed(), w.getErrorsEncountered()));
            totalErrors += w.getErrorsEncountered();
        }
        LOGGER.info(String.format("Total rows completed: %d / %d  |  Errors: %d  |  Time: %dms",
                resultStore.getCompletedRows(), ROWS_A, totalErrors, elapsed));

        // 7. Display and verify result
        printMatrix("Result Matrix C = A x B", resultStore.getResult());

        double[][] expected = serialMultiply(matrixA, matrixB);
        boolean correct = matricesEqual(resultStore.getResult(), expected, 1e-9);
        LOGGER.info(
                "Verification vs serial result: " + (correct ? "CORRECT" : "MISMATCH - check for race conditions!"));

        // 8. Write result to file
        try {
            resultStore.writeToFile(OUT_FILE);
            LOGGER.info("Result written to " + OUT_FILE);
        } catch (IOException e) {
            LOGGER.severe("Could not write output file: " + e.getMessage());
        }
    }

    // --- Helpers ---

    /** Generate a matrix filled with random doubles in [0, 10). */
    private static double[][] generateMatrix(int rows, int cols) {
        Random rng = new Random(42);
        double[][] m = new double[rows][cols];
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                m[i][j] = Math.round(rng.nextDouble() * 10.0 * 10) / 10.0;
        return m;
    }

    /** Serial O(n^3) multiply -- used for correctness verification. */
    private static double[][] serialMultiply(double[][] A, double[][] B) {
        int n = A.length, m = B[0].length, p = B.length;
        double[][] C = new double[n][m];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                for (int k = 0; k < p; k++)
                    C[i][j] += A[i][k] * B[k][j];
        return C;
    }

    /** Element-wise comparison with tolerance. */
    private static boolean matricesEqual(double[][] A, double[][] B, double tol) {
        for (int i = 0; i < A.length; i++)
            for (int j = 0; j < A[0].length; j++)
                if (Math.abs(A[i][j] - B[i][j]) > tol)
                    return false;
        return true;
    }

    /** Pretty-print a matrix to the logger. */
    private static void printMatrix(String label, double[][] m) {
        LOGGER.info("--- " + label + " (" + m.length + "x" + m[0].length + ") ---");
        for (double[] row : m) {
            StringBuilder sb = new StringBuilder("  ");
            for (double v : row)
                sb.append(String.format("%7.2f ", v));
            LOGGER.info(sb.toString());
        }
    }

    /** Configure a readable console logger (INFO level). */
    private static void configureLogging() {
        Logger root = Logger.getLogger("");
        root.setLevel(Level.INFO);
        for (var h : root.getHandlers())
            root.removeHandler(h);
        ConsoleHandler ch = new ConsoleHandler();
        ch.setLevel(Level.INFO);
        ch.setFormatter(new SimpleFormatter() {
            @Override
            public String format(java.util.logging.LogRecord r) {
                return String.format("[%s] %-7s %s%n",
                        new java.text.SimpleDateFormat("HH:mm:ss.SSS")
                                .format(new java.util.Date(r.getMillis())),
                        r.getLevel(),
                        r.getMessage());
            }
        });
        root.addHandler(ch);
    }
}
