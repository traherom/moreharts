<?php
include('settings.php');

if(isset($_POST['login']))
{
	// Hash password nicely
	$pw = md5($_POST['pw']);

	// Check against DB
	$stmt = $db->prepare('SELECT UserID FROM RM_USER 
								WHERE UserName=?
								  AND Password=?');
	$stmt->bind_param('ss', $_POST['user'], $pw);
	$stmt->execute();
	$stmt->bind_result($userID);
	$found = $stmt->fetch();
	$stmt->close();

	if($found)
	{
		$_SESSION['userID'] = $userID;
		header('Location: index.php');
	}
	else
		$errorToUser = "User name/password not found";	
}

include('header.php');
?>

<form action="<?php print($PHP_SELF); ?>" method="post">
<table>
	<tr>
		<td>Name</td>
		<td><input name="user" type="text" value="<?php print(@$_POST['user']); ?>" /></td>
	</tr>
	<tr>
		<td>Password</td>
		<td><input name="pw" type="password" /></td>
	</tr>
	<tr>
		<td>&nbsp;</td>
		<td><input type="submit" name="login" value="Login" /></td>
	</tr>
</table>
</form>

<?php
include('footer.php');
?>
