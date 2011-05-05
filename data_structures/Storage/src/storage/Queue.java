package storage;

/**
 * An interface for a FIFO queue.
 * 
 * @param <E> Type the queue will hold
 * @author Ryan Morehart
 */
public interface Queue<E>
{
	public void enqueue (E element);
	public E dequeue () throws QueueEmptyException;
	public E front () throws QueueEmptyException;
	public int size();
	public boolean isEmpty();
}
