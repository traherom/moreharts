package numeric;

class NumberInfo
{
	/**
	* Returns true if n is a multiple of m.
	*
	* @param n Number being checked as a multiple
	* @param m Smaller number
	*/
	public static boolean isMultiple(long n, long m)
	{
		return (n % m) == 0;
	}

	/**
	* Checks if the given number is odd using only addition.
	*
	* @param i Number to be checked 
	*/
	public static boolean isOdd(int i)
	{
		int count;
		for(count = 1; count < i; count += 2)
		{
			// Empty
		}

		// If count added to exactly i, it must be odd
		// because we started at 1
		return count == i;
	}

	/**
	* Returns the sum of all integers less than the given number.
	*
	* @param i Number to add to
	*/
	public static int summationTo(int i)
	{
		int total = 0;

		// I originally had this as for( ; i > 0; --i), but
		// still somehow included the first i... does Java
		// ignore the prefix/postfix when it comes to the for loop?
		for(i--; i > 0; i--)
		{
			total += i;
		}

		return total;
	}

	/**
	* Simple testing framework.
	*
	* @param args Command line arguments
	*/
	public static void main(String [] args)
	{
		if(args.length == 0 || args.length > 2)
		{
			System.out.println("Usage: NumberInfo <num 1> [num 2]");
			return;
		}

		// Pull off the first number and display the things we can
		int num1 = Integer.parseInt(args[0]);
		System.out.println(num1 + " is odd? " + isOdd(num1));
		System.out.println(num1 + " summation to = " + summationTo(num1));

		if(args.length == 2)
		{
			int num2 = Integer.parseInt(args[1]);
			System.out.println(num1 + " is multiple of " + num2 + "? " + isMultiple(num1, num2));
		}
	}
}
 
