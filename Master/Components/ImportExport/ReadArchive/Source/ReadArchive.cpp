/****************************************************************************/
/** @file ReadArchive.cpp
 *
 *  @brief read an archive - define a function
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-15
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Aug 31 2011, 13:15:10
 *
 */
/****************************************************************************/

#include "ImportExport/ReadArchive/Include/ReadArchive.h"


namespace ImportExport {

/* HELP CLASS *********/

ReadAndBuffer::ReadAndBuffer(FailSafeOpen* fd): m_fd(fd),
        m_buffer(QByteArray())
{
}


/**
 * @brief read 'size' bytes from m_fd, store it to buffer and return in
 *
 * @param size - how many bytes to read
 * @return - bytes read
 */

QByteArray ReadAndBuffer::read(int size)
{
    QByteArray ret = m_fd->read(size);
    m_buffer += ret;
    return ret;
}


/* MAIN FUNCTION FOR READING AN ARCHIVE */

/**
 * @brief read an archive
 *
 * The action depends on the Purpose string:
 * "Import" - unpack the files to disk, check the signature
 * "Viewer" - unpack the files to QMap, check the signature
 * "Leica" - only check signatures and collect filenames
 *
 * @param archive_name - absolute pathname of archive;
 *                      the archive name must have the structure
 *                      Colorado_zzz_xxx_yyy.cea
 *                      where xxx is the serial number and yyy is
 *                      a time stamp in ISO format (date and time
 *                      in UTC). zzz denotes the archive kind or
 *                      helps distinguish archives.
 * @param fd - AbstractFile instance
 * @param purpose - purpose for which the archive is opened; must have
 *                      one of the values of Constants::keynames
 * @param keydata - optional keydata byte array
 * @param importfilelist - optional import file list
 * @param filepath - optional for the path of file
 */

void ReadArchive(QByteArray archive_name,
	   AbstractFile* fout,
	   QByteArray purpose,
           QByteArray keydata, QStringList importfilelist, QString filepath)
{
    Q_ASSERT(Constants::keynames.contains(purpose));
    QByteArray archivefilename;

    // open fd
    FailSafeOpen fd(archive_name, 'r');

    archivefilename = archive_name.mid((archive_name.lastIndexOf('/') == 0 ? 0 :  archive_name.lastIndexOf('/') + 1));

    // extract deviceID
    QList<QByteArray> parts = archivefilename.split('_');

    Q_ASSERT(!parts.isEmpty());

    QByteArray deviceID = parts[2];

    // READ HEADER
    // buffer read bytes and compute HMACs at the end when the key
    // is computed or known

    ReadAndBuffer fdrb(&fd);

    // header magic
    if(fdrb.read(4) != Constants::headerMagic)
    {
        THROW("illegal magic in header");
    }

    // export configuration file version - future use
    int fileversion = General::byte2int(fdrb.read(2).data(), 2);

    // format_version 0
    char *version = fdrb.read(1).data();
    Q_ASSERT(version[0] == '\0');
    Q_UNUSED(version);

    // encrypted flag
    bool encrypt = fdrb.read(1).data()[0] != '\0';      //lint !e578 [Rw]

    // key_version
    int key_version = General::byte2int(fdrb.read(2).data(), 2);
    Q_ASSERT(!key_version);
    Q_UNUSED(key_version);

    // hash chain index
    int hashIndex = General::byte2int(fdrb.read(4).data());
    // XXX @TODO test on reasonable value

    // optionally read keydata

    if(keydata.isEmpty())
    {
        Q_ASSERT(purpose == "Import");

        // read the keys
        FailSafeOpen keyfile(Constants::keyfile, 'r');
        keydata = keyfile.read();
        keyfile.close();
    }
    else
    {
        // compute the right key
        int off = (purpose == "Viewer" ? Constants::HASH_SIZE : 0);
        QByteArray key = keydata.mid(off, Constants::HASH_SIZE);
        CryptoService::computeKey(key, deviceID, hashIndex);
        keydata.replace(off, Constants::HASH_SIZE, key);
    }

    // # of entries
    int noEntries = General::byte2int(fdrb.read(2).data(), 2);

    // length of archive name
    int lgname = General::byte2int(fdrb.read(2).data(), 2);

    // archive name
    QByteArray name = fdrb.read(lgname);    
    // sometime the file name consists of the path, so remove the path compare the file name
    if(name != archivefilename)
    {
        THROW("differing archive name in archive: " + name);
    }

    // check HMACs
    // HMACs are stored in the order of 'keynames' constant list

    CryptoService cs(deviceID, keydata);

    cs.initHmacs();
    cs.updateHMACs(fdrb.getBuffer());

    QMap<QByteArray, QByteArray> hmacs = cs.getHmacs();

    foreach(QByteArray name, Constants::keynames)
    {
        QByteArray hmacval = fd.read(4);
        if(name == purpose && hmacval != hmacs[name].left(4))
        {
            THROW("illegal header HMAC for key " + name);
        }
    }    

    // "file descriptor" for read entries and computing HMACs
    DecryptUncompress fdr(&fd, cs, encrypt);

    // counter for entries
    int counter = 0;

    // iterate over entries
    for(;;)
    {
        cs.initHmacs();

        // include entry counter in HMAC computation
        ++counter;
        cs.updateHMACs(General::int2byte(counter, 2));

        // entry magic
        QByteArray magic = fdr.read(4);

        if(!General::byte2int(magic.data()))
        {
            --counter;
            break;      // magic 0: last, dummy entry
        }

        if(magic != Constants::entryMagic)
        {
            THROW("illegal magic in entry no. " +
                                QString::number(counter));
        }        

        // length of entry name
        int enamelen = General::byte2int(fdr.read(2).data(), 2);

        // entry name
        QByteArray entryname = fdr.read(enamelen);

        // entry size
        int entrysize = General::byte2int(fdr.read(4).data());

        bool importfile = true;
        // check which files need to be imported
        foreach (QString importfilename, importfilelist) {            
            importfile = false;
            // check the file name
            if (QString(entryname).contains(importfilename))   {
                importfile = true;
                if (importfilelist.indexOf(importfilename) != -1) {
                    // remove the file name from the list
                    //importfilelist.removeAt(importfilelist.indexOf(importfilename));
                    entryname.clear();
                    entryname = qPrintable(filepath + QString(importfilename));
                }
                break;
            }
        }


        // check whether file required to import, if not dont create the file
        if (importfile) {
            // open abstract file for write
            fout->open(entryname);
        }

        while(entrysize > 0)
        {
            int bytes2read = entrysize < Constants::WRITE_ARCH_BUFSIZE ?
                entrysize : Constants::WRITE_ARCH_BUFSIZE;

            QByteArray readBytes = fdr.read(bytes2read);

            if(!readBytes.size())
            {
                THROW("EOF reached during read in entry no. " +
                                    QString::number(counter));
            }

            entrysize -= bytes2read;
            // check whether file required to import, if not dont write to the file
            if (importfile) {
                fout->write(QByteArray(readBytes));
            }
        }
        // check whether file required to import
        if (importfile) {
            fout->close();
        }

        // compare HMACs
        hmacs = cs.getHmacs();

        foreach(QByteArray name, Constants::keynames)
        {
            // read without updating HMACs (would yield error)
            QByteArray hmacval = fdr.read(4, false);
            if(name == purpose && hmacval != hmacs[name].left(4))
            {
                THROW("illegal entry HMAC for key " + name);
            }
        }
    }
    
    if(noEntries != counter)
    {
        THROW(QString::number(noEntries) + " found, " +
                            QString::number(counter) + " expected");
    }
}

}       // end namespace ImportExport

