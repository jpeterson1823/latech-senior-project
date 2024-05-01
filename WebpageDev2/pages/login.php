<?php
    // check if user is logged in
    session_start();
    if (isset($_SESSION['uid'])){
        header("Location: second");
    }
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="../styles/login-style.css">
    <title>PRISM | Login</title>
</head>
<body>
    <div class="Container1">
        <div class="Frame1">
            <div class="Group-Content">
                <div class="Group-Logo">
                    <!-- logo and picture here -->
                    <a href="/">
                        <img src="/assets/OIG4-grey-variant.png" alt="Diamond logo for PRISM">
                    </a>
                </div>
                <div class="Login-Text">
                    Login:
                </div>
                <form name="login-form" class="Username-Password" action="/php/login" target="_self" method="post" id="login-form">
                    <div class="Group-Username">
                        <label for="uname">Username:</label><br>
                        <input type="text" placeholder="Enter Username" id="uname" name="uname" required autofocus>
                    </div>
                    <div class="Group-Password">
                        <label for="pass">Password:</label><br>
                        <input type="password" placeholder="Enter Password" id="pass" name="pass" required>
                        <img src="/assets/show.png" alt="password visivility icon" id="togglePassword">
                        <!-- <span class="tooltip">"tooltip text"</span> -->
                    </div>
                    <div class="Login-Btn">
                        <input type="submit" value="Log In">
                    </div>
                </form>
                <div class="Group-SignUp" id="SignUp">
                    <p>Don't Have An Account? <a href="signup">Sign Up</a></p>
                </div>
            </div>
        </div>
    </div>
    <script>
        const togglePassword =
              document.querySelector('#togglePassword');
  
        togglePassword.
        addEventListener('click', function (e) {
            // Toggle the type attribute 
            togglePasswordVisibility();
 
            // Toggle the eye slash icon 
            if (togglePassword.src.match("/assets/hide.png")) {
                togglePassword.src = "/assets/show.png";
            } else {
                togglePassword.src = "/assets/hide.png";
            }
        }); 
        function togglePasswordVisibility() {
            var x = document.getElementById("pass");
            if (x.type === "password") {
                x.type = "text";
            } else {
                x.type = "password";
            }
        }
    </script>
</body>
</html>