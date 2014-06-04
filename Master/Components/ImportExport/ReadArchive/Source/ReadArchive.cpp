/****************************************************************************/
/*! \file ReadArchive.cpp
 *
 *  \brief read an archive - define a function
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
 *
 */
/****************************************************************************/

#include "ImportExport/ReadArchive/Include/ReadArchive.h"


namespace ImportExport {

/****************************************************************************/
/*!
 * \brief Constructor
 *
 * \iparam fd - device file
 */
/****************************************************************************/
ReadAndBuffer::ReadAndBuffer(FailSafeOpen* fd): mp_fd(fd),
        m_buffer(QByteArray())
{
}

/****************************************************************************/
/*!
 * \brief read 'size' bytes from mp_fd, store it to buffer and return in
 *
 * \iparam size - how many bytes to read
 *
 * \return bytes read
 */
/****************************************************************************/
QByteArray ReadAndBuffer::read(int size)
{
    QByteArray ret = mp_fd->read(size);
    m_buffer += ret;
    return ret;
}


/****************************************************************************/
/*!
 * \brief read an archive
 *
 * The action depends on the Purpose string:
 * "Import" - unpack the files to disk, check the signature
 * "Viewer" - unpack the files to QMap, check the signature
 * "Leica" - only check signatures and collect filenames
 *
 * \iparam archive_name - absolute pathname of archive;
 *                      the archive name must have the structure
 *                      Colorado_zzz_xxx_yyy.cea
 *                      where xxx is the serial number and yyy is
 *                      a time stamp in ISO format (date and time
 *                      in UTC). zzz denotes the archive kind or
 *                      helps distinguish archives.
 * \iparam fd - AbstractFile instance
 * \iparam purpose - purpose for which the archive is opened; must have
 *                      one of the values of Constants::keynames
 * \iparam keydata - optional keydata byte array
 * \iparam importfilelist - optional import file list
 * \iparam filepath - optional for the path of file
 */
/****************************************************************************/
void ReadArchive(QByteArray archive_name,
	   AbstractFile* fout,
	   QByteArray purpose,
           QByteArray keydata, QStringList importfilelist, QString filepath)
{
    QByteArray archivefilename;

    // open fd
    FailSafeOpen fd(archive_name, 'r');
    // sometimes the file name consists of the path, so remove the path for splitting the file name
    archivefilename = archive_name.mid((archive_name.lastIndexOf('/') == 0 ? 0 :  archive_name.lastIndexOf('/') + 1));

    // extract deviceID
    QList<QByteArray> parts = archivefilename.split('_');    

    if (parts.count() < 3)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ARCHIVEFILE_FORMAT_WRONG);
    }

    QByteArray deviceID = parts[2];

    // READ HEADER
    // buffer read bytes and compute HMACs at the end when the key
    // is computed or known
    ReadAndBuffer fdrb(&fd);

    // header magic
    if(fdrb.read(4) != Constants::headerMagic)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ILLEGAL_MAGIC_HEADER);
    }

    // export configuration file version - future use
    int fileversion = General::byte2int(fdrb.read(2).data(), 2);
    Q_UNUSED(fileversion);

    // format_version 0
    char *version = fdrb.read(1).data();
    Q_UNUSED(version);

    // encrypted flag
    bool encrypt = fdrb.read(1).data()[0] != '\0';      //lint !e578 [Rw]

    // compressed flag
    bool compressed = fdrb.read(1).data()[0] != '\0';      //lint !e578 [Rw]

    // key_version
    int key_version = General::byte2int(fdrb.read(2).data(), 2);
    Q_UNUSED(key_version);

    // hash chain index
    int hashIndex = General::byte2int(fdrb.read(4).data());

    ReadKeyData(keydata, hashIndex, deviceID, purpose);

    // # of entries
    int noentries = General::byte2int(fdrb.read(2).data(), 2);
    // length of archive name
    int lgname = General::byte2int(fdrb.read(2).data(), 2);

    // archive name
    QByteArray name = fdrb.read(lgname);    

    // sometimes the file name consists of the path, so remove the path compare the file name
    name = name.mid((name.lastIndexOf('/') == 0 ? 0 :  name.lastIndexOf('/') + 1));
    if(name != archivefilename)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ARCHIVEFILE_NAME_NOT_MATCHING);
    }

    // import the files
    ImportArchiveFiles(fd, fdrb, importfilelist, compressed, encrypt, fout,
                       filepath, noentries, purpose, keydata, deviceID);

}


