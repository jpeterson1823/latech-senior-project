<?php
    // support functions
    require 'support.php';

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        require 'connection.php';

        $params = array();

        if (isset($_POST['lfov'])) {
            $params[1] = test_input($_POST['lfov']);
        }
        if (isset($_POST['rfov'])) {
            $params[2] = test_input($_POST['rfov']);
        }
        if (isset($_POST['sense'])) {
            $params[3] = test_input($_POST['sense']);
        }
        
        if (empty($params)) {
            die();
        }

        $sql = "UPDATE Hardware SET ";
        $check = 0;
        foreach ($params as $key => $value) {
            if ($check) {
                $sql .= ", ";
            }
            $sql .= "setting" . $key . " = '" . $value . "'";
            $check++;
        }
        $sql .= ", changed = '1' WHERE idHardware='". $_POST['HardwareID'] . "';";
        $result = $conn->query($sql);
        redirect("/pages/second.php");
    }

?>