package termproject;

/**
 * @author Dr. David M. Gallagher
 */
public class Item {
    private Object itemKey;
    private Object itemElement;

    public Item() {
        this (null, null);
    }

    public Item(Object key, Object element) {
        itemKey = key;
        itemElement = element;
    }

    public Object key() {
        return itemKey;
    }
    public void setKey(Object key) {
        itemKey = key;
    }
    public Object element() {
        return itemElement;
    }
    public void setElement (Object element) {
        itemElement = element;
    }
}
