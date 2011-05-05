package parser;

import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;

public class Operator extends ParseObject
{
	private Token.Type type;

	private Operator(Token.Type type)
	{
		this.type = type;
	}

	public Token.Type getType()
	{
		return type;
	}

	public static Operator parse(Scanner sc) throws ParserException, ScannerException
	{
		return new Operator(sc.getNextToken().getType());
	}

	@Override
	public String toString(String tab)
	{
		return Token.toString(type);
	}
}
