#!/usr/bin/env python
# -*- coding: utf-8 -*-
# R.Wobst, @(#) Aug 04 2011, 18:35:15

import zlib, binascii, struct
import sys, time
from hashlib import sha1
from Crypto.Cipher import AES

Nullkey = 20 * '\0'
MAXSIZE = 1024*1024

def outhex(s, enc = False):
    z = zlib.compress(s)

    # the first summand is the length of original string added by Qt
    qz = struct.pack('>I', len(s)) + z

    # the first summand is the chunklength added by me
    sz = struct.pack('>I', len(qz)) + qz

    if(enc):
        sz = ctrEncrypt(sz)

    off0 = off = sz.find(10*'\0')
    if off == -1:
        hexout = binascii.b2a_hex(sz)
        if len(hexout) < 64:
            return '"%s"\n' % hexout
        retstr = ''
        print "crypt length:", len(sz)
        while hexout:
            retstr += '"%s"\n' % hexout[:64]
            hexout = hexout[64:]
        return retstr

    n = 0
    while sz[off] == '\0':
        n += 1
        off += 1
    return binascii.b2a_hex(sz[:off0]) + ("\n\t+ %d*'\\0'\n+\t " % n) + \
            binascii.b2a_hex(sz[off:])

print "short plain:", outhex(100*'a')
print "maxsize plain:", outhex(MAXSIZE*'X');
print "long plain:", outhex(MAXSIZE*'z') + outhex(MAXSIZE*'z') + \
        outhex('z');

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
print "short cipher:", outhex(100*'a', True)
initAES()
print "maxsize cipher:", outhex(MAXSIZE*'X', True);
initAES()
S = outhex(MAXSIZE*'z', True)
T = outhex(MAXSIZE*'z', True)
print "long cipher:", S + T + outhex('z', True);
