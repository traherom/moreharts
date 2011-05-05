package parser;

import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;

/**
 * All objects that are produced by the parser extend this, as it provides some basic
 * functionality and interface stuff.
 *
 * @author taherom
 */
public abstract class ParseObject
{
	/**
	 * Ensures the next token is the correct type, consuming it and throwing
	 * an exception it is isn't
	 * @param sc Scanner currently in use
	 * @param expected Token type it should be
	 * @throws ScannerException
	 * @throws ParserException
	 */
	protected static void match(Scanner sc, Token.Type expected)
			throws ScannerException, ParserException
	{
		Token t = sc.getNextToken();
		if(t.getType() != expected)
		{
			throw new ParserException(t, Token.toString(expected));
		}
	}

	/**
	 * Valid types for variables/returns
	 */
	public enum Type
	{
		INT,
		VOID,
		INT_ARRAY,
	}

	/**
	 * Outputs a representation of the current parse object. Calls
	 * the parse object's toString(String tab) method with a default
	 * of no tab.
	 * @return String representation of object
	 */
	@Override
	public String toString()
	{
		return toString("");
	}

	/**
	 * Outputs a representation of the current parse object.
	 * Must be overridden
	 * @param tab How far the object should indent itself
	 * @return String representation of object
	 */
	public abstract String toString(String tab);
}
