package numeric;

import java.util.*;
/**
* Contains a reverse polish notation calculator.
*
* @author Ryan Morehart
* @version 0.8
*/
public class RPNCalc
{
	/**
	* Main calculating function of a RPN string. Floating point and negative
	* numbers are supported. Currently supported operators are +, -, *, and /.
	* All others will result in an InvalidRPNString exception being thrown.
	* 
	* @param func Arithmetic string in Reverse Polish Notation.
	* @return Calculated value of the string.
	* @throws InvalidRPNString Thrown if the given string contains unrecognized operators,
	*			or an improper number of values are present given the operators 
	*			in the string. For example, "2 3 4 +" is improper as + expects
	*			only 2 numbers.
	*/
	public static double compute(String func) throws InvalidRPNString
	{
		// Stack to store numbers while we're looking for the operator
		Stack numbers = new Stack();

		// Scanner to parse function string
		Scanner sc = new Scanner(func);
		
		// Create the op varable out here so that it can be used in
		// the creation of the exception string
		String op = "";

		try
		{
			// Keep going until we run out of stuff to do
			while(sc.hasNext())
			{
				// Check what we have next
				if(sc.hasNextDouble())
				{
					// Plain old number, save to stack for use later
					Double num = sc.nextDouble();
					numbers.push((Object)num);
				}
				else
				{
					// An operator! Figure out which one it is and pull off the appropriate
					// number of values from the stack
					op = sc.next();
					
					if(op.equals("+"))
					{
						Double num1 = (Double)numbers.pop();
						Double num2 = (Double)numbers.pop();
						
						Double result = num1 + num2;
						numbers.push((Object)result);
					}
					else if(op.equals("-"))
					{
						Double num1 = (Double)numbers.pop();
						Double num2 = (Double)numbers.pop();
					
						Double result = num2 - num1;
						numbers.push((Object)result);
					}
					else if(op.equals("*"))
					{
						Double num1 = (Double)numbers.pop();
						Double num2 = (Double)numbers.pop();
						
						Double result = num1 * num2;
						numbers.push((Object)result);
					}
					else if(op.equals("/"))
					{
						Double num1 = (Double)numbers.pop();
						Double num2 = (Double)numbers.pop();
						
						Double result = num2 / num1;
						numbers.push((Object)result);
					}
					else
					{
						// Invalid opertor
						throw new InvalidRPNString("Error parsing '" + func + "': Invalid operator '" + op + "' in RPN string");
					}
				}
			}
		}
		catch(EmptyStackException e)
		{
			// If we were looking for more numbers and couldn't find them, the string must be poorly formed
			throw new InvalidRPNString("Error parsing '" + func + "': Not enough numbers for operator '" + op + "'");
		}

		// Check to see if there is more than 1 value left on the stack,
		// which indicates that the string was improperly formed
		if(numbers.size() > 1)
		{
			throw new InvalidRPNString("Error parsing '" + func + "': Too many numbers for the given operators");
		}

		// The top value of the stack is the result
		return (Double)numbers.pop();
	}

	/**
	* Main entry point for testing RPNCalc.
	*
	* @param args Command line arguements. If none are given RPNCalc is run through various
	*		test cases, otherwise the command line is executed as a single RPN string.
	*/
	public static void main(String[] args)
	{
		if(args.length == 0)
		{
			// Batch testing
			// Test case array, first column is arithmetic statement, second is expected value
			String[][] tests = {
				{ "3 5 -", "-2" }, // -
				{ "7 8 +", "15" }, // +
				{ "14 2 /", "7" }, // /
				{ "32 5 *", "160" }, // *
				{ "-10 -14 +", "-24" }, // Negative numbers
				{ "65.2", "65.2" }, // Single value
				{ "19 23 $", "1" }, // Invalid operator
				{ "9 +", "1000" }, // Not enough numbers
				{ "4 5 3 *", "1000" }, // Too many numbers
				{ "23.3 5 16.2 + 8 * -", "-146.3"} // Example from project description
				};

			// Run them all one by one
			for(int currCase = 0; currCase < tests.length; currCase++)
			{
				// Tell what case we are testing
				System.out.print("Testing " + tests[currCase][0] + ": ");
			
				try
				{
					// Run through calculator
					double result = compute(tests[currCase][0]);

					// Did we get the expected value?
					if(Math.abs(result - Double.parseDouble(tests[currCase][1])) <= 0.0000001)
					{
						System.out.println("success");
					}
					else
					{
						System.out.println("failed, gave " + result + ", should be " + tests[currCase][1]);
					}
				}
				catch(InvalidRPNString e)
				{
					// Tell what problem there was with that test case,
					// but allow it to continue with the next test
					System.out.println(e);
				}
			}
		}
		else
		{
			// Calculate based on command line
			// Combine command line into single string
			StringBuffer sb = new StringBuffer();
			for(int x = 0; x < (args.length - 1); x++)
			{
				sb.append(args[x]);
				sb.append(" ");
			}
			sb.append(args[args.length - 1]);
			
			// And run it
			try
			{
				System.out.println(compute(sb.toString()));
			}
			catch(InvalidRPNString e)
			{
				System.out.println(e);
			}
		}
	}
}

