/****************************************************************************/
/** @file WriteArchive.h
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
 *  last modified by owner: @(#) Aug 24 2011, 13:46:24
 *
 */
/****************************************************************************/

#ifndef IMPORT_READ_ARCHIVE_H
#define IMPORT_READ_ARCHIVE_H

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
        FailSafeOpen* m_fd;
        QByteArray m_buffer;
};

}


void ReadArchive(QByteArray archive_name,
       AbstractFile* fd,
       QByteArray purpose,
                 QByteArray keydata = QByteArray(), QStringList importfilelist = QStringList(), QString filepath = QString());

}       // end namespace ImportExport

#endif                  // IMPORT_READ_ARCHIVE_H
