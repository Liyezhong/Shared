#!/usr/bin/env python
# -*- coding: utf-8 -*-
# R.Wobst, @(#) Aug 09 2011, 16:01:12

# Analyze a created archive and present it in nice form.
# Usage: AnalyzeArchive.py _archive_ [encrypt]
# where 'encr' is any string - use it if archive body is encrypted

import zlib, struct
from binascii import b2a_hex as ba
import sys, hmac
from hashlib import sha1

# GLOBAL DATA --------------------------------------------

Nullkey = 20 * '\0'
Onekey = 19 * '\0' + '\1'
S0 = "3,1415926535897932@Leica"
S1 = "1.6180339887498949@Aciel"

# compute AES key used as in KeyManager test
lkey = sha1(Nullkey + 'deviceID').digest()
vkey = sha1(Onekey + 'deviceID').digest()
impkey = sha1(Onekey + S0).digest()
Keytriple = (lkey, vkey, impkey)
Keytriple = zip(range(3), Keytriple)

# FUNCTIONS AND CLASSES ----------------------------------

## @brief CTR en/decrypt s, starting with CTR block AES(000..01)
#
# @param s - string to be en/decrypted
# @return - en/decrypted string

def initAES():
    from Crypto.Cipher import AES
    global ctr, off, lststream, aes
    ctr = 16*[0]
    off = 0
    lststream = []
    aeskey = sha1(vkey + S1).digest()[:16]
    aes = AES.new(aeskey, AES.MODE_ECB)

def ctrDecrypt(s):
    global ctr, off, lststream

    soff = 0                    # offset in s
    enc = [ord(x) for x in s]

    while True:
        if not off%16:         # true on first run!
            # increase CTR block and encrypt it
            i = 15
            while True:
                ctr[i] += 1
                ctr[i] &= 0xff
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

## @brief create and use HMAC instances

class HMAC3:
    def __init__(self):
        self.hmac = {}
        for (index, key) in Keytriple:
            self.hmac[index] = hmac.new(key = key, digestmod = sha1)

    def update(self, msg):
        for index in range(3):
            self.hmac[index].update(msg)

    def digests(self):
        return [self.hmac[index].digest()[:4] for index in range(3)]

## @brief read and compute HMAC

class readCompHMAC:
    def __init__(self, buffer):
        self.buffer = buffer
        self.hmac = HMAC3()

    # read n bytes and hash them
    def read(self, n):
        s = self.buffer[:n]
        self.hmac.update(s)
        self.buffer = self.buffer[n:]
        return s

    # return HMACs
    def getHMACs(self):
        return self.hmac.digests()


## @brief show one element in hex form

class showElement:

    ## @brief set buffer
    #
    # @param buffer - buffer with elements
    # @param prefix - prefix to be printed before elements
    def __init__(self, buffer, prefix):
        self.buffer = buffer
        self.prefix = prefix
        self.elem = None
        self.bytesShown = 0

    ## @brief print one element
    #
    # @param name - name of element
    # @param size - length in bytes
    # @param hex  - if true, show in hex form, as string else
    def showOneElement(self, name, size, hex=True):
        out = self.elem = self.buffer[:size]
        if hex:
            out = "0x" + ba(self.elem)
            if len(out) > 72:
                out = out[:72] + "..."
        print "%s\t%s:\t%s" % (self.prefix, name, out)
        self.buffer = self.buffer[size:]
        self.bytesShown += size

    ## @brief return last shown element as string
    def getLastElement(self):
        return self.elem

    ## @brief return number of bytes shown
    def getBytesShown(self):
        return self.bytesShown

# START ---------------------------------------------------------

fd = open(sys.argv[1], 'r')
buffer = fd.read()
fd.close()

# *** analyze header

se = showElement(buffer, "header")
se.showOneElement("magic", 4)
se.showOneElement("fmt_version", 1)
se.showOneElement("encrypted", 1)
encrypt = se.getLastElement() != '\0'
se.showOneElement("key_version", 2)
se.showOneElement("chain_index", 4)
se.showOneElement("no_entries", 2)
se.showOneElement("name_length", 2)
lg = struct.unpack('>H', se.getLastElement())[0]
se.showOneElement("archive name", lg, False)

hm = HMAC3()
hm.update(buffer[:se.getBytesShown()])

buffer = buffer[se.getBytesShown():]

for (dig, name) in zip(hm.digests(), ("A", "B", "C")):
    if dig == buffer[:4]:
        print "HMAC_%s is OK (0x%s)" % (name, ba(dig))
    else:
        print "difference in HMAC_%s: found %s, expected %s" % \
                (name, ba(buffer[:4]), ba(dig))
    buffer = buffer[4:]

del hm

print "\n*** body:\n"

# *** analyze body

# unpack body

if encrypt:
    initAES()
    buffer = ctrDecrypt(buffer)

chunk = ''

while buffer:
    l = struct.unpack('>I', buffer[:4])[0]
    core = buffer[4:4+l]
    buffer = buffer[4+l:]
    chunk += zlib.decompress(core[4:])

# iterate over entries

counter = 1

while chunk:
    if not struct.unpack('>I', chunk[:4])[0]:
        print "\n>>> END MAGIC 0x00 00 00 00 FOUND"
        chunk = chunk[4:]
        break        # ! test on end

    se = showElement(chunk, "entry %d --" % counter)

    se.showOneElement("magic", 4)

    se.showOneElement("name_length", 2)
    nlg = struct.unpack('>H', se.getLastElement())[0]
    se.showOneElement("entry name", nlg, False)

    se.showOneElement("entry length", 4)
    elg = struct.unpack('>I', se.getLastElement())[0]
    print "\t\t\t(%d)" % elg
    se.showOneElement("entry:", elg)

    hm = HMAC3()
    hm.update(struct.pack('>H', counter))
    counter += 1
    hm.update(chunk[:se.getBytesShown()])
    chunk = chunk[se.getBytesShown():]

    del se

    for (dig, name) in zip(hm.digests(), ("A", "B", "C")):
        if dig == chunk[:4]:
            print "HMAC_%s is OK (0x%s)" % (name, ba(dig))
        else:
            print "difference in HMAC_%s: found %s, expected %s" % \
                    (name, ba(chunk[:4]), ba(dig))
            print "chunk length left: %d" % (len(chunk) - 4)
        chunk = chunk[4:]

    del hm

if chunk:
    print "*** ERROR: chunk rest =", ba(chunk)
