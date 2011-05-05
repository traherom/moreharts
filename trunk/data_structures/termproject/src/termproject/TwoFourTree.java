package termproject;

import java.util.Random;

/**
 * Implementation for an (2, 4) tree
 * @author Caleb Mays and Ryan Morehart
 */
public class TwoFourTree implements Dictionary
{
	private Comparator treeComp;
	private int size = 0;
	private TFNode treeRoot = null;

	/**
	 * Constructor for a TwoFourTree. The Comparator given should match the
	 * types of objects given.
	 * @param comp Class implementing the Comparator interface.
	 */
	public TwoFourTree(Comparator comp)
	{
		treeComp = comp;
	}

	/**
	 * Gives the number of nodes in the tree.
	 * @return count of the nodes in tree
	 */
	public int size()
	{
		return size;
	}

	/**
	 * True if the tree has no elements
	 * @return boolean indicatining if the tree is empty.
	 */
	public boolean isEmpty()
	{
		return (size == 0);
	}

	/**
	 * Begins the series of printTree() calls.
	 */
	public void printAllElements()
	{
		int indent = 0;
		if(root() == null)
		{
			System.out.println("The tree is empty");
		}
		else
		{
			printTree(root(), indent);
		}
	}
	
	/**
	 * Displays the tree in a tab-formatted text-based output, starting at the given
	 * node and indent level.
	 * @param start node to begin at
	 * @param indent indentation level to begin at
	 */
	public void printTree(TFNode start, int indent)
	{
		if(start == null)
		{
			return;
		}
		for(int i = 0; i < indent; i++)
		{
			System.out.print(" ");
		}
		printTFNode(start);
		indent += 4;
		int numChildren = start.numItem() + 1;
		for(int i = 0; i < numChildren; i++)
		{
			printTree(start.getChild(i), indent);
		}
	}

	/**
	 * Displays the items in a node in a line
	 * @param node node to print items of
	 */
	public void printTFNode(TFNode node)
	{
		int numItems = node.numItem();
		for(int i = 0; i < numItems; i++)
		{
			System.out.print(((Item)node.getItem(i)).element() + " ");
		}
		System.out.println();
	}

	/**
	 * Checks if tree is properly hooked up, i.e., children point to parents
	 * @param start node to begin the checking at. Give it the tree root if you wish
	 *		to check the whole thing.
	 */
	public void checkTree(TFNode start)
	{
		if(start == null)
		{
			return;
		}

		if(start.getParent() != null)
		{
			TFNode parent = start.getParent();
			int childIndex = 0;
			for(childIndex = 0; childIndex <= parent.numItem(); childIndex++)
			{
				if(parent.getChild(childIndex) == start)
				{
					break;
				}
			}
			// if child wasn't found, print problem
			if(childIndex > parent.numItem())
			{
				System.out.println("Child to parent confusion");
				printTFNode(start);
			}
		}

		if(start.getChild(0) != null)
		{//this is perhaps the coolest thing ever!!
			for(int childIndex = 0; childIndex <= start.numItem(); childIndex++)
			{
				if(start.getChild(childIndex).getParent() != start)
				{
					System.out.println("Parent to child confusion");
					printTFNode(start);
				}
			}
		}

		int numChildren = start.numItem() + 1;
		for(int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			checkTree(start.getChild(childIndex));
		}
	}

	/**
	 * Inserts an element into the tree
	 * @param key key to insert based on
	 * @param ele element to associate with that key
	 */
	public void insertElement(Object key, Object ele)
	{
		// Key works with comparator, right?
		if(!treeComp.isComparable(key))
		{
			throw new TwoFourTreeException("Comparator incompatible with key type given");
		}
		
		// Create the key->element pair
		Item newItem = new Item(key, ele);

		// If this is a fresh tree, make the first node
		if(treeRoot == null)
		{
			treeRoot = new TFNode();
		}

		// Stick node where it should initially go. This might screw up the tree, but
		// we'll correct that next.
		Position pos = findInsertPoint(key);
		TFNode insertLoc = pos.getNode();
		int index = pos.getIndex();
		insertLoc.insertItem(index, newItem);

		// Did we screw up the node we went into?
		while(insertLoc != null && insertLoc.numItem() == 4)
		{
			if(insertLoc.getParent() == null)
			{
				// We're the root, so break ourselves up
				splitNode(insertLoc);
			}
			else
			{
				// We have someone above, move data into them and break up
				moveElUp(insertLoc);
			}

			// Move up to parent so we can check them
			insertLoc = insertLoc.getParent();
		}

		// Element has been added!
		size++;
	}

