<html>
<head>
	<title>Javascript demonstration</title>
	
	<link rel="stylesheet" type="text/css" href="../home.css" />
	<link rel="stylesheet" type="text/css" href="other.css" />

	<script type="text/javascript" src="mquery.js"></script>
	<script type="text/javascript" src="logic.js"></script>
</head>
<body>

<div id="top_scroll"><img id="top_img" src="up_arrow.png" /></div>

<form id="params" onsubmit="return false;">
<table>
	<tr>
		<td>Move Delay</td>
		<td><input type="text" id="moverSpeed" value="10" /> ms</td>
		<td rowspan="3">
			<input type="submit" id="apply" value="Apply" />
			<br />
			<input type="button" id="resetToDefaults" value="Defaults" />
		</td>
	</tr>
	<tr>
		<td>Height</td>
		<td><input type="text" id="moverHeight" value="150" /> px</td>
	</tr>
	<tr>
		<td>Width</td>
		<td><input type="text" id="moverWidth" value="300" /> px</td>
	</tr>
</table>
</form>

<div id="menu">
	<span class="title">Text</span
	<div id="menu_links">
		<a href="#" id="instructions">Instructions</a>
		<a href="#" id="lorem">Lorem Ipsum</a>
		<a href="#" id="nonsense">Nonsense</a>
	</div>
</div>

<a id="link_back" href="/cgi-bin/peopleschoice/index.pl">People's Choice</a>

<textarea id="mover"></textarea>

<div id="bottom_scroll"><img id="bottom_img" src="down_arrow.png" /></div>

</body>
</html>
