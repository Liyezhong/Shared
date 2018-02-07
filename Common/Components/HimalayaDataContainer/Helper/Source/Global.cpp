/****************************************************************************/
/*! \file Global.h
 *
 *  \brief Implementation file for global variables and methods.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-25
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

#include "HimalayaDataContainer/Helper/Include/Global.h"

#include <QAtomicInt>


namespace DataManager
{

namespace Global
{

static QAtomicInt s_SystemIdle = 0;

/****************************************************************************/
/*!
 *  \brief  Check if the system is now idle,
 *          which means no processing is running.
 *
 *  \return true, if the system is idle; false, otherwise
 *
 */
/****************************************************************************/
bool IsSystemIdle(void)
{
    return (0 == s_SystemIdle);
}

/****************************************************************************/
/*!
 *  \brief  Set the system idle status.
 *
 *  \param[in] idle the new system idle status
 *
 */
/****************************************************************************/
void SetSystemIdle(bool idle)
{
    s_SystemIdle = idle ? 0 : 1;
}

}

}
