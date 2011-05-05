package raimServer;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.InputMismatchException;
import java.util.NoSuchElementException;
import java.util.Scanner;

class ClientConnection implements Runnable
{
	private final Thread clientListenerThread;
	private final Socket socket;
	private final Scanner reader;
	private final PrintWriter writer;

	private final Listener parent;

	private String user;
	private boolean notifyOnLogout;

	public ClientConnection(Listener parent, Socket socket) throws ServerException
	{
		// Save parent so we know who to report events to
		notifyOnLogout = true;
		this.parent = parent;

		// Get readers and writers
		try
		{
			this.socket = socket;
			reader = new Scanner(socket.getInputStream());
			writer = new PrintWriter(socket.getOutputStream(), true);
		}
		catch(IOException ex)
		{
			throw new ServerException("Unable to save socket for reading/writing to client: "
					+ ex.getMessage());
		}

		// Start ourselves threading :)
		clientListenerThread = new Thread(this);
		clientListenerThread.start();
	}

	public void run()
	{
		System.out.println("Client connected");

		try
		{
			// Keep talking to client until they tell us to shut up
			while(!socket.isClosed())
			{
				// Get next command from client
				int command = 0;
				try
				{
					command = reader.nextInt();
				}
				catch(InputMismatchException ex)
				{
					send("Bad command passed to server");
					reader.nextLine();
					continue;
				}

				String userCheck = null;
				String password = null;
				switch(command)
				{
				case 0: // Create account
					userCheck = reader.next();
					password = reader.nextLine().trim();
					try
					{
						if(!parent.createUser(userCheck, password))
						{
							send("Unable to create new user, maybe duplicate user name");
						}
					}
					catch(ServerException ex)
					{
						System.out.println("Error while creating user:" + ex.getMessage());
					}
					break;

				case 1: // Logon
					userCheck = reader.next();
					password = reader.next();
					try
					{
						parent.loginUser(this, userCheck, password);
					}
					catch(ServerException ex)
					{
						System.out.println("Error during login: " + ex.getMessage());
					}
					break;

				case 2: // Logoff
					userCheck = reader.next();
					if(isLoggedIn())
					{
						// Pointless check
						if(!userCheck.equals(user))
						{
							send("Just for future reference, pass your user name to logoff. "
									+ "I'll do it this time though");
						}
					}

					// Disconnect
					socket.close();
					break;

				case 3: // Incoming message
					String from = reader.next();
					String to = reader.next();
					String message = reader.nextLine();

					// Check
					if(!isLoggedIn())
					{
						send("Unable to send message, you are not logged in");
						break;
					}
					if(!from.equals(user))
					{
						send("Attempt to send message as a user other than yourself. Violation logged");
						System.out.println("SECURITY: " + user + " attempted to send message as " + from);
						break;
					}
					if(message.trim().isEmpty())
					{
						// Silently ignore empty messages
						break;
					}

					// Attempt to send
					if(!parent.sendTo(from, to, message))
					{
						send(to + " is not logged in, unable to send message");
					}
					break;
					
				default: // Unknown/not valid to send to server
					System.out.println("Bad command from " + user + ": " + command);
					send("Unrecognized command: " + command);

					// Munch rest of line
					reader.nextLine();
				}
			}
		}
		catch(IOException ex)
		{
			System.err.println("Fatal error in communication with client: "
					+ ex.getMessage());
		}
		catch(NoSuchElementException ex)
		{
			System.out.println("Inproper disconnect from " + user);
		}
		finally
		{
			// Log off user if needed
			if(notifyOnLogout && isLoggedIn())
			{
				parent.logoutUser(user);
			}

			// Remove from server list
			parent.removeClient(this);

			// Close actual sockets
			try
			{
				System.out.println("Client disconnected");
				reader.close();
				writer.close();
				socket.close();
			}
			catch(IOException ex)
			{
				System.out.println("Unable to cleanly close client connection");
			}
		}
	}

	/**
	 * Checks whether the user is logged in
	 * @return returns whether this user is logged in
	 */
	public boolean isLoggedIn()
	{
		return user != null;
	}

	/**
	 * Returns the user name of this user, assuming they're logged in
	 * @return user name or null if not logged in
	 */
	public String getUser()
	{
		return user;
	}

	/**
	 * Sends a message to this user from the server
	 * @param msg message to send to user
	 */
	public void send(String msg)
	{
		send("server", msg);
	}

	/**
	 * Sends a message to this user
	 * @param from who the message is from
	 * @param msg what the message is
	 */
	public void send(String from, String msg)
	{
		writer.println("3 " + from + " " + user + " " + msg.trim());
	}

	/**
	 * Called by main server to tell us about success/failure of a login
	 * @param success whether we succeeded or not
	 * @param name user name we attempted to login with
	 */
	public void notifyLogin(boolean success, String name)
	{
		// The requested login by this user either succeeded or didn't
		if(success)
		{
			// Sucess
			user = name;
			writer.println("6 " + name);
		}
		else
		{
			// Failed
			user = null;
			writer.println("7 " + name);
		}
	}

	/**
	 * Called by main server to tell us a user went on/offline
	 * @param changedUser person with the changed status
	 * @param online whether they're online or off now
	 */
	public void notifyUser(String changedUser, boolean online)
	{
		// Some has logged in or out, tell this user about it
		if(online)
		{
			writer.println("4 " + changedUser);
		}
		else
		{
			writer.println("5 " + changedUser);
		}
	}

	/**
	 * Closes connection to client (used by server to force disconnect)
	 * @param notify whether we should tell the server that we are now logged otu
	 */
	public void close(boolean notify) throws ServerException
	{
		try
		{
			notifyOnLogout = notify;
			socket.close();
		}
		catch(IOException ex)
		{
			throw new ServerException("Unable to close socket to client");
		}
	}
}