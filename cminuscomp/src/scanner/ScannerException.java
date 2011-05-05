package scanner;

import scanner.Token;

public class ScannerException extends Exception
{
	private Token token;
	
	/**
	 * Creates a new instance of <code>ScannerException</code> without detail message.
	 */
	public ScannerException()
	{
	}

	/**
	 * Constructs an instance of <code>ScannerException</code> with the specified detail message.
	 * @param msg the detail message.
	 */
	public ScannerException(String msg)
	{
		super(msg);
	}

	/**
	 * Automatically creates the most common type of scanner error from the given token
	 * @param token Token that caused the problem
	 */
	public ScannerException(Token token)
	{
		this.token = token;
	}

	/**
	 * Either uses the normal string or, if a token was given, prints something from that
	 */
	@Override
	public String getMessage()
	{
		if(token != null)
		{
			StringBuilder sb = new StringBuilder();
			sb.append("Error on line ");
			sb.append(token.getLine());
			//sb.append(", character ");
			//sb.append(token.getCharNum());
			sb.append(": ");
			sb.append(token.getDescription());
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
