package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.Function;
import drg.lowlevel.Operand;
import drg.lowlevel.Operation;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;

public class ReturnStatement extends Statement
{
	Expression expr = null;

	private ReturnStatement(Expression expr)
	{
		this.expr = expr;
	}

	public static ReturnStatement parse(Scanner sc) throws ScannerException, ParserException
	{
		match(sc, Token.Type.RETURN);

		// Parse expression here maybe, if not semicolon
		Expression expr = null;
		if(sc.viewNextToken().getType() != Token.Type.SEMICOLON)
		{
			expr = Expression.parse(sc);
		}

		match(sc, Token.Type.SEMICOLON);
		return new ReturnStatement(expr);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append("return ");

		if(expr != null)
		{
			sb.append(expr.toString());
		}

		sb.append("\n");
		return sb.toString();
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		BasicBlock returnBlock = new BasicBlock(func);

		// Generate the expression to return if applicable
		if(expr != null)
		{
			expr.genLLCode(func, vars);

			// Move result to return register
			Operation retSetOp = func.newOper();
			retSetOp.setType(Operation.OPER_ASSIGN);
			retSetOp.setSrcOperand(0, func.getLastDest());
			retSetOp.setDestOperand(0, new Operand(Operand.OPERAND_MACRO, "RetReg"));
			func.appendToCurrentBlock(retSetOp);
		}
		
		// Jump to function return block (the universal one)
		Operation returnOp = func.newOper();
		returnOp.setType(Operation.OPER_JMP);
		returnOp.setSrcOperand(0, new Operand(Operand.OPERAND_BLOCK, func.getReturnBlock().getBlockNum()));
		func.appendToCurrentBlock(returnOp);

		// Start new block for other stuff to add to,
		// if that happens to be needed
		BasicBlock newBlock = new BasicBlock(func);
		func.appendToCurrentBlock(newBlock);
		func.setCurrBlock(newBlock);
	}
}
