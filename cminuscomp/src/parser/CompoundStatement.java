package parser;

import compiler.CompilerException;
import compiler.SymbolTable;
import drg.lowlevel.BasicBlock;
import drg.lowlevel.Function;
import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;
import java.util.Vector;

public class CompoundStatement extends Statement
{
	private Vector<Declaration> decls = new Vector<Declaration>();
	private Vector<Statement> stmts = new Vector<Statement>();

	private CompoundStatement(Vector<Declaration> decls, Vector<Statement> stmts)
	{
		this.decls = decls;
		this.stmts = stmts;
	}

	public static CompoundStatement parse(Scanner sc) throws ScannerException, ParserException
	{
		match(sc, Token.Type.LEFT_CURLY);

		// Local declarations
		// Keep parsing decls until we run out of ints
		Vector<Declaration> decls = new Vector<Declaration>();
		while(sc.viewNextToken().getType() == Token.Type.INT)
		{
			decls.add(Declaration.parse(sc));
		}

		// Keep parsing statements until we reach the right curly
		Vector<Statement> stmts = new Vector<Statement>();
		while(sc.viewNextToken().getType() != Token.Type.RIGHT_CURLY)
		{
			stmts.add(Statement.parse(sc));
		}

		match(sc, Token.Type.RIGHT_CURLY);

		return new CompoundStatement(decls, stmts);
	}

	public String toString(String tab)
	{
		StringBuilder sb = new StringBuilder();

		if(decls.size() != 0 || stmts.size() != 0)
		{
			for(Declaration decl : decls)
			{
				sb.append(decl.toString(tab));
			}
			for(Statement stmt : stmts)
			{
				sb.append(stmt.toString(tab));
			}
		}
		else
		{
			// Visually show it's empty
			sb.append(tab);
			sb.append(";\n");
		}
		
		return sb.toString();
	}

	public void genLLCode(Function func, SymbolTable vars) throws CompilerException
	{
		// Add all variable declarations
		for(Declaration decl : decls)
			vars.insert(decl.getID());

		// Now on to statements
		for(Statement stmt : stmts)
		{
			stmt.genLLCode(func, vars);
		}
	}
}
