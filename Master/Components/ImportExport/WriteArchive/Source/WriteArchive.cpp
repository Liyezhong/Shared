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

#include "ImportExport/WriteArchive/Include/WriteArchive.h"

namespace ImportExport {
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
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ARCHIVEFILE_FORMAT_WRONG);
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
 * \iparam encrypt - flag for encryption
 */
/****************************************************************************/
void WriteFiles(WriteAndHmac &fdwh, FailSafeOpen &fd,
       CryptoService &cs, QList<QByteArray> &filelist, bool &compressed, bool &enCrypt)
{

    // add HMACs
    QMap<QByteArray, QByteArray> hmacs = fdwh.getHmacs();

    foreach(QByteArray name, Constants::keynames)
    {
        fd.write(hmacs[name].mid(0,4));
    }

    // "file descriptor" for writing entries and computing HMACs
    CompressEncrypt fdw(&fd, cs, enCrypt, compressed);
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

        QByteArray readBytes = fe.read(Constants::WRITE_ARCH_BUFSIZE);
        // read the bytes
        while(readBytes.size())
        {
            fdw.write(QByteArray(readBytes));
            readBytes = fe.read(Constants::WRITE_ARCH_BUFSIZE);
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

}       // end namespace ImportExport

