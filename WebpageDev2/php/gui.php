<?php
    // define header
    header("Content-Type: 'application/json'");

    // make database connection
    require 'connection.php';

    // make query to database to check against user data
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['key1'])) {         
            $date = test_input($_POST["key1"]);
            $sql = "SELECT task, completed FROM GUI_Data WHERE date='$date';";
            $result = $conn->query($sql);
            $return_data = [['','']];
            $i = 0;
            if ($result->num_rows > 0) {
                // output data of each row
                while($row = $result->fetch_assoc()) {
                    $return_data[$i][0] = $row["task"];
                    $return_data[$i][1] = $row["completed"];
                    $i += 1;
                }
            }
            echo json_encode($return_data);
        }
        elseif (isset($_POST['key2']) && isset($_POST['key3']) && isset($_POST['key4'])){
            $date = test_input($_POST["key2"]);
            $task = test_input($_POST["key3"]);
            $value = test_input($_POST["key4"]);
            echo json_encode([$date,$task,$value]);
            $sql = "UPDATE GUI_Data SET completed='$value' WHERE task=$task AND date=$date;";
        }
        elseif (isset($_POST['key5'])) {
            $data = test_input($_POST["key5"]);
        }
    }
    
    
    // support functions
    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }
    
    function redirect($url, $permanent = false) {
        header('Location: ' . $url, true, $permanent ? 301 : 302);
        exit();
    }
    
    // error handling
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
?>