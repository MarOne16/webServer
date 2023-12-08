<?php
$handle = fopen("php://stdin", "r");
if ($handle) {
    while (($line = fgets($handle)) !== false) {
        echo $line;
    }

    fclose($handle);
} else {
    die("Unable to open stdin!");
}
?>
