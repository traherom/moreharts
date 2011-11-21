// Check for password fields and tell manager about each one
var fields = $("input[type=password]");
if(fields.size() > 0) {
	console.log("Requesting password");
	chrome.extension.sendRequest({type: "get_pw"}, function(response) {
		if(response.success) {
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
}

