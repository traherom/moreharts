<?php
require_once('settings.php');

if(isset($_REQUEST['create']))
{
	try
	{
		// Check input
		if(!preg_match('/^[0-9]{4}$/', @$_REQUEST['newID']))
			throw new Exception('Student ID must be 4 digits long');
		if(@empty($_REQUEST['first']))
			throw new Exception('Student first name must be filled out');
		if(@empty($_REQUEST['last']))
			throw new Exception('Student last name must be filled out');
		if(!preg_match('/^([0-9]{3}-?){1,2}[0-9]{4}$/', @$_REQUEST['phone']))
			throw new Exception('New student phone number is not in the format xxx-xxx-xxxx');
		if(@empty($_REQUEST['class']))
			throw new Exception('Class must be selected');

		$stmt = $db->prepare('INSERT INTO cdr_db (
											StudentID,
											FirstName,
											LastName,
											PhoneNumber,
											Classification,
											SpecialInterests)
										VALUES (?, ?, ?, ?, ?, ?)');
		foreach($_REQUEST as $key => $val)
			$_REQUEST[$key] = stripslashes($val);
		$_REQUEST['phone'] = str_replace('-', '', $_REQUEST['phone']);
		$stmt->bind_param('ssssss',
							$_REQUEST['newID'],
							$_REQUEST['first'],
							$_REQUEST['last'],
							$_REQUEST['phone'],
							$_REQUEST['class'],
							$_REQUEST['interests']);
		$success = $stmt->execute();
		$stmt->close();

		if($success)
			header('Location: index.php?msg=Successfully+created+new+member');
		else
			throw new Exception('Unable to create new member, duplicate ID may exist');
	}
	catch(Exception $e)
	{
		$errorToUser = $e->getMessage();;
	}
}

if(isset($_REQUEST['delete']))
{
	if(count(@$_REQUEST['id']) > 0)
	{
		$stmt = $db->prepare('DELETE FROM cdr_db WHERE StudentID=?');
		$stmt->bind_param('s', $id);

		foreach($_REQUEST['id'] as $id)
		{
			$stmt->execute();	
		}

		$stmt->close();

		$messageToUser = 'All selected students deleted';
	}
	else
	{
		$errorToUser = 'Please select students to delete';
	}
}

if(isset($_REQUEST['edit']))
{
	if(count(@$_REQUEST['id']) == 1)
	{
		header("Location: edit.php?id=" . $_REQUEST[id][0]);
	}
	else
	{
		$errorToUser = 'You may only edit one student at a time';
	}
}

include('header.php');
?>

<h1>Current Members</h1>
<form action="<?php print($_SERVER['PHP_SELF']); ?>" method="post">
<table>
	<tr>
		<th>&nbsp;</th>
		<th colspan="6">
			<input type="submit" name="delete" value="Remove Selected" />
			<input type="submit" name="edit" value="Edit Selected" />
		</th>
	</tr>
	<tr>
		<th>&nbsp;</th>
		<th>StudentID</th>
		<th>First Name</th>
		<th>Last Name</th>
		<th>Phone</th>
		<th>Class</th>
		<th>Special Interests</th>
	</tr>
	
	<?php
	// Get students
	$stmt = $db->prepare('SELECT StudentID, FirstName, LastName,
								PhoneNumber, Classification,
								SpecialInterests
							FROM cdr_db
							ORDER BY StudentID ASC');
	$stmt->execute();
	$stmt->bind_result($id, $first, $last, $phone, $class, $interests);

	while($stmt->fetch())
	{
		print("\n\t<tr>");
		print("\n\t\t<td><input type='checkbox' name='id[]' value='");
		print(htmlentities($id));
		print("' /></td>");
		print("\n\t\t<td>");
		print(htmlentities($id));
		print("</td>");
		print("\n\t\t<td>");
		print(htmlentities($first));
		print("</td>");
		print("\n\t\t<td>");
		print(htmlentities($last));
		print("</td>");
		print("\n\t\t<td>");
		print(htmlentities($phone));
		print("</td>");
		print("\n\t\t<td>");
		print(htmlentities($class));
		print("</td>");
		print("\n\t\t<td>");
		print(htmlentities($interests));
		print("</td>");
		print("\n\t</tr>");
	}

	$stmt->close();
	?>

	<tr>
		<td>&nbsp;</td>
		<td><input type="text" name="newID" value="<?php print($_REQUEST['newID']); ?>" /></td>
		<td><input type="text" name="first"  value="<?php print($_REQUEST['first']); ?>"/></td>
		<td><input type="text" name="last" value="<?php print($_REQUEST['last']); ?>" /></td>
		<td><input type="text" name="phone" value="<?php print($_REQUEST['phone']); ?>" /></td>
		<td>
			<select name="class">
				<?php
				$options = array('freshman' => 'Freshman',
								 'sophomore' => 'Sophomore',
								 'junior' => 'Junior',
								 'senior' => 'Senior',
								 'grad' => 'Grad');
				foreach($options as $key => $val)
				{
					if(@$_REQUEST['class'] == $key)
						print("\n\t\t\t<option value='$key'>$val</option>");
					else
						print("\n\t\t\t<option selected='selected' value='$key'>$val</option>");
				}
				?>
			</select>	
		</td>
		<td><input type="text" name="interests" value="<?php print($_REQUEST['interests']); ?>" /></td>
		<td><input type="submit" name="create" value="Create New Member" /></td>
	</tr>
</table>
</form>

<?php
include('footer.php');
?>
