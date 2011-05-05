package raim;

/**
 * @author Alex Laird
 * @author Ryan Morehart
 */
public class RAIMException extends Exception {

    /**
     * Creates a new instance of <code>RAIMException</code> without detail message.
     */
    public RAIMException() {
    }


    /**
     * Constructs an instance of <code>RAIMException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public RAIMException(String msg) {
        super(msg);
    }
}
