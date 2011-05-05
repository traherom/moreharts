package parser;

import scanner.ScannerException;
import java.io.IOException;

public interface Parser
{
    public Program parse() throws ParserException, ScannerException, IOException;
}
