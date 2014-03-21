/****************************************************************************/
/*! \file ReadArchive.h
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

#ifndef IMPORTEXPORT_READARCHIVE_H
#define IMPORTEXPORT_READARCHIVE_H

#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QStringList>
#include "ImportExport/DecryptUncompress/Include/DecryptUncompress.h"
#include "ImportExport/AbstractFile/Include/AbstractFile.h"

namespace ImportExport {

/* HELP CLASS *********/

namespace {

class ReadAndBuffer
{
    public:
        ReadAndBuffer(FailSafeOpen* fd);
        QByteArray read(int size);
        inline QByteArray getBuffer() {return m_buffer;}

    private:
        FailSafeOpen* mp_fd;
        QByteArray m_buffer;
};

}


void ReadArchive(QByteArray archive_name,
       AbstractFile* fd,
       QByteArray purpose,
                 QByteArray keydata = QByteArray(),
                 QStringList importfilelist = QStringList(), QString filepath = QString());

void ReadKeyData(QByteArray &keydata, int &hashIndex, QByteArray &deviceID, QByteArray &purpose);

void ImportArchiveFiles(FailSafeOpen &fd, ReadAndBuffer &cs,
                        QStringList &importfilelist, bool &compressed,
                        bool &encrypt, AbstractFile* fout, QString &filepath,
                        int &noentries, QByteArray &purpose, QByteArray &keydata, QByteArray &deviceID);

void ExtractFileToMemory(AbstractFile* fout, DecryptUncompress &fdr, QStringList &importfilelist, QString &filepath);

bool CheckFileRequiresImport(const QByteArray &filename, const QStringList &filelist, const QString &filepath);

}       // end namespace ImportExport

#endif  // IMPORTEXPORT_READARCHIVE_H
