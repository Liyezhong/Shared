#!/usr/bin/env python
# -*- coding: utf-8 -*-
# R.Wobst, @(#) Aug 02 2011, 19:13:36

# output file contents in hex form, at most 64 chars per line

import sys, binascii

sz = open(sys.argv[1]).read()

hexout = binascii.b2a_hex(sz)
if len(hexout) < 64:
    print '"%s"\n' % hexout

print "crypt length:", len(sz)
print

while hexout:
    print '"%s"' % hexout[:64]
    hexout = hexout[64:]
