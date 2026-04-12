package matrix;

/**
 * Represents a single row-computation task in the parallel matrix multiplication system.
 * Each task computes one row of the result matrix C = A x B.
 */
public class MatrixTask {
    private final int rowIndex;
    private final double[] rowA;
    private final double[][] matrixB;
    private final int taskId;

    public MatrixTask(int taskId, int rowIndex, double[] rowA, double[][] matrixB) {
        this.taskId = taskId;
        this.rowIndex = rowIndex;
        this.rowA = rowA;
        this.matrixB = matrixB;
    }

    public int getRowIndex() { return rowIndex; }
    public double[] getRowA()  { return rowA; }
    public double[][] getMatrixB() { return matrixB; }
    public int getTaskId()     { return taskId; }

    @Override
    public String toString() {
        return String.format("MatrixTask[id=%d, row=%d]", taskId, rowIndex);
    }
}
