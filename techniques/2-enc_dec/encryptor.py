#!/bin/python
import random 
import argparse

#create parser
parser = argparse.ArgumentParser(description="xor encryption tool")
#add args
parser.add_argument("-i","--file",help="input bin file",required=True)
parser.add_argument("-l","--layers",help="encryption layers",required=True)
parser.add_argument("-o","--output",help="output file")
args = parser.parse_args()

#open input file
shell_code = open(args.file,"rb").read()
#layers
layers = int(args.layers)

#create random key func
def generate_key():
    return random.randint(0, 256)
#enc code with xor func
def enc(code,key):
    return bytes([b ^ key for b in code])

#crete keys
keys=[]
for i in range(layers):
    keys.append(generate_key())

#enc shell code with keys
for i in keys:
    shell_code = enc(shell_code,i)

#check if the user want output in file
if args.output is None:
    print("unsigned char shell_code[] = {",end="")
    for i , b in enumerate(shell_code):
        print(f"0x{b:02x}",end='')
        if i < len(shell_code) -1 :
            print(", ",end='')
    print("};")
else:
    f = open(args.output,"w")
    f.write("unsigned char shell_code[] = {")
    for i , b in enumerate(shell_code):
        f.write(f"0x{b:02x}")
        if i < len(shell_code) -1 :
            f.write(", ")
    f.write("};")
    f.write(f"\n// keys: {[hex(k) for k in keys]}")
#keys with hex
print(f"// keys: {[hex(k) for k in keys]}")