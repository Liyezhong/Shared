/****************************************************************************/
/** @file WriteArchive.cpp
 *
 *  @brief write and archive for export - define a function
 *
 *  $Version:   $ 0.1, 0.2
 *  $Date:      $ 2011-08-02, 2012-07-12
 *  $Author:    $ R.Wobst, Raju
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Aug 31 2011, 14:40:16
 *
 */
/****************************************************************************/

#include "ImportExport/WriteArchive/Include/WriteArchive.h"

namespace ImportExport {

/**
 * @brief create and write an archive for export
 *
 * @param archive_name - absolute pathname of archive
 * @param filelst - list of files in the archive, absolute pathnames
 * @param encrypt - true if archive body is encrypted, false else
 * @param key_version - version number of keys, recently 0
 */

void WriteArchive(QByteArray archive_name,
       QList<QByteArray> filelst, int version,
       bool encrypt,
       qint16 key_version)
{                                               //lint !e578 [Rw]
    // check filelst on double occurence
    {
        QSet<QByteArray> s;

        foreach(QByteArray f, filelst)
        {
            if(s.contains(f))
            {
                THROW(
                           QString("file %1 is contained twice").arg(
                                                           QString(f)));
            }
            s << f;
        }
    }

    // open fd
    FailSafeOpen fd(archive_name, 'w');

    // extract deviceID
    QList<QByteArray> parts = archive_name.split('_');

    Q_ASSERT(!parts.empty());

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
    // key_version
    fdwh.write(General::int2byte(key_version, 2));
    // hash chain index
    fdwh.write(General::int2byte(cs.getHashIndex()));
    // # of entries
    fdwh.write(General::int2byte(filelst.size(), 2));
    // length of archive name
    fdwh.write(General::int2byte(archive_name.size(), 2));
    // archive name
    fdwh.write(archive_name);

    // add HMACs
    QMap<QByteArray, QByteArray> hmacs = fdwh.getHmacs();

    foreach(QByteArray name, Constants::keynames)
    {
        fd.write(hmacs[name].mid(0,4));
    }

    // "file descriptor" for writing entries and computing HMACs
    CompressEncrypt fdw(&fd, cs, encrypt);

    // counter for entries
    int counter = 1;

    // iterate over filelst and write entries to fdw
    foreach(QByteArray filename, filelst)
    {
        FailSafeOpen fe;

        // open file, write size and contents
        // must be done here since there could be an error
        try
        {
            fe.open(filename, 'r');
        }
        catch(ImexException exc)
        {
            // warn(exc.getMsg());
        // XXX warning if cannot open, no fatal error, and continue
        }

        cs.initHmacs();

        // include entry counter in HMAC computation
        cs.updateHMACs(General::int2byte(counter, 2));
        ++counter;
        // entry magic
        fdw.write(QByteArray(Constants::entryMagic, 4));        
        // length of entry name
        fdw.write(General::int2byte(filename.size(), 2));
        // entry name
        fdw.write((filename.lastIndexOf('/') == -1 ? filename : filename.mid(filename.lastIndexOf('/') + 1)));
        // entry size
        fdw.write(General::int2byte(fe.size()));        

        while(true)
        {
            QByteArray readBytes =
                fe.read(Constants::WRITE_ARCH_BUFSIZE);
            if(!readBytes.size()) break;

            fdw.write(QByteArray(readBytes));
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

    // step forward in hash chain
    cs.stepHashChain();
}

}       // end namespace ImportExport

