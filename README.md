# Coding and data compression
Programs for coding and data compression lectures

[Huffman codes](README.md#huffman-codes)\
[LZ77](README.md#LZ77)\
[JPEG-LS](README.md#JPEG-LS)\
[Linear Quantization](README.md#Linear-Quantization)\
[Differential coding](README.md#Differential-coding)

# Huffman codes 
**Implementation of dynamic Huffman coding** \
Program calculates and creates coding tree. Program is able to code and decode files for 8-bit ASCII coding. \
Alongside coded/decoded file program returns also entropy of coded data, average length of coded words and level of compression.

# LZ77
**Implementation of LZ77 compression algorithm**\
Program implementing LZ77 algorithm using 2 numbers instead of 3. \
File are coded/decoded using: 
  * (0, ASCII-code) - if symbol is not in dictionary buffer 
  * (i, j) - for longest possible prefix of coding buffer that is the same as dictionary buffer extended by coding buffer. 
    * i - is a shift in dictionary
    * j - is number of signs to copy

In program dictionary buffer has 255 signs and coding buffer 256 (i & j are 8-bit numbers) \
In addition to coding/decoding program also gives information about length of input file, length of coded file and level of compression.

# JPEG-LS
**Implementation of losless JPEG compression**\
Program uses 7 old standards and new standard of JPEG predictors to convert TGA images to JPEG images. \
We assume that borders of image are black - RGB (0, 0, 0). \
Program returns entropy for whole input img as well as every single color (R, G B).\
Then it calculates entopy for every predictor and outputs it.  

# Linear Quantization
**Implementation of linear quantization for TGA image**
Program performes quatization of given tga image according to inputed data and outputs quantized image along with mean squared error for whole image and single color data (R, G, B) as well as signal to noise ratio. \
Program should be run as follows **./a.out input_file output_file r g b** where:
  * input_file - is a tga file to quantize
  * output_file - is a file that quantized image should be saved
  * r - is a number of bits for red color
  * g - is a number of bits for green color
  * b - is a number of bits for blue color
r, g, b - should be in range of <0, 8>

# Differential coding
**Implementation of differential coding using linear quantizator**
Program codes/decodes every color byte of TGA image first quantizing it and then coding using differential codes. \
Program needs integer input parameter from 1 to 7. This parameter represents bits for quantizator. 

