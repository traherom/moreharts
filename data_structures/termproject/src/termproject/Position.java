package termproject;

/**
 * Simple class to hold positions in the tree, including both the node and index in that node
 * @author Caleb Mays and Ryan Morehart	
 */
public class Position
{
	private TFNode node;
	private int index;

	Position()
	{
		node = null;
		index = -1;
	}

	Position(TFNode newNode, int newIndex)
	{
		node = newNode;
		index = newIndex;
	}

	public TFNode getNode()
	{
		return node;
	}

	public int getIndex()
	{
		return index;
	}

	public void setNode(TFNode newNode)
	{
		node = newNode;
	}

	public void setIndex(int newIndex)
	{
		index = newIndex;
	}
}
