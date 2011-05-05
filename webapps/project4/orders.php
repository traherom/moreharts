<?php
require_once('library.php');

// Must be logged in
if(!isLoggedIn())
	header('Location: login.php?ret=orders.php');

include('header.php');
?>

<a href="logout.php">Logout</a>
<a href="products.php">Indulge yourself more</a>

<h1>Orders</h1>
<table>
	<tr>
		<th>Order ID</th>
		<th>Date</th>
		<th>Items</th>
		<th>Ship to</th>
	</tr>

	<?php
	// Show each order as a single table row
	$orders = getOrders();
	$products = getProducts();
	foreach($orders as $orderID => $details)
	{
		print('<tr>');

		print("<td>$orderID</td>");
		print('<td>');
		print(date('d M, Y', $details[date]));
		print('</td>');

		print('<td>');
		foreach($details['products'] as $productID => $quantity)
		{
			print(htmlentities($products[$productID]));
			if($quantity > 1)
				print(" (x$quantity)");
			print('<br />');
		}
		print('</td>');

		print('<td>');
		print(str_replace("\n", "<br />", htmlentities($details['address'])));
		print('</td>');

		print("</tr>\n\t");
	}
	?>

</table>

<?php
include('footer.php');
?>
