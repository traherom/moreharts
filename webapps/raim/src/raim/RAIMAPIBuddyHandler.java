package raim;

public interface RAIMAPIBuddyHandler
{
	/**
	 * Called when a buddy comes online
	 * @param user buddy who came online
	 */
	public void buddyOnlineHandler(String user);

	/**
	 * Called when a buddy goes offline
	 * @param user buddy who went offline
	 */
	public void buddyOfflineHandler(String user);
}
