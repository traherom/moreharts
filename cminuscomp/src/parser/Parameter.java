package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.Data;
import drg.lowlevel.FuncParam;

public class Parameter extends ParseObject
{
	private Type type;
	private String id;

	public Parameter(Type type, String id)
	{
		this.type = type;
		this.id = id;
	}

	public Type getType()
	{
		return type;
	}

	public String getID()
	{
		return id;
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append(type + " " + id);
		return sb.toString();
	}

	/**
	 * Creates a low level FuncParam based for this parameter and inserts itself
	 * into the given symbol table
	 * @param locals local symbol table to insert this paramater into
	 * @return new FuncParam matching this Parameter
	 */
	protected FuncParam createFuncParam(SymbolTable locals) throws CompilerException
	{
		if(type == Type.INT)
		{
			locals.insert(id);
			return new FuncParam(Data.TYPE_INT, id);
		}
		else
			throw new CompilerException(type + " is unsupported as a function parameter type");
	}
}
