
# build all components recursive

TEMPLATE = subdirs
SUBDIRS = \
	../AbstractFile/Test/AbstractFile.pro \
	../AES/Test/AES.pro \
	../CompressEncrypt/Test/CompressEncrypt.pro \
	../CryptoService/Test/CryptoService.pro \
	../DecryptUncompress/Test/DecryptUncompress.pro \
	../HMAC/Test/HMAC.pro \
	../ReadArchive/Test/ReadArchive.pro \
	../WriteArchive/Test/WriteArchive.pro

CONFIG += ordered
