/****************************************************************************/
/*! \file WriteArchive.h
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
 *
 *
 */
/****************************************************************************/

#ifndef IMPORTEXPORT_WRITEARCHIVE_H
#define IMPORTEXPORT_WRITEARCHIVE_H

#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include "ImportExport/CompressEncrypt/Include/CompressEncrypt.h"

namespace ImportExport {

void WriteArchive(QByteArray archive_name,
       QList<QByteArray> filelist, int version,
       bool encrypt = false, bool compressed = false,
	   qint16 key_version=0);       // XXX for backup: no hash chain incr

void WriteFiles(WriteAndHmac &fdwh, FailSafeOpen &fd,
       CryptoService &cs, QList<QByteArray> &filelist, bool &compressed, bool &encrypt);
}       // end namespace ImportExport

#endif                  // IMPORTEXPORT_WRITEARCHIVE_H
