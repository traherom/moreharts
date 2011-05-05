package parser;

import scanner.Scanner;
import scanner.ScannerException;
import java.io.IOException;
import scanner.CMinusScanner;

public class CMinusParser implements Parser
{
    private Scanner sc;
	private Program parseTree;

	public CMinusParser(String fileName) throws ScannerException
	{
		sc = new CMinusScanner(fileName);
	}

    public CMinusParser(Scanner scanner)
	{
        sc = scanner;
    }

    public Program parse() throws ParserException, ScannerException, IOException
	{
		// Only parse once
		if(parseTree == null)
			parseTree = Program.parse(sc);
		return parseTree;
    }
}
