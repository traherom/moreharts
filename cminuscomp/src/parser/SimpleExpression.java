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

public class SimpleExpression extends Expression
{
	private AdditiveExpression2 first = null;
	private Operator relop = null;
	private AdditiveExpression second = null;

	private SimpleExpression()
	{
		// Epsilon
	}

	private SimpleExpression(AdditiveExpression2 firstAdditive, Operator relop, AdditiveExpression secondAdditive)
	{
		this.first = firstAdditive;
		this.relop = relop;
		this.second = secondAdditive;
	}

	private SimpleExpression(AdditiveExpression2 firstAdditive)
	{
		this(firstAdditive, null, null);
	}

	static SimpleExpression parse(Scanner sc) throws ScannerException, ParserException
	{
		// Maybe first part
		Token nextToken = sc.viewNextToken();
		AdditiveExpression2 firstAdditive = AdditiveExpression2.parse(sc);

		// Do we have to deal with a comparison?
		nextToken = sc.viewNextToken();
		if(nextToken.getType() == Token.Type.LESS_THAN_EQUAL ||
				nextToken.getType() == Token.Type.LESS_THAN ||
				nextToken.getType() == Token.Type.GREATER_THAN ||
				nextToken.getType() == Token.Type.GREATER_THAN_EQUAL ||
				nextToken.getType() == Token.Type.EQUAL ||
				nextToken.getType() == Token.Type.NOT_EQUAL)
		{
			Operator relop = Operator.parse(sc);
			AdditiveExpression secondAdditive = AdditiveExpression.parse(sc);
			return new SimpleExpression(firstAdditive, relop, secondAdditive);
		}
		else
		{
			return new SimpleExpression(firstAdditive);
		}
	}

	@Override
	public String toString(String tab)
	{
		if(first != null)
		{
			StringBuilder sb = new StringBuilder(tab);
			sb.append(first.toString());

			if(second != null)
			{
				sb.append(" ");
				sb.append(relop.toString());
				sb.append(" ");
				sb.append(second.toString());
			}

			return sb.toString();
		}
		else
		{
			// Epsilon
			return "";
		}
	}

	@Override
	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Epsilon?
		if(first == null)
			return;

		// First half of comparison (maybe), save off where the result is
		first.genLLCode(func, vars);
		Operand firstDest = func.getLastDest();

		// Comparison?
		if(second != null)
		{
			// Second expression in the comparison
			second.genLLCode(func, vars);

			// Compare
			Operation compOp = func.newOper();
			compOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			compOp.setSrcOperand(0, firstDest);
			compOp.setSrcOperand(1, func.getLastDest());

			Token.Type type = relop.getType();
			if(type == Token.Type.EQUAL)
				compOp.setType(Operation.OPER_EQUAL);
			else if(type == Token.Type.LESS_THAN)
				compOp.setType(Operation.OPER_LT);
			else if(type == Token.Type.LESS_THAN_EQUAL)
				compOp.setType(Operation.OPER_LTE);
			else if(type == Token.Type.GREATER_THAN)
				compOp.setType(Operation.OPER_GT);
			else if(type == Token.Type.GREATER_THAN_EQUAL)
				compOp.setType(Operation.OPER_GTE);
			else if(type == Token.Type.NOT_EQUAL)
				compOp.setType(Operation.OPER_NOTEQ);
			else
				throw new CompilerException("Invalid comparison operator '" + type + "' somehow snuck through");

			func.appendToCurrentBlock(compOp);
		}
	}
}
