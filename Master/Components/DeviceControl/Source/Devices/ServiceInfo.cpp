/****************************************************************************/
/*! \file   ServiceInfo.cpp
 *
 *  \brief  Implementaion file for class CServiceInfo
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

#include "DeviceControl/Include/Devices/InfoAnalogOutput.h"
#include "DeviceControl/Include/Devices/InfoBaseModule.h"
#include "DeviceControl/Include/Devices/InfoDigitalOutput.h"
#include "DeviceControl/Include/Devices/InfoFunctionModule.h"
#include "DeviceControl/Include/Devices/InfoStepperMotor.h"
#include "DeviceControl/Include/Devices/InfoTemperatureControl.h"
#include "DeviceControl/Include/Devices/ServiceInfo.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"

namespace DeviceControl
{

//! Signal transition for CServiceInfo
typedef CSignalTransition<CServiceInfo> CServiceInfoTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CServiceInfo
 *
 *  \iparam ModuleMap = Slave modules that are part of the device
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CServiceInfo::CServiceInfo(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent)
{   
    CState *p_Idle = new CState("Idle", this);
    CState *p_Active = new CState("Active", this);
    CState *p_LastState = NULL;
    setInitialState(p_Idle);

    addTransition(new CServiceInfoTransition(
        this, SIGNAL(GetServiceInfo()),
        *this, &CServiceInfo::ReportActive));

    p_Idle->addTransition(this, SIGNAL(GetServiceInfo()), p_Active);
    p_Active->addTransition(new CServiceInfoTransition(
        this, SIGNAL(OnReportError(ReturnCode_t)),
        *this, &CServiceInfo::ReportError,
        p_Idle));

    QMapIterator<QString, CModule *> Iterator(ModuleMap);
    while (Iterator.hasNext()) {
        CState *p_NewState = NULL;
        Iterator.next();

        switch (Iterator.value()->GetType()) {
            case CModuleConfig::CAN_OBJ_TYPE_NODE:
                p_NewState = CreateInfoModule<CInfoBaseModule>(dynamic_cast<CBaseModule *>(Iterator.value()),
                                                               QString("Board"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_DIGITAL_OUT_PORT:
                p_NewState = CreateInfoModule<CInfoDigitalOutput>(dynamic_cast<CDigitalOutput *>(Iterator.value()),
                                                                  QString("Digital Output"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_DIGITAL_IN_PORT:
                p_NewState = CreateInfoModule<CInfoFunctionModule>(dynamic_cast<CFunctionModule *>(Iterator.value()),
                                                                   QString("Digital Input"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_ANALOG_OUT_PORT:
                p_NewState = CreateInfoModule<CInfoAnalogOutput>(dynamic_cast<CAnalogOutput *>(Iterator.value()),
                                                                 QString("Analog Output"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_ANALOG_IN_PORT:
                p_NewState = CreateInfoModule<CInfoFunctionModule>(dynamic_cast<CFunctionModule *>(Iterator.value()),
                                                                   QString("Analog Input"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_STEPPERMOTOR:
                p_NewState = CreateInfoModule<CInfoStepperMotor>(dynamic_cast<CStepperMotor *>(Iterator.value()),
                                                                 QString("Stepper Motor"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_JOYSTICK:
                p_NewState = CreateInfoModule<CInfoFunctionModule>(dynamic_cast<CFunctionModule *>(Iterator.value()),
                                                                   QString("Joystick"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_RFID11785:
                p_NewState = CreateInfoModule<CInfoFunctionModule>(dynamic_cast<CFunctionModule *>(Iterator.value()),
                                                                   QString("RFID ISO 11785"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_RFID15693:
                p_NewState = CreateInfoModule<CInfoFunctionModule>(dynamic_cast<CFunctionModule *>(Iterator.value()),
                                                                   QString("RFID ISO 15693"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_TEMPERATURE_CTL:
                p_NewState = CreateInfoModule<CInfoTemperatureControl>(dynamic_cast<CTemperatureControl *>(Iterator.value()),
                                                                       QString("Temperature Control"), p_Active);
                break;
            case CModuleConfig::CAN_OBJ_TYPE_UART:
                p_NewState = CreateInfoModule<CInfoFunctionModule>(dynamic_cast<CFunctionModule *>(Iterator.value()),
                                                                   QString("UART"), p_Active);
                break;
            default:
                break;
        }

        if (p_NewState != NULL) {
            connect(p_NewState, SIGNAL(ReportError(ReturnCode_t)), this, SIGNAL(OnReportError(ReturnCode_t)));
            if (p_LastState == NULL) {
                p_Active->setInitialState(p_NewState);
            }
            else {
                p_LastState->addTransition(p_LastState, SIGNAL(finished()), p_NewState);
            }
            p_LastState = p_NewState;
        }
    }

    p_Active->addTransition(new CServiceInfoTransition(
        p_LastState, SIGNAL(finished()),
        *this, &CServiceInfo::ReportSuccess,
        p_Idle));
}


/****************************************************************************/
/*!
 *  \brief  Creates a state fetching the service info of a module
 *
 *  \iparam p_Module = The slave module
 *  \iparam Name = Name for the target data container
 *  \iparam p_Parent = Parent state
 *
 *  \return State that can be added to the state machine
 */
/****************************************************************************/
template <class TInfoModule, class TModule>
CState *CServiceInfo::CreateInfoModule(TModule *p_Module, const QString &Name, CState *p_Parent)
{
    CState *p_State = NULL;

    if (p_Module != NULL) {
        DataManager::CSubModule SubModule(p_Module->GetKey(), Name, "");
        m_ModuleInformation.AddSubModuleInfo(&SubModule);
        p_State = new TInfoModule(p_Module, m_ModuleInformation.GetSubModuleInfo(p_Module->GetKey()), p_Parent);
    }

    return p_State;
}

/****************************************************************************/
/*!
 *  \brief  Emitted when the information was fetched successfully
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceInfo::ReportSuccess(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportGetServiceInfo(DCL_ERR_FCT_CALL_SUCCESS, m_ModuleInformation);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Emitted when an error occurs during the transactions
 *
 *  \iparam p_Event = Parameters of the signal OnReportError
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceInfo::ReportError(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CServiceInfoTransition::GetEventValue(p_Event, 0);
    emit ReportGetServiceInfo(ReturnCode, m_ModuleInformation);
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
bool CServiceInfo::ReportActive(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportGetServiceInfo(DCL_ERR_INVALID_STATE, m_ModuleInformation);
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
