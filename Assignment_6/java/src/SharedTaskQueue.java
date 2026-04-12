package matrix;

import java.util.LinkedList;
import java.util.Queue;
import java.util.logging.Logger;

/**
 * Thread-safe shared queue of MatrixTask objects.
 * Uses synchronized blocks and wait/notify for producer-consumer coordination.
 * Supports a "poison pill" sentinel via markDone() to signal workers to stop.
 */
public class SharedTaskQueue {

    private static final Logger LOGGER = Logger.getLogger(SharedTaskQueue.class.getName());

    private final Queue<MatrixTask> queue = new LinkedList<>();
    private boolean done = false;         // set when no more tasks will be added
    private final int capacity;           // bounded capacity prevents unbounded memory use

    public SharedTaskQueue(int capacity) {
        this.capacity = capacity;
    }

    /**
     * Add a task to the queue. Blocks if the queue is at capacity.
     *
     * @param task the task to enqueue
     * @throws InterruptedException if the thread is interrupted while waiting
     */
    public synchronized void addTask(MatrixTask task) throws InterruptedException {
        while (queue.size() >= capacity) {
            LOGGER.fine("Queue full — producer waiting…");
            wait();
        }
        queue.offer(task);
        LOGGER.fine(() -> "Enqueued " + task);
        notifyAll();          // wake any consumers that were waiting for work
    }

    /**
     * Retrieve and remove the head task. Returns null only when the queue is
     * empty AND the producer has marked it as done (clean shutdown signal).
     *
     * @return the next MatrixTask, or null if there will be no more tasks
     * @throws InterruptedException if the thread is interrupted while waiting
     */
    public synchronized MatrixTask getTask() throws InterruptedException {
        while (queue.isEmpty() && !done) {
            LOGGER.fine("Queue empty — worker waiting…");
            wait();
        }
        if (queue.isEmpty()) {
            // done == true and nothing left → signal shutdown
            return null;
        }
        MatrixTask task = queue.poll();
        notifyAll();          // wake any producers that were waiting for space
        return task;
    }

    /**
     * Signal that no more tasks will be added.
     * Wakes all waiting consumers so they can drain and exit.
     */
    public synchronized void markDone() {
        done = true;
        notifyAll();
    }

    /** Snapshot of current queue depth (informational / logging only). */
    public synchronized int size() {
        return queue.size();
    }
}
