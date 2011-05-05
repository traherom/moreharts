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

class AdditiveExpression2 extends Expression
{
	private Vector<Object[]> factors;
	private Vector<Object[]> terms;

	private AdditiveExpression2(Vector<Object[]> factors, Vector<Object[]> terms)
	{
		this.factors = factors;
		this.terms = terms;
	}
	
	public static AdditiveExpression2 parse(Scanner sc) throws ScannerException, ParserException
	{
		// Term 2
		Token nextToken = sc.viewNextToken();
		Vector<Object[]> factors = new Vector<Object[]>();
		while(nextToken.getType() == Token.Type.MULTIPLY ||
				nextToken.getType() == Token.Type.DIVIDE)
		{
			factors.add(new Object[] {sc.getNextToken().getType(), Factor.parse(sc)});
			nextToken = sc.viewNextToken();
		}

		// {addop term}
		nextToken = sc.viewNextToken();
		Vector<Object[]> terms = new Vector<Object[]>();
		while(nextToken.getType() == Token.Type.PLUS ||
				nextToken.getType() == Token.Type.MINUS)
		{
			terms.add(new Object[] {sc.getNextToken().getType(), Term.parse(sc)});
			nextToken = sc.viewNextToken();
		}

		return new AdditiveExpression2(factors, terms);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder();

		// Surround everything with parens, so we can see the actual operation order
		for(Object[] factor : factors)
		{
			sb.append(" ");
			sb.append(Token.toString((Token.Type)factor[0]));
			sb.append(" ");
			sb.append(factor[1]);
		}

		// Now the lame terms
		for(Object[] term : terms)
		{
			sb.append(" ");
			sb.append(Token.toString((Token.Type)term[0]));
			sb.append(" ");
			sb.append(term[1]);
		}

		return sb.toString();
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Save where the first factor will be coming from
		Operand lastDest = func.getLastDest();

		// Factors
		for(Object[] factor : factors)
		{
			// Bring in the next factor
			((Factor)factor[1]).genLLCode(func, vars);

			// Multiply/divide the this one and the last one
			Operation mulOp = func.newOper();
			mulOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			mulOp.setSrcOperand(0, lastDest);
			mulOp.setSrcOperand(1, func.getLastDest());
			if((Token.Type)factor[0] == Token.Type.MULTIPLY)
				mulOp.setType(Operation.OPER_MUL_I);
			else
				mulOp.setType(Operation.OPER_DIV_I);
			func.appendToCurrentBlock(mulOp);

			// Change this to be the last one
			lastDest = func.getLastDest();
		}

		// Terms
		for(Object[] term : terms)
		{
			// Bring in the next term
			((Term)term[1]).genLLCode(func, vars);

			// Add/subtract the this one and the last one
			BasicBlock add2Block = new BasicBlock(func);
			Operation addOp = new Operation(add2Block);
			addOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			addOp.setSrcOperand(0, lastDest);
			addOp.setSrcOperand(1, func.getLastDest());
			if((Token.Type)term[0] == Token.Type.PLUS)
				addOp.setType(Operation.OPER_ADD_I);
			else
				addOp.setType(Operation.OPER_SUB_I);
			add2Block.appendOper(addOp);

			// Switch this to be last
			lastDest = func.getLastDest();
		}
	}
}
