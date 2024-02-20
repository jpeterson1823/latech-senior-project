<?php

    header("Content-Type: 'raw'");

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
    // echo "Connected successfully <br>"

    error_reporting(E_ALL);
    ini_set('display_errors', 1);

    // echo var_dump($_POST);

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['REQ']) && isset($_POST['DATA'])) {
            $REQ = test_input($_POST["REQ"]);
            $DATA = test_input($_POST["DATA"]);
            $sql = "SELECT IP FROM MacAddr WHERE MAC='$DATA';";
            $check = $conn->query($sql);
            if ($check->num_rows == 0) {
                $ipAddr = long2ip(rand(0, 4294967295));
                $sql = "INSERT INTO MacAddr (MAC, IP) VALUES ('$DATA', '$ipAddr');";
                $conn->query($sql);
                echo "$ipAddr";
            } else {
                $row = $check->fetch_assoc();
                echo $row['IP'];
            }
        }
    }

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        if (isset($_GET['REQ']) && isset($_GET['DATA'])) {
            $REQ = test_input($_GET["REQ"]);
            $DATA = test_input($_GET["DATA"]);
            $sql = "SELECT IP FROM MacAddr WHERE MAC='$DATA';";
            $check = $conn->query($sql);
            if ($check->num_rows == 0) {
                $ipAddr = long2ip(rand(0, 4294967295));
                $sql = "INSERT INTO MacAddr (MAC, IP) VALUES ('$DATA', '$ipAddr');";
                $conn->query($sql);
                echo "$ipAddr";
            } else {
                $row = $check->fetch_assoc();
                echo $row['IP'];
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