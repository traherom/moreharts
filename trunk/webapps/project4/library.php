<?php
// Random functions to do helpful stuff. I'm not interested in organization, let's
// just get this done.
require_once('settings.php');

// Returns an array of all products in system
function getProducts()
{
	global $db;

	// Query
	$stmt = $db->prepare('SELECT ProductID, Name FROM ar_product');
	$stmt->execute();
	$stmt->bind_result($id, $name);

	// Dump to array
	$products = array();
	while($stmt->fetch())
	{
		$products[$id] = $name;
	}

	// Finish
	$stmt->close();
	return $products;
}

// Saves an order to the database
// $cart should be an array with productID=>quantity
function saveOrder($userID, $address, $cart)
{
	global $db;

	// Transactionalize it all
	$db->autocommit(false);

	// Create the new order
	$stmt = $db->prepare('INSERT INTO ar_order (UserID, Date, Address) VALUES (?, NOW(), ?)');
	$stmt->bind_param('is', $userID, $address);
	$success = $stmt->execute();
	$stmt->close();
	if(!$success)
	{
		$db->rollback();
		$db->autocommit(true);
		throw new Exception('Unable to create new order');
	}

	// Get the new order ID
	$orderID = $db->insert_id;

	// Add each item to order
	$stmt = $db->prepare('INSERT INTO ar_order_product (OrderID, ProductID, Quantity)
							VALUES (?, ?, ?)');
	$stmt->bind_param('iii', $orderID, $productID, $quantity);

	foreach($cart as $productID => $quantity)
	{
		// If any fail, they all fail
		if(!$stmt->execute())
		{
			$db->rollback();
			$db->autocommit(true);
			throw new Exception("Unable to add $productID to order. Cancelling checkout.");
		}
	}

	$stmt->close();
	
	// And success! Changing autocommit back on automatically commits
	$db->autocommit(true);

	// Return the new order id
	return $orderID;
}

// Gets all orders for the current user
function getOrders()
{
	global $db;

	// Require login
	if(!isLoggedIn())
		throw new Exception('Must be logged in to get current user\'s orders');
	
	// Get all items/orders as a big old table
	// We'll break it down in the loop
	$stmt = $db->prepare('SELECT o.OrderID, UNIX_TIMESTAMP(Date), Address, ProductID, Quantity
							FROM ar_order o
							JOIN ar_order_product p ON o.OrderID=p.OrderID
							WHERE UserID=?
							ORDER BY Date DESC');
	$stmt->bind_param('i', $_SESSION['userID']);
	$stmt->execute();

	// Break it into the array
	$orders = array();
	$stmt->bind_result($orderID, $date, $address, $productID, $quantity);
	while($stmt->fetch())
	{
		// Initialze the main part of the order only once per order
		if(!isset($orders[$orderID]))
		{
			$orders[$orderID] = array('orderID' => $orderID,
									  'date' => $date,
									  'address' => trim($address),
									  'products' => array());
		}

		// Add this item to the order
		$orders[$orderID]['products'][$productID] = $quantity;
	}

	// Finish
	$stmt->close();
	return $orders;
}

// Returns user id if valid login, false otherwise
function loginUser($user, $pw)
{
	global $db;

	// MD5 password on this end, plaintext as short as possible
	$pw = md5($pw);

	// Allow logins either by user id, email, or user name
	$stmt = null;
	if(!is_numeric($user))
	{
		$stmt = $db->prepare('SELECT UserID FROM ar_user WHERE
								(UserName=? OR Email=?)
								AND Password=?');
		$stmt->bind_param('sss', $user, $user, $pw);
	}
	else
	{
		$stmt = $db->prepare('SELECT UserID FROM ar_user WHERE
								UserID=? AND Password=?');
		$stmt->bind_param('is', $user, $pw);
	}

	// Check
	$stmt->execute();
	$stmt->bind_result($id);
	if($stmt->fetch())
	{
		$_SESSION['userID'] = $id;
		return $id;
	}
	else
		return false;
}

// Logs out a user... simple, but for the sake of having the function...
function logoutUser()
{
	unset($_SESSION['userID']);
}

// Check if a user is logged in/is an admin
function isLoggedIn()
{
	return isset($_SESSION['userID']);
}

function isAdmin()
{
	return isset($_SESSION['isAdmin']);
}

// Get all data on current user
function currentUser()
{
	global $db;

	if(!isLoggedIn())
		throw new Exception('Must be logged in to get current user');

	// Get the user data
	$stmt = $db->prepare('SELECT u.UserID, Name, Email, IsAdmin, UserName, StudentID
							FROM ar_user u
							LEFT JOIN ar_user_shopping s ON u.UserID = s.UserID
							WHERE u.UserID=?');
	$stmt->bind_param('i', $_SESSION['userID']);
	$stmt->execute();
	$stmt->bind_result($id, $name, $email, $isAdmin, $userName, $studentID);
	$found = $stmt->fetch();
	$stmt->close();
	if(!$found)
		throw new Exception('Unable to find user');

	return array('userID' => $id,
				 'name' => $name,
				 'email' => $email,
				 'isAdmin' => $isAdmin,
				 'userName' => $userName,
				 'studentID' => $studentID);
}

// Set data on current user
function setStudentID($studentID)
{
	global $db;

	if(!isLoggedIn())
		throw new Exception('Must be logged in to update student id');

	// Set the student id
	$stmt = $db->prepare('INSERT INTO ar_user_shopping (UserID, StudentID)
							VALUES (?, ?)
							ON DUPLICATE KEY UPDATE StudentID=?');
	$stmt->bind_param('iss', $_SESSION['userID'], $studentID, $studentID);
	$success = $stmt->execute();
	$stmt->close();
	if(!$success)
		throw new Exception('Unable to update student id for user');
}

// Sets a message/error for the user
function setMessage($msg)
{
	global $messageToUser;
	$messageToUser = $msg;
}
function setError($msg)
{
	global $errorToUser;
	$errorToUser = $msg;
}

// Security things. Prevents the user's cookie
// from being exploited to perform actions they didn't intend
function addSID($print = true)
{
	if(!isset($_SESSION['sid']))
		$_SESSION['sid'] = uniqid(rand(), true);
	
	if($print)
		print("<input type='hidden' name='__sid' value='$_SESSION[sid]' />");
	else
		return $_SESSION['sid'];
}

function checkSID()
{
	if(!isset($_REQUEST['__sid']) || !isset($_SESSION['sid'])
		|| $_REQUEST['__sid'] != $_SESSION['sid'])
	{
		die("security violation");
	}
}
?>
