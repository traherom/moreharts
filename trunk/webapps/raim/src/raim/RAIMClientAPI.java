package raim;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.InputMismatchException;
import java.util.NoSuchElementException;
import java.util.Scanner;
import java.util.Vector;

/**
 *
 * @author Alex Laird
 * @author Ryan Morehart
 */
public class RAIMClientAPI implements Runnable
{
	private Thread listenerThread;
	private Socket socket;
	private Scanner reader;
	private PrintWriter writer;

	private Vector<RAIMAPIBuddyHandler> buddyHandlers = new Vector<RAIMAPIBuddyHandler>();
	private Vector<RAIMAPIErrorHandler> errorHandlers = new Vector<RAIMAPIErrorHandler>();
	private Vector<RAIMAPILogonHandler> logonHandlers = new Vector<RAIMAPILogonHandler>();
	private Vector<RAIMAPIMessageHandler> msgHandlers = new Vector<RAIMAPIMessageHandler>();
	private Vector<RAIMAPIDisconnectHandler> disconnectHandlers = new Vector<RAIMAPIDisconnectHandler>();

	private String user;

	/**
	 * Creates connection with default settings, assumes server
	 * is on localhost
	 * @throws RAIMException
	 */
	public RAIMClientAPI() throws RAIMException
	{
		this("localhost", 4220);
	}

	/**
	 * Creates connection on default port
	 * @param host host the server is on
	 * @throws RAIMException
	 */
	public RAIMClientAPI(String host) throws RAIMException
	{
		this(host, 4220);
	}

	/**
	 * Creates a connection to the given server
	 * @param host Host to connect to
	 * @param port Port on host
	 * @throws RAIMException
	 */
	public RAIMClientAPI(String host, int port) throws RAIMException
	{
		// Get readers and writers
		try
		{
			socket = new Socket(host, port);
			reader = new Scanner(socket.getInputStream());
			writer = new PrintWriter(socket.getOutputStream(), true);

			// Start up the listener, which waits for the server to tell us stuff
			listenerThread = new Thread(this);
			listenerThread.start();
		}
		catch(IOException ex)
		{
			throw new RAIMException("Unable to connect to server: " + ex.getMessage());
		}
	}

	/**
	 * Asks the server to create a new user
	 * @param user user name for new user
	 * @param password password for new user
	 */
	public void createUser(String user, String password)
	{
		writer.println("0 " + user + " " + password);
	}

	/**
	 * Attempts to login the given user. The callback on client will be called when the server
	 * returns to us.
	 * @param user user name
	 * @param password password
	 */
	public void login(String user, String password)
	{
		// Send in plaintext for now. Later add a version check and use encrypted for
		// when we're connected to our own server
		writer.println("1 " + user + " " + password);
	}

	/**
	 * Logs out current user
	 */
	public void logout()
	{
		writer.println("2 " + user);
		user = null;
	}

	/**
	 * Sends a message from the current user to the one given
	 * @param to who to send to
	 * @param message message to send
	 */
	public void send(String to, String message)
	{
		writer.println("3 " + user + " " + to + " " + message);
	}

	/**
	 * Return if this API is logged in
	 * @return true if logged in, false otherwise
	 */
	public boolean isLoggedIn()
	{
		return !(user == null);
	}

	/**
	 * User this API is using, null if not logged in
	 * @return user or null if not logged in
	 */
	public String getUser()
	{
		return user;
	}

	/**
	 * Adds a handler for messages
	 * @param handler
	 */
	public void addMessageHandler(RAIMAPIMessageHandler handler)
	{
		msgHandlers.add(handler);
	}
	
	/**
	 * Adds a handler for logon success/failure
	 * @param handler
	 */
	public void addLogonHandler(RAIMAPILogonHandler handler)
	{
		logonHandlers.add(handler);
	}
	
	/**
	 * Adds a handler for buddy sign in/out
	 * @param handler
	 */
	public void addBuddyHandler(RAIMAPIBuddyHandler handler)
	{
		buddyHandlers.add(handler);
	}

	/**
	 * Adds a handler for server disconnects
	 * @param handler
	 */
	public void addDisconnectHandler(RAIMAPIDisconnectHandler handler)
	{
		disconnectHandlers.add(handler);
	}
	
	/**
	 * Adds a handler for errors
	 * @param handler
	 */
	public void addErrorHandler(RAIMAPIErrorHandler handler)
	{
		errorHandlers.add(handler);
	}

	/**
	 * Tells each error handler about the error
	 * @param error error message to pass off
	 */
	private void broadcastError(String error)
	{
		for(RAIMAPIErrorHandler handler : errorHandlers)
		{
			handler.apiErrorHandler(error);
		}
	}

	/**
	 * Listens for messages from server and hands off to client handlers
	 */
	public void run()
	{
		try
		{
			// Keep talking to server until they disconnect
			while(!socket.isClosed())
			{
				// Get next command from server
				int command = 0;
				try
				{
					command = reader.nextInt();
				}
				catch(InputMismatchException ex)
				{
					broadcastError("Non-command from server");
					reader.nextLine();
					continue;
				}

				String param1;
				String param2;
				String msg;
				switch(command)
				{
				case 3: // Incoming message
					param1 = reader.next(); // From
					param2 = reader.next(); // To
					msg = reader.nextLine(); // Message

					// Don't bother with messages that aren't to us
					if(param2.equals(user) || (user == null && param2.equals("null")))
					{
						for(RAIMAPIMessageHandler handler : msgHandlers)
						{
							handler.messageReceivedHandler(param1, param2, msg.trim());
						}
					}
					break;

				case 4: // Buddy on
					param1 = reader.next();
					for(RAIMAPIBuddyHandler handler : buddyHandlers)
					{
						handler.buddyOnlineHandler(param1);
					}
					break;

				case 5: // Buddy off
					param1 = reader.next();
					for(RAIMAPIBuddyHandler handler : buddyHandlers)
					{
						handler.buddyOfflineHandler(param1);
					}
					break;

				case 6: // Successful login
					param1 = reader.next();
					user = param1;
					for(RAIMAPILogonHandler handler : logonHandlers)
					{
						handler.loginSucceededHandler(user);
					}
					break;

				case 7: // Failed login
					param1 = reader.next();
					user = null;
					for(RAIMAPILogonHandler handler : logonHandlers)
					{
						handler.loginFailedHandler(param1);
					}
					break;

				default: // Unknown/not valid to send to client
					broadcastError("Bad command from server: " + command);

					// Munch rest of line
					reader.nextLine();
				}
			}
		}
		catch(NoSuchElementException ex)
		{
			for(RAIMAPIDisconnectHandler handler : disconnectHandlers)
			{
				handler.serverDisconnectHandler();
			}
		}
		finally
		{
			try
			{
				reader.close();
				writer.close();
				socket.close();
			}
			catch(IOException ex)
			{
				broadcastError("Unable to cleanly close connection");
			}
		}
	}
}
