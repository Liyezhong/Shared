/****************************************************************************/
/*! \file   FmStepperInit.h
 *
 *  \brief  Definition file for class CFmStepperInit
 *          To enable and perform reference run for the stepper motor
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Torsten Scheck
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

#ifndef DEVICECONTROL_FMSTEPPERINIT_H
#define DEVICECONTROL_FMSTEPPERINIT_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CFmStepperInit : public CState
{
    Q_OBJECT

public:
    explicit CFmStepperInit(CStepperMotor *p_StepperMotor, const QString &Name, QState *p_Parent = 0);

signals:
    void NeedInitialize(ReturnCode_t);

private:
    bool Trans_Start_SetState(QEvent *p_Event);
    bool Trans_SetState_RefRun(QEvent *p_Event);
    bool Trans_RefRun_End(QEvent *p_Event);

    CStepperMotor *mp_StepperMotor,;
};

} //namespace

#endif // DEVICECONTROL_FMSTEPPERINIT_H

// vi: set ts=4 sw=4 et:

