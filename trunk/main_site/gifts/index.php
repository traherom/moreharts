<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8"> 
	<meta name="viewport" content="width=device-width, minimum-scale=1, maximum-scale=1"> 
	
	<title>Gift Exachanger</title>
	
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0a4.1/jquery.mobile-1.0a4.1.min.css" />
	<script src="http://code.jquery.com/jquery-1.5.2.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0a4.1/jquery.mobile-1.0a4.1.min.js"></script>
	
	<script src="main.js"></script>
</head>
<body>
	<div data-role="page" id="home" data-theme="b">
		<div  data-role="header">Gift Exchange</div> 
		<div  data-role="content">
			<form action="group_management.php" method="post">
				<input type="search" name="group-search" />
				<input type="submit" name="find" value="Find" />
				<input type="submit" name="find" value="Create" />
			</form>
			<ul data-role="listview" data-inset="true" data-theme="c" data-dividertheme="b">
				<li data-role="list-divider">Test</li>
				<li><a href="#create-group">Create group</a></li>
				<li><a href="">Create group</a></li>
			</ul>
		</div> 
		<div  data-role="footer">Copy</div> 
	</div>
	<div data-role="page" id="login">
		<div  data-role="header">Login</div> 
		<div  data-role="content">...</div> 
		<div  data-role="footer">...</div> 
	</div>
	<div data-role="page" id="create-group">
		<div  data-role="header">...</div> 
		<div  data-role="content">...</div> 
		<div  data-role="footer">...</div> 
	</div>
	<div data-role="page" id="view-group">
		<div  data-role="header">...</div> 
		<div  data-role="content">...</div> 
		<div  data-role="footer">...</div> 
	</div>
	<div data-role="page" id="create-member">
		<div  data-role="header">...</div> 
		<div  data-role="content">...</div> 
		<div  data-role="footer">...</div> 
	</div>
	<div data-role="page" id="view-member">
		<div  data-role="header">...</div> 
		<div  data-role="content">...</div> 
		<div  data-role="footer">...</div> 
	</div>
</body>
</html>