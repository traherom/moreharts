package scanner;

import scanner.Scanner;
import scanner.ScannerException;
import scanner.Token;
import java.io.EOFException;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

public class CMinusScanner implements Scanner
{
	private enum State
	{
		START, DONE,
		IN_NUMBER, IN_ID, IN_ASSIGNMENT,
		IN_INCREMENT, IN_DECREMENT,
		IN_LESS_THAN, IN_GREATER_THAN,
		IN_NOT_EQUAL,
		IN_COMMENT_START, IN_MULTI_COMMENT, IN_LINE_COMMENT, IN_COMMENT_END,
		V, VO, VOI, VOID,
		R, RE, RET, RETU, RETUR, RETURN,
		W, WH, WHI, WHIL, WHILE,
		E, EL, ELS, ELSE,
		I, IF,
		IN, INT
	};
	private RandomAccessFile inFile;
	private Token nextToken;
	private String currLine = null;
	private int currCharNum = 0;
	private int currLineNum = 1;

	public CMinusScanner(String fileName) throws ScannerException
	{
		try
		{
			inFile = new RandomAccessFile(new File(fileName), "r");
			nextToken = scanToken();
		}
		catch(FileNotFoundException ex)
		{
			throw new ScannerException(ex.getMessage());
		}
	}

	public Token getNextToken() throws ScannerException
	{
		Token returnToken = nextToken;
		if(nextToken.getType() != Token.Type.EOF)
			nextToken = scanToken();
		return returnToken;
	}

	public Token viewNextToken()
	{
		return nextToken;
	}

