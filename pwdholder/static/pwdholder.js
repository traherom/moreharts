// Transparently handles all the backend communication with server
var PwdHolder = function() {
	// In case we cached this locally and want to know the full URL
	// to send requests to
	var baseurl = "";
	
	// Status info
	var enc_key = null;
	
	// Public functions
	return {
		checkLogin: function(callback) {
			// Do we actually have an encyrption key sitting around?
			if(enc_key == null && window["localStorage"]["enc_key"] != null)
				enc_key = window["localStorage"]["enc_key"]
				
			if(enc_key == null) {
				// Well we don't have a key any more, so we can't do anything
				// Have to get the user's password to produce the key
				callback(false, 'No encryption key available');
			}
			
			// Valid still?
			$.ajax({
				url: "who_am_i",
				success: function(data) {
					// Inform caller
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
						enc_key = data.enc_key;
						callback(true, data);
					}
					else {
						enc_key = null;
						callback(false, data);
					}
					
					// Save to permanent store
					window["localStorage"]["enc_key"] = enc_key;
				}
			});
		},
		
		logout: function(callback) {
			// Remove local store
			window["localStorage"].removeItem("enc_key");
			
			$.ajax({
				url: "logout",
				success: function(data) {
					enc_key = null;
					callback(data.success, data);
				}
			});
		},
		
		set: function(site_url, site_user, site_pw, callback) {
			// Set password for given site
			$.ajax({
				url: "set_password",
				data: {
					enc_key: enc_key,
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
					enc_key: enc_key,
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

