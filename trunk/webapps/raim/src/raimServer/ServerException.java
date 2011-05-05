package raimServer;

public class ServerException extends Exception
{
    public ServerException()
	{
    }

    /**
     * Constructs an instance of <code>ServerException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public ServerException(String msg)
	{
        super(msg);
    }
}