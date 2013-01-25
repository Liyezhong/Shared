/****************************************************************************/
/*! \file   ServiceState.h
 *
 *  \brief  Definition file for class CServiceState
 *
 *  \version  0.1
 *  \date     2013-01-04
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

#ifndef DEVICECONTROL_SERVICESTATE_H
#define DEVICECONTROL_SERVICESTATE_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"
#include "DataManager/Containers/InstrumentHistory/Include/Module.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CModule;

/****************************************************************************/
/*!
 *  \brief  This state includes the service functionality
 */
/****************************************************************************/
class CServiceState : public CState
{
    Q_OBJECT

public:
    explicit CServiceState(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Requests the service information from the device
     */
    /****************************************************************************/
    void GetServiceInfo();

    /****************************************************************************/
    /*!
     *  \brief  Resets the service information of the device
     */
    /****************************************************************************/
    void ResetServiceInfo();

    // command response interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Returns the service information of a device
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam ModuleInfo = Contains the service information
     */
    /****************************************************************************/
    void ReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo);

    /****************************************************************************/
    /*!
     *  \brief  Acknowledges the reset of the service information of the device
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void ReportResetServiceInfo(ReturnCode_t ReturnCode);
};

} //namespace

#endif // DEVICECONTROL_SERVICESTATE_H
