import zlib
import sys

# load content to string
data = open(sys.argv[1], 'rb').read()

#remove first 5 chars - a length or crc?
data2 = data[6:]

#decompress
text = zlib.decompress(data2, -15)

#write to output text file
f = open(sys.argv[2], 'wb')
f.write(text)
f.close()
