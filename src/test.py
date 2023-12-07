
from PIL import Image, ImageDraw

# Create a new image with a white background
width, height = 300, 200
image = Image.new("RGB", (width, height), "white")

# Create a drawing object
draw = ImageDraw.Draw(image)

# Draw a red rectangle on the image
rectangle_color = (255, 0, 0)  # RGB color for red
rectangle_position = [(50, 50), (250, 150)]  # Top-left and bottom-right coordinates
draw.rectangle(rectangle_position, fill=rectangle_color)

# Save the image
image.save("output_image.png")

# Display the image (optional)
image.show()
