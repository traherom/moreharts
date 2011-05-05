package scanner;

public class Token
{
	/**
	 * All the valid types for a token
	 */
	public enum Type
	{
		// Status types
		UNKNOWN, ERROR, WARNING,
		// Reserved words/types. I WANT TO ADD STRINGS
		INT, VOID,
		IF, ELSE, RETURN, WHILE,
		NUMBER,
		// Separators
		SEMICOLON, COMMA, EOF,
		LEFT_PAREN, RIGHT_PAREN,
		LEFT_BRACKET, RIGHT_BRACKET,
		LEFT_CURLY, RIGHT_CURLY,
		// Math
		DIVIDE, MULTIPLY, PLUS, MINUS,
		INCREMENT, DECREMENT,
		// Comparison
		EQUAL, NOT_EQUAL,
		LESS_THAN, GREATER_THAN,
		LESS_THAN_EQUAL, GREATER_THAN_EQUAL,
		NOT,
		// Other
		COMMENT,
		ASSIGNMENT,
		IDENTIFIER
	};
	private Type tokenType;
	private Object tokenData;
	// Information useful for error messages
	// Mostly obvious, context is the symbol and the characters around it
	private int lineNum;
	private int charNum;
	private String context;
	private String description;

	public Token()
	{
		this(Type.UNKNOWN, null, 0, 0, null, null);
	}

	public Token(int lineNum)
	{
		this(Type.UNKNOWN, null, lineNum, 0, null, null);
	}

	public Token(int lineNum, int charNum)
	{
		this(Type.UNKNOWN, null, lineNum, charNum, null, null);
	}

	public Token(int lineNum, int charNum, String context, String description)
	{
		this(Type.UNKNOWN, null, lineNum, charNum, context, description);
	}

	public Token(Type type)
	{
		this(type, null, 0, 0, null, null);
	}

	public Token(Type type, Object data)
	{
		this(type, data, 0, 0, null, null);
	}

	public Token(Type type, Object data, int lineNum, int charNum, String context, String description)
	{
		this.lineNum = lineNum;
		this.charNum = charNum;
		this.context = context;
		this.description = description;
		this.tokenType = type;
		this.tokenData = data;
	}

	/**
	 *  Traslates a token type into a string for messages.
	 * @param t Token type to translate
	 * @return string representation of type
	 */
	public static String toString(Type t)
	{
		switch(t)
		{
			case SEMICOLON:
				return "';'";
			case COMMA:
				return "','";
			case LEFT_PAREN:
				return "'('";
			case RIGHT_PAREN:
				return "')'";
			case LEFT_BRACKET:
				return "'['";
			case RIGHT_BRACKET:
				return "']'";
			case LEFT_CURLY:
				return "'{'";
			case RIGHT_CURLY:
				return "'}'";
			case DIVIDE:
				return "'/'";
			case MULTIPLY:
				return "'*'";
			case PLUS:
				return "'+'";
			case MINUS:
				return "'-'";
			case INCREMENT:
				return "'++'";
			case DECREMENT:
				return "'--'";
			case EQUAL:
				return "'=='";
			case NOT_EQUAL:
				return "'!='";
			case LESS_THAN:
				return "'<'";
			case GREATER_THAN:
				return "'>'";
			case LESS_THAN_EQUAL:
				return "'<='";
			case GREATER_THAN_EQUAL:
				return "'>='";
			case NOT:
				return "'!'";
			case ASSIGNMENT:
				return "'='";
			default:
				return t.toString().toLowerCase();
		}
	}

	public Type getType()
	{
		return tokenType;
	}

	public void setType(Type t)
	{
		tokenType = t;
	}

	public Object getData()
	{
		return tokenData;
	}

	public void setData(Object d)
	{
		tokenData = d;
	}

	public int getLine()
	{
		return lineNum;
	}

	public void setLineNum(int lineNum)
	{
		this.lineNum = lineNum;
	}

	public int getCharNum()
	{
		return charNum;
	}

	public void setCharNum(int charNum)
	{
		this.charNum = charNum;
	}

	public String getContext()
	{
		return context;
	}

	public void setContext(String context)
	{
		this.context = context;
	}

	public String getDescription()
	{
		return description;
	}

	public void setDescription(String description)
	{
		this.description = description;
	}

	@Override
	public String toString()
	{
		StringBuilder sb = new StringBuilder();

		if(tokenType != Type.ERROR)
		{
			sb.append(tokenType);

			if(tokenData != null)
			{
				sb.append(" (");
				sb.append(tokenData);
				sb.append(")");
			}

			sb.append(": On line ");
			sb.append(lineNum);
			sb.append(", character ");
			sb.append(charNum);

			if(context != null)
			{
				sb.append(" Context: ");
				sb.append(context.trim());
			}

			sb.append('\n');
		}
		else
		{
			sb.append(tokenType);

			sb.append(" on line ");
			sb.append(lineNum);
			sb.append(", character ");
			sb.append(charNum);

			if(description != null)
			{
				sb.append(": ");
				sb.append(description.trim());
			}

			if(context != null)
			{
				sb.append("\n\tContext: ");
				sb.append(context.trim());
			}

			sb.append('\n');
		}

		return sb.toString();
	}
};
