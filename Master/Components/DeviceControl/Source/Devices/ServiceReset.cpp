/****************************************************************************/
/*! \file   ServiceReset.cpp
 *
 *  \brief  Implementaion file for class CServiceReset
 *
 *  \version  0.1
 *  \date     2012-01-15
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

#include "DeviceControl/Include/Devices/ResetModule.h"
#include "DeviceControl/Include/Devices/ResetTemperatureControl.h"
#include "DeviceControl/Include/Devices/ServiceReset.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"

namespace DeviceControl
{

//! Signal transition for CServiceReset
typedef CSignalTransition<CServiceReset> CServiceResetTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CServiceReset
 *
 *  \iparam ModuleMap = Slave modules that are part of the device
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CServiceReset::CServiceReset(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent)
{
    //lint -esym(1524, CServiceReset)
    CState *p_Idle = new CState("Idle", this);
    //lint -esym(593, p_Active)
    CState *p_Active = new CState("Active", this);
    CState *p_LastState = NULL;
    setInitialState(p_Idle);

    addTransition(new CServiceResetTransition(
        this, SIGNAL(ResetServiceInfo()),
        *this, &CServiceReset::ReportActive));

    p_Idle->addTransition(this, SIGNAL(ResetServiceInfo()), p_Active);
    p_Active->addTransition(this, SIGNAL(OnReportError(ReturnCode_t)), p_Idle);

    connect(this, SIGNAL(OnReportError(ReturnCode_t)), this, SIGNAL(ReportResetServiceInfo(ReturnCode_t)));

    QMapIterator<QString, CModule *> Iterator(ModuleMap);
    while (Iterator.hasNext()) {
        //lint -esym(423, p_NewState)
        CState *p_NewState = NULL;
        Iterator.next();

        CTemperatureControl *p_TemperatureControl = dynamic_cast<CTemperatureControl *>(Iterator.value());
        if(p_TemperatureControl != NULL) {
            p_NewState = new CResetTemperatureControl(p_TemperatureControl, p_TemperatureControl->GetKey(), p_Active);
            connect(p_NewState, SIGNAL(ReportError(ReturnCode_t)), this, SIGNAL(OnReportError(ReturnCode_t)));
        }

        CAnalogOutput *p_AnalogOutput = dynamic_cast<CAnalogOutput *>(Iterator.value());
        if(p_AnalogOutput != NULL) {
            p_NewState = new CResetModule(p_AnalogOutput, p_AnalogOutput->GetKey(), p_Active);
            connect(p_NewState, SIGNAL(ReportError(ReturnCode_t)), this, SIGNAL(OnReportError(ReturnCode_t)));
        }

        CDigitalOutput *p_DigitalOutput = dynamic_cast<CDigitalOutput *>(Iterator.value());
        if(p_DigitalOutput != NULL) {
            p_NewState = new CResetModule(p_DigitalOutput, p_DigitalOutput->GetKey(), p_Active);
            connect(p_NewState, SIGNAL(ReportError(ReturnCode_t)), this, SIGNAL(OnReportError(ReturnCode_t)));
        }

        CStepperMotor *p_StepperMotor = dynamic_cast<CStepperMotor *>(Iterator.value());
        if(p_StepperMotor != NULL) {
            p_NewState = new CResetModule(p_StepperMotor, p_StepperMotor->GetKey(), p_Active);
            connect(p_NewState, SIGNAL(ReportError(ReturnCode_t)), this, SIGNAL(OnReportError(ReturnCode_t)));
        }

        if (p_NewState != NULL) {
            if (p_LastState == NULL) {
                p_Active->setInitialState(p_NewState);
            }
            else {
                p_LastState->addTransition(p_LastState, SIGNAL(finished()), p_NewState);
            }
            p_LastState = p_NewState;
        }
    }

    if (p_LastState != NULL) {
        p_Active->addTransition(new CServiceResetTransition(
            p_LastState, SIGNAL(finished()),
            *this, &CServiceReset::ReportSuccess,
            p_Idle));
    }
    else {
        p_Active->addTransition(new CServiceResetTransition(
            p_Active, SIGNAL(entered()),
            *this, &CServiceReset::ReportSuccess,
            p_Idle));
    }
}

/****************************************************************************/
/*!
 *  \brief  Emitted when the information was reset successfully
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceReset::ReportSuccess(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportResetServiceInfo(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Emitted when the request is issued while another one is active
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceReset::ReportActive(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportResetServiceInfo(DCL_ERR_INVALID_STATE);
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
