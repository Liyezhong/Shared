/****************************************************************************/
/*! \file   ServiceInformation.h
 *
 *  \brief  Definition file for class CServiceInformation
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

#ifndef DEVICECONTROL_SERVICEINFORMATION_H
#define DEVICECONTROL_SERVICEINFORMATION_H

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
 *  \brief
 */
/****************************************************************************/
class CServiceInformation : public CState
{
    Q_OBJECT

public:
    explicit CServiceInformation(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void GetServiceInformation();

    // command response interface to DCP
    /****************************************************************************/
    /*!
     *  \brief
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void ReportGetServiceInformation(ReturnCode_t ReturnCode, DataManager::CModule ModuleInformation);

    void OnReportError(ReturnCode_t ReturnCode);

private:
    bool ReportSuccess(QEvent *p_Event);
    bool ReportError(QEvent *p_Event);
    DataManager::CModule m_ModuleInformation;
};

} //namespace

#endif // DEVICECONTROL_SERVICEINFORMATION_H
