<?php
include('settings.php');

// Must be logged in
if(!isset($_SESSION['userID']))
	header('Location: login.php');

if(isset($_POST['create']))
{
	try
	{
		// Check values

		// Insert main part
		$db->autocommit(false);
		$stmt = $db->prepare('INSERT INTO RM_CUSTOMER (FirstName,
													   MiddleName,
													   LastName,
													   Address,
													   City,
													   State,
													   Zip)
								VALUES (?, ?, ?, ?, ?, ?, ?)');
		$stmt->bind_param('sssssss', $_POST['first'],
									 $_POST['middle'],
									 $_POST['last'],
									 $_POST['address'],
									 $_POST['city'],
									 $_POST['state'],
									 $_POST['zip']);
		$success = $stmt->execute();
		$stmt->close();
		if(!$success)
			throw new Exception("Unable to add customer: $db->error");

		// Insert into subcategories as appropriate
		if(isset($_POST['product']))
		{
			$customerID = $db->insert_id;
			$stmt = $db->prepare('UPDATE RM_PRODUCT SET CustomerID=? WHERE ProductID=?');
			$stmt->bind_param('ii', $customerID, $productID);
		
			foreach($_POST['product'] as $productID)
			{
				if($productID == '')
					continue;
				if(!$stmt->execute())
					throw new Exception("Unable to add product to customer: $db->error");
			}
	
			$stmt->close();
		}

		// Success
		$db->commit();
		header('Location: index.php?msg=New+customer+created');
		die();
	}
	catch(Exception $e)
	{
		$db->rollback();
		$errorToUser = $e->getMessage();
	}
}

include('header.php');
?>

<h1>Add new customer</h1>
<form action="<?php print($PHP_SELF); ?>" method="post">
<table>
	<tr>
		<td>First Name</td>
		<td><input type="text" name="first" value="<?php print(htmlentities(@$_POST['first'])); ?>" /></td>
	</tr>
	<tr>
		<td>Middle</td>
		<td><input type="text" name="middle" value="<?php print(htmlentities(@$_POST['middle'])); ?>" /></td>
	</tr>
	<tr>
		<td>Last</td>
		<td><input type="text" name="last" value="<?php print(htmlentities(@$_POST['last'])); ?>" /></td>
	</tr>
	<tr>
		<td>Address</td>
		<td><input type="text" name="address" value="<?php print(htmlentities(@$_POST['address'])); ?>" /></td>
	</tr>
	<tr>
		<td>City</td>
		<td><input type="text" name="city" value="<?php print(htmlentities(@$_POST['city'])); ?>" /></td>
	</tr>
	<tr>
		<td>State</td>
		<td><input type="text" name="state" value="<?php print(htmlentities(@$_POST['state'])); ?>" /></td>
	</tr>
	<tr>
		<td>Zip</td>
		<td><input type="text" name="zip" value="<?php print(htmlentities(@$_POST['zip'])); ?>" /></td>
	</tr>
	<tr>
		<td>Products</td>
		<td id="productSelects"></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="submit" name="create" value="Create Customer" /></td>
	</tr>
</table>
</form>

<script type="text/javascript">
$(document).ready(function() {
	// Load data for all products
	var prodTd = $("#productSelects");
	$.getJSON('products.php?callback=?', function(data) {
		if(!data.success)
		{
			alert('Unable to load product data: ' + data.message);
			return;
		}

		var newSel = $("<select/>").attr('name', 'product[]')
								   .append('<option value="">Select one</option>')
					 			   .bind('change', function() {
										$("<a/>").attr('href', '#')
												 .text('Remove')
												 .bind('click', function() {
													$(this).prev().remove();
													$(this).next().remove();
													$(this).remove();
												 })
												 .appendTo(prodTd);
										$("<br />").appendTo(prodTd);
										$(this).clone(true).appendTo(prodTd);
								   })
								   .appendTo(prodTd);
			$.each(data.products, function(i, val) {
			$("<option/>").attr('value', val.id).text(val.id).appendTo(newSel);
		});
	});
});
</script>

<?php
include('footer.php');
?>
