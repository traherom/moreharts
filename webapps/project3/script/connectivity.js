var users = new Array();

/*
 * Log the given user in with the password in the field.
 */
function login()
{
	var passwordField = document.getElementById("passwordField");
	var changeInfo = document.getElementById("login_info");
	var userID = document.getElementById("usernames").value;
	if(userID == null || userID == "")
	{
		changeInfo.firstChild.nodeValue = "You must select a user";
		changeInfo.className = "error";
		document.getElementById("usernames").focus();
		return;
	}
	if($("#passwordField").val().length > 6)
	{
		$.getJSON("http://judah.cedarville.edu/rmoreha/LoginCheck?callback=?", {"userID": userID, "password": passwordField.value}, function(data)
		{
			// start session and take to logged in page
			if(data.success)
			{
				window.location = "loggedin.html";
			}
			// error
			else
			{
				changeInfo.firstChild.nodeValue = data.error;
				changeInfo.className = "error";
				passwordField.focus();
				passwordField.value = "";
			}
		});
	}
	else
	{
		changeInfo.firstChild.nodeValue = "You must enter a password, and all passwords are greater than six characters";
		changeInfo.className = "error";
		passwordField.focus();
	}
}

/*
 * Log the userID currently in the session out.
 */
function logout()
{
	$.getJSON("http://judah.cedarville.edu/rmoreha/Logout?callback=?", function(data)
	{
		if(data.success)
		{
			window.location = "index.html";
		}
	});
}

/*
 * Clear the edit profile view and return to the standard view.
 */
function uneditProfile()
{
	var standard = document.getElementById("standard");
	var editProfileDiv = document.getElementById("editProfileDiv");
	
	editProfileDiv.style.display = "none";
	standard.style.display = "block";
}

/*
 * Change from the standard view to the edit profile view in the sidebar.
 */
function editProfile()
{
	var standard = document.getElementById("standard");
	var editProfileDiv = document.getElementById("editProfileDiv");
	var displayNameTextField = document.getElementById("displayNameTextField");
	var emailTextField = document.getElementById("emailTextField");
	var firstPartnerSelect = document.getElementById("firstPartnerSelect");
	var secondPartnerSelect = document.getElementById("secondPartnerSelect");
	var changeInfo = document.getElementById("change_info");
	var profileInfo = document.getElementById("profile_info");
	var oldPass = document.getElementById("oldPass");
	var newPass = document.getElementById("newPass");
	var retypeNewPass = document.getElementById("retypeNewPass");
	
	oldPass.value = "";
	newPass.value = "";
	retypeNewPass.value = "";
	changeInfo.firstChild.nodeValue = "";
	profileInfo.firstChild.nodeValue = "";
	
	standard.style.display = "none";
	editProfileDiv.style.display = "block";
	
	$.getJSON("http://judah.cedarville.edu/rmoreha/IsLoggedIn?callback=?", function(data)
	{
		$.getJSON("http://judah.cedarville.edu/rmoreha/GetUser?callback=?", {"userID": data.userID}, function(data)
		{
			displayNameTextField.value = data.name;
			emailTextField.value = data.email;
			firstPartnerSelect.value = data.firstPartner;
			if(data.secondPartner != "")
			{
				var i = 1;
				while((i < secondPartnerSelect.options.length) && (secondPartnerSelect.options[i].value != data.secondPartner))
				{
					++i;
				}
				if(i == secondPartnerSelect.options.length)
				{
					i = 0;
				}
				secondPartnerSelect.selectedIndex = i;
			}
			else
			{
				secondPartnerSelect.selectedIndex = 0;
			}
			if(data.firstPartner != "")
			{
				var i = 1;
				while(i < firstPartnerSelect.options.length && firstPartnerSelect.options[i].value != data.firstPartner)
				{
					++i;
				}
				if(i == firstPartnerSelect.options.length)
				{
					i = 0;
				}
				firstPartnerSelect.selectedIndex = i;
			}
			else
			{
				firstPartnerSelect.selectedIndex = 0;
			}
			
			displayNameTextField.focus();
			displayNameTextField.select();
		});
	});
	
	displayNameTextField.focus();
	displayNameTextField.select();
}

