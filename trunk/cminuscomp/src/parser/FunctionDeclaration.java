package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.CodeItem;
import drg.lowlevel.Data;
import drg.lowlevel.FuncParam;
import drg.lowlevel.Function;
import java.util.Vector;

public class FunctionDeclaration extends Declaration
{
	private Vector<Parameter> params = new Vector<Parameter>();
	private CompoundStatement compoundStmt;

	public FunctionDeclaration(Type type, String id, Vector<Parameter> params, CompoundStatement stmt)
	{
		this.type = type;
		this.id = id;
		this.compoundStmt = stmt;
		this.params = params;
	}

	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append(type + " " + id + "(");

		// Each of the parameters
		for(Parameter param : params)
		{
			sb.append(param.toString());
			sb.append(", ");
		}

		sb.append(")\n");

		sb.append(compoundStmt.toString(tab + "  "));
		return sb.toString();
	}

	public CodeItem genLLCode(SymbolTable globals) throws CompilerException
	{
		// Is it already declared?
		if(globals.exists(id))
			throw new CompilerException("Function " + id + "() declared twice");

		// Create function code item
		Function func = null;
		if(type == Type.INT)
			func = new Function(Data.TYPE_INT, id);
		else if(type == Type.VOID)
			func = new Function(Data.TYPE_VOID, id);
		else
			throw new CompilerException(type + " is not supported by the compiler for function returns");

		// Symbol table for reference later
		SymbolTable locals = new SymbolTable(func, globals);

		// Parameters?
		if(!params.isEmpty())
		{
			// Have at least one param, put it in function. This includes
			// putting it in the symbol table for this function
			FuncParam currParam = params.get(0).createFuncParam(locals);
			func.setFirstParam(currParam);

			// Put any more in by attaching iteratively
			for(int i = 1; i < params.size(); i++)
			{
				FuncParam temp = params.get(i).createFuncParam(locals);
				currParam.setNextParam(temp);
			
				currParam = temp;
			}
		}

		// Stick in global symbol table
		globals.insert(id);

		// Put in the entry code and mark as the current block
		func.createBlock0();
		func.setCurrBlock(func.getFirstBlock());
		BasicBlock returnBlock = func.genReturnBlock();

		// Let the compound statement go to town
		compoundStmt.genLLCode(func, locals);
		func.setCurrBlock(func.getLastBlock());

		// Exit code
		func.appendToCurrentBlock(returnBlock);
		func.setCurrBlock(func.getLastBlock());

		// Add else(s) after return so that the code is accessible
		// If needed, obviously
		BasicBlock unconnectedBlocks = func.getFirstUnconnectedBlock();
		if(unconnectedBlocks != null)
			func.appendToCurrentBlock(unconnectedBlocks);

		// Go team
		return func;
	}
}
