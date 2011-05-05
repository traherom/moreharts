package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.CodeItem;
import drg.lowlevel.Data;

public class VariableDeclaration extends Declaration
{
	private int arraySize;
	private boolean isArray;

	public VariableDeclaration(Type type, String id)
	{
		this.type = type;
		this.id = id;
		this.isArray = false;
	}

	public VariableDeclaration(Type type, String id, int size) throws ParserException
	{
		this.type = type;
		this.id = id;
		this.arraySize = size;
		this.isArray = true;
	}

	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append(type + " " + id);

		if(isArray)
		{
			sb.append("[" + arraySize + "]");
		}

		sb.append("\n");

		return sb.toString();
	}

	public CodeItem genLLCode(SymbolTable vars) throws CompilerException
	{
		// Is it already declared?
		if(vars.exists(id))
			throw new CompilerException(id + " declared twice.");

		// Stick in global symbol table
		vars.insert(id);

		// Create code item for declaration
		if(type == Type.INT)
			return new Data(Data.TYPE_INT, id);
		else
			throw new CompilerException(type + " is not supported by the compiler");
	}
}
