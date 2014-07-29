/****************************************************************************/
/*! \file   ServiceReset.h
 *
 *  \brief  Definition file for class CServiceReset
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

#ifndef DEVICECONTROL_SERVICERESET_H
#define DEVICECONTROL_SERVICERESET_H

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
 *  \brief  This class resets the service information of a device
 */
/****************************************************************************/
class CServiceReset : public CState
{
    Q_OBJECT

public:
    explicit CServiceReset(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Resets the service information of the device
     */
    /****************************************************************************/
    void ResetServiceInfo();

    // command response interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Acknowledges the reset of the service information of the device
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void ReportResetServiceInfo(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*!
     *  \brief  Emitted when an error occurred during communication
     *
     *  \iparam ReturnCode = Error code
     */
    /****************************************************************************/
    void OnReportError(ReturnCode_t ReturnCode);

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CServiceReset)

    bool ReportSuccess(QEvent *p_Event);
    bool ReportActive(QEvent *p_Event);
};

} //namespace

#endif // DEVICECONTROL_SERVICERESET_H
