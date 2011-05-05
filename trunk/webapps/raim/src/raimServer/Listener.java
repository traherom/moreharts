package raimServer;

import com.mysql.jdbc.PreparedStatement;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.ServerSocket;
import java.net.Socket;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Vector;

public class Listener
{
	private ServerSocket socket;
	private Connection db;
	private Vector<ClientConnection> clients = new Vector<ClientConnection>();

	/**
	 * Default settings for server
	 */
	public Listener() throws ServerException, ServerException
	{
		this(4220, "jdbc:mysql://john.cedarville.edu/cs4220?user=cs4220");
	}

	/**
	 * Specify everything for server
	 */
	public Listener(int port, String connURL) throws ServerException
	{
		try
		{
			// Open socket
			System.out.println("Opening port");
			socket = new ServerSocket(port);
		}
		catch(IOException ex)
		{
			throw new ServerException("Unable to listen: " + ex.getMessage());
		}

		try
		{
			// Load JDBC MySQL driver
			System.out.println("Loading database driver");
			Class.forName("com.mysql.jdbc.Driver");
		}
		catch(ClassNotFoundException ex)
		{
			throw new ServerException("Unable to load database driver");
		}

		try
		{
			// Attempt to connect
			System.out.println("Connecting to database");
			db = DriverManager.getConnection(connURL);
		}
		catch(SQLException ex)
		{
			throw new ServerException("Unable to connect to database");
		}

		System.out.println("Initialization complete");
	}

	/**
	 * Close server connection
	 */
	protected void finalize() throws ServerException
	{
		try
		{
			// Close socket
			socket.close();
			db.close();
		}
		catch(SQLException ex)
		{
			throw new ServerException("Unable to close connection to database: " + ex.getMessage());
		}
		catch(IOException ex)
		{
			throw new ServerException("Unable to close listening port: " + ex.getMessage());
		}
	}

	/**
	 * Sits around waiting for connections and spawns off babies.
	 * Children, rather. Children.
	 */
	public void listen() throws ServerException
	{
		try
		{	
			// Just keep watching for new connections
			while(true)
			{
				System.out.println("Waiting for new client");
				Socket newSocket = socket.accept();

				// Spawn off new thread to talk to them
				ClientConnection newComer = new ClientConnection(this, newSocket);
				clients.add(newComer);
			}
		}
		catch(IOException ex)
		{
			throw new ServerException(ex.getMessage());
		}
	}

	/**
	 * Checks a user's login info against the database and informs friends when they come online
	 * @param child client connection requesting login
	 * @param user user name
	 * @param password password (md5()'d already or not)
	 * @return true if login was a success, false otherwise
	 * @throws ServerException
	 */
	public boolean loginUser(ClientConnection child, String user, String password) throws ServerException
	{
		try
		{
			// Check against database
			// MD5 password if needed. Has to be checked in case someone else's client is
			// using us
			PreparedStatement stmt = (PreparedStatement) db.prepareStatement("SELECT 1 FROM ar_im_user " + "WHERE UserName=? AND Password=?");
			stmt.setString(1, user);
			if(password.length() != 32)
				stmt.setString(2, md5(password));
			else
				stmt.setString(2, password);

			ResultSet rs = stmt.executeQuery();
			boolean success = rs.next();
			rs.close();
			stmt.close();

			// Tell client about result
			child.notifyLogin(success, user);
			if(success)
			{
				// Tell this user about all their friends on and
				// notify anyone who is this person's friend that they're on
				// If they were already logged in, kill the existing one
				for(ClientConnection other : clients)
				{
					// Don't tell ourselves about our login
					if(other == child)
						continue;

					// Is it another login from us?
					if(user.equals(other.getUser()))
					{
						// Kill it
						other.send("You have been logged in from another "
								+ "location and will be disconnected here.");
						other.close(false);
						child.send("You have been logged out from the other location");
						System.out.println("Duplicate login for " + user + ", disconnecting other");
						break; // No need to tell everyone about us again, they already know
					}

					if(other.isLoggedIn())
					{
						// Tell other person we're online
						other.notifyUser(user, true);

						// Tell child other person is online
						child.notifyUser(other.getUser(), true);
					}
				}

				// Login succeeded
				System.out.println(user + " logged in");
				return true;
			}
			else
			{
				System.out.println("Failed login attempt by " + user);
				return false;
			}
		}
		catch(SQLException ex)
		{
			throw new ServerException("Error logging user in: " + ex.getMessage());
		}
	}

