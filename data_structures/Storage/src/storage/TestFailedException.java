package storage;

/**
 * Used by testing frameworks to indicates a class failed a test.
 * @author Ryan Morehart
 */
public class TestFailedException extends Exception {

    /**
     * Creates a new instance of <code>TestFailedException</code> without detail message.
     */
    public TestFailedException() {
    }


    /**
     * Constructs an instance of <code>TestFailedException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public TestFailedException(String msg) {
        super(msg);
    }
}
