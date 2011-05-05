import java.sql.Connection;
import java.sql.SQLException;
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

/**
* Handles all actions regarding projects, including
* creation, deletion, and opening/closing for voting
*/
public class Project extends DBServlet
{
	/**
	* Handles request for login check
	*/
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		// Typical JSON response header
		response.setContentType("text/plain");
		PrintWriter out = response.getWriter();
		out.println(db);
	// GetProjectList
	// GetProject
	// OpenProject
	// CloseProject
	// CreateProject
	// DeleteProject
	// GetUser
	// CastVote
	// GetVotes
    }
}
