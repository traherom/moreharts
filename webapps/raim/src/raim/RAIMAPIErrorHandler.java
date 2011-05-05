package raim;

public interface RAIMAPIErrorHandler
{
	/**
	 * Called by the API when it has an error message that may need to be reported
	 * @param msg message to report
	 */
	public void apiErrorHandler(String msg);
}
