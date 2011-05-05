package scanner;

import scanner.ScannerException;
import scanner.Token;

public interface Scanner
{
	public Token getNextToken() throws ScannerException;

	public Token viewNextToken();
}
