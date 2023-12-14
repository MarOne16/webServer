<?php
    $queryString = $_SERVER['QUERY_STRING'];
    parse_str($queryString, $params);

    echo "Query String Parameters:<br>";
    foreach ($params as $key => $value) {
        echo "$key: $value<br>";
    }
?>