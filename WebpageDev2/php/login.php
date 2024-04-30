<html>
    <body>
        <?php
            // support functions
            require 'support.php';
            
            // make database connection
            require 'connection.php';

            // make query to database to check against user data
            if ($_SERVER["REQUEST_METHOD"] == "POST") {
                if (isset($_POST['uname']) && isset($_POST['pass'])) {                   
                    $UNAME = test_input($_POST["uname"]);
                    $PASS = test_input($_POST["pass"]);
                    $hashed_password = crypt($PASS,'$6$rounds=5000$QMDuozZ3Uboh61i5$');
                    $sql = "SELECT 1 FROM Users WHERE Username='$UNAME' AND Password='$hashed_password';";
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
        ?>
    </body>
</html>