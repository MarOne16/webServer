<?php
$handle = fopen("cgi_in", "r");
if ($handle) {
    while (($line = fgets($handle)) !== false) {
        echo $line;
    }

    fclose($handle);
} else {
    die("Unable to open stdin!");
}
?>