	/**
	 * Tells everyone else this user has logged out
	 * @param user user name of the person being logged out
	 */
	public void logoutUser(String user)
	{
		System.out.println(user + " logged out");

		// Notify anyone who is this person's friend that they're now off
		for(ClientConnection other : clients)
		{
			other.notifyUser(user, false);
		}
	}

	/**
	 * Creates a new user in the database
	 * @param user new user name
	 * @param password new password
	 * @return true if creation succeeds, false otherwise
	 */
	public boolean createUser(String user, String password) throws ServerException
	{
		try
		{
			// User name must just be letters and numbers
			user = user.trim();
			if(!user.matches("[a-zA-Z0-9]+"))
				return false;

			// Password can't be blank
			if(password.isEmpty())
				return false;

			java.sql.PreparedStatement stmt = db.prepareStatement("INSERT INTO ar_im_user (UserName, Password) " + "VALUES (?, ?)");
			stmt.setString(1, user);
			if(password.length() != 32)
				stmt.setString(2, md5(password));
			else
				stmt.setString(2, password);

			int affected = stmt.executeUpdate();
			stmt.close();

			// Return success
			if(affected == 1)
			{
				System.out.println("Created user " + user);
				return true;
			}
			else
			{
				System.out.println("Unable to create user " + user);
				return false;
			}
		}
		catch(SQLException ex)
		{
			throw new ServerException("Unable to create user: " + ex.getMessage());
		}
	}

	/**
	 * Sends a message from one user to another.
	 * @param from user message is from
	 * @param to user message is to
	 * @param message message to send
	 * @return true if message sent, false otherwise
	 */
	public boolean sendTo(String from, String to, String message)
	{
		// Find client with this user name and send to them
		for(ClientConnection client : clients)
		{
			if(to.equals(client.getUser()))
			{
				client.send(from, message);
				return true;
			}
		}

		// Didn't find the person
		return false;
	}

	/**
	 * Removes a client connection that is no longer valid from our list so we don't keep checknig it
	 * @param c client to be removed
	 */
	public void removeClient(ClientConnection c)
	{
		if(clients.remove(c))
			System.out.println("Client removed from list");
		else
			System.out.println("Client requested to be removed from list, but no record found");
	}

	/**
	 * Utility function to MD5 passwords
	 * @param plain plain-text version of thing to be hashed
	 */
	private String md5(String plain) throws ServerException
	{
		try
		{
			byte[] encoded = MessageDigest.getInstance("MD5").digest(plain.getBytes("UTF-8"));

			// Partially stolen from http://www.anyexample.com/programming/java/java_simple_class_to_compute_md5_hash.xml
			StringBuilder sb = new StringBuilder();
			for(int i = 0; i < encoded.length; i++)
			{
				int halfbyte = (encoded[i] >>> 4) & 0x0F;
				int two_halfs = 0;
				do
				{
					if((0 <= halfbyte) && (halfbyte <= 9))
						sb.append((char) ('0' + halfbyte));
					else
						sb.append((char) ('a' + (halfbyte - 10)));
					halfbyte = encoded[i] & 0x0F;
				} while(two_halfs++ < 1);
			}
			return sb.toString();
		}
		catch(NoSuchAlgorithmException ex)
		{
			throw new ServerException("Something went horribly wrong. Java doesn't know the MD5 algorithm.");
		}
		catch(UnsupportedEncodingException ex)
		{
			throw new ServerException("Something went horribly wrong. Java doesn't know the text encoding format given.");
		}
	}

	/**
	 * Parses arguments and starts server accordingly
	 * @param args
	 */
	public static void main(String[] args)
	{
		try
		{
			System.out.println("Creating listener");
			Listener l = new Listener();
			
			l.listen();

			System.out.println("Finished");
		}
		catch(ServerException ex)
		{
			System.err.println("Fatal server error: " + ex.getMessage());
			ex.printStackTrace();
		}
	}
}
