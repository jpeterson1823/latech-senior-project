<html>
<body>
<?php
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

    error_reporting(E_ALL);
    ini_set('display_errors', 1);

    //echo var_dump($_POST);

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['REQ']) && isset($_POST['DATA'])) {
            $REQ = test_input($_POST["REQ"]);
            $DATA = test_input($_POST["DATA"]);
            $sql = "INSERT INTO MacAddr (MAC, DATA) VALUES ('$REQ', '$DATA');";
            if ($conn->query($sql) == TRUE) {
            echo "Database Demo Successful <br>";
            $bool = true;
            } else {
            echo "Error: " . $sql . "<br>" . $conn->error;
            }
        }
    }

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        if (isset($_GET['REQ']) && isset($_GET['DATA'])) {
            $REQ = test_input($_GET["REQ"]);
            $DATA = test_input($_GET["DATA"]);
            $sql = "INSERT INTO MacAddr (MAC, DATA) VALUES ('$REQ', '$DATA');";
            if ($conn->query($sql) == TRUE) {
            echo "Database Demo Successful <br>";
            $bool = true;
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
?>
<body>
<html>