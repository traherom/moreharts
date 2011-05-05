/*****************************************************************************
 * Author: Alex Laird
 * Date: January 20th, 2010
 *
 * This script is referenced from any pages that desire the ability to launch a link in a new
 * window while remaining compliant with XHTML Strict standards. After including this script
 * in <head> with <script type="text/javascript" src="/external.js"></script>, simply append a
 * rel="external" tag to a link to use this script.
 ****************************************************************************/

function externals()
{
	// ensure elements exist in the document before continuing
   if(document.getElementsByTagName)
	{
		// retrieve a list of all link elements
		var anchors = document.getElementsByTagName("a");
		
		// iterate through all linnk elements, anchoring an external target to them
		// if they contain the tag rel="external"
		for (var i = 0; i < anchors.length; ++i)
		{
			var anchor = anchors[i];  
			if(anchor.getAttribute("href") && anchor.getAttribute("rel") == "external")
			{
				anchor.target = "_blank";
			}
		}
	}
}
