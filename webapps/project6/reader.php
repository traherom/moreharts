<?php
header('Content-type: text/plain');

// Disable caching
header('Expires: Fri, 30 Oct 1998 14:19:41 GMT');
header('Cache-Control: max-age=0, no-cache, must-revalidate');
print(trim(htmlentities(file_get_contents('latest.txt'))));
?>
