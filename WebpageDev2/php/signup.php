<html>
    <body>
        <?php    
            // make database connection
            require 'connection.php';

            // make querry to database to create new user
            if ($_SERVER["REQUEST_METHOD"] == "POST") {
                if (isset($_POST['uname']) && isset($_POST['pass'])) {
                    $UNAME = test_input($_POST["uname"]);
                    $PASS = test_input($_POST["pass"]);

                    $sql = "SELECT Username FROM Users WHERE Username='$UNAME';";
                    $result = $conn->query($sql);
                    echo "here";

                    if($result->num_rows == 0) {
                        $hashed_password = crypt($PASS,'$6$rounds=5000$QMDuozZ3Uboh61i5$');
                        $sql = "INSERT INTO Users (Username, Password) VALUES ('$UNAME', '$hashed_password')";
                        $conn->query($sql);

                        // after successful querry, redirect to next webpage
                        redirect("/pages/second.html");
                    } else {
                        redirect("/pages/signup.html");
                    }
                    
                    $conn->close();                    
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
    </body>
</html>