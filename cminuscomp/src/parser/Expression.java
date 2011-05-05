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

public class Expression extends ParseObject
{
	String id = null;
	Expression expr1 = null;
	Expression expr2 = null;
	SimpleExpression simple = null;
	int number;
	Vector<Expression> args = null;

	enum ExpressionType
	{
		VARIABLE_REF, ARRAY_REF, CALL,
		SIMPLE_MATH, COMPLEX_MATH,
		VARIABLE_ASSIGN, ARRAY_ASSIGN
	};
	private ExpressionType type;

	/**
	 * Used by subclasses for construction. Not used otherwise
	 */
	protected Expression()
	{
	}

	private Expression(String id, SimpleExpression followExpr)
	{
		type = ExpressionType.VARIABLE_REF;

		this.id = id;
		this.simple = followExpr;
	}

	private Expression(String id, Expression arrayIndex, SimpleExpression followExpr)
	{
		type = ExpressionType.ARRAY_REF;

		this.id = id;
		this.expr1 = arrayIndex;
		this.simple = followExpr;
	}

	private Expression(String id, Expression arrayIndex, Expression rhs)
	{
		type = ExpressionType.ARRAY_ASSIGN;

		this.id = id;
		this.expr1 = arrayIndex;
		this.expr2 = rhs;
	}

	private Expression(String id, Expression rhs)
	{
		type = ExpressionType.VARIABLE_ASSIGN;

		this.id = id;
		this.expr1 = rhs;
	}

	private Expression(String id, Vector<Expression> args, SimpleExpression followExpr)
	{
		type = ExpressionType.CALL;

		this.id = id;
		this.args = args;
		this.simple = followExpr;
	}

	private Expression(int num, SimpleExpression simple)
	{
		type = ExpressionType.SIMPLE_MATH;

		this.number = num;
		this.simple = simple;
	}

	private Expression(Expression expr, SimpleExpression simple)
	{
		type = ExpressionType.COMPLEX_MATH;

		this.expr1 = expr;
		this.simple = simple;
	}

	static Expression parse(Scanner sc) throws ScannerException, ParserException
	{
		Token nextToken = sc.getNextToken();
		if(nextToken.getType() == Token.Type.IDENTIFIER)
		{
			Token id = nextToken;

			// variable or function
			nextToken = sc.viewNextToken();
			if(nextToken.getType() == Token.Type.ASSIGNMENT)
			{
				match(sc, Token.Type.ASSIGNMENT);
				Expression rhs = parse(sc);

				return new Expression(id.getData().toString(), rhs);
			}
			else if(nextToken.getType() == Token.Type.LEFT_BRACKET)
			{
				// [ expr ] expr3
				match(sc, Token.Type.LEFT_BRACKET);
				Expression expr = parse(sc);
				match(sc, Token.Type.RIGHT_BRACKET);

				// expr3 is either another expr or a simple-expr
				nextToken = sc.viewNextToken();
				if(nextToken.getType() == Token.Type.ASSIGNMENT)
				{
					match(sc, Token.Type.ASSIGNMENT);
					Expression rhs = parse(sc);

					return new Expression(id.getData().toString(), expr, rhs);
				}
				else
				{
					// Let simple expression throw the error if needed
					SimpleExpression simple = SimpleExpression.parse(sc);

					return new Expression(id.getData().toString(), expr, simple);
				}
			}
			else if(nextToken.getType() == Token.Type.LEFT_PAREN)
			{
				// ( args )
				match(sc, Token.Type.LEFT_PAREN);
				Vector<Expression> args = parseArgs(sc);
				match(sc, Token.Type.RIGHT_PAREN);

				// Let simple expression throw the error if needed
				SimpleExpression simple = SimpleExpression.parse(sc);

				return new Expression(id.getData().toString(), args, simple);
			}
			else
			{
				// simple-expr
				// Let this throw an error if we can't actually go to it
				SimpleExpression simple = SimpleExpression.parse(sc);

				return new Expression(id.getData().toString(), simple);
			}
		}
		else if(nextToken.getType() == Token.Type.LEFT_PAREN)
		{
			// ( expr ) simple-expr
			Expression expr = parse(sc);
			match(sc, Token.Type.RIGHT_PAREN);

			SimpleExpression simple = SimpleExpression.parse(sc);

			return new Expression(expr, simple);

		}
		else if(nextToken.getType() == Token.Type.NUMBER)
		{
			// number simple-expr
			int num = Integer.parseInt(nextToken.getData().toString());
			SimpleExpression simple = SimpleExpression.parse(sc);

			return new Expression(num, simple);
		}
		else
		{
			throw new ParserException(nextToken, "identifier, left parenthesis, or number");
		}
	}

