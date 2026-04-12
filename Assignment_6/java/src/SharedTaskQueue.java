package matrix;

import java.util.LinkedList;
import java.util.Queue;
import java.util.logging.Logger;

/**
 * Thread-safe bounded queue of MatrixTask objects.
 *
 * Synchronization strategy:
 *   All public methods are synchronized on "this" (the monitor of the
 *   SharedTaskQueue instance). wait() / notifyAll() coordinate producers
 *   and consumers:
 *     - addTask() blocks when the queue is full (backpressure)
 *     - getTask() blocks when the queue is empty and not yet done
 *     - markDone() wakes all waiting threads for clean shutdown
 *
 * Empty-queue access:
 *   Rather than throwing an exception on an empty queue (which would
 *   require constant polling), getTask() blocks via wait() and logs the
 *   waiting state. This satisfies the requirement to "handle empty queue
 *   access" without busy-spinning.
 */
public class SharedTaskQueue {

    private static final Logger LOGGER = Logger.getLogger(SharedTaskQueue.class.getName());

    private final Queue<MatrixTask> queue = new LinkedList<>();
    private boolean done = false;     // true when no more tasks will be added
    private final int capacity;       // bounded to prevent memory runaway

    public SharedTaskQueue(int capacity) {
        if (capacity <= 0) throw new IllegalArgumentException("Capacity must be positive");
        this.capacity = capacity;
    }

    /**
     * Enqueue a task. Blocks (with logging) if the queue is at capacity.
     *
     * @param task the task to add; must not be null
     * @throws InterruptedException if the thread is interrupted while waiting
     * @throws IllegalStateException if called after markDone()
     */
    public synchronized void addTask(MatrixTask task) throws InterruptedException {
        if (task == null) throw new IllegalArgumentException("Task must not be null");
        if (done) throw new IllegalStateException("Cannot add tasks after markDone()");

        while (queue.size() >= capacity) {
            LOGGER.warning("SharedTaskQueue is full (capacity=" + capacity
                + ") - producer is blocking until a worker drains a slot.");
            wait();
        }
        queue.offer(task);
        LOGGER.fine(() -> "Enqueued " + task + "  (queue size now: " + queue.size() + ")");
        notifyAll();    // wake any workers blocked in getTask()
    }

    /**
     * Remove and return the next task. Returns null only when the queue is
     * empty AND markDone() has been called - the clean shutdown signal.
     *
     * Empty-queue handling: rather than throwing on empty access, this method
     * blocks and logs, then retries once the state changes (producer adds a
     * task or calls markDone).
     *
     * @return the next MatrixTask, or null if no more tasks will ever arrive
     * @throws InterruptedException if the thread is interrupted while waiting
     */
    public synchronized MatrixTask getTask() throws InterruptedException {
        while (queue.isEmpty() && !done) {
            LOGGER.fine("Queue is empty - worker waiting for next task...");
            wait();
        }

        if (queue.isEmpty()) {
            // done == true: graceful shutdown, no tasks left
            LOGGER.fine("Queue empty and done - returning null (shutdown signal).");
            return null;
        }

        MatrixTask task = queue.poll();
        notifyAll();    // wake any producers blocked in addTask()
        return task;
    }

    /**
     * Signal that no more tasks will be added.
     * All threads blocked in getTask() will be woken and allowed to drain
     * the remaining tasks before returning null.
     */
    public synchronized void markDone() {
        done = true;
        LOGGER.info("SharedTaskQueue marked as done. Waking all waiting workers.");
        notifyAll();
    }

    /** Current queue depth (informational). */
    public synchronized int size() {
        return queue.size();
    }

    /** Whether the producer has signaled completion. */
    public synchronized boolean isDone() {
        return done;
    }
}
