package raim;

public interface RAIMAPIMessageHandler
{
	/**
	 * Called when a message is received from the server
	 * @param from who the message is from
	 * @param to who the message is to (should always be yourself)
	 * @param msg what the message is
	 */
	public void messageReceivedHandler(String from, String to, String msg);
}