	/**
	 * Makes a new node, breaking apart the node the elment in the new node came from
	 * and attaching to the new node. Used when the root overflows.
	 * @param insertLoc node to split (should always be root, but for the sake of flexibility
	 *		this can be specified).
	 */
	protected void splitNode(TFNode insertLoc)
	{
		// Take third element, make parent of others
		TFNode newParent = new TFNode();
		Object newParentItem = insertLoc.getItem(2);
		newParent.addItem(0, newParentItem);

		// Create new right child using the 4th item
		TFNode newRight = new TFNode();
		Object newRightItem = insertLoc.getItem(3);
		newRight.addItem(0, newRightItem);

		// Reattach children if needed
		if(insertLoc.getChild(3) != null)
		{
			newRight.setChild(0, insertLoc.getChild(3));
			newRight.getChild(0).setParent(newRight);
		}
		if(insertLoc.getChild(4) != null)
		{
			newRight.setChild(1, insertLoc.getChild(4));
			newRight.getChild(1).setParent(newRight);
		}

		// Remove elements from left child (what insertLoc is becoming)
		insertLoc.removeItem(3);
		insertLoc.removeItem(2);

		// Hook up new nodes
		treeRoot = newParent;
		newParent.setChild(0, insertLoc);
		newParent.setChild(1, newRight);
		insertLoc.setParent(newParent);
		newRight.setParent(newParent);
	}

	/**
	 * Moves an element up from the given node, breaks apart the node into two,
	 * then attaches those nodes to the parent
	 * @param insertLoc node to move an element out of and break apart
	 */
	protected void moveElUp(TFNode insertLoc)
	{
		// We have a parent, stick third element into it
		TFNode parent = insertLoc.getParent();
		Item thirdEl = (Item)insertLoc.getItem(2);
		int thirdElNewPlace = findFirstGreaterThanOrEqualTo(thirdEl.key(), parent);
		parent.insertItem(thirdElNewPlace, thirdEl);

		// Create new child for the right of the moved element using the 4th item
		TFNode newRight = new TFNode();
		Object newRightItem = insertLoc.getItem(3);
		newRight.addItem(0, newRightItem);
		newRight.setParent(parent);

		// Reattach children if needed
		if(insertLoc.getChild(3) != null)
		{
			newRight.setChild(0, insertLoc.getChild(3));
			newRight.getChild(0).setParent(newRight);
		}
		if(insertLoc.getChild(4) != null)
		{
			newRight.setChild(1, insertLoc.getChild(4));
			newRight.getChild(1).setParent(newRight);
		}

		// Remove the 3rd and 4th elements from the original node
		insertLoc.removeItem(3);
		insertLoc.removeItem(2);

		// Insert new right element to the right of moved element
		parent.setChild(thirdElNewPlace + 1, newRight);
	}

