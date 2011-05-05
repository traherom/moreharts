package raim;

import java.util.Scanner;

/**
 * 
 * @author traherom
 */
public class RAIMcmdLine implements RAIMAPILogonHandler,
									RAIMAPIBuddyHandler,
									RAIMAPIErrorHandler,
									RAIMAPIMessageHandler,
									RAIMAPIDisconnectHandler
{
	private Scanner in;
	private RAIMClientAPI api;
	private boolean connected;

	public RAIMcmdLine() throws RAIMException
	{
		this("localhost");
	}

	public RAIMcmdLine(String server) throws RAIMException
	{
		in = new Scanner(System.in);

		api = new RAIMClientAPI(server);
		api.addBuddyHandler(this);
		api.addDisconnectHandler(this);
		api.addLogonHandler(this);
		api.addMessageHandler(this);
		api.addErrorHandler(this);

		connected = true;
	}

	public void execute()
	{
		// Login if needed
		if(api.getUser() == null)
		{
			System.out.print("User: ");
			String user = in.nextLine();
			System.out.print("Password: ");
			String password = in.nextLine();
			api.login(user, password);
		}

		try
		{
			// Wait for login
			while(!api.isLoggedIn())
				Thread.sleep(50);
		}
		catch(InterruptedException ex)
		{
			System.exit(-1);
		}

		// Wait for user to input a message to someone
		while(connected)
		{
			System.out.print("> ");
			String to = in.next();
			if(to.equals("quit"))
			{
				api.logout();
				break;
			}
			else
			{
				String message = in.nextLine();
				api.send(to, message);
			}
		}

		System.out.println("Disconnected from server");
	}

	public void loginSucceededHandler(String user)
	{
		System.out.println("Login successful");
	}

	public void loginFailedHandler(String user)
	{
		System.out.println("Login failed");
		System.exit(0);
	}

	public void messageReceivedHandler(String from, String to, String msg)
	{
		// Display it
		System.out.println(from + ": " + msg);
	}

	public void buddyOnlineHandler(String user)
	{
		System.out.println(user + " signed in");
	}

	public void buddyOfflineHandler(String user)
	{
		System.out.println(user + " signed off");
	}

	public void serverDisconnectHandler()
	{
		connected = false;
	}

	public void apiErrorHandler(String msg)
	{
		System.out.println("Error: " + msg);
	}

	public static void main(String args[])
	{
		try
		{
			RAIMcmdLine c;
			if(args.length == 1)
			{
				c = new RAIMcmdLine(args[0]);
			}
			else
			{
				c = new RAIMcmdLine();
			}
			
			c.execute();
		}
		catch(RAIMException ex)
		{
			System.out.println(ex.getMessage());
		}
	}
}
