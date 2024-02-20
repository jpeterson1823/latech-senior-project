
<?php
    header('Content-Type: application/json; charset=utf-8');

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
    //echo "Connected successfully <br>";

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        $sql = "SELECT * FROM MacAddr";
    }
    
    $result = $conn->query($sql);
    
    $data = [];
    
    while($row = $result->fetch_assoc()) {          
        $data[] = $row;  
    } 
    
    echo json_encode($data);
    
    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }
    ?>