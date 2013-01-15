/****************************************************************************/
/*! \file   ServiceInfo.h
 *
 *  \brief  Definition file for class CServiceInfo
 *
 *  \version  0.1
 *  \date     2013-01-15
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

#ifndef DEVICECONTROL_SERVICEINFO_H
#define DEVICECONTROL_SERVICEINFO_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DataManager/Containers/InstrumentHistory/Include/Module.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CModule;

/****************************************************************************/
/*!
 *  \brief  This class fetches the service information of a device
 */
/****************************************************************************/
class CServiceInfo : public CState
{
    Q_OBJECT

public:
    explicit CServiceInfo(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Requests the service information from the device
     */
    /****************************************************************************/
    void GetServiceInfo();

    // command response interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Returns the service information of the device
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam ModuleInfo = Contains the service information
     */
    /****************************************************************************/
    void ReportGetServiceInfo(ReturnCode_t ReturnCode, DataManager::CModule ModuleInfo);

    /****************************************************************************/
    /*!
     *  \brief  Emitted when an error occurred during communication
     *
     *  \iparam ReturnCode = Error code
     */
    /****************************************************************************/
    void OnReportError(ReturnCode_t ReturnCode);

private:
    bool ReportSuccess(QEvent *p_Event);
    bool ReportError(QEvent *p_Event);
    bool ReportActive(QEvent *p_Event);

    DataManager::CModule m_ModuleInformation;   //!< Contains the service information
};

} //namespace

#endif // DEVICECONTROL_SERVICEINFO_H
