package project7;

import java.util.Random;

/**
 * Implements a binary heap
 * @author Ryan Morehart
 */
public class ArrayHeap extends ArrayBinaryTree implements Heap
{
	Comparator heapComp;

	public ArrayHeap(Comparator newComp)
	{
		this(newComp, DEFAULT_SIZE);
	}

	public ArrayHeap(Comparator newComp, int newSize)
	{
		super(newSize);
		heapComp = newComp;
	}

	/**
	 * Adds an element to the heap
	 * @param newElement element to be added
	 * @throws project7.InvalidObjectException thrown if the Object given is not comparable 
	 *		using the given Comparator
	 */
	public void add(Object newElement) throws InvalidObjectException
	{
		// Ensure we can actually store this element
		if(!heapComp.isComparable(newElement))
		{
			throw new InvalidObjectException("Given object is not comparable " +
					"using the Comparator given");
		}

		// More than we have room for?
		if(size == btArray.length)
		{
			// Double array and copy values over
			ArrayPosition[] newStore = new ArrayPosition[btArray.length * 2];
			System.arraycopy(btArray, 0, newStore, 0, btArray.length);
			for(int i = 0; i < btArray.length; i++)
			{
				newStore[i] = btArray[i];
			}
			btArray = newStore;
			capacity = btArray.length;
		}

		// Put as the last element in the tree (size, since this is an array-based tree)
		ArrayPosition newNode = new ArrayPosition(size, newElement);
		btArray[size] = newNode;
		size++;

		// Correct
		bubbleUp(newNode);
	}

	/**
	 * Removes the root from the heap, which happens to mean that the minimum element is removed.
	 * @return Element at the root of tree
	 * @throws project7.EmptyHeapException thrown if there are no elements on heap to remove
	 */
	public Object removeRoot() throws EmptyHeapException
	{
		// Can only remove if we aren't empty
		if(isEmpty())
		{
			throw new EmptyHeapException("No elements on heap to remove");
		}

		// Get root element for returning to caller
		Object rootValue = root().element();

		// Grab final element and make it root. Clear where it used to be
		size--;
		btArray[size].setIndex(ROOT);
		btArray[ROOT] = btArray[size];
		btArray[size] = null;

		// Correct
		bubbleDown(btArray[ROOT]);

		return rootValue;
	}

	/**
	 * Moves a newly inserted node upward in the tree until it is greater than its parent
	 * @param insertedNode where to begin the bubble up
	 */
	private void bubbleUp(ArrayPosition insertedNode)
	{
		// Keep moving up tree till inserted is greater than parent
		while(!isRoot(insertedNode) && heapComp.isLessThan(insertedNode.element(),
				parent(insertedNode).element()))
		{
			swapElements(insertedNode, (ArrayPosition)parent(insertedNode));
		}
	}

	/**
	 * Fixes the heap after an element is removed and something has been rotated into its place
	 * @param currNode where to start the bubble down. Typically the root, but this way there's
	 *		flexibility if needed.
	 */
	private void bubbleDown(ArrayPosition currNode)
	{
		while(currNode != null)
		{
			// Which is smallest: me, my left child, or my right child?
			ArrayPosition smallest = (ArrayPosition)smallestElement(currNode,
					leftChild(currNode), rightChild(currNode));

			if(smallest != currNode)
			{
				swapElements(smallest, currNode);
			}
			else
			{
				// The current node is less than or equal to children, we're done
				currNode = null;
			}
		}
	}

	/**
	 * Compares the elements in the given Positions and returns the Position
	 * which points to the smallest element. The first Position must be given,
	 * but the other two will be ignored if they are null.
	 * @param a first element
	 * @param b second element
	 * @param c third element
	 * @return Position of the smallest of the three given
	 */
	private Position smallestElement(Position a, Position b, Position c)
	{
		Position small = a;
		if(b != null && heapComp.isLessThan(b.element(), small.element()))
		{
			small = b;
		}
		if(c != null && heapComp.isLessThan(c.element(), small.element()))
		{
			small = c;
		}
		return small;
	}

	/**
	 * Exchanges the places of the given two ArrayPositions, moving
	 * both the location in the array and the index of the ArrayPosition
	 * @param a first element
	 * @param b second element
	 */
	private void swapElements(ArrayPosition a, ArrayPosition b)
	{
		// Swap elements
		// Requires moving both the indexes in the ArrayPositions and the array elements
		int temp = a.getIndex();
		a.setIndex(b.getIndex());
		b.setIndex(temp);
		btArray[b.getIndex()] = b;
		btArray[a.getIndex()] = a;
	}

	public static void main(String[] args)
	{
		Comparator myComp = new IntegerComparator();
		Heap myHeap = new ArrayHeap(myComp, 20);

		// Example testing code from Dr. G
		myHeap.add(new Integer(14));
		myHeap.add(new Integer(17));
		myHeap.add(new Integer(3));
		myHeap.add(new Integer(21));
		myHeap.add(new Integer(8));
		myHeap.add(new Integer(18));
		myHeap.add(new Integer(1));
		myHeap.add(new Integer(11));
		myHeap.add(new Integer(17));
		myHeap.add(new Integer(6));

		System.out.println(myHeap.size());
		while(!myHeap.isEmpty())
		{
			Integer removedInt = (Integer)myHeap.removeRoot();
			System.out.println("Removed " + removedInt);
		}
		System.out.println("All nodes removed");
		// End Dr. G's example test

		// More testing. Reuse same heap as above as that'll tell us if the heap keeps working
		// even after being emptied and refilled
		// First off, the heap is empty. Does it know that?
		try
		{
			Object t = myHeap.removeRoot();
			System.out.println("FAILED: Pulled " + t + " from heap when it should be empty");
			return;
		}
		catch(EmptyHeapException e)
		{
			// Exception expected, allow flow to continue
		}

		// Put in 15 random values and pull off a few, ensuring they come in the correct order
		Random r = new Random();
		for(int i = 0; i < 15; i++)
		{
			myHeap.add((Integer)r.nextInt(100));
		}

		// Right size?
		if(myHeap.size() != 15)
		{
			System.out.println("FAILED: Not the correct size after inserting random values");
			return;
		}

		// Do they come off in the correct order?
		Integer last = new Integer(-1);
		for(int i = 0; i < 7; i++)
		{
			Integer curr = (Integer)myHeap.removeRoot();
			if(last > curr)
			{
				System.out.println("FAILED: Removed " + curr + " after " + last
						+ ", which is clearly bigger.");
				return;
			}
			last = curr;
		}

		// 15 - 7 values should be 8 left
		if(myHeap.size() != 8)
		{
			System.out.println("FAILED: Not the correct size after removing");
			return;
		}

		// What happens if we put in the wrong type of thing?
		try
		{
			myHeap.add(new Double(5.5));
			System.out.println("FAILED: Allowed an invalid object to be added");
			return;
		}
		catch(InvalidObjectException e)
		{
			// Exception expected, allow flow to continue
		}

		// Stick in enough to force an expansion
		for(int i = 0; i < 50; i++)
		{
			myHeap.add((Integer)r.nextInt(50));
		}

		// Remove until empty, ensuring order
		while(myHeap.isEmpty())
		{
			Integer curr = (Integer)myHeap.removeRoot();
			if(last > curr)
			{
				System.out.println("FAILED: Removed " + curr + " after " + last
						+ ", which is clearly bigger.");
				return;
			}
			last = curr;
		}

		// Yeah!
		System.out.println("PASSED: All checks passed");
	}
}