/***************************************************
* mQuery - Ryan Morehart's pimpin' jQuery wanna be *
***************************************************/

// Add some functions to document to always have around
document.getHeight = function() {
	if(window.innerHeight)
		return window.innerHeight;
	else if(document.documentElement.clientHeight)
		return document.documentElement.clientHeight;
	else
		return document.body.clientHeight;
};
document.getWidth = function() {
	if(window.innerWidth)
		return window.innerWidth;
	else if(document.documentElement.clientWidth)
		return document.documentElement.clientWidth;
	else
		return document.body.clientWidth;
};

// Call mQuery to insert helper functions into standard DOM elements
// Can be called with just a string to get the element with that ID
var mQuery = $ = function(obj) {
	// Object we're wrapping around
	if(!obj)
		return false;

	// Just return it back if it already is us
	if(obj.isMQ)
		return obj;

	// Get element if we're given an ID.
	// No, we're not going crazy like jQuery with Sizzle. lol
	if(typeof(obj) == 'string')
		obj = document.getElementById(obj);

	// Mark as being already extended
	obj.isMQ = true;

	// Params
	obj.attr = function(name, value) {
		if(value)
		{
			this.setAttribute(name, value);
			return this;
		}
		else
		{
			return this.getAttribute(name);
		}
		};

	// CSS
	obj.css = function(name, value) {
		if(value)
		{
			this.style[name] = value;
			return this;
		}
		else
			return this.style[name];
		};
	obj.setClass = function(class) {
		this.setAttribute('class', class);
		};
	obj.clearClasses = function() {
		this.setAttribute('');
		};
	obj.width = function() {
		if(this.style.pixelWidth)
			return this.style.pixelWidth;
		else
			return this.offsetWidth;
		};
	obj.height = function() {
		if(this.style.pixelHeight)
			return this.style.pixelHeight;
		else
			return this.offsetHeight;
		};
	
	// Events	
	obj.bind = function(event, cb) {
		if(this.addEventListener)
			this.addEventListener(event, cb, false);
		else
			this.attachEvent(event, cb);
		return this;
		};
	obj.hover = function(overCB, offCB) {
		this.bind('mouseover', function() {
			// Don't handle multiple times
			if(this.isOver)
				return;
			this.isOver = true;
			
			overCB();
			});
		this.bind('mouseout', function(event) {
			// This "check if it's on a sub element" code I did steal from
			// jQuery. Straight forward though
			// Check if mouse(over|out) are still within the same parent element
			var parent = event.relatedTarget;

			// Traverse up the tree
			while(parent && parent !== this )
			{
				// Firefox sometimes assigns relatedTarget a XUL element
				// which we cannot access the parentNode property of
				try
				{
					parent = parent.parentNode;
				}
				catch(e)
				{
					break;
				}
			}

			if(parent !== this)
			{
				// handle event if we actually just moused on to a non sub-element
				this.isOver = false;
				offCB();
			}
			});
		};

	// Form things
	obj.val = function(newVal) {
		if(newVal)
		{
			if(this.value)
				this.value = newVal;
			else
				this.innerHTML = newVal;
		}
		else
		{
			if(this.value)
				return this.value;
			else
				return this.innerHTML;
		}
		};

	// Return the augmented object
	return obj;
};

// Static helper functions, keep from polluting the namespace
// Functions/attributes for readiness of DOM
mQuery.ready = function(cb) {
	// Save callback
	if(!mQuery.ready.cbs)
		mQuery.ready.cbs = new Array();
	mQuery.ready.cbs.push(cb);

	// Bind the proper onload event
	if(document.addEventListener)
	{
		document.addEventListener("DOMContentLoaded", mQuery.DOMContentLoaded, false);
		window.addEventListener("load", mQuery.ready, false);
	}
	else
	{
		document.attachEvent("onreadystatechange", mQuery.DOMContentLoaded);
		window.attachEvent("onload", mQuery.ready);
	}
};
mQuery.isReady = false;
mQuery.DOMContentLoaded = function() {
	if(document.addEventListener)
		mQuery.readyHandler();
	else
	{
		if(document.readyState === "complete")
			mQuery.readyHandler();
	}
};
mQuery.readyHandler = function() {
	// Don't let the handler run multiple times
	if(mQuery.isReady)
		return;
	mQuery.isReady = true;

	// Run each function registered
	while(mQuery.ready.cbs.length)
	{
		mQuery.ready.cbs.pop()();
	}
};

