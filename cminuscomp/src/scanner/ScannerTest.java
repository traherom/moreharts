package scanner;

public class ScannerTest
{
	public static void main(String[] args)
	{
		try
		{
			// Now display all tokens in the file until EOF and end
			Scanner sc = new CMinusScanner("scannerTest.c");
			Token currToken = sc.getNextToken();
			while(currToken.getType() != Token.Type.EOF)
			{
				System.out.print(currToken);
				currToken = sc.getNextToken();
			}

			// And show the EOF token
			System.out.print(currToken);
		}
		catch(ScannerException ex)
		{
			System.out.println("Scanner error: " + ex.getMessage());
		}
		catch(Exception ex)
		{
			System.out.println("Unknown Error: " + ex.getMessage());
		}
	}
}
