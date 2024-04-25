<html>
    <body>
        <?php
            // make database connection
            require 'connection.php';

            // make query to database to check against user data
            if ($_SERVER["REQUEST_METHOD"] == "POST") {
                if (isset($_POST['uname']) && isset($_POST['pass'])) {                   
                    $UNAME = test_input($_POST["uname"]);
                    $PASS = test_input($_POST["pass"]);
                    $sql = "SELECT 1 FROM Users WHERE Username='$UNAME' AND Password='$PASS';";
                    $result = $conn->query($sql);

                    // after successful query, redirect to next webpage
                    if ($result->fetch_assoc()) {
                        redirect("/pages/second.html");
                    } 
                    // if query fails or returns false, prompt user
                    else {
                        redirect("/pages/login.html");
                    }
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