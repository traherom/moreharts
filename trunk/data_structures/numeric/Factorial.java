package numeric;

/**
* Basic class which contains functions related to finding
* the factorial of a given number
* 
* @author Ryan Morehart
* @version 1.0
*/
public class Factorial
{
	/** 
	* Computes the factorial of the given number.
	*
	* @param num Number to find the factorial of
	* @return The factorial of the given number
	*/
	public static double compute(int num)
	{
		double total = num;
		for(int i = num - 1; i > 1; i--)
		{
			total *= i;
		}
		
		return total;
	}

	public static void main(String[] args)
	{
		if(args.length != 1)
		{
			System.out.println("Usage: java Factorial <num>");
			return;
		}

		// Calculate and display GCD
		int num = Integer.parseInt(args[0]);
		double factorial = Factorial.compute(num);

		System.out.println(factorial);
	}
}

