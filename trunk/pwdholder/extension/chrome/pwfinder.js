// Check for password fields and tell manager about each one
// Cache to detect a password change for saving
var pwCache = new Array();

// Find password fields
var fields = $("input[type=password]");
if(fields.size() > 0) {

	// Get password
	console.log("Requesting password");
	chrome.extension.sendRequest({type: "get_pw"}, function(response) {
		if(response.success) {
			// Save cache
			pwCache[0] = {user: response.site_user, pw: response.site_pw};
		
			// Yeah, fill each password field
			console.log("Password received");
			
			fields.each(function(index, el) {
				pwField = $(el);
				
				// Fill password
				pwField.val(response.site_pw);
			
				// Find user and fill that too
				// Recursively go up a parent and look for text input until we find one
				function searchUpDOM(node) {
					inputs = node.find("input[type=text]");
					if(inputs.size() > 0) {
						// Fill
						inputs.each(function(index, el) {
							$(el).val(response.site_user);
						});
					}
					else {
						// Keep looking
						searchUpDOM(node.parent());
					}
				}
				searchUpDOM(pwField.parent());
			});
		}
		else
			console.log("No password available");
	});
	
	// Watch for changes to username/password
	fields.closest("form").bind("submit", function() {
		form = $(this);
		chrome.extension.sendRequest({
				type: "set_pw",
				user: form.find("input[type=text]").val(),
				pw: form.find("input[type=password]").val(),
			},
			function(response) {
			
		});
	});
}

