package compiler;

import java.io.IOException;
import parser.CMinusParser;
import parser.Parser;
import parser.ParserException;
import parser.Program;
import scanner.ScannerException;

public class CompilerTest
{
	public static void main(String[] args)
	{
		try
		{
			// User specified file to compile?
			String progFile = "compilerTest.c";
			if(args.length == 1)
				progFile = args[0];
			
			Parser myParser = new CMinusParser(progFile);
			Program parseTree = myParser.parse();
			System.out.println(parseTree);

			// Compile
			CMinusCompiler compiler = new CMinusCompiler(myParser);
			compiler.compile(progFile);

			// Success
			System.out.println("No errors");
		}
		catch(CompilerException ex)
		{
			System.out.println("Compiler error: " + ex.getMessage());
		}
		catch(ParserException ex)
		{
			System.out.println("Parse error: " + ex.getMessage());
		}
		catch(ScannerException ex)
		{
			System.out.println("Scanner error: " + ex.getMessage());
		}
		catch(IOException ex)
		{
			System.out.println("I/O error: " + ex.getMessage());
		}
	}
}
