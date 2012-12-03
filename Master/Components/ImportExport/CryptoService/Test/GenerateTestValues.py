#!/usr/bin/env python
# -*- coding: utf-8 -*-
# R.Wobst, @(#) Jul 29 2011, 09:02:26

# Generate test values for unit tests of KeyManager.
# If any arg is given, the benchmark value is not computed.

import sys, hmac, time
from hashlib import sha1

Nullkey = 20 * '\0'

# key check: SHA1 value of 20*Nullbyte

lvhash = sha1(Nullkey)
print "initial keys (Leica, Viewer):", lvhash.hexdigest()

ihash = sha1(Nullkey + '3,1415926535897932@Leica')
print "initial Import key:", ihash.hexdigest()

# check HMAC without data
hm = hmac.new(key = lvhash.digest(), digestmod = sha1)
print "HMAC with no data:", hm.hexdigest()

# check HMAC with data
hm.update('some junk')
print "HMAC with data:", hm.hexdigest()

# check hash chain - indices 0 and 1000.000

idhash = sha1(Nullkey + "deviceID")
print "chain index 0:", idhash.hexdigest()

# *** BENCHMARK -- skipped if an arg is given

CNT = 1000*1000

key = idhash.digest()

if len(sys.argv) == 1:
    t0 = time.time()

    for i in xrange(CNT):
        h = sha1(key)
        key = h.digest()

    dt = time.time() - t0

    print "chain index %d:" % CNT, h.hexdigest()
    print "speed: %d updates/sec" % int(CNT/float(dt))

# compute members of index 1 and 2 of hash chain

key0 = lvhash.digest()
hash1 = sha1(key0)

print "hash chain index 1:", hash1.hexdigest()
key1 = hash1.digest()
print "hash chain index 2:", sha1(key1).hexdigest()

# compute key for initial AES encryption and encrypt block 0x00...01

S1 = "1.6180339887498949@Aciel"
aeskey = sha1(key0 + S1).digest()[:16]

from Crypto.Cipher import AES
aes = AES.new(aeskey, AES.MODE_ECB)
ctr = 15*'\0' + '\1'
c = aes.encrypt(ctr)

from binascii import b2a_hex
print "ciphertext:", b2a_hex(c)
