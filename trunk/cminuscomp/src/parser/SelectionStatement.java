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

public class SelectionStatement extends Statement
{
	private Expression expr;
	private Statement ifStmt;
	private Statement elseStmt;

	private SelectionStatement(Expression expr, Statement ifStmt, Statement elseStmt)
	{
		this.expr = expr;
		this.ifStmt = ifStmt;
		this.elseStmt = elseStmt;
	}

	public static SelectionStatement parse(Scanner sc) throws ScannerException, ParserException
	{
		match(sc, Token.Type.IF);
		match(sc, Token.Type.LEFT_PAREN);
		Expression expr = Expression.parse(sc);
		match(sc, Token.Type.RIGHT_PAREN);
		Statement ifStmt = Statement.parse(sc);

		Statement elseStmt = null;
		if(sc.viewNextToken().getType() == Token.Type.ELSE)
		{
			match(sc, Token.Type.ELSE);
			elseStmt = Statement.parse(sc);
		}
		
		return new SelectionStatement(expr, ifStmt, elseStmt);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);
		sb.append("if(");
		sb.append(expr.toString());
		sb.append(")\n");
		sb.append(ifStmt.toString(tab + "  "));

		if(elseStmt != null)
		{
			sb.append(tab);
			sb.append("else\n");
			sb.append(elseStmt.toString(tab + "  "));
		}

		return sb.toString();
	}

	@Override
	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Create new block to continue the function
		// at after we exit from the if/else
		BasicBlock continuationBlock = new BasicBlock(func);

		// Save current block off so we can insert the jump in it afterward
		BasicBlock checkBlock = func.getCurrBlock();

		// Create the basic block for the statements if
		// expression is true and link it into place
		BasicBlock thenBlock = new BasicBlock(func);
		func.appendToCurrentBlock(thenBlock);
		ifStmt.genLLCode(func, vars);
		func.appendToCurrentBlock(continuationBlock);

		// Create block for the else and attach it to the function unconnected
		BasicBlock elseBlock = null;
		if(elseStmt != null)
		{
			// Statements in else
			elseBlock = new BasicBlock(func);
			func.appendUnconnectedBlock(elseBlock);
			func.setCurrBlock(elseBlock);
			elseStmt.genLLCode(func, vars);

			// Point else to continuation block
			Operation jumpBackOp = func.newOper();
			jumpBackOp.setType(Operation.OPER_JMP);
			jumpBackOp.setSrcOperand(0, new Operand(Operand.OPERAND_BLOCK, continuationBlock.getBlockNum()));
			func.appendToCurrentBlock(jumpBackOp);
		}

		// Drop in op to head to appropriate branch
		// If there is no else go directly to the continuation
		func.setCurrBlock(checkBlock);
		expr.genLLCode(func, vars);
		
		Operation exprOp = func.newOper();
		exprOp.setType(Operation.OPER_BNE);
		exprOp.setSrcOperand(0, new Operand(Operand.OPERAND_INT, 1));
		exprOp.setSrcOperand(1, checkBlock.getLastOper().getDestOperand(0));
		if(elseBlock != null)
			exprOp.setSrcOperand(2, new Operand(Operand.OPERAND_BLOCK, elseBlock.getBlockNum()));
		else
			exprOp.setSrcOperand(2, new Operand(Operand.OPERAND_BLOCK, continuationBlock.getBlockNum()));
		func.appendToCurrentBlock(exprOp);

		// And continue after all that is done
		func.setCurrBlock(continuationBlock);
	}
}
