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
				sid = localStorage["sid"]
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
					localStorage["sid"] = sid;
				}
			});
		},
		
		logout: function(callback) {
			// Remove local store
			localStorage.removeItem("sid");
			
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

$(document).ready(function() {
	$("#login_form").submit(function() {
		PwdHolder.login(
			$("#master_user").val(),
			$("#master_pw").val(),
			function(success, data) {
				if(success)
					$.mobile.changePage("#get_password");
				else
					alert(data.message); // TBD: prettier
			});
		return false;
	});
	
	$("#set_form").submit(function() {
		PwdHolder.set(
			$("#site_url_set").val(),
			$("#site_user_set").val(),
			$("#site_pw_set").val(),
			function(success, data) {
				if(success) {
					// Immediately make call out to get the new values
					$("#site_url_get").val($("#site_url_set").val());
					$("#get_form").submit();
					$.mobile.changePage("#get_password", { reverse: true });
				}
				else
					alert(data.message); // TBD: prettier
			});
		return false;
	});
	
	$("#get_form").submit(function() {
		PwdHolder.get(
			$("#site_url_get").val(),
			function(success, data) {
				if(success) {
					$("#site_user_get_result").val(data.site_user);
					$("#site_pw_get_result").val(data.site_pw);
				}
				else {
					alert(data.message); // TBD: prettier
				}
			});
		return false;
	});
	
	$("#generate_form").submit(function() {
		PwdHolder.generate(
			$("#generate_min_len").val(),
			$("#generate_max_len").val(),
			$("#generate_include_punc").val(),
			function(success, data) {
				if(success)
					$("#generate_pw_result").val(data.password);
				else
					$("#generate_pw_result").val(data.message);
			});
		return false;
	});
	
	$("a[href=#login]").click(function(event) {
		PwdHolder.logout(function() {
			$.mobile.changePage("#login");
		});
		
		event.preventDefault();
	});
	
	// Easy copying for username/password
	$("#copy_site_user").click(function() {
		$("#site_user_get_result").select();
	});
	$("#copy_site_pw").click(function() {
		$("#site_pw_get_result").select();
	});
	$("#copy_generated_pw").click(function() {
		$("#generate_pw_result").select();
	});
	
	// Choose starting page
	PwdHolder.checkLogin(function(loggedIn, data) {
		if(loggedIn) {
			// Logged in!
			$.mobile.changePage("#get_password");
		}
		else {
			// Not logged in
			$.mobile.changePage("#login");
		}
	});
});
