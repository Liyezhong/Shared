/****************************************************************************/
/*! \file ExportTypes.h
 *
 *  \brief User defined data Templates for ExportConfiguration
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-11
 *  $Author:    $ Aparna AL
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

#ifndef EXPORTTYPES_H
#define EXPORTTYPES_H

#include <QString>
#include <QList>


// type of users for file export
typedef enum
{
    USERCONFIG,
    SERVICECONFIG,
    LANGUAGE,
    LEICAPROGRAM,
    USERREPORT
} ImportExportConfig_t;

//Attributes of files/groups for export
typedef struct
{
    bool isPackageTypeNative;
    bool isPacked;
    bool isEncrypted;
    bool isCertified;
    bool isCompressed;
}StructArchiveAttribs_t;

#endif // EXPORTTYPES_H
