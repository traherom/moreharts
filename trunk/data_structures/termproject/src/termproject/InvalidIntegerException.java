package termproject;

/**
 *
 * @author Caleb
 */
public class InvalidIntegerException extends RuntimeException {

    /**
     * Creates a new instance of <code>InvalidIntegerException</code> without detail message.
     */
//    public InvalidIntegerException() {
//    }


    /**
     * Constructs an instance of <code>InvalidIntegerException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public InvalidIntegerException(String errorMsg) {
        super(errorMsg);
    }
}