	/**
	 * Removes the first element with the given key in the tree.
	 * @param key element key to remove
	 * @return the element found attached to that key
	 */
	public Object removeElement(Object key)
	{
		// Key works with comparator, right?
		if(!treeComp.isComparable(key))
		{
			throw new TwoFourTreeException("Comparator incompatible with key type given");
		}
		
		// Where is this key?
		Position pos = findElement(key);
		TFNode removePos = pos.getNode();
		int index = pos.getIndex();

		// Save element so we can return it
		Object element = ((Item)removePos.getItem(index)).element();
		
		// Move element we want to move to bottom of tree, then delete it
		TFNode currNode = findInOrderSuccessor(pos);
		removePos.replaceItem(index, currNode.getItem(0));
		currNode.removeItem(0);

		// Is the node now underflowing (no items) and hence in need of fixing?
		while(currNode.numItem() == 0)
		{
			// If the underflow is in the root, just delete the root and make the
			// tree root the only child the root should have
			if(currNode == treeRoot)
			{
				treeRoot = currNode.getChild(0);
				if(treeRoot != null)
				{
					treeRoot.setParent(null);
				}
				break;
			}

			// We'll need to work with our parent to fix this mess
			TFNode parent = currNode.getParent();
			int indexInParent = indexInParent(currNode);

			// How should we handle this?
			if(indexInParent != 0 && parent.getChild(indexInParent - 1).numItem() >= 2)
			{
				// Rotate from left sibling
				TFNode leftSib = parent.getChild(indexInParent - 1);
				Item temp = (Item)parent.getItem(indexInParent - 1);
				parent.replaceItem(indexInParent - 1, leftSib.getItem(leftSib.numItem() - 1));

				// Move right most child from left sib to be our left most child
				currNode.insertItem(0, temp);
				if(leftSib.getChild(leftSib.numItem()) != null)
				{
					currNode.setChild(0, leftSib.getChild(leftSib.numItem()));
					currNode.getChild(0).setParent(currNode);
				}

				leftSib.removeItem(leftSib.numItem() - 1);
			}
			else if(indexInParent != parent.numItem() && parent.getChild(indexInParent + 1).numItem() >= 2)
			{
				// Rotate from right sibling
				TFNode rightSib = parent.getChild(indexInParent + 1);
				Item temp = (Item)parent.getItem(indexInParent);
				parent.replaceItem(indexInParent, rightSib.getItem(0));

				// Move left most child from right sib to be our right most child
				currNode.insertItem(0, temp);
				if(rightSib.getChild(0) != null)
				{
					currNode.setChild(currNode.numItem(), rightSib.getChild(0));
					currNode.getChild(currNode.numItem()).setParent(currNode);
				}

				rightSib.removeItem(0);
			}
			else if(indexInParent != 0)
			{
				// Do left fusion
				TFNode leftSib = parent.getChild(indexInParent - 1);
				leftSib.insertItem(leftSib.numItem(), parent.getItem(indexInParent - 1));
				if(currNode.getChild(0) != null)
				{
					leftSib.setChild(leftSib.numItem(), currNode.getChild(0));
					currNode.getChild(0).setParent(leftSib);
				}
				parent.removeItem(indexInParent - 1);

				// Move currNode so we can examine the parent and check for an underflow
				currNode = parent;
			}
			else
			{
				// Do right fusion
				TFNode rightSib = parent.getChild(indexInParent + 1);
				rightSib.insertItem(0, parent.getItem(0));
				parent.setChild(0, rightSib);
				if(currNode.getChild(0) != null)
				{
					rightSib.setChild(0, currNode.getChild(0));
					currNode.getChild(0).setParent(rightSib);
				}
				parent.removeItem(0);

				// Move currNode so we can examine the parent and check for an underflow
				currNode = parent;
			}
		}

		// One less node in the tree!
		size--;
		return element;
	}

	/**
	 * Finds the index of a given node in its parent
	 * @param child node to find in parent
	 * @return index of child in parent
	 */
	protected int indexInParent(TFNode child) throws ElementNotFoundException
	{
		TFNode parent = child.getParent();
		
		// Where is the child in the parent?
		for(int i = 0; i <= parent.numItem(); i++)
		{
			if(parent.getChild(i) == child)
			{
				return i;
			}
		}

		// If this happens you've got something seriously wrong
		throw new ElementNotFoundException("The child specified was not in the parent");
	}

	/**
	 * Finds the first element that matches the given one. Difference from findInsertPoint
	 * is that this looks for an exact match and stops at the first one.
	 * @param key key to find
	 * @return Position pointing to the found element
	 */
	public Position findElement(Object key) throws ElementNotFoundException
	{
		// Object works with comparator, right?
		if(!treeComp.isComparable(key))
		{
			throw new TwoFourTreeException("Comparator incompatible with key type given");
		}
		
		// Start search at top
		TFNode currNode = treeRoot;
		int index = 0;
		
		while(true)
		{
			// Look for where we are/the child we would be in
			index = findFirstGreaterThanOrEqualTo(key, currNode);

			if(index != currNode.numItem() && ((Item)currNode.getItem(index)).key().equals(key))
			{
				// Found a match in this node
				return new Position(currNode, index);
			}
			else if(currNode.getChild(index) != null)
			{
				// No match, but a child to look at. Switch currNode to there
				currNode = currNode.getChild(index);
			}
			else
			{
				// No match found here and no children to follow
				throw new ElementNotFoundException("Element not found");
			}
		}
	}

	/**
	 * Finds the key/element pair that is next biggest in the tree
	 * @param pos Node/index to start looking from
	 * @return Node where the in-order successor is. The successor is always the first
	 *		element in that node
	 */
	protected TFNode findInOrderSuccessor(Position pos)
	{
		// Right
		TFNode currPos = pos.getNode().getChild(pos.getIndex() + 1);
		if(currPos == null)
		{
			// We're already at the bottom...
			return pos.getNode();
		}

		// Then all teh way left
		while(currPos.getChild(0) != null)
		{
			currPos = currPos.getChild(0);
		}

		return currPos;
	}

