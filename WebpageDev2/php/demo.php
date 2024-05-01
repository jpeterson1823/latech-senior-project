<?php

    header("Content-Type: 'text/plain'; charset=utf-8");

    // support functions
    require 'support.php';

    // make database connection
    require 'connection.php';
    
    // Pair Sensor data informatin
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['ipaddr']) && isset($_POST['macaddr']) && isset($_POST['uid'])) {
            $ipaddr = test_input($_POST["ipaddr"]);
            $macaddr = test_input($_POST["macaddr"]);
            $sensor_name = "test sesnsor 1";
            $userID = test_input($_POST["uid"]);
            $sql = "SELECT ipaddr FROM Hardware WHERE macaddr='$macaddr';";
            $check = $conn->query($sql);
            if ($check->num_rows == 0) {
                $sql = "INSERT INTO Hardware (macaddr, ipaddr, sensor_name"
                . ", setting1, setting2, setting3) VALUES "
                . "('$macaddr', '$ipaddr', '$sensor_name', '0', '0', '0');";
                $conn->query($sql);

                $sql = "SELECT idHardware FROM Hardware WHERE macaddr = '$macaddr';";
                $result = $conn->query($sql);
                $hardwareID = $result->fetch_assoc()['idHardware'];

                $sql = "INSERT INTO Users_has_Hardware (Users_idUsers, Hardware_idHardware) VALUES ('$userID', '$hardwareID');";
                $conn->query($sql);

                echo "$ipaddr";
            } else {
                $row = $check->fetch_assoc();
                echo $row['ipaddr'];
            }
        }
    }
    
    // Get to see if Sensor data has changed
    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        if (isset($_GET['macaddr'])) {
            $macaddr = test_input($_GET["macaddr"]);
            $sql = "SELECT COUNT(changed) FROM Hardware WHERE changed='1';";
            $result = $conn->query($sql);
            $data = (int) $result->fetch_assoc()['COUNT(changed)'];
            if ($data == 0) {
                die();
            }

            $sql = "SELECT h.macaddr, h.setting1, h.setting2, h.setting3 FROM Hardware h"
                . " INNER JOIN Users_has_Hardware uh on h.idHardware = uh.Hardware_idHardware"
                . " INNER JOIN Users u on uh.Users_idUsers = u.idUsers"
                . " WHERE h.changed='1' AND h.macaddr='$macaddr';";
            $result = $conn->query($sql);
            echo json_encode($result->fetch_assoc());

            $sql = "UPDATE Hardware h"
            . " INNER JOIN Users_has_Hardware uh on h.idHardware = uh.Hardware_idHardware"
            . " INNER JOIN Users u on uh.Users_idUsers = u.idUsers"
            . " SET h.changed = '0'"
            . " WHERE h.changed = '1';";
            $conn->query($sql);
        }
    }
?>