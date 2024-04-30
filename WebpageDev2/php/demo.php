<?php

    header("Content-Type: 'text/plain'; charset=utf-8");

    // support functions
    require 'support.php';

    // make database connection
    require 'connection.php';
    
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['ipaddr']) && isset($_POST['macaddr']) && isset($_POST['sensor_name'])) {
            $ipaddr = test_input($_POST["ipaddr"]);
            $macaddr = test_input($_POST["macaddr"]);
            $sensor_name = test_input($_POST["sensor_name"]);
            $sql = "SELECT ipaddr FROM Hardware WHERE macaddr='$macaddr';";
            $check = $conn->query($sql);
            if ($check->num_rows == 0) {
                $sql = "INSERT INTO Hardware (macaddr, ipaddr, sensor_name"
                . ", setting1, setting2, setting3) VALUES "
                . "('$macaddr', '$ipaddr', '$sensor_name', '0', '0', '0');";
                $conn->query($sql);
                echo "$ipaddr";
            } else {
                $row = $check->fetch_assoc();
                echo $row['ipaddr'];
            }
        }
    }
    
    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        if (isset($_GET['macaddr'])) {
            $macaddr = test_input($_GET["macaddr"]);
            $sql = "SELECT COUNT(changed) FROM Hardware WHERE changed='1';";
            $result = $conn->query($sql);
            $data = (int) $result->fetch_assoc()['COUNT(changed)'];
            if ($data == 0) {
                die();
            }

            $sql = "SELECT macaddr, setting1, setting2, setting3 FROM Hardware WHERE changed='1' AND macaddr='$macaddr';";
            $result = $conn->query($sql);
            echo json_encode($result->fetch_assoc());

            $sql = "UPDATE Hardware SET changed = '0' WHERE changed = '1';";
            $conn->query($sql);
        }
    }
?>