/*
 * Verify that the information entered in the profile is valid. If it is, call updateUser().
 */
function verifyProfile()
{
	var displayNameTextField = document.getElementById("displayNameTextField");
	var emailTextField = document.getElementById("emailTextField");
	var changeInfo = document.getElementById("change_info");
	var oldPass = document.getElementById("oldPass");
	var newPass = document.getElementById("newPass");
	var retypeNewPass = document.getElementById("retypeNewPass");
	
	// validate the username
	var displayReg = /[a-zA-Z0-9_-]/;
	
	// validate the email
	var emailReg = /^[^@]+@[^@.]+\.[^@]*\w\w$/;
	
	if(displayReg.test(displayNameTextField.value))
	{
		if(displayNameTextField.value.length >= 3)
		{
			if(displayNameTextField.value != "None")
			{
				if(emailReg.test(emailTextField.value))
				{
					if(oldPass.value.length > 6 || oldPass.value.length == 0)
					{
						if(newPass.value.length > 6 || newPass.value.length == 0)
						{
							if(retypeNewPass.value.length > 6 || retypeNewPass.value.length == 0)
							{
								if(newPass.value == retypeNewPass.value)
								{
									if(updateUser())
									{
										return true;
									}
									else
									{
										return false;
									}
								}
								else
								{
									newPass.value = "";
									retypeNewPass.value = "";
									changeInfo.firstChild.nodeValue = "The new password and the retyped new password did not match";
									changeInfo.className = "error";
									newPass.focus();
								}
							}
							else
							{
								changeInfo.firstChild.nodeValue = "You must retype the new password, and all passwords are greater than six characters";
								changeInfo.className = "error";
								retypeNewPass.focus();
							}
						}
						else
						{
							changeInfo.firstChild.nodeValue = "You must enter a new password, and all passwords are greater than six characters";
							changeInfo.className = "error";
							newPass.value = "";
							newPass.focus();
						}
					}
					else
					{
						changeInfo.firstChild.nodeValue = "You must enter an old password, and all passwords are greater than six characters";
						changeInfo.className = "error";
						oldPass.value = "";
						oldPass.focus();
					}
				}
				// the entered email was not valid
				else
				{
					changeInfo.firstChild.nodeValue = "You must enter a valid email address";
					changeInfo.className = "error";
					emailTextField.focus();
					emailTextField.select();
				}
			}
			// the user tried to name themselves "None"
			else
			{
				changeInfo.firstChild.nodeValue = "You cannot name yourself that.";
				changeInfo.className = "error";
				emailTextField.focus();
				emailTextField.select();
			}
		}
		// the username was not long enough
		else
		{
			changeInfo.firstChild.nodeValue = "You must enter a display name, and all display must be greater than two characters";
			changeInfo.className = "error";
			displayNameTextField.focus();
			displayNameTextField.select();
		}
	}
	// the new username was not valid
	else
	{
		changeInfo.firstChild.nodeValue = "A display name can only containt letters and numbers";
		changeInfo.className = "error";
		displayNameTextField.focus();
		displayNameTextField.select();
	}
	
	return false;
}

/*
 * Assuming verifyProfile() has already been called, hand the new user settings to the server.
 */
