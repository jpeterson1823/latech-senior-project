<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Prism</title>
</head>
<body>
<form method="POST" action="<?php echo $_SERVER['PHP_SELF'];?>">
  Name: <input type="text" name="fname">
  <input type="submit">
</form>
<form action="<?php echo $_SERVER['PHP_SELF'];?>" method="POST">
  <input type="submit" value="reset" name="reset">
</form>

<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

echo var_dump($_POST);

$name = (isset($name)) ? $name : "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  $GLOBALS["name"] = (isset($_POST['fname'])) ? htmlspecialchars($_POST['fname']) : null;
  if (empty($name)) {
    echo "Name is empty <br>";
  } else {
    echo "$name <br>";
  }
}
$servername = "mysql";
$username = "apache-server";
$password = "PRISM3";
$database = "mydb";
$port = "3306";

// Create connection
$conn = new mysqli($servername, $username, $password, $database);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}
echo "Connected successfully <br>";

$sql = "INSERT INTO names (name) VALUES ('$name')";

if ($conn->query($sql) === TRUE) {
  echo "New record created successfully <br>";
} else {
  echo "Error: " . $sql . "<br>" . $conn->error;
}

$sql = "TRUNCATE TABLE names";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  if (isset($_POST['reset'])) {
    if ($conn->query($sql) === TRUE) {
      echo "Database reset successfully <br>";
    } else {
      echo "Error: " . $sql . "<br>" . $conn->error;
    }
  }
}

//temp here

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  if (isset($_POST['REQ']) && isset($_POST['DATA'])) {
    $REQ = test_input($_POST["REQ"]);
    $DATA = test_input($_POST["DATA"]);
    $sql = "INSERT INTO MacAddr (MAC, DATA) VALUES ('$REQ', '$DATA');";
    if ($conn->query($sql) == TRUE) {
      echo "Database Demo Successfule <br>";
    } else {
      echo "Error: " . $sql . "<br>" . $conn->error;
    }
  }
}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}

// temp stops here


$sql = "SELECT * FROM names";

$result = $conn->query($sql);

if ($result->num_rows > 0) {
  // output data of each row
  while($row = $result->fetch_assoc()) {
    echo "Name: " . $row["name"] . "<br>";
  }
} else {
  echo "0 results";
}

echo "<br>";

// more temp here
$sql = "SELECT * FROM MacAddr";

$result = $conn->query($sql);

if ($result->num_rows > 0) {
  // output data of each row
  while($row = $result->fetch_assoc()) {
    echo "MAC: " . $row["MAC"] . "<br>";
    echo "DATA: " . $row["DATA"] . "<br>";
  }
} else {
  echo "0 results";
}
// end more temp here

$conn->close();
?>
</body>
</html>