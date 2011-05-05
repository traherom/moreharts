<?php
require('settings.php');

header('Content-type: application/json');
header('Cache-control: no-cache, must-revalidate, max-age=0');
if(isset($_REQUEST['callback']))
	print("$_REQUEST[callback](");

print('{');

// Must be logged in
if(!isset($_SESSION['userID']))
{
	print('"success": false,');
	print('"message": "Not logged in"}');
	die();
}

// Get all products
$stmt = null;
if(isset($_REQUEST['type']))
{
	$stmt = $db->prepare('SELECT ProductID FROM RM_PRODUCT WHERE TypeID=? ORDER BY DateProduced DESC;');
	$stmt->bind_param('i', $_REQUEST['type']);
}
else
	$stmt = $db->prepare('SELECT ProductID FROM RM_PRODUCT ORDER BY DateProduced DESC;');

$success = $stmt->execute();
$stmt->bind_result($productID);

print('"products": [');
while($stmt->fetch())
{
	print('{');
	print("\"id\": $productID,\n");
	print('},');
}
$stmt->close();

print('],');
print('"success": true');

print('}');

if(isset($_REQUEST['callback']))
	print(')');