function updateUser()
{
	var oldPass = document.getElementById("oldPass");
	var newPass = document.getElementById("newPass");
	var retypeNewPass = document.getElementById("retypeNewPass");
	var displayNameTextField = document.getElementById("displayNameTextField");
	var emailTextField = document.getElementById("emailTextField");
	var firstPartnerSelect = document.getElementById("firstPartnerSelect");
	var secondPartnerSelect = document.getElementById("secondPartnerSelect");
	var profileInfo = document.getElementById("profile_info");
	var changeInfo = document.getElementById("change_info");
	
	$.getJSON("http://judah.cedarville.edu/rmoreha/IsLoggedIn?callback=?", function(data)
   {
		var secondPartner = secondPartnerSelect.value;
		var userID = data.userID;
		if(secondPartner == firstPartnerSelect.value)
		{
			secondPartner = "";
		}
		$.getJSON("http://judah.cedarville.edu/rmoreha/UpdateUser?callback=?", {"userID": userID, "oldPassword": oldPass.value, "newPassword": newPass.value, "name": displayNameTextField.value, "email": emailTextField.value, "firstPartner": firstPartnerSelect.value, "secondPartner": secondPartner}, function(data)
		{
			if(data.success)
			{
				document.getElementById("loggedInUser").firstChild.nodeValue = displayNameTextField.value;
				uneditProfile();
				profileInfo.firstChild.nodeValue = "Profile updated successfully!";
				profileInfo.className = "good";
				return true;
			}
			else
			{
				changeInfo.firstChild.nodeValue = data.error;
				changeInfo.className = "error";
				newPass.value = "";
				retypeNewPass.value = "";
				oldPass.value = "";
				oldPass.focus();
				return false;
			}
		});
   });
	
	return false;
}

/*
 * Go to the voting page with the specific projectID.
 */
function voteProject()
{
	var voteSelect = document.getElementById("voteSelect");
	
	$.getJSON("http://judah.cedarville.edu/rmoreha/IsLoggedIn?callback=?", function(data)
   {
		if(!data.isAdmin)
		{
			if(voteSelect.selectedIndex != -1)
			{
				window.location = "vote.html?project=" + voteSelect.value;
			}
		}
		else
		{
			document.getElementById("profile_info").firstChild.nodeValue = "Sorry, Admin can't vote!";
			document.getElementById("profile_info").className = "error";
		}
   });
}

/*
 * Go to the viewing page for the specific projectID.
 */
function viewProject()
{
	var viewSelect = document.getElementById("viewSelect");
	
	if(viewSelect.selectedIndex != -1)
	{
		window.location = "view.html?project=" + viewSelect.value;
	}
}

/*
 * Fill the view table with the projects by users matrix.
 */
function fillProjects()
{
	$.getJSON("http://judah.cedarville.edu/rmoreha/IsLoggedIn?callback=?", function(data)
	{
		var userID = data.userID;
		
		// populate the projects table and fill from the database
		$.getJSON("http://judah.cedarville.edu/rmoreha/GetProjectList?callback=?", function(data)
		{
			// write the column headers row
			var headerRow = "<tr><td></td>";
			var style = "border-left: 2px inset #003; border-bottom: 2px inset #003; border-right: 2px inset #003";
			
			for(var i = 0; i < data.projects.length; ++i)
			{
				headerRow += "<td style=\"" + style + "\"><div align=\"center\">" + data.projects[i].description + "</div></td>";
			}
			headerRow += "</tr>";
			$("#projectsTable").append(headerRow);
			
			// write a row for each user
			for(var i = 0; i < users.length; ++i)
			{
				if(!users[i][3])
				{
					style = "border-top: 2px inset #003; border-right: 2px inset #003; border-bottom: 2px inset #003;";
					var row = "<tr><td style=\"width: 120px; " + style + "\"><a id=\"name" + users[i][2] + "\" href=\"http://judah.cedarville.edu/~" + users[i][1] + "/cs4220.html\" title=\"" + users[i][0] + "\"><div align=\"center\">" + users[i][0] + "</div></a></td>";
					
					for(var j = 0; j < data.projects.length; ++j)
					{
						style += "border-left: 2px inset #003;";
						var image = "";
						if($.inArray(users[i][2], data.projects[j].first) != -1)
						{
							image = "<img src=\"image/first.png\" />";
						}
						else if($.inArray(users[i][2], data.projects[j].second) != -1)
						{
							image = "<img src=\"image/second.png\" />";
						}
						else if($.inArray(users[i][2], data.projects[j].third) != -1)
						{
							image = "<img src=\"image/third.png\" />";
						}
						row += "<td style=\"" + style + "\" id=\"" + users[i][0] + data.projects[j].projectID + "\">" + image + "</td>";
					}
					row += "</tr>";
					$("#projectsTable").append(row);
				}
			}
		});
	});
}

