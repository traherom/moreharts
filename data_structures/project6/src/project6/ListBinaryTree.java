package project6;

public class ListBinaryTree implements BinaryTree
{
	STNode treeRoot;
	int size;

	public void fillTree()
	{
		treeRoot = new STNode(new Integer(0), null, null, null);
		STNode node1 = new STNode(new Integer(1), treeRoot, null, null);
		STNode node2 = new STNode(new Integer(2), treeRoot, null, null);
		treeRoot.setLeftChild(node1);
		node1.setSibling(node2);

		STNode node3 = new STNode(new Integer(3), node1, null, null);
		STNode node4 = new STNode(new Integer(4), node1, null, null);
		node1.setLeftChild(node3);
		node3.setSibling(node4);

		STNode node5 = new STNode(new Integer(5), node2, null, null);
		STNode node6 = new STNode(new Integer(6), node2, null, null);
		node2.setLeftChild(node5);
		node5.setSibling(node6);

		STNode node7 = new STNode(new Integer(7), node3, null, null);
		STNode node8 = new STNode(new Integer(8), node3, null, null);
		node3.setLeftChild(node7);
		node7.setSibling(node8);

		STNode node9 = new STNode(new Integer(9), node4, null, null);
		STNode node10 = new STNode(new Integer(10), node4, null, null);
		node4.setLeftChild(node9);
		node9.setSibling(node10);

		size = 11;
	}

	public ListBinaryTree()
	{
		size = 0;
		treeRoot = null;
	}

	public ListBinaryTree(Object initialNode)
	{
		treeRoot = new STNode(initialNode, null, null, null);
		size = 1;
	}

	public Position root() throws EmptyTreeException
	{
		if(treeRoot == null)
		{
			throw new EmptyTreeException();
		}

		return (Position)treeRoot;
	}

	public Position leftChild(Position pos) throws InvalidPositionException
	{
		if(!isInTree(pos))
		{
			throw new InvalidPositionException("Position not in tree");
		}

		STNode s = (STNode)pos;
		return (Position)s.getLeftChild();
	}

	public Position rightChild(Position pos) throws InvalidPositionException
	{
		if(!isInTree(pos))
		{
			throw new InvalidPositionException("Position not in tree");
		}

		STNode s = (STNode)pos;
		return (Position)s.getLeftChild().getSibling();
	}

	public Position sibling(Position pos) throws InvalidPositionException
	{
		if(!isInTree(pos))
		{
			throw new InvalidPositionException("Position not in tree");
		}

		STNode s = (STNode)pos;

		// Left or right child?
		if(s.getSibling() == null)
		{
			// Left child, easy to do
			return (Position)s.getSibling();
		}
		else
		{
			// Right hand child, go up and down to get our sibling
			return (Position)s.getParent().getLeftChild();
		}
	}

	public Position parent(Position pos) throws InvalidPositionException
	{
		if(!isInTree(pos))
		{
			throw new InvalidPositionException("Position not in tree");
		}

		STNode s = (STNode)pos;
		return (Position)s.getParent();
	}

	public boolean isInternal(Position pos) throws InvalidPositionException
	{
		return !isExternal(pos);
	}

	public boolean isExternal(Position pos) throws InvalidPositionException
	{
		if(!isInTree(pos))
		{
			throw new InvalidPositionException("Position not in tree");
		}

		// Must be a leaf, so make sure this node either has no children
		// Note that because of the way the nodes are connected there can never be 
		// a right child if there isn't a left
		STNode s = (STNode)pos;
		return (s.getLeftChild() == null);
	}

	public boolean isRoot(Position pos) throws InvalidPositionException
	{
		if(!isInTree(pos))
		{
			throw new InvalidPositionException("Position not in tree");
		}

		return (STNode)pos == treeRoot;
	}

	public int size()
	{
		return size;
	}

	public boolean isEmpty()
	{
		return size == 0;
	}

	/**
	 * Checks if the given position is in this tree or a different one.
	 * This would be much more efficient if each node knew who the root is, but
	 * for the sake of not changing that class I'll do it this way.
	 * @param pos Position in tree
	 * @return true if this node is part of tree
	 */
	private boolean isInTree(Position pos) throws InvalidPositionException
	{
		if(!(pos instanceof STNode))
		{
			throw new InvalidPositionException("Position given not an STNode");
		}

		// If we find our root node as an ancestor of the given position, then we know
		// the position is in the tree
		STNode curr = (STNode)pos;
		while(curr != null)
		{
			curr = curr.getParent();
			if(curr != treeRoot)
			{
				return true;
			}
		}

		// If we didn't hit the root 
		return false;
	}

	public static void main(String[] args)
	{		
		// Tree for touring/testing
		ListBinaryTree myTree = new ListBinaryTree();
		if(!myTree.isEmpty())
		{
			System.out.println("Not empty as expected.");
			return;
		}
		
		// Fill up with eleven elements
		// Note following testing just assumes fillTree() will never change
		myTree.fillTree();
		if(myTree.size() != 11)
		{
			System.out.println("Size not correct.");
			return;
		}
		
		// Look at a node, see if it's working
		Position p = myTree.root();
		if((Integer)p.element() != 0)
		{
			System.out.println("Root is not 0 as expected.");
		}

		// And try out the 3 tours
		System.out.print("In order tour:   ");
		InOrderTour inT = new InOrderTour(myTree);
		inT.execute();
		
		System.out.print("\nPre order tour:  ");
		PreOrderTour preT = new PreOrderTour(myTree);
		preT.execute();
		
		System.out.print("\nPost order tour: ");
		PostOrderTour postT = new PostOrderTour(myTree);
		postT.execute();
	}
}