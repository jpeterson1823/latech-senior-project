<?php

    header("Content-Type: 'text/plain'; charset=utf-8");

    // make database connection
    require 'connection.php';
    
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['ipaddr']) && isset($_POST['macaddr'])) {
            $ipaddr = test_input($_POST["ipaddr"]);
            $macaddr = test_input($_POST["macaddr"]);
            $sql = "SELECT ipaddr FROM Hardware WHERE macaddr='$macaddr';";
            $check = $conn->query($sql);
            if ($check->num_rows == 0) {
                $sql = "INSERT INTO Hardware (macaddr, ipaddr) VALUES ('$macaddr', '$ipaddr');";
                $conn->query($sql);
                echo "$ipaddr";
            } else {
                $row = $check->fetch_assoc();
                echo $row['ipaddr'];
            }
        }
    }
    
    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        if (isset($_GET['ipaddr']) && isset($_GET['macaddr'])) {
            $ipaddr = test_input($_GET["ipaddr"]);
            $macaddr = test_input($_GET["macaddr"]);
            $sql = "SELECT ipaddr FROM Hardware WHERE macaddr='$macaddr';";
            $check = $conn->query($sql);
            if ($check->num_rows == 0) {
                $sql = "INSERT INTO Hardware (macaddr, ipaddr) VALUES ('$macaddr', '$ipaddr');";
                $conn->query($sql);
                echo "$ipaddr";
            } else {
                $row = $check->fetch_assoc();
                echo $row['ipaddr'];
            }
        }
    }
    
    
    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }

    // error handling
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
?>