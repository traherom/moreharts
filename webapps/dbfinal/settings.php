<?php
// Settings as well as establishment of session and DB connection
$dbHost = 'john.cedarville.edu';
$dbUser = 'cs3610';
$dbPassword = 'cs3610';
$dbSchema = 'cs3610';

// Connect to database
$db = new mysqli($dbHost, $dbUser, $dbPassword, $dbSchema);
if(mysqli_connect_error())
	throw new Exception('Unable to connect to database');

// Start user session
if(headers_sent())
	throw new Exception('Unable to initialize session, called too late');
session_start();
?>
