package termproject;

/**
 *
 * @author Caleb
 */
public interface Dictionary {
    public int size();
    public boolean isEmpty();

    public Object findElement (Object key) throws ElementNotFoundException;

    public void insertElement (Object key, Object element);

    public Object removeElement (Object key) throws ElementNotFoundException;
}
