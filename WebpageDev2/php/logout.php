<?php
    // check if user is logged in
    session_start();
    if (isset($_SESSION['uid'])){
        header("Location: /");
        session_destroy();
    }
?>