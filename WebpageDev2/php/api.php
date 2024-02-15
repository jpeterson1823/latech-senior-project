<html>
<body>

<?php
if ($_SERVER["REQUEST_METHOD"] == "GET") 
    echo "Study " . $_GET['subject'] . " at " . $_GET['web'];

if ($_SERVER["REQUEST_METHOD"] == "POST")
    echo "Key: DATA, DATA: " . $_POST['DATA']
?>

</body>
</html>