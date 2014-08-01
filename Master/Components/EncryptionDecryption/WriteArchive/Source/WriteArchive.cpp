/****************************************************************************/
/*! \file WriteArchive.cpp
 *
 *  \brief write and archive for export - define a function
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-11-26
 *  $Author:    $ Raju
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "EncryptionDecryption/WriteArchive/Include/WriteArchive.h"

namespace EncryptionDecryption {
/****************************************************************************/
/*!
 * \brief create and write an archive for export
 *
 * \iparam archive_name - absolute pathname of archive
 * \iparam filelist - list of files in the archive, absolute pathnames
 * \iparam encrypt - true if archive body is encrypted, false else
 * \iparam compressed - true if archive body is compressed, false else
 * \iparam key_version - version number of keys, recently 0
 */
/****************************************************************************/
void WriteArchive(QByteArray archive_name,
       QList<QByteArray> filelist, int version,
       bool encrypt, bool compressed,
       qint16 key_version)
{                                               //lint !e578 [Rw]    
    // open fd
    FailSafeOpen fd(archive_name, 'w');

    QByteArray archivefilename;
    // sometime the file name consists of the path, so remove the path for splitting the file name
    archivefilename = archive_name.mid((archive_name.lastIndexOf('/') == 0 ? 0 :  archive_name.lastIndexOf('/') + 1));

    // extract deviceID
    QList<QByteArray> parts = archivefilename.split('_');

    if (parts.count() < 3)
    {
        THROWEXCEPTIONNUMBER(ERROR_ENCRYPTIONDECRYPTION_ARCHIVEFILE_FORMAT_WRONG);
    }

    QByteArray deviceID = parts[2];

    // CryptoService
    CryptoService cs(deviceID);

    // WRITE HEADER
    // write indirectly over WriteAndHmac class to compute HMACS
    WriteAndHmac fdwh(cs, &fd); // initHmacs() called for cs

    // header magic
    fdwh.write(Constants::headerMagic);
    // write the version number of the file
    fdwh.write(QByteArray(General::int2byte(version, 2)));
    // format_version 0
    fdwh.write(QByteArray(1,'\0'));
    // encrypted flag
    fdwh.write(QByteArray(1, encrypt ? '\1' : '\0'));
    // encrypted flag
    fdwh.write(QByteArray(1, compressed ? '\1' : '\0'));
    // key_version
    fdwh.write(General::int2byte(key_version, 2));
    // hash chain index
    fdwh.write(General::int2byte(cs.getHashIndex()));
    // # of entries
    fdwh.write(General::int2byte(filelist.size(), 2));
    // length of archive name
    fdwh.write(General::int2byte(archive_name.size(), 2));
    // archive name
    fdwh.write(archive_name);

    WriteFiles(fdwh, fd, cs, filelist, compressed, encrypt);

    // step forward in hash chain
    cs.stepHashChain();
}

/****************************************************************************/
/*!
 * \brief write files in the archive file
 *
 * \oparam fdwh - compress and encrypt
 * \oparam fd - file to open
 * \oparam cs - crypto service
 * \iparam filelist - list of files in the archive, absolute pathnames
 * \iparam compressed - flag for compression
 * \iparam encryption - flag for encryption
 */
/****************************************************************************/
void WriteFiles(WriteAndHmac &fdwh, FailSafeOpen &fd,
       CryptoService &cs, QList<QByteArray> &filelist, bool &compressed, bool &encryption)
{

    // add HMACs
    QMap<QByteArray, QByteArray> hmacs = fdwh.getHmacs();

    foreach(QByteArray name, Constants::keynames)
    {
        fd.write(hmacs[name].mid(0,4));
    }

    // "file descriptor" for writing entries and computing HMACs
    CompressEncrypt fdw(&fd, cs, encryption, compressed);
    // counter for entries
    int counter = 1;

    // iterate over filelist and write entries to fdw
    foreach(QByteArray filename, filelist)
    {
        FailSafeOpen fe;

        // open file, write size and contents
        // must be done here since there could be an error        
        fe.open(filename, 'r');

        cs.initHmacs();

        // include entry counter in HMAC computation
        cs.updateHMACs(General::int2byte(counter, 2));
        ++counter;
        // entry magic
        fdw.write(QByteArray(Constants::entryMagic, 4));
        // length of entry name
        fdw.write(General::int2byte(filename.size(), 2));
        // entry name
        fdw.write(filename);
        // entry size
        fdw.write(General::int2byte(fe.size()));
        /// file size returns in bytes
        qint64 filesize = fe.size();
        /// Why we are not reading the complete file?
        /// Ans: Sometimes log files might be keep on writing the data and at the same time when export is requested
        /// we end up writing more data in the archive file and LPKG extractor cannot extract the file as the file size is different
        /// and the bytes written to the file are different. For example if the file is continuously updating the data and export
        /// requested then when export reads file size as 1KB and writes the file size in archive file. While reading the data
        /// then file size is increased to 2KB because the file is keep on updating with the log data. While the file is getting
        /// retreived from the LPKG extractor then it has to read only 1KB of the data from the archive file. After that it checks
        /// security footer information and it does not find and gives file is tampered.
        /// - To avoid the above situation we read only the file size eventhough if the data is getting updated.
        QByteArray readBytes = fe.read(Constants::WRITE_ARCH_BUFSIZE, filesize);
        // read the bytes
        while(readBytes.size())
        {
            fdw.write(QByteArray(readBytes));
            filesize-= Constants::WRITE_ARCH_BUFSIZE;
            /// if the file size is negative then we stop reading further
            if (filesize < 0) {
                break;
            }
            readBytes = fe.read(Constants::WRITE_ARCH_BUFSIZE, filesize);
        }
        fe.close();

        // write HMACs
        hmacs = cs.getHmacs();

        // write HMACs without updating HMAC in cs (would be illegal)
        foreach(QByteArray name, Constants::keynames)
        {
            fdw.write(hmacs[name].mid(0,4), false);
        }
    }

    // write last magic without updating HMAC in cs
    fdw.write(QByteArray(4, '\0'), false);
}

}       // end namespace EncryptionDecryption

