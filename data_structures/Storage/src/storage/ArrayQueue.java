/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package storage;

/**
 * A queue using an array as storage
 * 
 * @param <E> Type that the ArrayQueue will hold
 * @author Ryan Morehart
 */
public class ArrayQueue<E> implements Queue<E>
{

	/**
	 * Storage space of queue
	 */
	private E[] queueStore;
	/**
	 * The element that will be the next to be dequeued
	 */
	private int front;
	/**
	 * The most recently placed element on the queue
	 */
	private int back;
	/**
	 * We could figure this out from math with front/back, but this is
	 * easy and straight forward.
	 */
	private int size;

	/**
	 * Main constructor of ArrayQueue
	 * @param initialCapacity Initial size of the queue. Will automatically expand beyond
	 *		this as necessary.
	 */
	public ArrayQueue(int initialCapacity)
	{
		queueStore = (E[]) new Object[initialCapacity];
		front = 0;
		back = 0;
		size = 0;
	}

	/**
	 * Constructs an ArrayQueue with a reasonable default size.
	 */
	public ArrayQueue()
	{
		this(10);
	}

	/**
	 * Places an new object on the queue
	 * @param element Object to be queued up
	 */
	public void enqueue(E element)
	{
		// Do we need to make more space?
		if (size == queueStore.length)
		{
			// Create new storage
			E[] newStore = (E[]) new Object[2 * queueStore.length];

			// Copy elements over, placing at the begining of the array
			for (int i = 0; i < queueStore.length; i++)
			{
				newStore[i] = queueStore[(front + i) % queueStore.length];
			}

			// Move indexes
			front = 0;
			back = queueStore.length;

			// Make it our new storage
			queueStore = newStore;
		}

		// Place on bottom
		queueStore[back] = element;

		// Change around variables
		back = (back + 1) % queueStore.length;
		size++;
	}

	/**
	 * Removes the next element of the queue and returns it
	 * @return The next element of the queue
	 * @throws storage.QueueEmptyException
	 */
	public E dequeue() throws QueueEmptyException
	{
		// Pull off front value
		// Use the other function so we don't have to duplicate check
		E frontEl = front();

		// And change around our variables to remove it from the list
		queueStore[front] = null;
		front = (front + 1) % queueStore.length;
		size--;

		// Return element
		return frontEl;
	}

	/**
	 * Gives the next element of the queue, but does not actually remove it
	 * or advance to the next element
	 * @return The next element of the queue
	 * @throws storage.QueueEmptyException
	 */
	public E front() throws QueueEmptyException
	{
		// Do we have stuff to 'front,' if you will?
		if (size == 0)
		{
			throw new QueueEmptyException("No elements on queue");
		}
		return queueStore[front];
	}

	/**
	 * The current number of elements of the queue
	 * @return Number of elements on queue
	 */
	public int size()
	{
		return size;
	}

	/**
	 * Returns true if there are currently no elements on the queue
	 * @return True if there are no elements on queue.
	 */
	public boolean isEmpty()
	{
		return (size == 0);
	}

	/**
	 * Gives the number of elements the queue can curretly hold before having to expand.
	 * @return current capacity of queue
	 */
	public int capacity()
	{
		return queueStore.length;
	}

	/**
	 * Runs the ArrayQueue through a series of test.
	 * @param args ignored
	 */
	public static void main(String[] args)
	{
		System.out.println("Running tests...");

		try
		{
			ArrayQueue<Integer> test = new ArrayQueue<Integer>(20);

			// Basic tests
			addEl(test, 1, 5);
			removeEl(test, 1, 5);
			addEl(test, 6, 10);
			removeEl(test, 6, 8);
			addEl(test, 11, 15);
			removeEl(test, 9, 12);

			// Removal of too much
			try
			{
				removeEl(test, 13, 18);
				throw new TestFailedException("Failed to throw when queue was empty");
			}
			catch (QueueEmptyException e)
			{
				// This was supposed to fail, we pulled off more than there was
				System.out.println("Failed as expected, more were removed than existed");
			}

			// Queue is empty, right?
			if (!test.isEmpty())
			{
				throw new TestFailedException("Queue is not empty as expected");
			}
			System.out.println("Queue is now empty.");

			// Now, let us test a wrap around
			addEl(test, 1, 10);
			removeEl(test, 1, 5);
			removeEl(test, 6, 8);
			addEl(test, 11, 14);
			removeEl(test, 9, 14);

			// Force an expansion!
			int currSize = test.capacity();
			addEl(test, 1, currSize + 5);
			if (test.capacity() < currSize + 5)
			{
				throw new TestFailedException("Failed to expand queue storage when needed");
			}
			System.out.println("Queue successfully expanded");
			removeEl(test, 1, currSize - 5);
			removeEl(test, currSize - 4, currSize + 5);

			// Is everything still working?
			addEl(test, 1, 10);
			try
			{
				removeEl(test, 1, 20);
				throw new TestFailedException("Failed to throw when queue was empty");
			}
			catch (QueueEmptyException e)
			{
				// This was supposed to fail, we pulled off more than there was
				System.out.println("Failed as expected, more were removed than existed");
			}

			// Queue is empty, right?
			if (!test.isEmpty())
			{
				throw new TestFailedException("Queue is not empty as expected");
			}
			System.out.println("Queue is now empty.");

			// Add a huge number, forcing the queue to expand two more times
			addEl(test, 1, 100);
			removeEl(test, 1, 100);

			// If we make it here, everything is correct
			System.out.println("Passed");
		}
		catch (QueueEmptyException e)
		{
			System.out.println(e);
			System.out.println("Failed");
		}
		catch (TestFailedException e)
		{
			System.out.println(e);
			System.out.println("Failed");
		}
	}

	/**
	 * Adds sequential integers <code>start</code> to <code>end</code> to the given queue
	 * and prints out info about the queue after the operation
	 * @param q Queue to add to
	 * @param start Beginning of sequence
	 * @param end End of sequence
	 */
	private static void addEl(ArrayQueue<Integer> q, int start, int end)
	{
		System.out.print("Adding " + (end - start) + " elements. Size: ");
		for (int i = start; i <= end; i++)
		{
			q.enqueue(i);
		}
		System.out.println(q.size() + " Capacity: " + q.capacity());
	}

	/**
	 * Removes elements from the queue and ensures that they are in the sequence
	 * from <code>start</code> to <code>end</code>. Prints out information about
	 * the queue after the operation.
	 * @param q Queue to operate on
	 * @param start Beginning of the sequence expected
	 * @param end End of the sequence
	 * @throws storage.TestFailedException
	 * @throws storage.QueueEmptyException
	 */
	private static void removeEl(ArrayQueue<Integer> q, int start, int end)
		throws TestFailedException, QueueEmptyException
	{
		System.out.print("Removing " + (end - start) + " elements. Size: ");

		for (int i = start; i <= end; i++)
		{
			// Test ArrayQueue.front() as well
			if (q.front() != i)
			{
				throw new TestFailedException("Front is " + q.front() + ", expected " + i);
			}

			int t = q.dequeue();

			// Ensure what is popped off is what is expected
			if (t != i)
			{
				throw new TestFailedException("Pulled off " + t + ", expected " + i);
			}
		}

		System.out.println(q.size() + " Capacity: " + q.capacity());
	}
}
