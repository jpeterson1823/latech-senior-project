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

    $sql = "DROP TABLE MacAddr;CREATE TABLE MacAddr (MAC varchar(32), DATA varchar(255))";

    $conn->query($sql);
?>
</body>
</html>