	public Token scanToken() throws ScannerException
	{
		// Token to be returned
		Token token = new Token(Token.Type.UNKNOWN);
		StringBuilder tokenSB = new StringBuilder();
		State currState = State.START;
		boolean shouldSave = true;

		while(currState != State.DONE)
		{
			char currChar = getNextChar();
			shouldSave = true;

			switch(currState)
			{
			case START: // Beginning of new identifier
				token.setLineNum(currLineNum);
				token.setCharNum(currCharNum);
				token.setContext(currLine);

				// Assume a one char token and that we'll be done at
				// the end of the switch
				currState = State.DONE;

				switch(currChar)
				{
				case ' ': // Skip whitespace
				case '\n':
				case '\t':
					shouldSave = false;
					currState = State.START;
					break;
				case '=':
					currState = State.IN_ASSIGNMENT;
					break;
				case '/':
					currState = State.IN_COMMENT_START;
					break;
				case '-':
					//currState = State.IN_DECREMENT;
					token.setType(Token.Type.MINUS);
					break;
				case '+':
					//currState = State.IN_INCREMENT;
					token.setType(Token.Type.PLUS);
					break;
				case '<':
					currState = State.IN_LESS_THAN;
					break;
				case '>':
					currState = State.IN_GREATER_THAN;
					break;
				case '!':
					currState = State.IN_NOT_EQUAL;
				case '*':
					token.setType(Token.Type.MULTIPLY);
					break;
				case '(':
					token.setType(Token.Type.LEFT_PAREN);
					break;
				case ')':
					token.setType(Token.Type.RIGHT_PAREN);
					break;
				case '[':
					token.setType(Token.Type.LEFT_BRACKET);
					break;
				case ']':
					token.setType(Token.Type.RIGHT_BRACKET);
					break;
				case '{':
					token.setType(Token.Type.LEFT_CURLY);
					break;
				case '}':
					token.setType(Token.Type.RIGHT_CURLY);
					break;
				case ';':
					token.setType(Token.Type.SEMICOLON);
					break;
				case ',':
					token.setType(Token.Type.COMMA);
					break;
				case 'i':
					currState = State.I;
					break;
				case 'e':
					currState = State.E;
					break;
				case 'w':
					currState = State.W;
					break;
				case 'r':
					currState = State.R;
					break;
				case 'v':
					currState = State.V;
					break;
				case 0: // End of file
					shouldSave = false;
					token.setType(Token.Type.EOF);
					break;
				default:
					if(Character.isDigit(currChar))
						currState = State.IN_NUMBER;
					else if(Character.isLetter(currChar))
						currState = State.IN_ID;
					else
					{
						token.setType(Token.Type.ERROR);
						token.setDescription("Unrecognized character '" + currChar + "'");
					}
				}
				break;

			case I: // Int or if
				if(currChar == 'f')
				{
					currState = State.IF;
				}
				else if(currChar == 'n')
				{
					currState = State.IN;
				}
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IF: // Maybe if
				// Prevent char from being consumed
				shouldSave = false;
				restoreChar();
				
				if(!Character.isLetterOrDigit(currChar))
				{
					token.setType(Token.Type.IF);
					currState = State.DONE;
				}
				else
				{
					// Not really an if. Sigh
					currState = State.IN_ID;
				}
				break;

			case IN: // Maybe int
				if(currChar == 't')
					currState = State.INT;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case INT: // Maybe int
				// Prevent char from being consumed
				shouldSave = false;
				restoreChar();

				if(!Character.isLetterOrDigit(currChar))
				{
					token.setType(Token.Type.INT);
					currState = State.DONE;
				}
				else
				{
					// Not really int
					currState = State.IN_ID;
				}
				break;

			case V: // Maybe void
				if(currChar == 'o')
					currState = State.VO;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case VO: // Maybe void
				if(currChar == 'i')
					currState = State.VOI;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case VOI: // Maybe void
				if(currChar == 'd')
					currState = State.VOID;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;
				
			case VOID: // Maybe void
				// Prevent char from being consumed
				shouldSave = false;
				restoreChar();
				
				if(!Character.isLetterOrDigit(currChar))
				{
					token.setType(Token.Type.VOID);
					currState = State.DONE;
				}
				else
				{
					// Not really a void. Sigh
					currState = State.IN_ID;
				}
				break;

			case W: // Maybe while
				if(currChar == 'h')
					currState = State.WH;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case WH: // Maybe while
				if(currChar == 'i')
					currState = State.WHI;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case WHI: // Maybe while
				if(currChar == 'l')
					currState = State.WHIL;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case WHIL: // Maybe while
				if(currChar == 'e')
					currState = State.WHILE;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case WHILE: // Maybe while
				// Prevent char from being consumed
				shouldSave = false;
				restoreChar();
				
				if(!Character.isLetterOrDigit(currChar))
				{
					token.setType(Token.Type.WHILE);
					currState = State.DONE;
				}
				else
				{
					// Not really a while
					currState = State.IN_ID;
				}
				break;

			case R: // Maybe return
				if(currChar == 'e')
					currState = State.RE;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case RE: // Maybe return
				if(currChar == 't')
					currState = State.RET;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case RET: // Maybe return
				if(currChar == 'u')
					currState = State.RETU;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case RETU: // Maybe return
				if(currChar == 'r')
					currState = State.RETUR;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case RETUR: // Maybe return
				if(currChar == 'n')
					currState = State.RETURN;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case RETURN: // Maybe return
				// Prevent char from being consumed
				shouldSave = false;
				restoreChar();

				if(!Character.isLetterOrDigit(currChar))
				{
					// End of return
					token.setType(Token.Type.RETURN);
					currState = State.DONE;
				}
				else
				{
					// Not actually a return
					currState = State.IN_ID;
				}
				break;

			case E: // Maybe else
				if(currChar == 'l')
					currState = State.EL;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case EL: // Maybe else
				if(currChar == 's')
					currState = State.ELS;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case ELS: // Maybe else
				if(currChar == 'e')
					currState = State.ELSE;
				else
				{
					currState = State.IN_ID;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case ELSE: // Maybe else
				// Prevent char from being consumed
				shouldSave = false;
				restoreChar();

				if(!Character.isLetterOrDigit(currChar))
				{
					token.setType(Token.Type.ELSE);
					currState = State.DONE;
				}
				else
				{
					// Not really an else
					currState = State.IN_ID;
				}
				break;

			case IN_LESS_THAN: // < or <=
				if(currChar == '=')
				{
					// <= op complete
					token.setType(Token.Type.LESS_THAN_EQUAL);
					currState = State.DONE;
				}
				else
				{
					// Just a <
					token.setType(Token.Type.LESS_THAN);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;
				
			case IN_GREATER_THAN: // > or >=
				if(currChar == '=')
				{
					// >= op complete
					token.setType(Token.Type.GREATER_THAN_EQUAL);
					currState = State.DONE;
				}
				else
				{
					// Just a <
					token.setType(Token.Type.GREATER_THAN);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_NOT_EQUAL: // ! or !=
				if(currChar == '=')
				{
					// != op complete
					token.setType(Token.Type.NOT_EQUAL);
					currState = State.DONE;
				}
				else
				{
					// Just a !
					token.setType(Token.Type.NOT);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_COMMENT_START:
				if(currChar == '*')
					currState = State.IN_MULTI_COMMENT;
				else if(currChar == '/')
					currState = State.IN_LINE_COMMENT;
				else
				{
					// Must be a divide op
					token.setType(Token.Type.DIVIDE);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_LINE_COMMENT:
				if(currChar == '\n' || currChar == 0)
				{
					// Reached EOF without hitting the end of the comment
					tokenSB = new StringBuilder();
					shouldSave = false;
					currState = State.START;

					// No need to prevent char from being consumed as we are just
					// going to dump it anyway. Helps performance, no need to do
					// extra seeking in file to restore context of previous line
					//restoreChar();
					shouldSave = false;
				}
				break;

			case IN_MULTI_COMMENT:
				if(currChar == '*')
				{
					// Might have reached the end of the comment
					currState = State.IN_COMMENT_END;
				}
				else if(currChar == 0)
				{
					// Reached EOF without hitting the end of the comment
					token.setType(Token.Type.ERROR);
					token.setDescription("Unterminated comment");
					currState = State.DONE;

					// Prevent char from being consumed
					// Note we don't unget because that will throw an exception (invalid line)
					// and there's no need since getNextChar() will always return EOF anyway
					shouldSave = false;
				}
				break;

			case IN_COMMENT_END:
				if(currChar == '/')
				{
					// Done with comment. Don't return token for it, ignore
					tokenSB = new StringBuilder();
					shouldSave = false;
					currState = State.START;
				}
				else if(currChar != '*')
				{
					// False start on, well, the end.
					currState = State.IN_MULTI_COMMENT;
				}
				else if(currChar == 0)
				{
					// Reached EOF without hitting the end of the comment
					token.setType(Token.Type.ERROR);
					token.setDescription("Unterminated comment");
					currState = State.DONE;

					// Prevent char from being consumed
					// Note we don't unget because that will throw an exception (invalid line)
					// and there's no need since getNextChar() will always return EOF anyway
					shouldSave = false;
				}
				break;

			case IN_INCREMENT:
				if(currChar == '+')
				{
					// Increment op complete
					token.setType(Token.Type.INCREMENT);
					currState = State.DONE;
				}
				else
				{
					// Just an addition
					token.setType(Token.Type.PLUS);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_DECREMENT:
				if(currChar == '-')
				{
					// Decrement op complete
					token.setType(Token.Type.DECREMENT);
					currState = State.DONE;
				}
				else
				{
					// Just a subtraction
					token.setType(Token.Type.MINUS);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_ASSIGNMENT:
				currState = State.DONE;
				if(currChar == '=')
				{
					// Actually an equals sign
					token.setType(Token.Type.EQUAL);
				}
				else
				{
					token.setType(Token.Type.ASSIGNMENT);

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_NUMBER:
				if(Character.isLetter(currChar))
				{
					// Illegal, ID and num not separated
					token.setType(Token.Type.ERROR);
					token.setDescription("Number followed by identifier with no separating charaters");
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				else if(!Character.isDigit(currChar))
				{
					// Done with number
					token.setType(Token.Type.NUMBER);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case IN_ID:
				if(Character.isDigit(currChar))
				{
					// Illegal, ID and num not separated
					token.setType(Token.Type.ERROR);
					token.setDescription("Identifier followed by number with no separating charaters");
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				else if(!Character.isLetter(currChar))
				{
					// Done with identifier
					token.setType(Token.Type.IDENTIFIER);
					currState = State.DONE;

					// Prevent char from being consumed
					shouldSave = false;
					restoreChar();
				}
				break;

			case DONE:
				throw new ScannerException("The DONE state was encountered in the Scanner switch");
				
			default:
				throw new ScannerException("An unknown state was encountered in the Scanner switch");
			}

			// Is this character part of the token?
			if(shouldSave)
				tokenSB.append(currChar);
		}

		// Put the token data in. The SB builder was faster than doing
		// repeatedly assigning, retrieving, and changing
		if(tokenSB.length() > 0)
			token.setData(tokenSB.toString());

		// If this is an error token then throw an exception
		// If we don't, then we have to handle error tokens everywhere in the parser
		// Well, either that or it just doesn't handle all the nice error info
		// that we made here
		if(token.getType() == Token.Type.ERROR)
			throw new ScannerException(token);

		return token;
	}

	private char getNextChar() throws ScannerException
	{
		try
		{
			// Get line we if we haven't yet
			if(currLine == null)
				currLine = getCurrentLine();

			// Get next character. Work with Windows by ignoring carriage return
			currCharNum++;
			char nextChar = (char)inFile.readByte();
			if(nextChar == '\r')
				nextChar = (char)inFile.readByte();

			// New line? Need to keep our debug info current
			if(nextChar == '\n')
			{
				currLineNum++;
				currLine = getCurrentLine();
				currCharNum = 1;
			}

			return nextChar;
		}
		catch(EOFException ex)
		{
			// Reached end of file (duh). Return special char for that
			return 0;
		}
		catch(IOException ex)
		{
			throw new ScannerException("Unable to get next character: " + ex.getMessage());
		}
	}

	private void restoreChar() throws ScannerException
	{
		try
		{
			// Move to previous character
			currCharNum--;
			long newPos = inFile.getFilePointer() - 1;
			inFile.seek(newPos);

			// Did we cross a line?
			char prevChar = (char) inFile.readByte();
			if(prevChar == '\n')
			{
				currLineNum--;
				currLine = getCurrentLine();
				currCharNum = currLine.length();
			}

			// And go back to where we were (actually just one ahead)
			inFile.seek(newPos);
		}
		catch(IOException ex)
		{
			throw new ScannerException("Unable to back up a character: " + ex.getMessage());
		}
	}

	private String getCurrentLine() throws ScannerException
	{
		try
		{
			// Current position so we can come back to it
			long pos = inFile.getFilePointer();
			long linePos = pos;

			// Back up to beginning of line
			while(inFile.getFilePointer() != 0
					&& inFile.getFilePointer() != inFile.length())
			{
				// Are we at a newline char?
				char currChar = (char)inFile.readByte();
				if(currChar == '\n' || currChar == '\r')
				{
					break;
				}

				// Go back another char
				linePos--;
				inFile.seek(linePos);
			}

			// Get line and dump whitespace
			String line = inFile.readLine();
			if(line != null)
				line = line.trim();
			else
				line = "";

			// Go back to where we were
			inFile.seek(pos);

			return line;
		}
		catch(IOException ex)
		{
			throw new ScannerException("Unable to get current line: " + ex.getMessage());
		}
	}
}
