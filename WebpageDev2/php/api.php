<!-- expected json format for input
    {
        'ipaddr': '123.456.789.000',
        'macaddr': '12:34:56:78:90:00',
        'uid': '1',
        'sensorhex': '0x33',
        'action': 'pair'
    }
-->
<?php
    // get data sent in http request
    $data = file_get_contents('php://input');

    // parse data from http request
    $json = (isJson($data)) ? json_decode($data, true) : [];
    
    // if there is action to take
    if (array_key_exists('action', $json)) {
        switch ($json['action']) {

            // sending pair sensor data to database
            case 'pair':
                // sanitize input just in case
                require 'support.php';
                $ipaddr = test_input($json['ipaddr'] ?? '');
                $macaddr = test_input($json['macaddr'] ?? '');
                $userID = test_input($json['uid'] ?? '');
                $sensor_name = getSensorName($json['sensorhex'] ?? '');    // define sensor name dynamically

                // connect to database and insert sensor data accordingly
                require 'connection.php';
                $sql = "SELECT ipaddr FROM Hardware WHERE macaddr='$macaddr';"; // check if macaddr already in DB
                $check = $conn->query($sql);

                if ($check->num_rows == 0) {    // if macaddr not in DB then proceed to insert sensor data
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
                } else {    // else macaddr already in database, recieve needed data associated with macaddr
                    $row = $check->fetch_assoc();
                    echo $row['ipaddr'];
                }
                break;
            
            // query the database to see if any changes to sensors has occurred
            case 'changed':
                // sanitize input just in case
                require 'support.php';
                $macaddr = test_input($json['macaddr'] ?? '');

                // connect to and query database
                require 'connection.php';
                $sql = "SELECT COUNT(changed) FROM Hardware WHERE changed='1';";
                $result = $conn->query($sql);

                // if no changes then return nothing and terminate php scripts
                $data = (int) $result->fetch_assoc()['COUNT(changed)'];
                if ($data == 0) {
                    die();
                }

                // query database to determine what changed
                $sql = "SELECT h.macaddr, h.setting1, h.setting2, h.setting3 FROM Hardware h"
                    . " INNER JOIN Users_has_Hardware uh on h.idHardware = uh.Hardware_idHardware"
                    . " INNER JOIN Users u on uh.Users_idUsers = u.idUsers"
                    . " WHERE h.changed='1' AND h.macaddr='$macaddr';";
                $result = $conn->query($sql);
                echo json_encode($result->fetch_assoc());   // output changed data

                // update database to reflect that all changes to sensors are "seen" (known)
                $sql = "UPDATE Hardware h"
                . " INNER JOIN Users_has_Hardware uh on h.idHardware = uh.Hardware_idHardware"
                . " INNER JOIN Users u on uh.Users_idUsers = u.idUsers"
                . " SET h.changed = '0'"
                . " WHERE h.changed = '1';";
                $conn->query($sql);

                break;
            
            // proxy for sensors struggling to communicate with base controller
            case 'proxy':
                // get data to forward
                $data = file_get_contents('php://input');
                $url = 'http://192.168.9.49:23120'; // temp hardcoded destination

                // creating http request
                $context = [
                    'http' => [
                        'method' => 'POST',
                        'header'  => 'Content-type: application/x-www-form-urlencoded',
                        'content' => $data,
                        ],
                    ];

                // forward http request and get the http response
                $context = stream_context_create($context);
                $response = file_get_contents($url, False, $context);
            
            // terminate script if no actions to perform
            default:
                echo "No action specified, terminating..";
        }
        
    }

    ### extra support functions below ###

    // checks if a string is in json format
    function isJson($string) {
        json_decode($string);
        return json_last_error() === JSON_ERROR_NONE;
    }

    // function to (somewhat) dynamically determine a sensor name given hex data
    function getSensorName($string) {
        switch ($string) {
            case '0x01':
                $return = 'sensor name 1';
                break;
            case '0x02':
                $return = 'sensor name 2';
                break;
            case '0x03':
                $return = 'sensor name 3';
                break;
            default:
                $return = 'test sensor';
        }
        return $return;
    }
?>