/****************************************************************************/
/*!
 * \brief read the key file data
 *
 * The action depends on the Purpose string:
 * "Import" - unpack the files to disk, check the signature
 * "Viewer" - unpack the files to QMap, check the signature
 * "Leica" - only check signatures and collect filenames
 *
 * \oparam keydata - byte array
 * \iparam hashIndex - hash index
 * \iparam deviceID - ID of the device
 * \iparam purpose - purpose to import
 */
/****************************************************************************/
void ReadKeyData(QByteArray &keydata, int &hashIndex, QByteArray &deviceID, QByteArray &purpose)
{
    // optionally read keydata
    if(keydata.isEmpty())
    {
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
}

/****************************************************************************/
/*!
 * \brief import the archive files
 *
 * Sometimes all the files are not required to import so if the string list is
 * passed then import files which are specified in the list
 *
 * \oparam fd - file class
 * \iparam fdrb - Read the buffer
 * \iparam importfilelist - list fo the import files
 * \iparam compressed - flag for compression
 * \iparam encrypt - flag for encryption
 * \oparam fout - files to store in
 * \iparam filepath - path of the file name
 * \iparam noentries - number of entire
 * \iparam purpose - purpose to import
 * \iparam keydata - byte array
 * \iparam deviceID - ID of the device
 *
 */
/****************************************************************************/
void ImportArchiveFiles(FailSafeOpen &fd, ReadAndBuffer &fdrb,
                        QStringList &importfilelist, bool &compressed,
                        bool &enCrypt, AbstractFile* fout, QString & filepath,
                        int &noentries, QByteArray &purpose, QByteArray &keydata, QByteArray &deviceID)
{
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
            THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ILLEGAL_HEADER_HMAC);
        }
    }

    // "file descriptor" for read entries and computing HMACs
    DecryptUncompress fdr(&fd, cs, enCrypt, compressed);
    // counter for entries
    int counter = 0;
    // iterate over entries
    while(noentries != counter)
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
            THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ILLEGAL_MAGIC_ENTRY);
        }

        ExtractFileToMemory(fout, fdr, importfilelist, filepath);

        // compare HMACs
        hmacs = cs.getHmacs();

        foreach(QByteArray name, Constants::keynames)
        {
            // read without updating HMACs (would yield error)
            QByteArray hmacval = fdr.read(4, false);
            if(name == purpose && hmacval != hmacs[name].left(4))
            {
                THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ILLEGAL_ENTRY_HMAC);
            }
        }
    }

    if(noentries != counter)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ENTRIES_NOT_MATCHING);
    }
}


/****************************************************************************/
/*!
 * \brief extract the files into memory
 *
 * \oparam fout - name of the file
 * \iparam fdr - decrypt uncompress
 * \iparam importfilelist - list of the files
 * \iparam filepath - path of the file
 *
 */
/****************************************************************************/
void ExtractFileToMemory(AbstractFile* fout, DecryptUncompress &fdr,
                         QStringList &importfilelist, QString & filepath)
{
    // length of entry name
    int enamelen = General::byte2int(fdr.read(2).data(), 2);

    QByteArray entryname = fdr.read(enamelen);

    int entrysize = General::byte2int(fdr.read(4).data());

    bool importfile = CheckFileRequiresImport(entryname, importfilelist, filepath);

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
            THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_EOF_ENTRY);
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
}

/****************************************************************************/
/*!
 * \brief Check whether file needs to be imported
 *
 * Sometimes all the files are not required to import so if the string list is
 * passed then import files which are specified in the list
 *
 * \oparam filename - name of the file
 * \iparam filelist - list of the files
 * \iparam filepath - path of the file
 *
 * \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CheckFileRequiresImport(const QByteArray &filename, const QStringList &filelist, const QString &filepath)
{
    bool importfile = true;
    // check which files need to be imported
    foreach (QString importfilename, filelist) {
        importfile = false;
        // create the regular expression to search for the matched- create a wild card entry to make the search
        // easier
        QRegExp regExp("*" + importfilename);
        // make the pattern search as wild card search
        regExp.setPatternSyntax(QRegExp::Wildcard);
        // check the file name if it matches
        if (regExp.exactMatch(QString(filename)))   {
            importfile = true;
            // change the path of the file to save
            QString tempfilename(filename);
            if (tempfilename.lastIndexOf("/") != -1) {
                // store only the file name not the path
                tempfilename = tempfilename.mid(tempfilename.lastIndexOf("/") + 1);
            }
            // clear the byte array
            const_cast<QByteArray&>(filename).clear();
            // append the target path and file name in the byte array
            const_cast<QByteArray&>(filename) = qPrintable(filepath + tempfilename);
            break;
        }
    }
    return importfile;
}

}       // end namespace ImportExport

