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

    if ($_GET['request'] == 'update') {
        $sql = "SELECT * FROM MacAddr";
    } elseif ($_GET['request'] == 'reset') {
        $sql = "DELETE FROM MacAddr";
    } elseif ($_GET['request'] == 'setup') {
        $sql = "CREATE TABLE MacAddr (MAC varchar(32), DATA varchar(255))";
    }

    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        // output data of each row
        while($row = $result->fetch_assoc()) {
            echo "<li>Mac: " . $row["MAC"] . "</li>";
            echo "<ul><li>Data: " . $row["IP"] . "</li></ul>";
        }
    } else {
        echo "0 results";
    }
?>
</body>
</html>