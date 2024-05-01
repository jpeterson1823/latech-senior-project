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
                    $sql = "SELECT idUsers FROM Users WHERE Username='$UNAME' AND Password='$hashed_password';";
                    $result = $conn->query($sql)->fetch_assoc();

                    
                    
                    // after successful query, redirect to next webpage
                    if ($result) {
                        session_start();
                        $_SESSION['uid'] = $result['idUsers'];
                        redirect("/pages/second");
                    } 
                    // if query fails or returns false, prompt user
                    else {
                        redirect("/pages/login");
                    }
                }
            }
        ?>
    </body>
</html>