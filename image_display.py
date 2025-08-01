from PIL import Image

# Your partial RGB matrix (fill out all 256 if you have them)
pixels = [

#  ██████╗░░█████╗░░██████╗████████╗███████╗  ██╗░░██╗███████╗██████╗░███████╗
#  ██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██╔════╝  ██║░░██║██╔════╝██╔══██╗██╔════╝
#  ██████╔╝███████║╚█████╗░░░░██║░░░█████╗░░  ███████║█████╗░░██████╔╝█████╗░░
#  ██╔═══╝░██╔══██║░╚═══██╗░░░██║░░░██╔══╝░░  ██╔══██║██╔══╝░░██╔══██╗██╔══╝░░
#  ██║░░░░░██║░░██║██████╔╝░░░██║░░░███████╗  ██║░░██║███████╗██║░░██║███████╗
#  ╚═╝░░░░░╚═╝░░╚═╝╚═════╝░░░░╚═╝░░░╚══════╝  ╚═╝░░╚═╝╚══════╝╚═╝░░╚═╝╚══════╝

(55,57,63), (28,28,37), (25,37,47), (33,47,59), (37,51,62), (37,52,63), (73,83,93), (74,83,90), (52,65,77), (63,91,105), (38,64,80), (79,107,122), (94,114,129), (116,127,135), (95,118,131), (79,105,119), (103,126,138), (82,113,131), (26,49,68), (25,58,76), 
(66,66,72), (39,40,51), (31,43,54), (42,56,68), (45,58,69), (72,75,84), (67,81,94), (52,63,74), (18,25,37), (30,43,58), (0,18,34), (48,73,89), (146,140,141), (112,107,110), (25,53,74), (75,103,117), (107,129,139), (80,110,127), (35,41,58), (39,24,46), 
(56,58,66), (46,46,58), (31,42,54), (37,50,63), (42,55,69), (76,75,84), (10,38,59), (38,50,62), (48,40,47), (49,44,53), (42,37,45), (108,93,100), (152,155,165), (68,83,98), (3,24,39), (37,64,67), (76,89,95), (70,80,88), (50,35,58), (83,19,60), 
(29,43,48), (44,42,52), (45,53,63), (62,72,80), (68,82,94), (26,53,75), (11,34,53), (71,79,85), (133,114,118), (144,108,114), (164,121,121), (141,119,130), (78,133,171), (101,134,160), (35,57,79), (87,92,103), (74,71,77), (71,63,72), (88,28,65), (91,33,72), 
(30,43,54), (37,46,59), (48,58,72), (64,74,87), (70,82,96), (27,59,84), (4,26,45), (21,64,91), (172,161,170), (194,155,159), (200,186,194), (103,118,143), (34,98,141), (71,121,153), (76,98,120), (107,112,130), (69,71,80), (78,66,78), (101,37,75), (149,125,144), 
(27,33,47), (32,41,56), (28,38,53), (32,41,56), (41,49,66), (22,38,56), (0,17,33), (0,55,90), (176,175,187), (190,152,155), (215,203,210), (143,143,159), (71,104,134), (96,129,149), (117,140,160), (142,146,162), (130,129,129), (130,111,120), (83,22,59), (84,30,67), 
(26,34,48), (34,44,59), (47,58,73), (35,39,55), (40,49,64), (29,42,58), (1,17,32), (0,37,70), (161,159,172), (200,155,155), (202,179,184), (211,155,148), (142,119,126), (128,151,166), (176,186,195), (174,163,164), (173,164,156), (152,136,136), (62,13,42), (80,13,54), 
(22,35,49), (64,72,81), (110,112,115), (113,112,113), (133,127,125), (105,113,118), (0,30,55), (2,38,70), (141,120,129), (161,138,145), (142,93,92), (181,128,123), (144,126,133), (129,149,163), (133,159,178), (188,175,174), (162,148,139), (156,141,133), (71,45,68), (43,23,56), 
(80,85,91), (116,115,116), (131,127,124), (134,130,126), (146,138,132), (109,119,125), (0,46,81), (48,61,85), (102,86,98), (54,81,106), (93,88,99), (198,154,149), (191,175,176), (177,177,181), (97,126,146), (177,176,180), (149,138,132), (150,141,135), (86,103,114), (58,89,109), 
(114,113,113), (120,117,116), (129,124,121), (134,127,124), (145,134,128), (122,130,135), (11,62,95), (32,40,57), (24,38,59), (23,47,69), (165,153,155), (218,195,192), (195,192,195), (155,164,171), (136,151,160), (172,177,180), (164,169,169), (166,174,174), (167,175,177), (168,176,177), 
(112,112,114), (121,119,120), (133,128,127), (138,133,131), (150,141,136), (113,121,128), (3,27,50), (26,42,55), (14,42,63), (85,76,84), (214,157,147), (164,128,124), (127,137,145), (140,153,159), (142,155,160), (154,165,167), (157,167,169), (158,167,169), (142,156,161), (134,149,157), 
(137,137,139), (145,145,144), (154,151,149), (163,157,153), (174,165,160), (127,130,133), (0,15,33), (66,86,99), (26,57,78), (77,77,86), (197,140,132), (113,92,96), (103,119,130), (171,173,173), (163,168,169), (151,160,163), (140,152,157), (135,149,155), (121,139,149), (115,134,145), 
(151,147,146), (160,153,151), (170,160,155), (177,165,158), (189,174,165), (138,137,138), (0,20,39), (72,95,110), (29,57,77), (65,72,83), (185,132,124), (115,91,96), (122,135,144), (180,179,176), (167,170,169), (159,165,166), (145,156,159), (139,151,157), (116,135,146), (121,138,148), 
(127,129,131), (153,148,147), (158,152,149), (160,154,151), (168,160,155), (121,125,129), (0,21,41), (83,107,120), (44,68,84), (49,63,77), (184,144,141), (98,82,89), (121,136,144), (175,175,173), (171,172,170), (160,166,166), (149,157,159), (141,153,157), (116,134,145), (111,130,142), 
(101,113,122), (142,141,142), (146,144,145), (153,150,149), (161,155,153), (129,130,133), (0,22,42), (74,101,117), (45,68,84), (47,67,83), (225,212,220), (127,129,142), (126,141,148), (168,170,169), (168,170,168), (159,164,165), (149,156,159), (143,154,158), (120,137,147), (112,132,143), 
(126,128,133), (141,140,141), (152,147,146), (161,153,149), (168,158,153), (162,154,150), (73,89,106), (74,101,117), (103,117,129), (81,98,113), (205,201,209), (128,140,152), (111,130,142), (144,155,159), (146,156,159), (151,160,162), (148,156,159), (149,157,160), (117,134,144), (95,118,132), 
(146,141,140), (152,146,143), (158,150,147), (163,154,149), (168,157,151), (199,186,183), (168,171,180), (66,95,113), (172,173,182), (144,152,163), (173,170,173), (62,83,97), (56,87,107), (90,114,130), (101,122,136), (112,131,142), (133,146,153), (131,144,150), (91,114,128), (80,105,121), 
(135,134,136), (141,139,140), (146,143,143), (150,146,145), (158,153,151), (164,163,165), (124,134,141), (92,113,129), (186,183,188), (98,109,118), (59,73,83), (49,75,91), (37,70,92), (91,112,126), (97,117,131), (84,109,127), (111,131,143), (96,118,131), (90,113,128), (91,115,130), 
(128,130,134), (136,137,140), (144,142,144), (148,146,146), (149,147,147), (143,142,142), (122,128,133), (133,138,143), (149,150,153), (105,113,119), (119,124,128), (111,119,125), (97,109,118), (119,129,135), (118,129,138), (115,128,137), (126,137,143), (102,116,125), (81,99,111), (87,106,118)
]

# Ensure the total number of pixels is exactly 256 by padding with black pixels
while len(pixels) < 256:
    pixels.append((0, 0, 0))  # Padding with black pixels

# Check the length of pixels list
print(len(pixels))  # This should print 256 for a 16x16 image, 3072 for 32 x32

# Create the image and put the pixel data
img = Image.new("RGB", (4, 4)) # RGB image

# Create a grayscale image instead of RGB
# img = Image.new("L", (20, 20))  # Grayscale image
# Convert RGB tuples to grayscale values
# pixels = [round(0.299 * r + 0.587 * g + 0.114 * b) for (r, g, b) in pixels] # Grayscale image

img.putdata(pixels)

# Resize for better visibility
img = img.resize((160, 160), Image.NEAREST)  # Upscale for better visibility
img.show()  # Display the image

# Tony: python3 /Users/anthonytatsuta/Documents/School/FSO-Project/image_display.py
# Wyatt: (If ya'll want to, idc)
# Sam: 
