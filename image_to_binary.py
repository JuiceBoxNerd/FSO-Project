from PIL import Image

# Bash Paste:
# python3 /Users/anthonytatsuta/Documents/School/FSO-Project/image_to_binary.py

#  ░█████╗░██████╗░███████╗███╗░░██╗  ████████╗██╗░░██╗███████╗  ██╗███╗░░░███╗░█████╗░░██████╗░███████╗
#  ██╔══██╗██╔══██╗██╔════╝████╗░██║  ╚══██╔══╝██║░░██║██╔════╝  ██║████╗░████║██╔══██╗██╔════╝░██╔════╝
#  ██║░░██║██████╔╝█████╗░░██╔██╗██║  ░░░██║░░░███████║█████╗░░  ██║██╔████╔██║███████║██║░░██╗░█████╗░░
#  ██║░░██║██╔═══╝░██╔══╝░░██║╚████║  ░░░██║░░░██╔══██║██╔══╝░░  ██║██║╚██╔╝██║██╔══██║██║░░╚██╗██╔══╝░░
#  ╚█████╔╝██║░░░░░███████╗██║░╚███║  ░░░██║░░░██║░░██║███████╗  ██║██║░╚═╝░██║██║░░██║╚██████╔╝███████╗
#  ░╚════╝░╚═╝░░░░░╚══════╝╚═╝░░╚══╝  ░░░╚═╝░░░╚═╝░░╚═╝╚══════╝  ╚═╝╚═╝░░░░░╚═╝╚═╝░░╚═╝░╚═════╝░╚══════╝


# RGB
img = Image.open("/Users/anthonytatsuta/Documents/_D757650.jpg").resize((20, 20)).convert("RGB")
# --------------------
# Grayscale
# img = Image.open("/Users/anthonytatsuta/Documents/_D757650.jpg").resize((20, 20)).convert('L')  # 'L' = grayscale


# Get the pixel data (list of tuples)
pixels = list(img.getdata())


# Convert RGB to binary
binary_data = ''.join([format(r, '08b') + format(g, '08b') + format(b, '08b') for (r, g, b) in pixels])
# --------------------
# Convert grayscale to binary
# binary_data = ''.join([format(val, '08b') for val in pixels])


# Print or save the binary data
print(binary_data)

# Optionally, save binary data to a text file
with open("output_binary.txt", "w") as file:
    file.write(binary_data)