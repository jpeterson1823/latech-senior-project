<?php
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