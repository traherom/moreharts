function adminFillLists()
{
	$("#project_info").empty();
	$.getJSON("http://judah.cedarville.edu/rmoreha/GetProjectList?callback=?", function(data) {
		if(!data.success)
		{
			$("#project_info").addClass("error").removeClass("good").text("Unable to get project list");
			return;
		}

		// Add each project to the appropiate lists
		$("#projSelect").add("#closeProjSelect").add("#openProjSelect").empty();
		$.each(data.projects, function(i, proj) {
			var op = $("<option />").append(proj.description).val(proj.projectID);
			$("#projSelect").append(op);
			if(proj.isOpen)
				$("#closeProjSelect").append(op.clone());
			else
				$("#openProjSelect").append(op.clone());
		});
	});

	$("#user_info").empty();
	$.getJSON("http://judah.cedarville.edu/rmoreha/GetUserList?callback=?", function(data) {
		if(!data.success)
		{
			$("#user_info").addClass("error").removeClass("good").text("Unable to get user list");
			return;
		}

		// Add each user to the appropiate list
		$("#userSelect").empty();
		$.each(data.users, function(i, user) {
			var op = $("<option />").append(user.name).val(user.userID);
			$("#userSelect").append(op);
		});
	});
}

function adminBindEvents()
{
	$("#openProj").bind('click', function() {
		var projID = $("#openProjSelect").val();
		if(projID == "")
		{
			$("#project_info").addClass("error").removeClass("good").text("Please select a project");
			return;
		}
		$.getJSON("http://judah.cedarville.edu/rmoreha/OpenProject?callback=?", {"projectID": projID}, function (data) {
			if(data.success)
			{
				// Reload page
				adminFillLists();
				$("#project_info").addClass("good").removeClass("error").text("Project opened");
			}
			else
				$("#project_info").addClass("error").removeClass("good").text("Failed to open project: " + data.error);
		});
	});
	$("#closeProj").bind('click', function() {
		var projID = $("#closeProjSelect").val();
		if(projID == "")
		{
			$("#project_info").addClass("error").removeClass("good").text("Please select a project");
			return;
		}
		$.getJSON("http://judah.cedarville.edu/rmoreha/CloseProject?callback=?", {"projectID": projID}, function (data) {
			if(data.success)
			{
				// Reload page
				adminFillLists();
				$("#project_info").addClass("good").removeClass("error").text("Project closed");
			}
			else
				$("#project_info").addClass("error").removeClass("good").text("Failed to close project: " + data.error);
		});
	});
	$("#deleteProj").bind('click', function() {
		var projID = $("#projSelect").val();
		if(projID == "")
		{
			$("#project_info").addClass("error").removeClass("good").text("Please select a project");
			return;
		}

		// Double check
		if(!confirm("Are you sure you want to delete project ID " + projID + "?\nThis will remove all votes associated with this project as well."))
		{
			return;
		}

		$.getJSON("http://judah.cedarville.edu/rmoreha/DeleteProject?callback=?", {"projectID": projID}, function (data) {
			if(data.success)
			{
				// Reload page
				adminFillLists();
				$("#project_info").addClass("good").removeClass("error").text("Project deleted");
			}
			else
				$("#project_info").addClass("error").removeClass("good").text("Failed to delete project: " + data.error);
		});
	});
	$("#deleteUser").bind('click', function() {
		var userID = $("#userSelect").val();
		if(userID == "")
		{
			$("#user_info").addClass("error").removeClass("good").text("Please select a user");
			return;
		}

		// Double check
		if(!confirm("Are you sure you want to delete user ID " + userID + "?\nThis will remove all votes by this person as well."))
		{
			return;
		}

		$.getJSON("http://judah.cedarville.edu/rmoreha/DeleteUser?callback=?", {"userID": userID}, function (data) {
			if(data.success)
			{
				// Reload page
				adminFillLists();
				$("#user_info").addClass("good").removeClass("error").text("User deleted");
			}
			else
				$("#user_info").addClass("error").removeClass("good").text("Failed to delete user: " + data.error);
		});
	});

	// Creation stuff
	$("#createUser").bind('click', function() {
		$.getJSON("http://judah.cedarville.edu/rmoreha/CreateUser?callback=?", {
						"userName": $("#userName").val(),
						"name": $("#userDisplayName").val(),
						"email": $("#userEmail").val(),
						"password": $("#userPassword").val()
						},
						function(data) {
							if(data.success)
							{
								// Reload page
								adminFillLists();
								$("#user_info").addClass("good").removeClass("error").text("User created");

								// Clear form
								$("#userName").val("");
								$("#userEmail").val("");
								$("#userDisplayName").val("");
								$("#userPassword").val("");
							}
							else
								$("#user_info").addClass("error").removeClass("good").text("Unable to create user: " + data.error);
						});

	});
	$("#createProj").bind('click', function() {
		$.getJSON("http://judah.cedarville.edu/rmoreha/CreateProject?callback=?", {
						"description": $("#projName").val(),
						"isPartner": $("#projPartners").val()
						},
						function(data) {
							if(data.success)
							{
								// Reload page data
								adminFillLists();
								$("#project_info").addClass("good").removeClass("error").text("Project created");

								// Clear form
								$("#projName").val("");
								$("#projPartners").attr('checked', false);
							}
							else
								$("#project_info").addClass("error").removeClass("good").text("Unable to create project: " + data.error);
						});

	});

	// Editing stuff. Note that we hide the edit buttons by default
	$("#editUserCommit").add("#editUserCancel").add("#editUserTitle").hide();
	$("#editProjCommit").add("#editProjCancel").add("#editProjTitle").hide();

	$("#editUser").bind('click', function() {
		$.getJSON("http://judah.cedarville.edu/rmoreha/GetUser?callback=?", {"userID": $("#userSelect").val()},
			function(data) {
				if(data.success)
				{
					// Show buttons to save changes
					$("#editUserCommit").add("#editUserCancel").add("#editUserTitle").show();
					$("#createUser").add("#createUserTitle").hide();

					// And show loaded data
					$("#editUserCommit").data('userID', data.userID);
					$("#userName").val(data.userName);
					$("#userEmail").val(data.email);
					$("#userDisplayName").val(data.name);
				}
				else
					$("#user_info").addClass("error").removeClass("good").text("Unable to load user data");
			});
		});
	$("#editUserCancel").bind('click', function() {
			// Show creation things now, hide edit
			$("#editUserCommit").add("#editUserCancel").add("#editUserTitle").hide();
			$("#createUser").add("#createUserTitle").show();
		
			// Clear form
			$("#userName").val("");
			$("#userEmail").val("");
			$("#userDisplayName").val("");
			$("#userPassword").val("");
		});
	$("#editUserCommit").bind('click', function() {
		$.getJSON("http://judah.cedarville.edu/rmoreha/UpdateUser?callback=?", {
				"userID": $("#editUserCommit").data('userID'),
				"userName": $("#userName").val(),
				"email": $("#userEmail").val(),
				"name": $("#userDisplayName").val(),
				"newPassword": $("#userPassword").val()
			},
			function(data) {
				if(data.success)
				{
					// Success!
					adminFillLists();
					$("#user_info").addClass("good").removeClass("error").text("Changes saved");

					// Show creation things now, hide edit
					$("#editUserCommit").add("#editUserCancel").add("#editUserTitle").hide();
					$("#createUser").add("#createUserTitle").show();
		
					// Clear form
					$("#userName").val("");
					$("#userEmail").val("");
					$("#userDisplayName").val("");
					$("#userPassword").val("");
				}
				else
					$("#user_info").addClass("error").removeClass("good").text("Failed to save changes: " + data.error);
			});
		});

	$("#editProj").bind('click', function() {
		$.getJSON("http://judah.cedarville.edu/rmoreha/GetProject?callback=?", {"projectID": $("#projSelect").val()},
			function(data) {
				if(data.success)
				{
					// Show buttons to save changes
					$("#editProjCommit").add("#editProjCancel").add("#editProjTitle").show();
					$("#createProj").add("#createProjTitle").hide();

					// And show loaded data
					$("#editProjCommit").data('projectID', data.projectID);
					$("#projName").val(data.description);
					$("#projPartners").attr('checked', data.isPartner);
				}
				else
					$("#proj_info").addClass("error").removeClass("good").text("Unable to load project data");
			});
		});
	$("#editProjCancel").bind('click', function() {
			// Show creation things now, hide edit
			$("#editProjCommit").add("#editProjCancel").add("#editProjTitle").hide();
			$("#createProj").add("#createProjTitle").show();
		
			// Clear form
			$("#projName").val("");
			$("#projPartners").attr('checked', false);
		});
	$("#editProjCommit").bind('click', function() {
		$.getJSON("http://judah.cedarville.edu/rmoreha/UpdateProject?callback=?", {
				"projectID": $("#editProjCommit").data('projectID'),
				"description": $("#projName").val(),
				"isPartner": $("#projPartners").attr('checked')
			},
			function(data) {
				if(data.success)
				{
					// Success!
					adminFillLists();
					$("#project_info").addClass("good").removeClass("error").text("Changes saved");

					// Show creation things now, hide edit
					$("#editProjCommit").add("#editProjCancel").add("#editProjTitle").hide();
					$("#createProj").add("#createProjTitle").show();
		
					// Clear form
					$("#projName").val("");
					$("#projPartners").attr('checked', false);
				}
				else
					$("#project_info").addClass("error").removeClass("good").text("Failed to save changes: " + data.error);
			});
		});
}

$(document).ready(function() {
	adminFillLists();
	adminBindEvents()
});

