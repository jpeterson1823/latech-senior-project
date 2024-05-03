<?php
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $data = file_get_contents('php://input');
        $url = 'http://192.168.9.49:23120';
        $proxy = '/';

        $context = [
            'http' => [
                'method' => 'POST',
                'header'  => 'Content-type: application/x-www-form-urlencoded',
                'content' => $data,
                ],
            ];

        $context = stream_context_create($context);

        $body = file_get_contents($url, False, $context);
    }
?>