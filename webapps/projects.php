<?php
header("Content-Type: application/json");
if(isset($_GET['jsoncallback']))
	print($_GET['jsoncallback']);
?>
({
	"name" : "Ryan Morehart",
	"course" : "CS4220",	
	"projects" : [
		<?php
		$projects = glob("*", GLOB_ONLYDIR);
		$isFirst = true;
		foreach($projects as $name)
		{
			if(!$isFirst)
				print(',');		

			print("{\n\t\t\"title\" : \"");
			print(basename($name));
			print("\",\n\t\t\"url\" : \"");
			print($name);
			print("\",\n\t\t\"description\" : \"");
			if(file_exists("$name/description.txt"))
				print(str_replace(array("\n", '"'), array('\n', '\"'), trim(file_get_contents("$name/description.txt"))));
			else
				print("<p>No description supplied.</p>");
			print("\"\n\t\t}\n");

			$isFirst = false;
		}
		?>
	]
})
