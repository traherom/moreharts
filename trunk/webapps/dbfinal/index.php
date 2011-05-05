<?php
require_once('settings.php');

// Must be logged in
if(!isset($_SESSION['userID']))
	header('Location: login.php');

include('header.php');
?>

<form action="<?php print($PHP_SELF); ?>" method="post">

<div id="customers">
	<h1>Customers (<a href="newCustomer.php">add</a>)</h1>
	<table>
		<tr>
			<td>ID</td>
			<td>Name</td>
			<td>Address</td>
			<td>City</td>
			<td>State</td>
			<td>Zip</td>
		</tr>

		<?php
		// Get all products
		$stmt = $db->prepare('SELECT CustomerID,
									 CONCAT(LastName, ", ", FirstName, " ", MiddleName) AS Name,
									 Address,
									 City,
									 State,
									 Zip
									FROM RM_CUSTOMER
									ORDER BY LastName, FirstName, MiddleName DESC;');
		$success = $stmt->execute();
		$stmt->bind_result($customerID,
							$name,
							$address,
							$city,
							$state,
							$zip);

		while($stmt->fetch())
		{
			print("\n\t\t<tr>\n");
			print("\n\t\t<td>");
			print($customerID);
			print("</td>");
			print("\n\t\t<td>");
			print($name);
			print("</td>");
			print("\n\t\t<td>");
			print($address);
			print("</td>");
			print("\n\t\t<td>");
			print($city);
			print("</td>");
			print("\n\t\t<td>");
			print($state);
			print("</td>");
			print("\n\t\t<td>");
			print($zip);
			print("</td>");
			print("\n\t\t</tr>\n");
		}

		$stmt->close();
		?>
	</table>
</div>

<div id="employees">
	<h1>Employees (<a href="newEmp.php">add</a>)</h1>
	<table>
		<tr>
			<td>ID</td>
			<td>Name</td>
			<td>Address</td>
			<td>City</td>
			<td>State</td>
			<td>Zip</td>
		</tr>

		<?php
		// Get all products
		$stmt = $db->prepare('SELECT EmployeeID,
									 CONCAT(LastName, ", ", FirstName, " ", MiddleName) AS Name,
									 Address,
									 City,
									 State,
									 Zip
									FROM RM_EMPLOYEE
									ORDER BY LastName, FirstName, MiddleName DESC;');
		$success = $stmt->execute();
		$stmt->bind_result($employeeID,
							$name,
							$address,
							$city,
							$state,
							$zip);

		while($stmt->fetch())
		{
			print("\n\t\t<tr>\n");
			print("\n\t\t<td>");
			print($employeeID);
			print("</td>");
			print("\n\t\t<td>");
			print($name);
			print("</td>");
			print("\n\t\t<td>");
			print($address);
			print("</td>");
			print("\n\t\t<td>");
			print($city);
			print("</td>");
			print("\n\t\t<td>");
			print($state);
			print("</td>");
			print("\n\t\t<td>");
			print($zip);
			print("</td>");
			print("\n\t\t</tr>\n");
		}

		$stmt->close();
		?>
	</table>
</div>

<div id="products">
	<h1>Products</h1>
	<table>
		<tr>
			<td>ID</td>
			<td>Date Produced</td>
			<td>Time to Make</td>
			<td>Type</td>
			<td>Producer</td>
			<td>Tester</td>
			<td>Buyer</td>
		</tr>

		<?php
		// Get all products
		$stmt = $db->prepare('SELECT ProductID,
									 DateProduced,
									 TimeToMake,
									 TypeID,
									 ProducerID,
									 QualityControllerID,
									 CustomerID
									FROM RM_PRODUCT
									ORDER BY DateProduced DESC;');
		$success = $stmt->execute();
		$stmt->bind_result($productID,
							$date,
							$time,
							$type,
							$producerID,
							$qcID,
							$customerID);

		while($stmt->fetch())
		{
			print("\n\t\t<tr>\n");
			print("\n\t\t<td>");
			print($productID);
			print("</td>");
			print("\n\t\t<td>");
			print($date);
			print("</td>");
			print("\n\t\t<td>");
			print($time);
			print("</td>");
			print("\n\t\t<td>");
			print($type);
			print("</td>");
			print("\n\t\t<td>");
			print($producerID);
			print("</td>");
			print("\n\t\t<td>");
			print($qcID);
			print("</td>");
			print("\n\t\t<td>");
			print($customerID);
			print("</td>");
			print("\n\t\t</tr>\n");
		}

		$stmt->close();
		?>
	</table>
</div>

<div id="accounts">
	<h1>Accounts (<a href="newAccount.php">add</a>)</h1>
	<table>
		<tr>
			<td>Num</td>
			<td>Type</td>
			<td>Established</td>
			<td>Cost</td>
		</tr>

		<?php
		// Get all products
		$stmt = $db->prepare('SELECT AccountNum,
									 AccountType,
									 DateEstablished,
									 Cost
									FROM RM_ACCOUNT
									ORDER BY DateEstablished DESC;');
		$success = $stmt->execute();
		$stmt->bind_result($accountNum,
							$type,
							$date,
							$cost);

		while($stmt->fetch())
		{
			print("\n\t\t<tr>\n");
			print("\n\t\t<td>");
			print($accountNum);
			print("</td>");
			print("\n\t\t<td>");
			print($type);
			print("</td>");
			print("\n\t\t<td>");
			print($date);
			print("</td>");
			print("\n\t\t<td>");
			print($cost);
			print("</td>");
		}

		$stmt->close();
		?>
	</table>
</div>
</form>

<?php
include('footer.php');
?>