/*
 * Cast the user's votes and write-ins to the server and, if successful, send them to the
 * respective viewing page.
 */
function goVote()
{
	var voteInfo = document.getElementById("vote_info");
	var firstPlaceSelect = document.getElementById("firstPlaceSelect");
	var secondPlaceSelect = document.getElementById("secondPlaceSelect");
	var thirdPlaceSelect = document.getElementById("thirdPlaceSelect");
	var firstWriteInUser = document.getElementById("firstWriteInUser");
	var firstWriteInText = document.getElementById("firstWriteIn");
	var secondWriteInUser = document.getElementById("secondWriteInUser");
	var secondWriteInText = document.getElementById("secondWriteIn");
	var thirdWriteInUser = document.getElementById("thirdWriteInUser");
	var thirdWriteInText = document.getElementById("thirdWriteIn");
	
	if(firstPlaceSelect.selectedIndex == 0)
	{
		voteInfo.firstChild.nodeValue = "You must specify a first place user";
		voteInfo.className = "error";
		firstPlaceSelect.focus();
		return false;
	}
	else if(secondPlaceSelect.selectedIndex == 0)
	{
		voteInfo.firstChild.nodeValue = "You must specify a second place user";
		voteInfo.className = "error";
		secondPlaceSelect.focus();
		return false;
	}
	else if(thirdPlaceSelect.selectedIndex == 0)
	{
		voteInfo.firstChild.nodeValue = "You must specify a third place user";
		voteInfo.className = "error";
		thirdPlaceSelect.focus();
		return false;
	}
	
	if(firstPlaceSelect.selectedIndex == secondPlaceSelect.selectedIndex || firstPlaceSelect.selectedIndex == thirdPlaceSelect.selectedIndex)
	{
		voteInfo.firstChild.nodeValue = "The same user cannot be voted for in two places";
		voteInfo.className = "error";
		firstPlaceSelect.focus();
		return false;
	}
	else if(secondPlaceSelect.selectedIndex == thirdPlaceSelect.selectedIndex)
	{
		voteInfo.firstChild.nodeValue = "The same user cannot be voted for in two places";
		voteInfo.className = "error";
		secondPlaceSelect.focus();
		return false;
	}
	
	if(firstWriteInText.value != "" && firstWriteInUser.selectedIndex == 0)
	{
		voteInfo.firstChild.nodeValue = "You must select a user for this write-in or leave it blank";
		voteInfo.className = "error";
		firstWriteInUser.select();
		return false;
	}
	if(secondWriteInText.value != "" && secondWriteInUser.selectedIndex == 0)
	{
		voteInfo.firstChild.nodeValue = "You must select a user for this write-in or leave it blank";
		voteInfo.className = "error";
		secondWriteInUser.select();
		return false;
	}
	if(thirdWriteInText.value != "" && thirdWriteInUser.selectedIndex == 0)
	{
		voteInfo.firstChild.nodeValue = "You must select a user for this write-in or leave it blank";
		voteInfo.className = "error";
		thirdWriteInUser.select();
		return false;
	}
	
	// Cast the ranking votes
	window.votesCompleted = 0;
	function castVote(projectID, creatorID, place)
	{
		$.getJSON("http://judah.cedarville.edu/rmoreha/CastVote?callback=?", {
			"projectID": projectID,
			"place": place,
			"creatorID": creatorID
			},
			function(data) {
				window.votesCompleted++;
			});
	}
	castVote(projectID, $("#firstPlaceSelect").val(), 1);
	castVote(projectID, $("#secondPlaceSelect").val(), 2);
	castVote(projectID, $("#thirdPlaceSelect").val(), 3);

	// Cast the write ins
	function castWriteIn(projectID, creatorID, description)
	{
		$.getJSON("http://judah.cedarville.edu/rmoreha/CastWriteIn?callback=?", {
			"projectID": projectID,
			"writeIn": description,
			"creatorID": creatorID
			},
			function(data) {
				window.votesCompleted++;
			});
	}
	castWriteIn(projectID, $("#firstWriteInUser").val(), $("#firstWriteIn").val());
	castWriteIn(projectID, $("#secondWriteInUser").val(), $("#secondWriteIn").val());
	castWriteIn(projectID, $("#thirdWriteInUser").val(), $("#thirdWriteIn").val());

	// Wait until all submits return
	setTimeout(function() {
		//if(window.votesCompleted == 6)
			window.location = "view.html?project=" + projectID;
		}, 500);
}

