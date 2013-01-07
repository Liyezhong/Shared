/****************************************************************************/
/*! \file   ServiceBaseModule.h
 *
 *  \brief  Definition file for class CServiceBaseModule
 *
 *  \version  0.1
 *  \date     2013-01-07
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

#ifndef DEVICECONTROL_SERVICEBASEMODULE_H
#define DEVICECONTROL_SERVICEBASEMODULE_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CBaseModule;

/****************************************************************************/
/*!
 *  \brief
 */
/****************************************************************************/
class CServiceBaseModule : public CState
{
    Q_OBJECT

public:
    explicit CServiceBaseModule(CBaseModule *p_BaseModule, const QString &Name, QState *p_Parent = 0);

signals:
    void ReportError(ReturnCode_t ReturnCode);

private:
    bool ReqSerialNumber(QEvent *p_Event);
    bool ReqEndTestResult(QEvent *p_Event);

    CBaseModule *mp_BaseModule;
};

} //namespace

#endif // DEVICECONTROL_SERVICEBASEMODULE_H
