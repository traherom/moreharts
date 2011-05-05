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

class AdditiveExpression extends Expression
{
	private Term term;
	private Vector<Object[]> terms;

	private AdditiveExpression(Term term, Vector<Object[]> terms)
	{
		this.term = term;
		this.terms = terms;
	}

	public static AdditiveExpression parse(Scanner sc) throws ScannerException, ParserException
	{
		Term term = Term.parse(sc);
		
		Token nextToken = sc.viewNextToken();
		Vector<Object[]> terms = new Vector<Object[]>();
		while(nextToken.getType() == Token.Type.PLUS ||
				nextToken.getType() == Token.Type.MINUS)
		{
			terms.add(new Object[] {sc.getNextToken().getType(), Term.parse(sc)});
			nextToken = sc.getNextToken();
		}

		return new AdditiveExpression(term, terms);
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder();

		sb.append(term);
		
		for(Object[] term2 : terms)
		{
			sb.append(" ");
			sb.append(Token.toString((Token.Type)term2[0]));
			sb.append(" ");
			sb.append(term2[1]);
		}

		return sb.toString();
	}

	@Override
	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Bring in the required term
		term.genLLCode(func, vars);
		Operand lastDest = func.getLastDest();

		// Any additional
		for(Object[] term2 : terms)
		{
			// Bring in the next term
			((Term)term2[1]).genLLCode(func, vars);

			// Add/subtract the this one and the last one
			BasicBlock addBlock = new BasicBlock(func);
			Operation addOp = new Operation(addBlock);
			addOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			addOp.setSrcOperand(0, lastDest);
			addOp.setSrcOperand(1, func.getLastDest());
			if((Token.Type)term2[0] == Token.Type.PLUS)
				addOp.setType(Operation.OPER_ADD_I);
			else
				addOp.setType(Operation.OPER_SUB_I);
			addBlock.appendOper(addOp);

			// Rotate
			lastDest = func.getLastDest();
		}
	}
}