/*
 * Fill the viewing table in with the information regarding the current project.
 */
function fillView()
{
	var viewTable = document.getElementById("viewTable");
	
	$.getJSON("http://judah.cedarville.edu/rmoreha/GetVotes?callback=?", function(data)
	{
		var writeInsHeader = "<tr><td colspan=\"3\" style=\"border-bottom: 2px inset #003;\"><div align=\"center\"><b>Write-Ins</b></div></td></tr>";
		$("#writeInsTable").append(writeInsHeader);
		var pointsHeader = "<tr><td><div align=\"center\"><b>Student</b></div></td><td><div align=\"center\"><b>Brooms Earned</b></div></td><td><div align=\"center\"><b>Points</b></div></td></tr>";
		$("#viewTable").append(pointsHeader);
		
		if(data.votes[projectID] == null)
		{
			return;
		}
		for(var i = 0; i < data.votes[projectID].length; ++i)
		{
			var row = "<tr id=\"" + i + "\">";
			var name;
			var userID = data.votes[projectID][i].creatorID;
			var projectData = data.votes[projectID][i];
			
			row += "<td style=\"border-right: 2px inset #003; border-bottom: 2px inset #003;\"><div align=\"center\">" + projectData.name + "</div></td>";
			var image = "";
			for(var j = 0; j <= projectData.points; j += 2)
			{
				image += "<img src=\"image/voter.jpg\" />";
			}
			if(projectData.points % 2 != 0 && projectData.points != 1)
			{
				image += "<img src=\"image/halfVote.jpg\" />";
			}
			row += "<td style=\"border-bottom: 2px inset #003;\">" + image + "</td>";
			row += "<td style=\"border-left: 2px inset #003; border-bottom: 2px inset #003;\"><div align=\"center\">" + projectData.points + "</div></td>";
			
			row += "</tr>";
			$("#viewTable").append(row);
			
			// write the write-ins to their table
			for(var j = 0; j < projectData.writeIns.length; ++j)
			{
				var row = "<tr><td style=\"border-right: 2px inset #003;\"><div align=\"center\">" + projectData.name + "</div</td>";
				row += "<td colspan=\"2\"><div align=\"center\">" + projectData.writeIns[j] + "</div></td>";
				
				row += "</tr>";
				$("#writeInsTable").append(row);
			}
		}
	});
}

/**
* Fills the "select a project to view/vote on" boxes
*/
function fillProjectSelects()
{
	$.getJSON("http://judah.cedarville.edu/rmoreha/IsLoggedIn?callback=?", function(data)
	{
		var userID = data.userID;
		
		$.getJSON("http://judah.cedarville.edu/rmoreha/GetProjectList?callback=?", function(data)
		{					
			// fill the projects voting list (if that project is open for voting) and the
			// projects view list (if that project has been voted on
			for(var i = 0; i < data.projects.length; ++i)
			{
				var project = data.projects[i];
				var option = "<option value=\"" + project.projectID + "\">" + project.description + "</option>";
				if(project.first.length > 0)
					$("#viewSelect").append(option);
				
				$.getJSON("http://judah.cedarville.edu/rmoreha/HasVoted?callback=?",
					{"userID": userID, "projectID": project.projectID},
					function(data)
					{
						var option2 = "<option value=\"" + data.projectID + "\">" + data.description + "</option>";
						if(data.isOpen && !data.hasVoted)
							$("#voteSelect").append(option2);
					});
			}
		});
	});
}
