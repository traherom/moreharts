package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.CodeItem;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;
import java.util.Vector;

public class Program extends ParseObject
{
    private Vector<Declaration> decls = new Vector<Declaration>();

	private Program(Vector<Declaration> decls)
	{
		this.decls = decls;
	}

	public static Program parse(Scanner sc) throws ScannerException, ParserException
	{
		Vector<Declaration> decls = new Vector<Declaration>();

		// Basically just keep parsing decls until we reach EOF
		while(sc.viewNextToken().getType() != Token.Type.EOF)
		{
			decls.add(Declaration.parse(sc));
		}

		return new Program(decls);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append("Program\n");

		// Add each of our decls
		for(Declaration decl : decls)
			sb.append(decl.toString(tab + "  "));

		return sb.toString();
	}

	public CodeItem genLLCode(SymbolTable globals) throws CompilerException
	{
		// Ensure we actually have declarations to work with
		if(decls.isEmpty())
			throw new CompilerException("Program is empty, nothing to do");

		// Add the putchar() function, just so it's ready if needed
		globals.insert("putchar");

		// Get first itemCodeItem firstItem = null;
		CodeItem firstItem = decls.get(0).genLLCode(globals);

		// Go through each declation and add to symbol table, plus genCode any functions
		CodeItem currItem = firstItem;
		for(int i = 1; i < decls.size(); i++)
		{
			CodeItem temp = decls.get(i).genLLCode(globals);
			currItem.setNextItem(temp);
			currItem = temp;
		}

		return firstItem;
	}
}