	/**
	 * Parses arguments being passed to a function
	 * @param sc Current Scanner
	 * @return Vector of each of the expressions that are being passed to the function
	 * @throws ScannerException
	 * @throws ParserException
	 */
	public static Vector<Expression> parseArgs(Scanner sc) throws ScannerException, ParserException
	{
		Vector<Expression> args = new Vector<Expression>();

		// Are there any?
		if(sc.viewNextToken().getType() == Token.Type.RIGHT_PAREN)
		{
			return args;
		}

		// Must be one
		args.add(Expression.parse(sc));

		// Any additional arguments
		while(sc.viewNextToken().getType() == Token.Type.COMMA)
		{
			match(sc, Token.Type.COMMA);
			args.add(Expression.parse(sc));
		}

		// Return
		return args;
	}

	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder(tab);

		switch(type)
		{
			case VARIABLE_REF:
				sb.append(id);
				sb.append(simple.toString());
				break;
			case ARRAY_REF:
				sb.append(id);
				sb.append("[");
				sb.append(expr1.toString());
				sb.append("]");
				sb.append(simple.toString());
				break;
			case VARIABLE_ASSIGN:
				sb.append(id);
				sb.append(" = ");
				sb.append(expr1.toString());
				break;
			case ARRAY_ASSIGN:
				sb.append(id);
				sb.append("[");
				sb.append(expr1.toString());
				sb.append("] = ");
				sb.append(expr2.toString());
				break;
			case CALL:
				sb.append(id);
				sb.append("(");

				for(Expression arg : args)
				{
					sb.append(arg.toString());
					sb.append(", ");
				}

				sb.append(")");
				sb.append(simple.toString());
				break;
			case SIMPLE_MATH:
				sb.append(number);
				sb.append(simple.toString());
				break;
			case COMPLEX_MATH:
				sb.append(expr1.toString());
				sb.append(simple.toString());
				break;
		}

		return sb.toString();
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Fill out data about operation, including process anything else
		// tied to us. Remember that we need to do things in order of availablitity,
		// not left to right as it came in (well, precedence). This mostly means
		// reverse, going deeper into the tree first
		if(type == ExpressionType.VARIABLE_REF)
		{
			// Access variable is wildly inefficient until optimized
			Operation accessOp = func.newOper();
			accessOp.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));

			// Local or global?
			int reg = vars.get(id);
			if(reg != -1)
			{
				// Local
				accessOp.setType(Operation.OPER_ASSIGN);
				accessOp.setSrcOperand(0, new Operand(Operand.OPERAND_REG, reg));
			}
			else
			{
				// Global
				accessOp.setType(Operation.OPER_LOAD_I);
				accessOp.setSrcOperand(0, new Operand(Operand.OPERAND_STRING, id));
			}

			// Tack stuff together
			func.appendToCurrentBlock(accessOp);

			// Whatever follows us
			simple.genLLCode(func, vars);
		}
		else if(type == ExpressionType.VARIABLE_ASSIGN)
		{
			// Process the expression being assigned to us
			expr1.genLLCode(func, vars);

			// Set up actual assignment
			Operation op = func.newOper();
			op.setSrcOperand(0, func.getLastDest());

			// Local or global?
			int reg = vars.get(id);
			if(reg != -1)
			{
				// Local
				op.setType(Operation.OPER_ASSIGN);
				op.setDestOperand(0, new Operand(Operand.OPERAND_REG, reg));
			}
			else
			{
				// Global
				op.setType(Operation.OPER_STORE_I);
				op.setSrcOperand(1, new Operand(Operand.OPERAND_STRING, id));
			}

			// Tack stuff together
			func.appendToCurrentBlock(op);
		}
		else if(type == ExpressionType.CALL)
		{
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

			// Do deeper in tree
			simple.genLLCode(func, vars);
		}
		else if(type == ExpressionType.SIMPLE_MATH)
		{
			// Create operation which accesses our boring little number
			Operation op = func.newOper();
			op.setType(Operation.OPER_ASSIGN);
			op.setSrcOperand(0, new Operand(Operand.OPERAND_INT, number));
			op.setDestOperand(0, new Operand(Operand.OPERAND_REG, func.getNewRegNum()));
			func.appendToCurrentBlock(op);
			
			// Deeper in tree
			simple.genLLCode(func, vars);
		}
		else if(type == ExpressionType.COMPLEX_MATH)
		{
			// Do complex part
			expr1.genLLCode(func, vars);

			// Deeper in tree
			simple.genLLCode(func, vars);
		}
		else
		{
			throw new CompilerException("Invalid expression type '" + type + "'");
		}
	}
}
