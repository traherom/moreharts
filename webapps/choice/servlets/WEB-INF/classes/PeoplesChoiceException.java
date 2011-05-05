import javax.servlet.*;

public class PeoplesChoiceException extends ServletException
{
    public PeoplesChoiceException()
	{
    }

    public PeoplesChoiceException(String msg)
	{
        super(msg);
    }
}
