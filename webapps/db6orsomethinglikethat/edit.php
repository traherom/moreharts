<?php
require_once('settings.php');

// Save ID we're editing
if(isset($_GET['id']))
{
	$_SESSION['id'] = $_GET['id'];
}

// Must have ID
if(!isset($_SESSION['id']))
{
	header("Location: index.php?msg=You+must+select+someone+to+edit");
	die();
}

// Get current values
$stmt = $db->prepare('SELECT StudentID, FirstName, LastName,
							PhoneNumber, Classification,
							SpecialInterests
						FROM cdr_db
						WHERE StudentID=?');
$stmt->bind_param('s', $_SESSION['id']);
$stmt->execute();
$stmt->bind_result($id, $first, $last, $phone, $class, $interests);
$found = $stmt->fetch();
$stmt->close();

if(!$found)
{
	header("Location: index.php?msg=Unable+to+find+user+to+edit");
	die();
}

// Is there an attempt to edit?
if(isset($_REQUEST['update']))
{
	try
	{
		// Check input
		if(@empty($_REQUEST['first']))
			throw new Exception('Student first name must be filled out');
		if(@empty($_REQUEST['last']))
			throw new Exception('Student last name must be filled out');
		if(!preg_match('/^([0-9]{3}-?){1,2}[0-9]{4}$/', @$_REQUEST['phone']))
			throw new Exception('New student phone number is not in the format xxx-xxx-xxxx');
		if(@empty($_REQUEST['class']))
			throw new Exception('Class must be selected');

		$stmt = $db->prepare('UPDATE cdr_db SET FirstName=?,
												LastName=?,
												PhoneNumber=?,
												Classification=?,
												SpecialInterests=?
											WHERE StudentID=?');
		foreach($_REQUEST as $key => $val)
			$_REQUEST[$key] = stripslashes($val);
		$_REQUEST['phone'] = str_replace('-', '', $_REQUEST['phone']);
		$stmt->bind_param('ssssss',
							$_REQUEST['first'],
							$_REQUEST['last'],
							$_REQUEST['phone'],
							$_REQUEST['class'],
							$_REQUEST['interests'],
							$_SESSION['id']);
		$success = $stmt->execute();
		$stmt->close();

		if($success)
		{
			unset($_SESSION['id']);
			header('Location: index.php?msg=Successfully+updated+member');
		}
		else
			throw new Exception('Unable to update member');
	}
	catch(Exception $e)
	{
		$errorToUser = $e->getMessage();;

		// Fill in values with what the user submitted
		$first = $_REQUEST['first'];
		$last = $_REQUEST['last'];
		$phone = $_REQUEST['phone'];
		$class = $_REQUEST['class'];
		$interests = $_REQUEST['interests'];
	}
}

include('header.php');
?>

<h1>Update Member</h1>
<form action="<?php print($_SERVER['PHP_SELF']); ?>" method="post">
<table>
	<tr>
		<td>First Name</td>
		<td><input type="text" name="first" value="<?php print(htmlentities($first)); ?>" /></td>
	</tr>
	<tr>
		<td>Last Name</td>
		<td><input type="text" name="last" value="<?php print(htmlentities($last)); ?>" /></td>
	</tr>
	<tr>
		<td>Phone</td>
		<td><input type="text" name="phone" value="<?php print(htmlentities($phone)); ?>" /></td>
	</tr>
	<tr>
		<td>Classification</td>
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
	</tr>
	<tr>
		<td>Special Interests</td>
		<td><input type="text" name="interests" value="<?php print(htmlentities($interests)); ?>" /></td>
	</tr>
	<tr>
		<td>&nbsp;</td>
		<td>
			<input type="submit" name="update" value="Update Member" />
			<input type="reset" value="Reset" />	
		</td>
	</tr>
</table>
</form>
<?php
include('footer.php');
?>
