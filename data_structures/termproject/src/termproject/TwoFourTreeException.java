package termproject;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Caleb
 */
public class TwoFourTreeException extends RuntimeException {

    public TwoFourTreeException() {
        super ("Problem with TwoFourTree");
    }
    public TwoFourTreeException(String errorMsg) {
        super (errorMsg);
    }
}
