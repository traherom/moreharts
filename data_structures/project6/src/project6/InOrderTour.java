package project6;

/**
 * Displays a tree in  in-order fashion
 * @author Ryan
 */
public class InOrderTour extends EulerTour
{
	public InOrderTour(BinaryTree newTree)
	{
		super(newTree);
	}
	
	public Object execute()
	{
		return performTour(tree.root());
	}
	
    /**
	 * @param pos is the node being visited
	 * @param result is a storage mechanism for results computed as this node
	 */
    protected void visitInorder(Position pos, TraversalResult result)
    {
		System.out.print(pos.element() + " ");
    }
	
    /**
	 * @param pos is the node being visited
	 * @param result is a storage mechanism for results computed as this node
	 */
    protected void visitExternal(Position pos, TraversalResult result)
    {
		visitInorder(pos, result);
    }
}
