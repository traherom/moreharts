package numeric;

public class LargestElement
{
	public static int findLargest(int [] n)
	{
		if(n.length > 2)
		{
			// Split array in two
			// Note that we add the extra element (if this is an odd sized array)
			// to the second half
			int [] firstHalf = new int[n.length / 2];
			int [] secondHalf = new int[(int)(n.length / 2.0 + 0.5)];
			System.arraycopy(n, 0, firstHalf, 0, n.length / 2);
			System.arraycopy(n, n.length / 2, secondHalf, 0, (int)(n.length / 2.0 + 0.5));

			// Max of each
			int firstMax = findLargest(firstHalf);
			int secondMax = findLargest(secondHalf);

			// Which is bigger?
			if(firstMax > secondMax)
				return firstMax;
			else
				return secondMax;
		}
		else if(n.length == 2)
		{
			if(n[0] > n[1])
				return n[0];
			else
				return n[1];
		}
		else
		{
			return n[0];
		}
	}

	public static void main(String [] args)
	{
		// Check command line
		if(args.length < 1)
		{
			System.out.println("Usage: LargestElement <num 1> [num 2] [num 3] [...]");
			return;
		}
		
		// Covert command line to ints
		int [] nums = new int[args.length];
		for(int i = 0; i < args.length; i++)
		{
			nums[i] = Integer.parseInt(args[i]);
		}
	
		// And find largest
		System.out.println("Largest: " + findLargest(nums));
	}
}

