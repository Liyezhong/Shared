/****************************************************************************/
/*! \file   InfoDigitalInput.h
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

#ifndef DEVICECONTROL_INFODIGITALINPUT_H
#define DEVICECONTROL_INFODIGITALINPUT_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"


namespace DataManager {
    class CSubModule;
}

namespace DeviceControl
{

class CDigitalInput;

/****************************************************************************/
/*!
 *  \brief  Fetches the service information from the digital input
 */
/****************************************************************************/
class CInfoDigitalInput : public CState
{
    Q_OBJECT

public:
    explicit CInfoDigitalInput(CDigitalInput *p_DigitalInput, DataManager::CSubModule *p_SubModule, QState *p_Parent = 0);

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
    Q_DISABLE_COPY(CInfoDigitalInput)

    bool ReqLifeTimeData(QEvent *p_Event);
    bool Finished(QEvent *p_Event);

    CDigitalInput *mp_DigitalInput;       //!< Function module used to get the information
    DataManager::CSubModule *mp_SubModule;  //!< Data container in which the data is stored
};

} //namespace

#endif // DEVICECONTROL_INFODIGITALINPUT_H
