package numeric;

/**
* Basic class which contains functions related to finding
* greatest common denominators.
* 
* @author Ryan Morehart
* @version 1.0
*/
public class GCD
{
	/** 
	* Computes the GDC of the given numbers using the Euclidean
	* algorithm. Psuedocode for the algorithm stolen from Wikipedia.
	*
	* @param num1 First of the numbers to find the GCD of
	* @param num2 Second of the numbers to find the GCD of
	* @return The greatest common denominator of the given numbers
	*/
	public static int compute(int num1, int num2)
	{
		int temp;
		
		while(num2 != 0)
		{
			temp = num2;
			num2 = num1 % num2;
			num1 = temp;
		}
		
		return num1;
	}

	public static void main(String[] args)
	{
		if(args.length != 2)
		{
			System.out.println("Usage: java GCD <num1> <num2>");
			return;
		}

		// Calculate and display GCD
		int num1 = Integer.parseInt(args[0]);
		int num2 = Integer.parseInt(args[1]);
		int gcd = GCD.compute(num1, num2);

		System.out.println(gcd);
	}
}

