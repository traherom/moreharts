// Bind stuff
$(document).ready(function() {
	$("#products div").bind('click', function() {
		// Toggle this product being checked
		var c = $(this).children("input[type=checkbox]");
		c.attr('checked', !c.attr('checked'));
	});
});

