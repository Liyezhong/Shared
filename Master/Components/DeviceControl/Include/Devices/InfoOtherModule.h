/****************************************************************************/
/*! \file   InfoOtherModule.h
 *
 *  \brief  Definition file for class CInfoOtherModule
 *
 *  \version  0.1
 *  \date     2014-07-05
 *  \author   A.Yang
 *
 *  \b Description:
 *
 *   N/A
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2014 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DEVICECONTROL_INFOOTHERMODULE_H
#define DEVICECONTROL_INFOOTHERMODULE_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

#include <QDateTime>

namespace DataManager {
    class CSubModule;
}

namespace DeviceControl
{

class COtherModule;

/****************************************************************************/
/*!
 *  \brief  Fetches the service information from the temperature control
 */
/****************************************************************************/
class CInfoOtherModule : public CState
{
    Q_OBJECT

public:
    explicit CInfoOtherModule(COtherModule *p_OtherModule, DataManager::CSubModule *p_SubModule, QState *p_Parent = 0);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Emitted when an error occurred during communication
     *
     *  \iparam ReturnCode = Error code
     */
    /****************************************************************************/
    void ReportError(ReturnCode_t ReturnCode);

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CInfoOtherModule)
    bool RequestPumpLifeTimeData(QEvent *p_Event);
    bool Finished(QEvent *p_Event);

    COtherModule *mp_OtherModule; //!< Function module used to get the information
    DataManager::CSubModule *mp_SubModule;      //!< Data container in which the data is stored
    QDateTime m_LastSaveTime;
};

} //namespace

#endif // DEVICECONTROL_INFOOTHERMODULE_H
