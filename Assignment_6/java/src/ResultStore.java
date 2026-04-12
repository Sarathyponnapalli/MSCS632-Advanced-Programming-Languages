package matrix;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Logger;

/**
 * Thread-safe shared result store.
 * Workers write their computed rows here; the main thread reads the final matrix.
 *
 * Synchronization strategy: a ReentrantLock per-row slot is not needed because
 * each row slot is written by exactly one worker. We only need a single lock for
 * the output file and the completedRows counter.
 */
public class ResultStore {

    private static final Logger LOGGER = Logger.getLogger(ResultStore.class.getName());

    private final double[][] result;
    private int completedRows = 0;

    public ResultStore(int rows, int cols) {
        result = new double[rows][cols];
    }

    /**
     * Store a computed row. Safe to call from multiple threads because each
     * rowIndex is unique - no two workers receive the same row task.
     */
    public synchronized void storeRow(int rowIndex, double[] rowData) {
        result[rowIndex] = rowData;
        completedRows++;
        LOGGER.fine(() -> String.format("Stored row %d  (%d total complete)", rowIndex, completedRows));
    }

    /** Returns the fully assembled result matrix (call after all workers finish). */
    public double[][] getResult() {
        return result;
    }

    /**
     * Write the result matrix to a text file.
     *
     * @param filename output file path
     * @throws IOException if writing fails
     */
    public synchronized void writeToFile(String filename) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filename))) {
            writer.write("=== Matrix Multiplication Result ===\n");
            for (double[] row : result) {
                StringBuilder sb = new StringBuilder();
                for (int j = 0; j < row.length; j++) {
                    sb.append(String.format("%10.2f", row[j]));
                    if (j < row.length - 1) sb.append("  ");
                }
                writer.write(sb.toString());
                writer.newLine();
            }
            LOGGER.info("Result matrix written to " + filename);
        } catch (IOException e) {
            LOGGER.severe("Failed to write result file: " + e.getMessage());
            throw e;          // re-throw so caller can handle / log at higher level
        }
    }

    public synchronized int getCompletedRows() {
        return completedRows;
    }
}
