package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.CodeItem;
import drg.lowlevel.Function;
import drg.lowlevel.Operand;
import drg.lowlevel.Operation;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;

public class IterationStatement extends Statement
{
	private Expression expr;
	private Statement stmt;

	private IterationStatement(Expression expr, Statement stmt)
	{
		this.expr = expr;
		this.stmt = stmt;
	}

	public static IterationStatement parse(Scanner sc) throws ScannerException, ParserException
	{
		match(sc, Token.Type.WHILE);
		match(sc, Token.Type.LEFT_PAREN);
		Expression expr = Expression.parse(sc);
		match(sc, Token.Type.RIGHT_PAREN);
		Statement stmt = Statement.parse(sc);

		return new IterationStatement(expr, stmt);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append("while(");
		sb.append(expr.toString());
		sb.append(")\n");
		sb.append(stmt.toString(tab + "  "));
		return sb.toString();
	}

	public CodeItem genLLCode(SymbolTable globals)
	{
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Block where code will continue at once the loop terminates
		BasicBlock continuationBlock = new BasicBlock(func);

		// Let condition expression be evaluated the first time
		// Go to continuation block if this is false
		// We'll repeat this after the loop for efficiency
		expr.genLLCode(func, vars);

		Operation loopSkipOp = func.newOper();
		loopSkipOp.setType(Operation.OPER_BNE);
		loopSkipOp.setSrcOperand(0, new Operand(Operand.OPERAND_INT, 1));
		loopSkipOp.setSrcOperand(1, func.getLastDest());
		loopSkipOp.setSrcOperand(2, new Operand(Operand.OPERAND_BLOCK, continuationBlock.getBlockNum()));
		func.appendToCurrentBlock(loopSkipOp);

		// Create the basic block for the loop itself
		BasicBlock loopBlock = new BasicBlock(func);
		func.appendToCurrentBlock(loopBlock);
		func.setCurrBlock(loopBlock);
		stmt.genLLCode(func, vars);

		// Now put the comparison at the bottom of the loop
		// If it's true to go the loop head, otherwise we'll fall to the continuation
		expr.genLLCode(func, vars);

		Operation loopOp = func.newOper();
		loopOp.setType(Operation.OPER_BEQ);
		loopOp.setSrcOperand(0, new Operand(Operand.OPERAND_INT, 1));
		loopOp.setSrcOperand(1, func.getLastDest());
		loopOp.setSrcOperand(2, new Operand(Operand.OPERAND_BLOCK, loopBlock.getBlockNum()));
		func.appendToCurrentBlock(loopOp);

		// Continuation goes on right after the loop
		func.appendToCurrentBlock(continuationBlock);
		func.setCurrBlock(continuationBlock);
	}
}
