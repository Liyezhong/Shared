/****************************************************************************/
/*! \file   PositionXyz.h
 *
 *  \brief  Definition file for class CPositionXyz. To store position values
 *          for Adjustment module
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-08
 *  $Author:    $ Stalin
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
#ifndef DATAMANAGER_POSITIONXYZ_H
#define DATAMANAGER_POSITIONXYZ_H

#include "DataManager/Helper/Include/Types.h"

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief  To store position values for Adjustment module
 */
/****************************************************************************/
class CPositionXyz
{
public:
    void Clear()
    {
        PositionX = 0;
        PositionY = 0;
        PositionZ = 0;
    }

    qint32 PositionX;
    qint32 PositionY;
    qint32 PositionZ;

    bool Active;
};

}

#endif // DATAMANAGER_POSITIONXYZ_H
