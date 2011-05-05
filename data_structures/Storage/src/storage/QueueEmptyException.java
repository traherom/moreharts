package storage;

/**
 * Exception for when an attempt is made to access an element of a queue and
 * the queue is empty.
 * @author Ryan Morehart
 */
public class QueueEmptyException extends Exception {

    /**
     * Creates a new instance of <code>QueueEmptyException</code> without detail message.
     */
    public QueueEmptyException() {
    }


    /**
     * Constructs an instance of <code>QueueEmptyException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public QueueEmptyException(String msg) {
        super(msg);
    }
}
