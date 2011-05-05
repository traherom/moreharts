package termproject;

/**
 *
 * @author Caleb
 */
public class TFNodeException extends RuntimeException {

    public TFNodeException() {
        super ("Problem with TFNode");
    }
    public TFNodeException(String errorMsg) {
        super (errorMsg);
    }
}
