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
		if(empty($_POST['num']))
			throw new Exception('Account number must be given');
		if($_POST['type'] > 3 || $_POST['type'] < 1)
			throw new Exception('Account type must be 1-3');
		$date = strtotime($_POST['date']);
		if(!$date)
			$date = time();

		// Insert main part
		$db->autocommit(false);
		$stmt = $db->prepare('INSERT INTO RM_ACCOUNT (AccountNum,
									AccountType, DateEstablished, Cost)
								VALUES (?, ?, ?, ?)');
		$stmt->bind_param('siid', $_POST['num'], $_POST['type'], $date, $_POST['cost']);
		$success = $stmt->execute();
		$stmt->close();
		if(!$success)
			throw new Exception("Unable to add account: $db->error");

		// Insert into subcategories as appropriate
		if(isset($_POST['product']))
		{
			$accountID = $db->insert_id;
			if($_POST['type'] == 1)
			{
				$stmt = $db->prepare('INSERT INTO RM_ACCOUNT1 (AccountNum, ProductID) VALUES (?, ?)');
				$stmt->bind_param('ii', $accountID, $productID);
		
				foreach($_POST['product'] as $productID)
				{
					if($productID == '')
						continue;

					if(!$stmt->execute())
						throw new Exception("Unable to add product to account: $db->error");
				}
		
				$stmt->close();
			}
			else if($_POST['type'] == 2)
				{
				$stmt = $db->prepare('INSERT INTO RM_ACCOUNT2 (AccountNum, ProductID) VALUES (?, ?)');
				$stmt->bind_param('ii', $accountID, $productID);
				
				foreach($_POST['product'] as $productID)
				{
					if($productID == '')
						continue;

					if(!$stmt->execute())
						throw new Exception("Unable to add product to account: $db->error");
				}
					
				$stmt->close();
			}
			else if($_POST['type'] == 3)
			{
				$stmt = $db->prepare('INSERT INTO RM_ACCOUNT3 (AccountNum, ProductID) VALUES (?, ?)');
				$stmt->bind_param('ii', $accountID, $productID);
				
				foreach($_POST['product'] as $productID)
				{
					if($productID == '')
						continue;

					if(!$stmt->execute())
						throw new Exception("Unable to add product to account: $db->error");
				}
			
				$stmt->close();
			}
		}

		// Success
		$db->commit();
		header('Location: index.php?msg=New+account+created');
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

<h1>Add new account</h1>
<form action="<?php print($PHP_SELF); ?>" method="post">
<table>
	<tr>
		<td>Account Number</td>
		<td><input type="text" name="num" value="<?php print(htmlentities(@$_POST['num'])); ?>" /></td>
	</tr>
	<tr>
		<td>Type</td>
		<td>
			<select name="type" id="type">
				<option value="">Select one</option>
				<?php
				for($i = 1; $i <= 3; $i++)
				{
					if(@$_POST['type'] != $i)
						print("\n\t\t\t\t<option value='$i'>$i</option>");
					else
						print("\n\t\t\t\t<option value='$i' selected='selected'>$i</option>");
				}
				?>
			</select>
		</td>
	</tr>
	<tr>
		<td>Established On</td>
		<td><input type="text" name="date" value="<?php print(htmlentities(@$_POST['date'])); ?>" /></td>
	</tr>
	<tr>
		<td>Cost</td>
		<td><input type="text" name="cost" value="<?php print(htmlentities(@$_POST['cost'])); ?>" /></td>
	</tr>
	<tr>
		<td>Products</td>
		<td id="productSelects"></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="submit" name="create" value="Create Account" /></td>
	</tr>
</table>
</form>

<script type="text/javascript">
$(document).ready(function() {
	$("#type").bind('change', function() {
		// Remove top "select one" element if it exists and
		// we've moved off of it now
		var sel = $(this);
		if(sel.val() != '')
			sel.children("[value='']").remove();

		// Clear out old product selections
		var prodTd = $("#productSelects");
		prodTd.empty();

		// Load data for this product type
		$.getJSON('products.php?callback=?', {type: sel.val()}, function(data) {
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
});
</script>

<?php
include('footer.php');
?>
