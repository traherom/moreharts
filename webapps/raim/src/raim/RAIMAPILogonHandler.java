package raim;

public interface RAIMAPILogonHandler
{
	/**
	 * Called when the server returns that we successfully logged in
	 * @param user user we are logged in as
	 */
	public void loginSucceededHandler(String user);

	/**
	 * Called when the server returns that we failed to login correctly
	 * @param user user we attempted to login as
	 */
	public void loginFailedHandler(String user);
}
