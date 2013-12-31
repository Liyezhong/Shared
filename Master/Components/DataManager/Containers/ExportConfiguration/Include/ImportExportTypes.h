/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Include/ImportExportTypes.h
 *
 *  \brief User defined data Templates for ExportConfiguration
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-11
 *  $Author:    $ Aparna AL, Ramya GJ
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


/****************************************************************************/
/**
 * \brief type of users for file export
 */
/****************************************************************************/
typedef enum
{
    USERCONFIG,
    SERVICECONFIG,
    LANGUAGE,
    LEICAPROGRAM,
    USERREPORT
} ImportExportConfig_t;

/****************************************************************************/
/**
 * \brief Scheduler action struct
 */
/****************************************************************************/
typedef struct
{
    bool isPackageTypeNative;       ///< package type
    bool isPacked;                  ///< packed status
    bool isEncrypted;               ///< encrypted status
    bool isCertified;               ///< certified status
    bool isCompressed;              ///< compressed status
}StructArchiveAttribs_t;

#endif // EXPORTTYPES_H
