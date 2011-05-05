package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.Function;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;

public class ExpressionStatement extends Statement
{
	private Expression expr;

	private ExpressionStatement(Expression expr)
	{
		this.expr = expr;
	}

	public static ExpressionStatement parse(Scanner sc) throws ScannerException, ParserException
	{
		Expression expr = null;
		if(sc.viewNextToken().getType() != Token.Type.SEMICOLON)
		{
			expr = Expression.parse(sc);
		}

		match(sc, Token.Type.SEMICOLON);

		return new ExpressionStatement(expr);
	}

	@Override
	public String toString(String tab)
	{
		if(expr != null)
			return expr.toString(tab) + "\n";
		else
			return tab + ";\n";
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// We can just ignore empty expressions
		if(expr != null)
			expr.genLLCode(func, vars);
	}
}
