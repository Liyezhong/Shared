#!/usr/bin/env python
# -*- coding: utf-8 -*-
# R.Wobst, @(#) Aug 12 2011, 21:18:26

# generate test files for DecryptUncompress tests

import zlib, struct
import sys, time
from hashlib import sha1
from Crypto.Cipher import AES

Nullkey = 20 * '\0'
MAXSIZE = 1024*1024

## @brief output chunk stream
#
# @param s - original stream
# @param enc - True if encryption

def chunk(s, enc = False):
    z = zlib.compress(s)

    # the first summand is the length of original string added by Qt
    qz = struct.pack('>I', len(s)) + z

    # the first summand is the chunklength added by me
    sz = struct.pack('>I', len(qz)) + qz

    if(enc):
        sz = ctrEncrypt(sz)

    return sz


## @brief output chunk stream
#
# @param name - name of output file
# @param ch - chunk

def out(name, ch):
    print "%s: %s byte" % (name, len(ch))
    fd = open('testresults/'+name, 'w')
    fd.write(ch)
    fd.close()

out("shortPlain", chunk(100*'a'))
out("maxsizePlain", chunk(MAXSIZE*'X'));
out("longPlain", 2*chunk(MAXSIZE*'z') +chunk('z'));

print "\nENCRYPTION:\n"

# compute AES key used as in KeyManager test
lvhash = sha1(Nullkey)
key0 = lvhash.digest()
S1 = "1.6180339887498949@Aciel"
aeskey = sha1(key0 + S1).digest()[:16]

aes = AES.new(aeskey, AES.MODE_ECB)

## @brief CTR encrypt s, starting with CTR block AES(000..01)
#
# @param s - string to be encrypted
# @return - encrypted string

def initAES():
    global ctr, off, lststream
    ctr = 16*[0]
    off = 0
    lststream = []

def ctrEncrypt(s):
    global ctr, off, lststream

    soff = 0                    # offset in s
    enc = [ord(x) for x in s]

    while True:
        if not off%16:         # true on first run!
            # increase CTR block and encrypt it
            i = 15
            while True:
                ctr[i] += 1
                if ctr[i]:
                    break
                i -= 1      # can never reach 0

            stream = ''.join([chr(x) for x in ctr])
            stream = aes.encrypt(stream)
            lststream = [ord(x) for x in stream]
            off = 0

        if soff > len(s) - 1:
            break

        enc[soff] ^= lststream[off]
        off += 1
        soff += 1

    return ''.join([chr(x) for x in enc])

initAES()
out("shortCipher", chunk(100*'a', True))
initAES()
out("maxsizeCipher", chunk(MAXSIZE*'X', True))
initAES()
S = chunk(MAXSIZE*'z', True)
T = chunk(MAXSIZE*'z', True)
U = chunk('z', True)
out("longCipher", S + T + U);
