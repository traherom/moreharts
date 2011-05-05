<?php
require_once('library.php');

// Must be logged in
if(!isLoggedIn())
	header('Location: login.php?ret=checkout.php');

// Get current user
$user = currentUser();

// Must have a cart
if(!@$_SESSION['cart'])
	header('Location: products.php?msg=Please+select+items');

// Ready to save?
if(isset($_REQUEST['finalize']))
{
	checkSID();

	// Save order
	saveOrder($user['userID'], $_REQUEST['address'], $_SESSION['cart']);
	setStudentID($_REQUEST['studentID']);

	// Clear cart out and send them on to see the order
	$_SESSION['cart'] = array();
	header('Location: orders.php?msg=Your+order+has+been+received');
	die();
}

include('header.php');
?>

<form action="checkout.php" method="post">
<h1>Checkout</h1>
<table>
	<tr>
		<td>Name</td>
		<td><input type="text" name="name" value="<?php print($user['name']); ?>" /></td>
	</tr>
	<tr>
		<td>Student ID</td>
		<td><input type="text" name="studentID" value="<?php print($user['studentID']); ?>" /></td>
	</tr>
	<tr>
		<td>Shipping Address</td>
		<td><textarea name="address" rows="2" cols="20"></textarea></td>
	</tr>
	<tr>
		<td>&nbsp;</td>
		<td>
			<?php addSID(); ?>
			<input type="submit" name="finalize" value="Checkout" />
		</td>
	</tr>
</table>
</form>

<?php
include('footer.php');
?>
