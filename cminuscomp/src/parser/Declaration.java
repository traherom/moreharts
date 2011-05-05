package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.CodeItem;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;
import java.util.Vector;

public abstract class Declaration extends ParseObject
{
	protected Type type;
	protected String id;
	
	public static Declaration parse(Scanner sc) throws ScannerException, ParserException
	{
		// Function or variable?
		Token type = sc.getNextToken();
		if(type.getType() == Token.Type.VOID)
		{
			// Function declaration
			Token id = sc.getNextToken();
			if(id.getType() != Token.Type.IDENTIFIER)
			{
				throw new ParserException(id, "identifier");
			}

			match(sc, Token.Type.LEFT_PAREN);
			Vector<Parameter> params = parseParameters(sc);
			match(sc, Token.Type.RIGHT_PAREN);
			CompoundStatement compound = CompoundStatement.parse(sc);

			return new FunctionDeclaration(Type.VOID, id.getData().toString(),
										   params, compound);
		}
		else if(type.getType() == Token.Type.INT)
		{
			// Identifier
			Token id = sc.getNextToken();
			if(id.getType() != Token.Type.IDENTIFIER)
			{
				throw new ParserException(id, "identifier");
			}

			// Could still be either
			Token clarification = sc.viewNextToken();
			if(clarification.getType() == Token.Type.LEFT_PAREN)
			{
				// Function declaration
				match(sc, Token.Type.LEFT_PAREN);
				Vector<Parameter> params = parseParameters(sc);
				match(sc, Token.Type.RIGHT_PAREN);
				CompoundStatement compound = CompoundStatement.parse(sc);

				return new FunctionDeclaration(Type.INT, id.getData().toString(),
											   params, compound);
			}
			else if(clarification.getType() == Token.Type.LEFT_BRACKET)
			{
				// Array declaration
				match(sc, Token.Type.LEFT_BRACKET);

				// Size of array
				Token size = sc.getNextToken();
				if(size.getType() != Token.Type.NUMBER)
				{
					// Error
					throw new ParserException(size, "array dimension");
				}
				int arraySize = Integer.parseInt(size.getData().toString());

				// Finish up array
				match(sc, Token.Type.RIGHT_BRACKET);
				match(sc, Token.Type.SEMICOLON);

				return new VariableDeclaration(Type.INT, id.getData().toString(),
											   arraySize);
			}
			else if(clarification.getType() == Token.Type.SEMICOLON)
			{
				// Integer declaration
				match(sc, Token.Type.SEMICOLON);
				return new VariableDeclaration(Type.INT, id.getData().toString());
			}
			else
			{
				// Errors
				throw new ParserException(clarification, "'(', '[', or ';'");
			}
		}
		else
		{
			// Errors
			throw new ParserException(type, "int or void");
		}
	}

	/**
	 * Returns the type of this declaration, int, void, or int array
	 * In the case of a function this is the return type
	 */
	public Type getType()
	{
		return type;
	}

	/**
	 * Returns the identifier for this declaration
	 */
	public String getID()
	{
		return id;
	}

	/**
	 * Helper parse function, takes, care of function parameters
	 * @return vector of parameters
	 */
	private static Vector<Parameter> parseParameters(Scanner sc)
			throws ScannerException, ParserException
	{
		Vector<Parameter> params = new Vector<Parameter>();

		// Are there any?
		Token type = sc.getNextToken();
		if(type.getType() == Token.Type.VOID)
		{
			return params;
		}

		// First one is an int then
		Token id = sc.getNextToken();
		if(id.getType() != Token.Type.IDENTIFIER)
		{
			// Errors
			throw new ParserException(id, "identifier");
		}

		// Is an array?
		Type t = Type.INT;
		if(sc.viewNextToken().getType() == Token.Type.LEFT_BRACKET)
		{
			t = Type.INT_ARRAY;
			match(sc, Token.Type.LEFT_BRACKET);
			match(sc, Token.Type.RIGHT_BRACKET);
		}

		Parameter param = new Parameter(t, id.getData().toString());
		params.add(param);

		// Now each one after it
		// Don't much token here because if it's a ) we want to leave it in stream
		while(sc.viewNextToken().getType() == Token.Type.COMMA)
		{
			match(sc, Token.Type.COMMA);
			match(sc, Token.Type.INT);

			// Param name
			id = sc.getNextToken();
			if(id.getType() != Token.Type.IDENTIFIER)
			{
				// Errors
				throw new ParserException(id, "identifier");
			}

			// Is an array?
			t = Type.INT;
			if(sc.viewNextToken().getType() == Token.Type.LEFT_BRACKET)
			{
				t = Type.INT_ARRAY;
				match(sc, Token.Type.LEFT_BRACKET);
				match(sc, Token.Type.RIGHT_BRACKET);
			}

			// And add
			param = new Parameter(t, id.getData().toString());
			params.add(param);
		}

		return params;
	}

	public abstract CodeItem genLLCode(SymbolTable vars) throws CompilerException;
}
