package compiler;

import drg.lowlevel.Function;
import java.util.HashMap;

/**
 * Holds symbols for a given scope.
 */
public class SymbolTable
{
	/** Higher level symbol table. Null if this is the global scope */
	private SymbolTable outer = null;
	/**
	 * Symbols declared directly in this scope.
	 * This does not use generics because it has to interface with G's code
	 */
	private HashMap symbols;
	/** Function we are a part of, letting us get reg numbers if needed */
	private Function func = null;

	/** Creates a global symbol table */
	public SymbolTable()
	{
		symbols = new HashMap();
	}

	/** Creates a function-level symbol table */
	public SymbolTable(Function func, SymbolTable outer)
	{
		this.outer = outer;
		this.func = func;
		this.symbols = func.getTable();
	}

	/**
	 * Returns whether the given symbol name exists in this context or not
	 * @param name name we are looking for
	 * @return true if the symbol is in scope
	 */
	public boolean exists(String name)
	{
		if(symbols.containsKey(name))
			return true;
		else if(outer != null)
			return outer.exists(name);
		else
			return false;
	}

	/**
	 * Inserts a symbol into this scope, getting register if applicable
	 * @param name name of variable
	 * @return register number of new variable or -1 if global
	 */
	public int insert(String name) throws CompilerException
	{
		// Not allowed to redeclare
		if(exists(name))
			throw new CompilerException("Redeclaration of '" + name + "' not allowed");

		if(func != null)
		{
			int reg = func.getNewRegNum();
			symbols.put(name, new Integer(reg));
			return reg;
		}
		else
		{
			symbols.put(name, -1);
			return -1;
		}
	}

	/**
	 * Returns the next higher scope
	 * @return scope directly surrounding this one
	 */
	public SymbolTable getOuter()
	{
		return outer;
	}

	/**
	 * Returns the symbol with the given name from either this scope or the closest one with it
	 * @param name symbol name
	 * @return register number of symbol or -1 if it is a global
	 */
	public int get(String name) throws CompilerException
	{
		// Is it in this scope?
		if(symbols.containsKey(name))
			return (Integer)symbols.get(name);
		else if(outer != null)
			return outer.get(name);
		else
			throw new CompilerException("Symbol '" + name + "' not found");
	}
}
