// Check for password fields and tell manager about each one
var cache = null;
$("input[type=password]").each(function(index, pwField) {
	// Get passwords if this this is the first one
	if(cache == null) {
		console.log("Requesting password");
		chrome.extension.sendRequest({type: "get_pw"}, function(response) {
			if(response.success) {
				console.log("Password received");
				cache = {user: response.site_user, pw: response.site_pw};
			}
			else {
				console.log("No password available");
				cache = false;
			}
		});
	}
	
	// Fill in fields if there's a password available
	if(cache) {
		// Find related user field
		// TBD
		
		// And obviously fill in password
		pwField.val(cache.pw);
	}
	else {
		// No point in continuing to fill password fields, we don't
		// have any info
		return false;
	}
});
