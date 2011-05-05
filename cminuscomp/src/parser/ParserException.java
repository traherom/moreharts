package parser;

import scanner.Token;

public class ParserException extends Exception
{
	private Token token = null;
	private String expected = null;

	/**
	 * Creates a new instance of <code>ParserException</code> without detail message.
	 */
	private ParserException()
	{
	}

	/**
	 * Constructs an instance of <code>ParserException</code> with the specified detail message.
	 * @param msg the detail message.
	 */
	private ParserException(String msg)
	{
		super(msg, null);
	}

	/**
	 * Constructs an instance of <code>ParserException</code> with the specified detail message.
	 * @param token Token actually found
	 * @param expected Description of what was expected
	 */
	public ParserException(Token token, String expected)
	{
		this.token = token;
		this.expected = expected;
	}

	/**
	 * Use normal getMessage() if we don't have the default set up
	 */
	@Override
	public String getMessage()
	{
		if(expected != null)
		{
			StringBuilder sb = new StringBuilder();
			sb.append("Error on line ");
			sb.append(token.getLine());
			//sb.append(", character ");
			//sb.append(token.getCharNum());
			sb.append(": Expected ");
			sb.append(expected);
			sb.append(", found ");

			if(token.getData() != null)
				sb.append("'" + token.getData() + "'");
			else
				sb.append(token.getType());

			sb.append("\n\tContext: ");
			sb.append(token.getContext());
			return sb.toString();
		}
		else
		{
			return super.getMessage();
		}
	}
}
