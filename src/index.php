<?php

// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    // Get the raw POST data
    $imageData = file_get_contents('php://input');

    if ($imageData !== false) {
        // Generate a unique filename for the image
        $filename = 'uploaded_image_' . uniqid() . '.png';

        // Save the image data to a file
        file_put_contents($filename, $imageData);

        // Output a success message
        echo "Image uploaded successfully. File: $filename";
    } else {
        // Output an error message
        echo "Error reading image data.";
    }

} else {
    // Output an error message for unsupported request method
    echo "Unsupported request method.";
}
?>
