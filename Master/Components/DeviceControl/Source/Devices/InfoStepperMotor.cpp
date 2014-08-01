/****************************************************************************/
/*! \file   InfoStepperMotor.cpp
 *
 *  \brief  Implementaion file for class CInfoStepperMotor
 *
 *  \version  0.1
 *  \date     2012-01-31
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

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DeviceControl/Include/Devices/InfoStepperMotor.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoStepperMotor
typedef CSignalTransition<CInfoStepperMotor> CInfoStepperMotorTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoStepperMotor
 *
 *  \iparam p_StepperMotor = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoStepperMotor::CInfoStepperMotor(CStepperMotor *p_StepperMotor, DataManager::CSubModule *p_SubModule,
                                     QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_StepperMotor(p_StepperMotor), mp_SubModule(p_SubModule)
{
    //lint -esym(1524, CInfoStepperMotor)
    CState *p_Init = new CState("Init", this);
    CState *p_RequestOperationTimeData = new CState("RequestOperationTimeData", this);
    //CState *p_RequestRevolutionCountData = new CState("RequestRevolutionCountData", this);
    //CState *p_RequestDirChangeCountData = new CState("RequestDirChangeCountData", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoStepperMotorTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoStepperMotor::RequestOperationTimeData,
        p_RequestOperationTimeData));
    /*p_RequestOperationTimeData->addTransition(new CInfoStepperMotorTransition(
        mp_StepperMotor, SIGNAL(ReportLifeCycleData(quint32, ReturnCode_t, quint32, quint32, quint32)),
        *this, &CInfoStepperMotor::RequestRevolutionCountData,
        p_RequestRevolutionCountData));*/
    /*p_RequestRevolutionCountData->addTransition(new CInfoStepperMotorTransition(
        mp_StepperMotor, SIGNAL(ReportLifeCycleData(quint32, ReturnCode_t, quint32, quint32, quint32)),
        *this, &CInfoStepperMotor::RequestDirChangeCountData,
        p_RequestDirChangeCountData));*/

    p_RequestOperationTimeData->addTransition(new CInfoStepperMotorTransition(
        mp_StepperMotor, SIGNAL(ReportLifeCycleData(quint32, ReturnCode_t, quint32, quint32, quint32)),
        *this, &CInfoStepperMotor::Finished,
        p_Final));

    /*p_RequestDirChangeCountData->addTransition(new CInfoStepperMotorTransition(
        mp_StepperMotor, SIGNAL(ReportLifeCycleData(quint32, ReturnCode_t, quint32, quint32, quint32)),
        *this, &CInfoStepperMotor::Finished,
        p_Final));*/

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    //mp_SubModule->AddParameterInfo("RevolutionCounter", QString());
    //mp_SubModule->AddParameterInfo("DirectionChangeCounter", QString());
    mp_SubModule->AddParameterInfo("OperationTime", "minutes", QString());
    //mp_SubModule->AddParameterInfo("Distance", "mm", QString());
}

/****************************************************************************/
/*!
 *  \brief  Requests the life time data from the function module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoStepperMotor::RequestOperationTimeData(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_StepperMotor->RequestOperationTimeData();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the life time data from the function module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoStepperMotor::RequestRevolutionCountData(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_StepperMotor->RequestRevolutionCountData();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the life time data from the function module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoStepperMotor::RequestDirChangeCountData(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_StepperMotor->RequestDirChangeCountData();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives the life time data from the function module
 *
 *  \iparam p_Event = Parameters of the signal ReportLifeCycleData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoStepperMotor::Finished(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint32 StepCounter;
    quint32 DirChangeCounter;
    quint32 OperationTime;
    QString Version = QString().setNum(mp_StepperMotor->GetBaseModule()->GetModuleSWVersion(mp_StepperMotor->GetType()));

    ReturnCode = CInfoStepperMotorTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    /*if (!CInfoStepperMotorTransition::GetEventValue(p_Event, 2, StepCounter)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoStepperMotorTransition::GetEventValue(p_Event, 3, DirChangeCounter)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }*/
    if (!CInfoStepperMotorTransition::GetEventValue(p_Event, 4, OperationTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    /*if (!mp_SubModule->UpdateParameterInfo("RevolutionCounter", QString().setNum(StepCounter))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("DirectionChangeCounter", QString().setNum(DirChangeCounter))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }*/
    if (!mp_SubModule->UpdateParameterInfo("OperationTime", QString().setNum(OperationTime))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
   /* if (!mp_SubModule->UpdateParameterInfo("Distance", QString().setNum(StepCounter * mp_StepperMotor->GetCircumference()))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }*/

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
