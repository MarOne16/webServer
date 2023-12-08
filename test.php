<?php
// Path to the input file
$input_file_path = '/goinfre/cgi_in';

// Check if the file exists
if (file_exists($input_file_path)) {
    // Read input from the file
    $input_data = file_get_contents($input_file_path);

    // Print the input
    echo "Input from {$input_file_path}:\n";
    echo $input_data;
} else {
    echo "Error: Input file not found.\n";
}
?>
