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
import java.util.Vector;

public class Term extends ParseObject
{
	private Factor factor = null;
	private Vector<Object[]> factors;

	private Term(Factor factor, Vector<Object[]> factors)
	{
		this.factor = factor;
		this.factors = factors;
	}

	private Term(Vector<Object[]> factors)
	{
		this.factors = factors;
	}

	public static Term parse(Scanner sc) throws ScannerException, ParserException
	{
		Factor factor = Factor.parse(sc);

		// Grab each following mulop/factor set
		Token nextToken = sc.viewNextToken();
		Vector<Object[]> factors = new Vector<Object[]>();
		while(nextToken.getType() == Token.Type.MULTIPLY ||
				nextToken.getType() == Token.Type.DIVIDE)
		{
			factors.add(new Object[] {sc.getNextToken().getType(), Factor.parse(sc)});
			nextToken = sc.viewNextToken();
		}

		return new Term(factor, factors);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder();

		// Only surround with parens if there's more than one factor in this term
		if(!factors.isEmpty())
			sb.append("(");

		// Show the required factor
		sb.append(factor);

		// Any additional
		for(Object[] factor2 : factors)
		{
			sb.append(" ");
			sb.append(Token.toString((Token.Type)factor2[0]));
			sb.append(" ");
			sb.append(factor2[1]);
		}

		if(!factors.isEmpty())
			sb.append(")");

		return sb.toString();
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Bring in the required factor
		factor.genLLCode(func, vars);
		Operand lastDest = func.getLastDest();

		// Any additional
		for(Object[] factor2 : factors)
		{
			// Bring in the next factor
			((Factor)factor2[1]).genLLCode(func, vars);

			// Multiply/divide the this one and the last one
			BasicBlock multBlock = new BasicBlock(func);
			Operation mulOp = new Operation(multBlock);
			mulOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			mulOp.setSrcOperand(0, lastDest);
			mulOp.setSrcOperand(1, func.getLastDest());
			if((Token.Type)factor2[0] == Token.Type.MULTIPLY)
				mulOp.setType(Operation.OPER_MUL_I);
			else
				mulOp.setType(Operation.OPER_DIV_I);
			multBlock.appendOper(mulOp);

			// Rotate this and last
			lastDest = func.getLastDest();
		}
	}
}
