package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.Attribute;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.Function;
import drg.lowlevel.Operand;
import drg.lowlevel.Operation;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;
import java.util.Vector;

public class Factor extends ParseObject
{
	private Expression expr = null;
	private Vector<Expression> args = null;
	private int num;
	private String id = null;

	private Factor(Expression expr)
	{
		this.expr = expr;
	}

	private Factor(String id, Expression expr)
	{
		this.id = id;
		this.expr = expr;
	}

	private Factor(String id, Vector<Expression> args)
	{
		this.id = id;
		this.args = args;
	}

	private Factor(String id)
	{
		this.id = id;
	}

	private Factor(int num)
	{
		this.num = num;
	}
	
	public static Factor parse(Scanner sc) throws ScannerException, ParserException
	{
		Token nextToken = sc.getNextToken();
		if(nextToken.getType() == Token.Type.LEFT_PAREN)
		{
			Expression expr = Expression.parse(sc);
			match(sc, Token.Type.RIGHT_PAREN);

			return new Factor(expr);
		}
		else if(nextToken.getType() == Token.Type.IDENTIFIER)
		{
			String id = nextToken.getData().toString();

			nextToken = sc.viewNextToken();
			if(nextToken.getType() == Token.Type.LEFT_BRACKET)
			{
				match(sc, Token.Type.LEFT_BRACKET);
				Expression expr = Expression.parse(sc);
				match(sc, Token.Type.RIGHT_BRACKET);

				return new Factor(id, expr);
			}
			else if(nextToken.getType() == Token.Type.LEFT_PAREN)
			{
				match(sc, Token.Type.LEFT_PAREN);
				Vector<Expression> args = Expression.parseArgs(sc);
				match(sc, Token.Type.RIGHT_PAREN);

				return new Factor(id, args);
			}
			else
			{
				// Plain variable
				return new Factor(id);
			}
		}
		else if(nextToken.getType() == Token.Type.NUMBER)
		{
			return new Factor(Integer.parseInt(nextToken.getData().toString()));
		}
		else
		{
			throw new ParserException(nextToken, "left paren, identifier, or number");
		}
	}

	@Override
	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder();

		if(id != null)
		{
			// Variable, array, or function
			sb.append(id);

			if(args != null)
			{
				// Function call
				sb.append("(");

				for(Expression arg : args)
				{
					sb.append(arg);
					sb.append(", ");
				}

				sb.append(")");
			}
			else if(expr != null)
			{
				// Array access
				sb.append("[");
				sb.append(expr);
				sb.append("]");
			}
		}
		else if(expr != null)
		{
			// Complex factor
			sb.append("(");
			sb.append(expr);
			sb.append(")");
		}
		else
		{
			// Boring factor
			sb.append(num);
		}

		return sb.toString();
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		if(id != null)
		{
			// Variable, array, or function
			if(args != null)
			{
				// Function call
				// Check if the function is actually defined
				if(!vars.exists(id))
					throw new CompilerException("Function " + id + "() is not defined");

				BasicBlock callBlock = new BasicBlock(func);

				// Parameters
				for(Expression arg : args)
				{
					// Generation expression so we know what register to draw from
					// This is weird. I wish I could have the stack all nicely pushed on
					// right before the call. But it should work, just less clear
					// when reading the LL code
					arg.genLLCode(func, vars);

					// Push onto stack, or whatever PASS does
					Operation paramOp = new Operation(callBlock);
					paramOp.setType(Operation.OPER_PASS);
					paramOp.setSrcOperand(0, func.getLastDest());
					callBlock.appendOper(paramOp);
				}

				// Make function call
				Operation callOp = new Operation(callBlock);
				callOp.setType(Operation.OPER_CALL);
				callOp.setSrcOperand(0, new Operand(Operand.OPERAND_STRING, id));
				callOp.addAttribute(new Attribute("numParams", String.valueOf(args.size())));
				callBlock.appendOper(callOp);

				// Put return into a register. we just assume everything returns something
				Operation retSaveOp = new Operation(callBlock);
				retSaveOp.setType(Operation.OPER_ASSIGN);
				retSaveOp.setSrcOperand(0, new Operand(Operand.OPERAND_MACRO, "RetReg"));
				retSaveOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
				callBlock.appendOper(retSaveOp);

				// Make actual call
				func.appendToCurrentBlock(callBlock);
				func.setCurrBlock(callBlock);
			}
			else if(expr != null)
			{
				// Array access
				throw new CompilerException("Array access is not supported");
			}
			else
			{
				// Variable access
				BasicBlock accessBlock = new BasicBlock(func);
				Operation op = new Operation(accessBlock);
				op.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));

				// Local or global?
				int reg = vars.get(id);
				if(reg != -1)
				{
					// Local
					op.setType(Operation.OPER_ASSIGN);
					op.setSrcOperand(0, new Operand(Operand.OPERAND_REG, reg));
				}
				else
				{
					// Global
					op.setType(Operation.OPER_LOAD_I);
					op.setSrcOperand(0, new Operand(Operand.OPERAND_STRING, id));
				}

				// Tack stuff together
				accessBlock.appendOper(op);
			}
		}
		else if(expr != null)
		{
			// Complex factor
			expr.genLLCode(func, vars);
		}
		else
		{
			// Boring factor
			// Create operation which "accesses" our boring little number
			// This had better be optimized later
			Operation op = func.newOper();
			op.setType(Operation.OPER_ASSIGN);
			op.setSrcOperand(0, new Operand(Operand.OPERAND_INT, num));
			op.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			func.appendToCurrentBlock(op);
		}
	}
}
