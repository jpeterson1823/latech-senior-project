<?php
    // info needed to connect to mysql server
    $servername = "mysql";
    $username = "apache-server";
    $password = "PRISM3";
    $database = "PRISM_DB";
    $port = "16842";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $database);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }
?>