import java.sql.Connection;
import java.sql.SQLException;
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

/** 
* Handles all user related info, including getting user information,
* creating, deleting, marking as admin, and checking login
*/
public class User extends DBServlet
{
	/**
	* Handles request for login check
	*/
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		startJSON(request, response);

		// Call appropriate function
		String action = request.getServletPath();
		if(action.equals("/LoginCheck"))
			checkLogin(request, response);
		else if(action.equals("/GetUserList"))
			getUserList(request, response);
		else if(action.equals("/IsLoggedIn"))	
			isLoggedIn(request, response);
		else if(action.equals("/ChangePassword"))
			updateUser(request, response);
		else if(action.equals("/CreateUser"))
			createUser(request, response);
		else if(action.equals("/DeleteUser"))
			deleteUser(request, response);
		else if(action.equals("/GetUser"))
			getUser(request, response);

	// IsLoggedIn
	// ChangePassword
	// CreateUser
	// DeleteUser

	// Team stuff

		endJSON(request, response);
	}

	private void checkLogin(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		out.println("\"success\": true");
	}

	private void getUserList(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		for(int i = 0; i < 5; i++)
			out.println("\"" + i + "\": {\"id\": 5, \"user\": \"dummy\"},");
	}

	private void isLoggedIn(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		out.println("\"userID\": 5");
	}

	private void updateUser(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		out.println("\"success\": true");
	}

	private void createUser(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		out.println("\"success\": true, \"userID\": 6");
	}

	private void deleteUser(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		out.println("\"success\": true");
	}

	private void getUser(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		out.println("\"userID\": 5, \"name\": \"dummy\", \"isAdmin\": false, \"email\": \"morehart@gmail.com\"");
	}
}
