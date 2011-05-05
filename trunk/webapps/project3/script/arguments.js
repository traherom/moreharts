/*****************************************************************************
 * Author: Alex Laird
 * Date: January 28th, 2010
 *
 * This script will split a URL apart by its arguments. It assumes that at the
 * end of the natural URL, a ? is placed. Arguments are then put in the format
 * of type=value. The user may call getArgument(type) and the value of that
 * argument will be returned.
 ****************************************************************************/

function getArgument(name)
{
	var separator = "&";
	var equivalence = "=";
	var argumentString;
	var arguments = new Array();
	
	var argumentString = window.location.search.replace(/%20/g, " ");
	var start = argumentString.indexOf("?");

	// assuming a ? is found, grab the list of arguments and split them by &
	if(start != -1)
	{
		argumentString = argumentString.substring(start + 1, argumentString.length);
		arguments = argumentString.split(separator);
	}
	
	// search through the arguments to find the specified one
	for(var i = 0; i < arguments.length; ++i)
	{
		var pieces = arguments[i].split(equivalence);
		// check the current argument against the requested one
		if (pieces[0] == name)
		{
			// return its value
			return pieces[1];
		}
	}
	
	// the specified argument was never found
	return null;
}
