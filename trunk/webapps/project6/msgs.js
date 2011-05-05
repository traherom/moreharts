// Defaults/statuses
window.defaultMsg = "Enter new message";
window.isSubmitting = false;
window.isChecking = false;
window.latestMsg = '';

function getXMLHttpRequest()
{
	try
	{
		return new XMLHttpRequest();
	}
	catch(e)
	{
		try
		{
			return new ActiveXObject("Msxml2.XMLHTTP");
		}
		catch(e)
		{
			try
			{
				return new ActiveXObject("Microsoft.XMLHTTP");
			}
			catch(e)
			{
				alert("Unable to get request object to update messages");
			}
		}
	}
}

function getLatestMsg()
{
	// Don't check if we already are trying
	if(window.isChecking)
		return;

	// Ok, note we're now trying
	window.isChecking = true;

	// Get request object
	// Doing this by hand for the purposes of class
	// I'm more than aware thet jquery does it
	var req = getXMLHttpRequest();
	req.onreadystatechange = function() {
		if(req.readyState == 4)
		{
			// If there isn't a new message then ignore it
			if(req.responseText != window.latestMsg)
			{
				// Push old message into log
				var oldMsg = window.latestMsg;
				if(oldMsg != '')
				{
					$("#oldMsgs").fadeOut(function() {
						$("#oldMsgs").prepend($("<div/>").html(oldMsg))
									 .fadeIn();
					});
				}

				// Put in new message
				window.latestMsg = req.responseText;
				$("#latestMsg").fadeOut(function() {
					$("#latestMsg").html(window.latestMsg).fadeIn();		
				});
			}

			// All done with this fetch, we're allowed to run again
			window.isChecking = false;
		}
	};
	req.open('GET', 'reader.php', true);
	req.send(null);
}

function submitNew(newMsg)
{
	// Get request object
	// Doing this by hand for the purposes of class
	// I'm more than aware thet jquery does it
	var req = getXMLHttpRequest();
	req.onreadystatechange = function() {
		if(req.readyState == 4)
		{
			if(req.responseText == '')
			{
				// Let button appear again
				window.isSubmitting = false;
				$("#newMsg").val('').blur();
			}
			else
			{
				// error occurred
				alert(req.responseText);
			}
		}
	};
	req.open('GET', 'saver.php?msg=' + $("#newMsg").val(), true);
	req.send(null);
}

// Bind everything and fill initial
$(document).ready(function() {
	// Get newest message frequently
	setInterval(getLatestMsg, 500);
	getLatestMsg();

	// Tell user where to put their stuff
	$("#newMsg").addClass('default').val(window.defaultMsg);

	// Look for keypresses anywhere but in the text box
	$("#newMsg").bind('keypress', function(e) {
		e.stopPropagation();
	});
	$(document).bind('keypress', function(e) {
		// Don't handle non printable (based on standard ascii)
		if(e.which < 32 || e.which > 126)
			return;

		// Allow the usual handler clear/non-default class us
		var oldMsg = $("#newMsg").focus().val();

		// Put the key in after a pause. This pause lets
		// us check to see if the browser already did it for us...
		// Chrome is the offender here
		var key = String.fromCharCode(e.which);
		setTimeout(function() {
			var newMsg = $("#newMsg");
			if(newMsg.val() == oldMsg)
				newMsg.val(newMsg.val() + key);
		}, 1);
	});

	// Clear default message from text box when clicked the first time
	$("#newMsg").bind('focus', function() {
		var newMsg = $("#newMsg");
		newMsg.removeClass('default');
		if(newMsg.val() == window.defaultMsg)
			newMsg.val('');
	})

	// Put "post here" type message back in if the user doesn't enter anything
	.bind('blur', function() {
		if($("#newMsg").val() == '')
		{
			$("#newMsg").addClass('default').val(window.defaultMsg);

			// Hide the prompt button
			$("#submitPrompt").stop().fadeOut();
		}
		else if(!window.isSubmitting)
			$("#submitPrompt").fadeIn();
	});

	// Post message when user hits enter
	$("#post").bind('submit', function(e) {
		// Stop the prompt button from appearing when they blur off of
		// the text box they're in
		window.isSubmitting = true;

		// Only submit if the box actually has stuff
		var msg = $("#newMsg").val();
		if(msg != '')
			submitNew(msg);

		// Reset the box to the default
		$("#newMsg").val('').blur();

		// Don't actually submit form
		e.preventDefault();
	});
});

