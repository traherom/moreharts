function checkForm()
{
	var allGood = true;

	var s = $('moverSpeed');
	if(!isNaN(s.val()))
	{
		s.setClass('good');
	}
	else
	{
		s.setClass('bad');
		allGood = false;
	}

	var w = $('moverWidth');
	if(!isNaN(w.val()))
	{
		w.setClass('good');
	}
	else
	{
		w.setClass('bad');
		allGood = false;
	}

	var h = $('moverHeight');
	if(!isNaN(h.val()))
	{
		h.setClass('good');
	}
	else
	{
		h.setClass('bad');
		allGood = false;
	}

	return allGood;
}

// Alter box functions
function fillBox(sel)
{
	var text = new Array();
	text['instructions'] = "Welcome to one of the best Javascript demos ever.\n\n"
		+ "The arrows at the top and bottom of this page will cause this text box to scroll."
		+ " Use the form on the left of the page to change the parameters for this box, "
		+ "including how fast it bounces (delay between each pixel of movement) and how "
		+ "large it is in pixels. The menu on the right can be used to change the actual "
		+ "text in the box.\n\n"
		+ "Once you're done here, use the link in the lower right to return to People's Choice.";
	text['lorem'] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin "
		+ "vel feugiat nunc. Phasellus auctor, lectus id facilisis fermentum, sapien "
		+ "est tristique leo, et varius est neque non erat. Morbi vehicula lorem quam, "
		+ "id condimentum diam. Ut gravida eleifend quam, ac mollis dui consectetur in. "
		+ "Proin eu dolor eget mauris accumsan lacinia eu ac velit. Vestibulum a feugiat "
		+ "tellus. Proin a sapien justo. Nam turpis enim, tincidunt congue ultrices aliquam, "
		+ "adipiscing a tellus. Aenean id quam tellus. Suspendisse volutpat quam eu metus "
		+ "facilisis vel volutpat lacus accumsan. Pellentesque habitant morbi tristique "
		+ "senectus et netus et malesuada fames ac turpis egestas. Suspendisse ac felis eu "
		+ "nisi sagittis consequat. Quisque laoreet egestas ullamcorper. Nulla eget sem ut "
		+ "urna tincidunt accumsan in nec velit. Suspendisse potenti. Nunc eget sem non "
		+ "libero cursus sodales.\n\n"
		+ "Vestibulum vel mauris non augue placerat cursus. Proin venenatis dignissim "
		+ "nisl, at ornare orci molestie imperdiet. Vestibulum convallis tellus in ante "
		+ "bibendum id porta leo tristique. Donec aliquet sem sed orci volutpat ut varius "
		+ "lectus dapibus. Donec semper interdum justo quis dignissim. Vestibulum eget "
		+ "erat nec diam egestas pulvinar. In quis sapien neque, ut bibendum metus.";
	text['nonsense'] = "I can't believe you honestly took the time to click on a link for "
		+ "nonsense. Lorem ipsum I can sort of understand, maybe you don't know how wildly "
		+ "exciting that is. But seriously, if you know me at all you know that the nonsense "
		+ "paragraph is going to be totally, unbelievably, undeniably among the stupidest "
		+ "things you've ever read in your life. And if you're still reading at this point, "
		+ "you might want to be questioning your own sanity.";
	text['unknown'] = "Unknown text selected";
		
	if(text[sel])
	{
		$('mover').val(text[sel]);
	}
	else
	{
		$('mover').val(text['unknown']);
	}
}
function applyParams()
{
	if(checkForm())
	{
		bouncer.speed = $('moverSpeed').val();
		var width = $('moverWidth').val();
		var height = $('moverHeight').val();
		$('mover').css('height', height + 'px')
				  .css('width', width + 'px');
	}
	else
		alert('Correct the red-highlighted boxes.');
	return false;
}

// The box itself
var bouncer = {
	move : function() {
		// Find new location
		var obj = $("mover");
		if(bouncer.dir[0] == "n")
		{
			bouncer.y--;
			if(bouncer.y <= 0)
				bouncer.dir = bouncer.dir.replace('n', 's');
		}
		else
		{
			bouncer.y++;
			if(bouncer.y + obj.height() >= document.getHeight() - 1)
				bouncer.dir = bouncer.dir.replace('s', 'n');
		}
		if(bouncer.dir[1] == "w")
		{
			bouncer.x--;
			if(bouncer.x <= 0)
				bouncer.dir = bouncer.dir.replace('w', 'e');
		}
		else
		{
			bouncer.x++;
			if(bouncer.x + obj.width() >= document.getWidth() - 1)
				bouncer.dir = bouncer.dir.replace('e', 'w');
		}

		// Change div
		obj.css('top',  bouncer.y + "px")
		   .css('left', bouncer.x + "px");

		setTimeout(bouncer.move, bouncer.speed);
		},
	dir : "se",
	speed : 10,
	x : 0,
	y : 0
	};

var scrollTimer = null;

function registerEvents()
{
	// Form
	$('params').bind('submit', applyParams);
	$('moverSpeed').bind('change', checkForm);
	$('moverHeight').bind('change', checkForm);
	$('moverWidth').bind('change', checkForm);
	$('resetToDefaults').bind('click', function() {
		$('moverSpeed').val('10');
		$('moverHeight').val('150');
		$('moverWidth').val('300');
		//$('params').submit();
		});
	
	// Text choices
	$('menu').hover(
		function() {
			$('menu_links').css('height', 'auto');
		},
		function() {
			$('menu_links').css('height', '0');
		});
	$('instructions').bind('click', function() {
		fillBox('instructions');
		});
	$('lorem').bind('click', function() {
		fillBox('lorem');
		});
	$('nonsense').bind('click', function() {
		fillBox('nonsense');
		});
	
	// Arrows
	$('top_scroll').hover(
		function() {
			$('top_img').css('height', '60px')
						.css('width', '120px');
			scrollTimer = setInterval(function() {
				var m = $('mover');
				if(m.scrollTop > 0)
					m.scrollTop = m.scrollTop - 1;
				else
					clearInterval(scrollTimer);
				}, 20);
		},
		function() {
			$('top_img').css('height', '50px').css('width', '100px');
			clearInterval(scrollTimer);
		});
	$('bottom_scroll').hover(
		function() {
			$('bottom_img').css('top', '15px')
						   .css('height', '60px')
						   .css('width', '120px');
			scrollTimer = setInterval(function() {
				var m = $('mover');
				m.scrollTop = m.scrollTop + 1;
				}, 20);
		},
		function() {
			$('bottom_img').css('top', '25px')
						   .css('height', '50px')
						   .css('width', '100px');
			clearInterval(scrollTimer);
		});
}

// And get things going
$.ready(registerEvents);
$.ready(bouncer.move);
$.ready(applyParams);
$.ready(function() { fillBox('instructions'); });
