<html>
    <body>
        <?php
            // make database connection
            require 'connection.php';

            if ($_GET['request'] == 'update') {
                $sql = "SELECT * FROM Hardware";
            } elseif ($_GET['request'] == 'reset') {
                $sql = "DELETE FROM Hardware";
            }

            $result = $conn->query($sql);

            if ($result->num_rows > 0) {
                // output data of each row
                while($row = $result->fetch_assoc()) {
                    echo "<li>Mac: " . $row["macaddr"] . "</li>";
                    echo "<ul><li>Data: " . $row["ipaddr"] . "</li></ul>";
                }
            } else {
                echo "0 results";
            }
        ?>
    </body>
</html>