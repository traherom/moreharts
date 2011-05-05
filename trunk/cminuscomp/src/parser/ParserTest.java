package parser;

import scanner.ScannerException;

/**
 * Puts the parser through it's paces and prints the resulting tree
 */
public class ParserTest
{
	public static void main(String[] args)
	{
		try
		{
			// Run parser
			Parser ps = new CMinusParser("parserTest.c");
			Program prog = ps.parse();

			// Display
			System.out.println(prog);
		}
		catch(ScannerException ex)
		{
			System.out.println("Scanner error: " + ex.getMessage());
		}
		catch(ParserException ex)
		{
			System.out.println("Parser error: " + ex.getMessage());
		}
		catch(Exception ex)
		{
			System.out.println("Unknown Error: " + ex.getMessage());
			ex.printStackTrace(System.out);
		}
	}
}
