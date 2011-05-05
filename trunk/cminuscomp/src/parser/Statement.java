package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.Function;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;
import java.util.Vector;

public abstract class Statement extends ParseObject
{
	private Vector<Expression> exprs = new Vector<Expression>();

	public static Statement parse(Scanner sc) throws ScannerException, ParserException
	{
		Token nextToken = sc.viewNextToken();
		if(nextToken.getType() == Token.Type.RETURN)
		{
			// Return
			return ReturnStatement.parse(sc);
		}
		else if(nextToken.getType() == Token.Type.IF)
		{
			// If
			return SelectionStatement.parse(sc);
		}
		else if(nextToken.getType() == Token.Type.WHILE)
		{
			// While
			return IterationStatement.parse(sc);
		}
		else if(nextToken.getType() == Token.Type.LEFT_CURLY)
		{
			// Compound statement
			return CompoundStatement.parse(sc);
		}
		else
		{
			// Assume it's an expression, that'll kill us if it's not
			return ExpressionStatement.parse(sc);
		}
	}

	public abstract void genLLCode(Function func, SymbolTable vars) throws CompilerException;
}
