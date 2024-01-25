#!/usr/bin/python3

import sys
import math

def reg_hex(infilename, Nbits=944):

    if infilename[-4:] != '.dat':
        return -1
    outfilename = infilename[:-4] + '.com'

    f_in  = open(infilename,  'r')
    f_out = open(outfilename, 'w')
    bit = []

    for k, line in enumerate(f_in):
        word = str.split(line)
        bit_line = int(math.log(int(word[1])+1)/math.log(2))
        format_str = '%db' % bit_line
        bit_str = format(int(word[2]), format_str)

        for i in range(bit_line):
            if bit_str[i] == ' ':
                bit.append(0)
            else :
                bit.append(int(bit_str[i]))

    if len(bit)!=Nbits:
        print('Error::Number of bits is wrong!!', file=sys.stderr)
        sys.exit(1)
    
    for i in range(Nbits):
        if i%16==0:
            j = 0
            val = 0
        val += bit[i]*(2**(15-j))
        j=j+1
        if i%16==15:
            f_out.write( '%4.4X\n' % val)

    f_out.write( '%4.4X\n' % 0)
    f_out.write( '%4.4X\n' % 0)
    f_out.write( '%4.4X\n' % 0)
    f_out.write( '%4.4X\n' % 0)
    f_out.write( '%4.4X'   % 0)

    f_in.close()
    f_out.close()


if __name__=='__main__':
    reg_hex(sys.argv[1])
    print(sys.argv[1][:-4] + '.com is produced.')
