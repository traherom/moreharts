<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" 
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
	<title>Ryan Morehart - Because The Last Project Said Alex At The Top</title>

	<link rel="stylesheet" href="style.css" type="text/css" />

	<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.2/jquery.min.js"></script>
	<?php
	// Link to JS for this page
	$page = substr(basename($_SERVER['PHP_SELF']), 0, -4);
	if(file_exists("$page.js"))
		print("<script type='text/javascript' src='$page.js'></script>");
	?>
</head>
<body>

<?php
if(@$_REQUEST['msg'])
	$messageToUser = $_REQUEST['msg'];

if(isset($messageToUser))
	print("<div id='message' class='success'>$messageToUser</div>");

if(isset($errorToUser))
	print("<div id='message' class='error'>$errorToUser</div>");
?>

<!-- Begin content -->


