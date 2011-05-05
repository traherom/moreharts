import java.sql.Connection;
import java.sql.SQLException;
import java.sql.DriverManager;
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

/** 
* Handles all user related info, including getting user information,
* creating, deleting, marking as admin, and checking login
*/
public abstract class DBServlet extends HttpServlet
{
	protected Connection db;

	/**
	* Builds connection to database so we don't have to on each call
	*/
	public void init(ServletConfig config) throws ServletException
	{
		super.init(config);

		try
		{
			// Load JDBC MySQL driver
			Class.forName("com.mysql.jdbc.Driver");
		}
		catch(ClassNotFoundException ex)
		{
			throw new PeoplesChoiceException("Unable to load MySQL driver");
		}

		// Get configuration information
		ServletContext context = getServletContext();
		String url = context.getInitParameter("jdbcConnectionURL");

		try
		{
			// Attempt to connect
			db = DriverManager.getConnection(url);
		}
		catch(SQLException ex)
		{
			//throw new PeoplesChoiceException("Unable to connect to database using the connection URL " + url + ex.getMessage());
		}
	}

	/**
	* Tears down connection to database.
	*/
	public void destroy()
	{
		super.destroy();

		try
		{
			db.close();
		}
		catch(SQLException e)
		{
			// Silently ignore for now. No good way to report this
		}
	}

	/**
	* Let both GET and POST work correctly
	*/
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		doGet(request, response);
	}

	/**
	* Initializes for JSON output
	*/
	public void startJSON(HttpServletRequest request, HttpServletResponse response) throws IOException
	{
		// JSON type
		response.setContentType("application/json");

		// Beginning of wrapper for JSON
		PrintWriter out = response.getWriter();
		if(request.getParameter("callback") != null)
		{
			out.print(request.getParameter("callback"));
			out.print("(");
		}
		out.println("{");
	}

	/**
	* Finishes a JSON output
	*/
	public void endJSON(HttpServletRequest request, HttpServletResponse response) throws IOException
	{
		PrintWriter out = response.getWriter();
		out.println("}");
		if(request.getParameter("callback") != null)
		{
			out.print(")");
		}
	}
}
