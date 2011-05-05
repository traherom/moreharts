/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package termproject;

/**
 *
 * @author Caleb
 */
public class ElementNotFoundException extends RuntimeException {

    /**
     * Creates a new instance of <code>ElementNotFoundException</code> without detail message.
     */
    public ElementNotFoundException() {
    }


    /**
     * Constructs an instance of <code>ElementNotFoundException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public ElementNotFoundException(String msg) {
        super(msg);
    }
}
