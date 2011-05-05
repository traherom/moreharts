<?php
require_once('library.php');

// Save where we should go to
if(isset($_REQUEST['ret']))
	$_SESSION['ret'] = $_REQUEST['ret'];

// Check login
if(isset($_POST['login']))
{
	checkSID();
	if(!loginUser($_POST['user'], $_POST['pw']))
		setMessage('Invalid user/password');
}

// If logged in go to the original page
// Down here so that if we hit the login page and already are
// it automatically goes through
if(isLoggedIn())
{
	if(isset($_SESSION['ret']))
	{
		$ret = $_SESSION['ret'];
		unset($_SESSION['ret']);
	}
	else
		$ret = 'orders.php';

	header("Location: $ret");
}

include('header.php');
?>

<form action='login.php' method='post'>
<table>
	<tr>
		<td>User name</td>
		<td><input type="text" name="user" /></td>
	</tr>
	<tr>
		<td>Password</td>
		<td><input type="password" name="pw" /></td>
	</tr>
	<tr>
		<td><?php addSID(); ?></td>
		<td><input type="submit" name="login" value="Login" /></td>
	</tr>
</table>
</form>

<?php
include('footer.php');
?>
