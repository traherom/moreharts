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
		$stmt = $db->prepare('INSERT INTO RM_EMPLOYEE (
									FirstName, MiddleName, LastName,
									Address, City, State, Zip)
								VALUES (?, ?, ?, ?, ?, ?, ?)');
		$stmt->bind_param('sssssss', $_POST['first'], $_POST['middle'], $_POST['last'],
							$_POST['address'], $_POST['city'], $_POST['state'], $_POST['zip']);
		$success = $stmt->execute();
		$stmt->close();
		if(!$success)
			throw new Exception("Unable to add employee: $db->error");

		// Insert into subcategories as appropriate
		$employeeID = $db->insert_id;
		if($_POST['job'] == 'qc')
		{
			$stmt = $db->prepare('INSERT INTO RM_QUALITY_CONTROLLER (EmployeeID, TypeID) VALUES (?,?)');
			$stmt->bind_param('ii', $employeeID, $_POST['type']);
			$success = $stmt->execute();
			$stmt->close();
			if(!$success)
				throw new Exception("Problem adding employee to quality controller list: $db->error");
		}
		elseif($_POST['job'] == 'tech')
		{
			$stmt = $db->prepare('INSERT INTO RM_TECH_STAFF (EmployeeID, Position) VALUES (?,?)');
			$stmt->bind_param('is', $employeeID, $_POST['position']);
			$success = $stmt->execute();
			$stmt->close();
			if(!$success)
				throw new Exception("Problem adding employee to tech staff: $db->error");

		}
		elseif($_POST['job'] == 'worker')
		{
			$stmt = $db->prepare('INSERT INTO RM_WORKER (EmployeeID, MaxUnitsProduced) VALUES (?,?)');
			$stmt->bind_param('ii', $employeeID, $_POST['units']);
			$success = $stmt->execute();
			$stmt->close();
			if(!$success)
				throw new Exception("Problem adding employee to workers: $db->error");

		}
		else
		{
			throw new Exception('Invalid job');
		}

		// Success
		$db->commit();
		header('Location: index.php?msg=New+employee+created');
		die();
	}
	catch(Exception $e)
	{
		$errorToUser = $e->getMessage();
	}
}

include('header.php');
?>

<h1>Add new employee</h1>
<form action="<?php print($PHP_SELF); ?>" method="post">
<table>
	<tr>
		<td>First name</td>
		<td><input type="text" name="first" value="<?php print(htmlentities(@$_POST['first'])); ?>" /></td>
	</tr>
	<tr>
		<td>Middle name</td>
		<td><input type="text" name="middle" value="<?php print(htmlentities(@$_POST['middle'])); ?>" /></td>
	</tr>
	<tr>
		<td>Last name</td>
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
		<td>Job</td>
		<td>
			<select name="job" id="job">
				<?php
				if(!isset($_POST['job']))
					print('<option id="expander" value="">Select job</option>');

				$jobs = array('qc' => 'Quality Controller',
							  'tech' => 'Tech Staff',
							  'worker' => 'Worker');
				foreach($jobs as $key => $val)
				{
					if(@$_POST['job'] != $key)
						print("\n\t\t\t\t<option value='$key'>$val</option>");
					else
						print("\n\t\t\t\t<option value='$key' selected='selected'>$val</option>");
				}
				?>
			</select>
		</td>
	</tr>
	<tr class="tech expandable">
		<td>Position</td>
		<td><input type="text" name="position" value="<?php print(htmlentities(@$_POST['position'])); ?>" /></td>
	</tr>
	<tr class="qc expandable">
		<td>Product Type</td>
		<td>
			<select name="type">
				<?php
				for($i = 1; $i <= 3; $i++)
				{
					if($_POST["type"] != $i)
						print("\n\t\t\t\t<option value='$i'>$i</option>");
					else
						print("\n\t\t\t\t<option value='$i' selected='selected'>$i</option>");
				}
				?>
			</select>
		</td>
	</tr>
	<tr class="worker expandable">
		<td>Max Units</td>
		<td><input type="text" name="units" value="<?php print(htmlentities(@$_POST['units'])); ?>" /></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="submit" name="create" value="Add Employee" /></td>
	</tr>
</table>
</form>

<script type="text/javascript">
$(document).ready(function() {
	$("#job").bind('change', function() {
		// Remove top "select one" element if it exists and
		// we've moved off of it now
		var sel = $("#job");
		if(sel.val() != '')
			$("#expander").remove();

		// Show appropirate part of table
		if(sel.val() == 'qc')
		{
			$('tr.qc').show();
			$('tr.tech').hide();
			$('tr.worker').hide();
		}
		else if(sel.val() == 'tech')
		{
			$('tr.qc').hide();
			$('tr.tech').show();
			$('tr.worker').hide();
		}
		else if(sel.val() == 'worker')
		{
			$('tr.qc').hide();
			$('tr.tech').hide();
			$('tr.worker').show();
		}
	});
});
</script>

<?php
include('footer.php');
?>
