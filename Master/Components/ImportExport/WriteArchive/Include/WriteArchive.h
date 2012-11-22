/****************************************************************************/
/** @file WriteArchive.h
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
 *  last modified by owner: @(#) Aug 24 2011, 12:45:33
 *
 */
/****************************************************************************/

#ifndef IMPORT_WRITE_ARCHIVE_H
#define IMPORT_WRITE_ARCHIVE_H

#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include "ImportExport/CompressEncrypt/Include/CompressEncrypt.h"

namespace ImportExport {

void WriteArchive(QByteArray archive_name,
       QList<QByteArray> filelst, int version,
       bool encrypt = false,
	   qint16 key_version=0);       // XXX for backup: no hash chain incr

}       // end namespace ImportExport

#endif                  // IMPORT_WRITE_ARCHIVE_H
