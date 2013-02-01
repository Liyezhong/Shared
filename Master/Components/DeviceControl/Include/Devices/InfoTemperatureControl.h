/****************************************************************************/
/*! \file   InfoTemperatureControl.h
 *
 *  \brief  Definition file for class CInfoTemperatureControl
 *
 *  \version  0.1
 *  \date     2013-01-31
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef DEVICECONTROL_INFOTEMPERATURECONTROL_H
#define DEVICECONTROL_INFOTEMPERATURECONTROL_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DataManager {
    class CSubModule;
}

namespace DeviceControl
{

class CTemperatureControl;

/****************************************************************************/
/*!
 *  \brief  Fetches the service information from the temperature control
 */
/****************************************************************************/
class CInfoTemperatureControl : public CState
{
    Q_OBJECT

public:
    explicit CInfoTemperatureControl(CTemperatureControl *p_TemperatureControl, DataManager::CSubModule *p_SubModule, QState *p_Parent = 0);

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
    bool GetHeaterOperatingTime(QEvent *p_Event);
    bool Finished(QEvent *p_Event);

    CTemperatureControl *mp_TemperatureControl; //!< Function module used to get the information
    DataManager::CSubModule *mp_SubModule;      //!< Data container in which the data is stored
};

} //namespace

#endif // DEVICECONTROL_INFOTEMPERATURECONTROL_H