	/**
	 * Locates where a given key should go in the tree
	 * @param key key to be insert
	 * @return
	 */
	protected Position findInsertPoint(Object key)
	{
		// Object works with comparator, right?
		if(!treeComp.isComparable(key))
		{
			throw new TwoFourTreeException("Comparator incompatible with key type given");
		}
		
		//so we want to search for which child the elements needs to fall into
		//if the Node is full, we need to have the index to follow the child
		//and insert into the child node.
		TFNode currNode = treeRoot;
		while(true)
		{
			int i = findFirstGreaterThanOrEqualTo(key, currNode);
			if(currNode.getChild(i) == null)
			{
				// Found the base of the tree
				Position pos = new Position(currNode, i);
				return pos;
			}
			else
			{
				// Move down to child
				currNode = currNode.getChild(i);
			}
		}
	}

	/**
	 * Looks through a given node and finds the first item index in that node
	 *		where the given key is equal to or just less than it.
	 * @param key key to look for in node
	 * @param node node to look in
	 * @return index of where key belongs
	 */
	protected int findFirstGreaterThanOrEqualTo(Object key, TFNode node)
	{
		// Object works with comparator, right?
		if(!treeComp.isComparable(key))
		{
			throw new TwoFourTreeException("Comparator incompatible with key type given");
		}
		
		// Find index of first place greater
		for(int i = 0; i < node.numItem(); i++)
		{
			Item c = (Item)node.getItem(i);
			if(treeComp.isLessThanOrEqualTo(key, c.key()))
			{
				return i;
			}
		}

		// None bigger, go last
		return node.numItem();
	}

	/**
	 * Returns the tree root node
	 * @return tree root node
	 */
	public TFNode root()
	{
		return treeRoot;
	}

	/**
	 * Code performing various tests on the functionallity of the tree
	 * @param args One arguement here will give the size for the random test case. Two or more
	 *		specify the exact test case to use. No arguements uses a default size for the test
	 *		case.
	 */
	public static void main(String[] args)
	{
		// Check parameters and create test case
		Random gen = new Random();
		int[] test;
		
		if(args.length <= 1)
		{
			// Random test case, with the size maybe specified
			int size = 20;
			if(args.length == 1)
			{
				size = Integer.parseInt(args[0]);
			}
			
			// Don't try to do something dumb
			if(size < 1)
			{
				System.out.println("Test case size must be at least one.");
				return;
			}
			
			// Create test
			test = new int[size];
			for(int i = 0; i < test.length; i++)
			{
				test[i] = gen.nextInt(100);
			}
		}
		else
		{
			// Tree values given on command line
			test = new int[args.length];
			for(int i = 0; i < test.length; i++)
			{
				test[i] = Integer.parseInt(args[i]);
			}
		}
		
		try
		{
			// Create tree
			Comparator myComp = new IntegerComparator();
			TwoFourTree myTree = new TwoFourTree(myComp);

			// We're empty, rigth?
			if(!myTree.isEmpty() || myTree.size() != 0)
			{
				System.out.println("Tree not empty at initial creation as expected");
				return;
			}

			// Insert them all
			System.out.println("Inserting:");
			for(int i = 0; i < test.length; i++)
			{
				System.out.println("  " + test[i]);
				myTree.insertElement(test[i], test[i]);
			}

			// Pointers all good?
			myTree.checkTree(myTree.root());

			// Are we sized correctly?
			if(myTree.size() != test.length)
			{
				System.out.println("Tree does not contain " + test.length + " nodes as expected");
				return;
			}

			// Display result
			System.out.println("\nTree after insertions:");
			myTree.printAllElements();

			// Remove them all, starting at a random point and looping around
			System.out.println("\nRemoving:");
			int end = gen.nextInt(test.length);
			for(int i = (end + 1) % test.length; i != end; i = (i + 1) % test.length)
			{
				System.out.println("  " + test[i] + ": " + myTree.removeElement(test[i]));
			}
			System.out.println("  " + test[end] + ": " + myTree.removeElement(test[end]));

			// Pointers all good?
			myTree.checkTree(myTree.root());

			// Are we sized correctly?
			if(!myTree.isEmpty())
			{
				System.out.println("Tree not empty as expected");
				return;
			}

			// Show (hopefully) empty tree
			System.out.println("\nTree after removals (should be empty):");
			myTree.printAllElements();

			// All done!
			System.out.println("\nTesting complete");
		}
		catch(Exception e)
		{
			System.out.println("Exception caught: " + e.getMessage());
			System.out.println("Stack trace:");
			e.printStackTrace(System.out);
		}
	}
}
