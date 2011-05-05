<?php
require_once('library.php');

// Do we have products to add?
if(isset($_REQUEST['productIn']))
{
	checkSID();

	foreach($_REQUEST['productIn'] as $id)
	{
		$_SESSION['cart'][$id] = 1;
	}

	setMessage('Products added to cart');
}

// Do we have products to remove?
if(isset($_REQUEST['productOut']))
{
	checkSID();

	foreach($_REQUEST['productOut'] as $id)
	{
		unset($_SESSION['cart'][$id]);
	}

	setMessage('Products removed from cart');
}

// Are we supposed to check out?
if(isset($_REQUEST['checkout']))
{
	checkSID();
	header('Location: checkout.php');
}

include('header.php');

print('<form action="products.php" method="post" id="products">');

print('<a href="orders.php">View past spending sprees</a>');

if(isLoggedIn())
{
	print(' <a href="logout.php">Logout</a>');
}

// Get all products and show any not in cart
print('<h1>Products</h1>');
$products = getProducts();
foreach($products as $prodID => $name)
{
	if(isset($_SESSION['cart'][$prodID]))
		continue;

	// Is there an image for this product?
	$img = "products/prodUnknown.png";
	if(file_exists("products/prod$prodID.png"))
		$img = "products/prod$prodID.png";

	print('<div>');
	print("<img src='$img' alt='");
	print(htmlentities($name));
	print("' />");
	print('<span class="name">');
	print(htmlentities($name));
	print('</span>');
	print("<input type='checkbox' name='productIn[]' value='$prodID' class='hidden' />");
	print("</div>\n");
}

print('<div>');
addSID();
print('<input type="submit" name="add" value="Add to cart" />');
print('</div>');

// Show all products in cart
print('<h1>Cart</h1>');
if(@$_SESSION['cart'])
{
	foreach($_SESSION['cart'] as $prodID => $quantity)
	{
		// Is there an image for this product?
		$img = "products/prodUnknown.png";
		if(file_exists("products/prod$prodID.png"))
			$img = "products/prod$prodID.png";

		print('<div>');
		print("<img src='$img' alt='");
		print(htmlentities($products[$prodID]));
		print("' />");
		print('<span class="name">');
		print(htmlentities($products[$prodID]));
		print('</span>');
		print("<input type='checkbox' name='productOut[]' value='$prodID' class='hidden' />");
		print("</div>\n");
	}

	print('<div>');
	print('<input type="submit" name="remove" value="Remove from cart" /> ');
	print('<input type="submit" name="checkout" value="Checkout" />');
	print('</div>');
}
else
{
	print('<p>Your cart is currently empty</p>');
}


print('</form>');

include('footer.php');
?>
