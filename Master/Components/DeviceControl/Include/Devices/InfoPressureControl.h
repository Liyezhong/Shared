/****************************************************************************/
/*! \file   InfoPressureControl.h
 *
 *  \brief  Definition file for class CInfoPressureControl
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

#ifndef DEVICECONTROL_PRESSURECONTROL_H
#define DEVICECONTROL_PRESSURECONTROL_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlReturnCode.h"



namespace DataManager {
    class CSubModule;
}

namespace DeviceControl
{

class CPressureControl;

/****************************************************************************/
/*!
 *  \brief  Fetches the service information from the temperature control
 */
/****************************************************************************/
class CInfoPressureControl : public CState
{
    Q_OBJECT

public:
    explicit CInfoPressureControl(CPressureControl *p_PressureControl, DataManager::CSubModule *p_SubModule, QState *p_Parent = 0);

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
    Q_DISABLE_COPY(CInfoPressureControl)
    bool RequestPumpLifeTimeData(QEvent *p_Event);
    bool Finished(QEvent *p_Event);

    CPressureControl *mp_PressureControl; //!< Function module used to get the information
    DataManager::CSubModule *mp_SubModule;      //!< Data container in which the data is stored
};

} //namespace

#endif // DEVICECONTROL_PRESSURECONTROL_H
