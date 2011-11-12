// Transparently handles all the backend communication with server
var PwdHolder = function() {
	// In case we cached this locally and want to know the full URL
	// to send requests to
	var baseurl = "";
	
	// Status info
	var sid = null;
	
	// Public functions
	return {
		checkLogin: function(callback) {
			// Checks if we (a) have a stored session, (b) if that stored session
			// is valid still, and (c) who the session is for
			if(sid == null) {
				sid = window["localStorage"]["sid"]
				if(sid == null) {
					callback(false, "No session stored");
					return;
				}
			}
			
			// Valid still?
			$.ajax({
				url: "who_am_i",
				data: {sid: sid},
				success: function(data) {
					callback(data.success, data);
				}
			});
		},
		
		login: function(user, pw, callback) {
			// Attempt to get session from server
			$.ajax({
				url: "login",
				data: {user: user, pw: pw},
				type: "GET",
				success: function(data) {
					if(data.success) {
						sid = data.sid;
						callback(true, data);
					}
					else {
						sid = null;
						callback(false, data);
					}
					
					// Save to permanent store
					window["localStorage"]["sid"] = sid;
				}
			});
		},
		
		logout: function(callback) {
			// Remove local store
			window["localStorage"].removeItem("sid");
			
			$.ajax({
				url: "logout",
				data: {sid: sid},
				success: function(data) {
					sid = null;
					callback(data.success, data);
				}
			});
		},
		
		set: function(site_url, site_user, site_pw, callback) {
			// Set password for given site
			$.ajax({
				url: "set_password",
				data: {
					sid: sid,
					site: site_url,
					site_user: site_user,
					site_pw: site_pw
				},
				type: "GET",
				success: function(data) {
					callback(data.success, data);
				}
			});
		},
		
		get: function(site_url, callback) {
			// Gets the password for given site
			$.ajax({
				url: "get_password",
				data: {
					sid: sid,
					site: site_url
				},
				type: "GET",
				success: function(data) {
					callback(data.success, data);
				}
			});
		},
		
		generate: function(min_len, max_len, extra_chars, callback) {
			$.ajax({
				url: "generate_password",
				data: {
					min_len: min_len,
					max_len: max_len,
					extra_chars: extra_chars
				},
				success: function(data) {
					callback(data.success, data);
				}
			});
		},
	}
}();

