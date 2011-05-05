<?php
// Double check
if(!isset($_REQUEST['msg']))
	die('Must provide msg');
if(trim($_REQUEST['msg']) == '')
	die('Blank msg not allowed');

// Shorten if needed
$msg = substr($_REQUEST['msg'], 0, 1024);

// Write message to file
$file = fopen('latest.txt', 'w');
fwrite($file, stripslashes($msg));
fclose($file);
?>
