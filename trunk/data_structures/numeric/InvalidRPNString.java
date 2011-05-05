package numeric;

/**
* Exception that PRNCalc throws on errors in the statement
* it was trying to calculate.
*
* @author Ryan Morehart
* @version 1.0
*/
public class InvalidRPNString extends RuntimeException
{
	public InvalidRPNString(String msg)
	{
		super(msg);
	}
}

