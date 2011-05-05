package project6;

/**
 * Displays a tree in  pre-order fashion
 * @author Ryan
 */
public class PreOrderTour extends EulerTour
{
	public PreOrderTour(BinaryTree newTree)
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
    protected void visitPreorder(Position pos, TraversalResult result)
    {
		System.out.print(pos.element() + " ");
    }
	
    /**
	 * @param pos is the node being visited
	 * @param result is a storage mechanism for results computed as this node
	 */
    protected void visitExternal(Position pos, TraversalResult result)
    {
		visitPreorder(pos, result);
    }
}
