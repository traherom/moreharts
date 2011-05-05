package compiler;

import parser.*;
import drg.lowlevel.*;
import java.io.*;
import drg.optimizer.*;
import drg.x86codegen.*;
import drg.dataflow.*;
import scanner.ScannerException;

public class CMinusCompiler
{
	private Parser parser;
	private Program prog;
	private CodeItem llCode;
	private boolean isOptimized = false;

	private SymbolTable globalSymbols = new SymbolTable();

	public CMinusCompiler(String fileName) throws ScannerException
	{
		parser = new CMinusParser(fileName);
	}

	public CMinusCompiler(Parser parser)
	{
		this.parser = parser;
	}

	/**
	 * Returns the parser that is being used for this compile
	 * @return parser associated with compiler
	 */
	public Parser getParser()
	{
		return parser;
	}

	/**
	 * Compiles code using a default optimization level of 2
	 * @param progName
	 * @param optimizationLevel
	 */
	public void compile(String progName) throws ParserException, ScannerException, IOException, CompilerException
	{
		compile(progName, 2);
	}

	/**
	 * Performs all steps needed to go from a parser to x86, saving the partial
	 * results after each step
	 * @param progName Name of the program, likely from the original source name
	 * @param optimizationLevel level that the code should be optimized to
	 */
	public void compile(String progName, int optimizationLevel) throws ParserException, ScannerException, IOException, CompilerException
	{
		// Strip off extension
		String shortName = progName.substring(0, progName.lastIndexOf("."));

		// Initial low level generation
		createLowLevel();
		writeLL(shortName + ".l");

		// Optimize
		if(optimizationLevel > 0)
		{
			optimize(optimizationLevel);
			writeLL(shortName + ".opti");
		}

		// X86
		x86Code();
		writeLL(shortName + ".x86");
		x86Assembly(shortName + ".s");
	}

	private void writeLL(String fileName) throws IOException
	{
		PrintWriter outFile = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
		llCode.printLLCode(outFile);
		outFile.close();
	}

	/**
	 * Psuedonym for compile() to make it clearer that the code isn't actually
	 * being recompiled if this is used. Compiles code if needed though
	 * @return Low level code for top level Program
	 */
	public CodeItem createLowLevel() throws ParserException, ScannerException, IOException, CompilerException
	{
		// Only compile if actually needed
		if(prog == null)
		{
			prog = parser.parse();
			llCode = prog.genLLCode(globalSymbols);
		}
		
		return llCode;
	}

	/**
	 * Optimizes the generated code with a default level of 2
	 */
	public void optimize() throws CompilerException
	{
		optimize(2);
	}

	/**
	 * Optimizes the generated code
	 * @param level optimization level
	 */
	public CodeItem optimize(int level) throws CompilerException
	{
		if(llCode == null)
			throw new CompilerException("Optimizer called before code compiled");
		if(isOptimized)
			throw new CompilerException("Extra call to optimizer. Likely error in higher logic");

		// Optimize to given level
		LowLevelCodeOptimizer optimizer = new LowLevelCodeOptimizer(llCode, level);
		optimizer.optimize();
		return llCode;
	}

	public CodeItem x86Code() throws CompilerException
	{
		if(llCode == null)
			throw new CompilerException("x86 code generator called before code compiled");

		X86CodeGenerator x86gen = new X86CodeGenerator(llCode);
		x86gen.convertToX86();
		return llCode;
	}

	public void x86Assembly(String fileName) throws IOException, CompilerException
	{
		// Can we also check that x86Code() was called?
		if(llCode == null)
			throw new CompilerException("x86 assembly generator called before code compiled");

		// simply walks functions and finds in and out edges for each BasicBlock
		ControlFlowAnalysis cf = new ControlFlowAnalysis(llCode);
		cf.performAnalysis();
		// cf.printAnalysis(null);

		LivenessAnalysis liveness = new LivenessAnalysis(llCode);
		liveness.performAnalysis();
		liveness.printAnalysis();

		// Allocate registers based on analysis
		int numRegs = 7;
		X86RegisterAllocator regAlloc = new X86RegisterAllocator(llCode, numRegs);
		regAlloc.performAllocation();

		// Produce assembly and write to file
		PrintWriter outFile = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
		X86AssemblyGenerator assembler = new X86AssemblyGenerator(llCode, outFile);
		assembler.generateAssembly();
		outFile.close();
	}
}
