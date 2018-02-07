/****************************************************************************/
/*! \file HimalayaDataContainer/Helper/Include/Types.h
 *
 *  \brief Definition file for types used from datamanager
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-07
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_TYPES_H
#define DATAMANAGER_TYPES_H

#include <QList>
#include <QHash>

#include "Global/Include/TranslatableString.h"

typedef QHash<quint32, Global::tTranslatableStringList> ErrorHash_t; ///<ErrorHash_t
typedef QList<ErrorHash_t *>                            ListOfErrors_t; ///<ListOfErrors_t

// data container type
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum DataContainerType_t
 */
/****************************************************************************/
typedef enum
{
    UNDEFINED_DATA_CONTAINER = -1,
    PROGRAMS,
    REAGENTS,
    REAGENT_GROUPS,
    STATIONS,
    USERSETTINGS
} DataContainerType_t;

//}
#endif // DATAMANAGER_TYPES_H
