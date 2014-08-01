/****************************************************************************/
/*! \file   InfoDigitalInput.cpp
 *
 *  \brief  Implementaion file for class CInfoDigitalInput
 *
 *  \version  0.1
 *  \date     2014-07-05
 *  \author   A.Yang
 *
 *  \b Description:
 *
 *   N/A
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2014 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DeviceControl/Include/Devices/InfoDigitalInput.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoDigitalIntput
typedef CSignalTransition<CInfoDigitalInput> CInfoDigitalInputTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoDigitalInput
 *
 *  \iparam p_DigitalInput = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoDigitalInput::CInfoDigitalInput(CDigitalInput *p_DigitalInput, DataManager::CSubModule *p_SubModule,
                                       QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_DigitalInput(p_DigitalInput), mp_SubModule(p_SubModule)
{
    //lint -esym(1524, CInfoDigitalOutput)
    CState *p_Init = new CState("Init", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoDigitalInputTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoDigitalInput::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("OperationTime", QString());
}

/****************************************************************************/
/*!
 *  \brief  Receives the life time data from the function module
 *
 *  \iparam p_Event = Parameters of the signal ReportLifeTimeData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoDigitalInput::Finished(QEvent *p_Event)
{
    QString Version = QString().setNum(mp_DigitalInput->GetBaseModule()->GetModuleSWVersion(mp_DigitalInput->GetType()));

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("OperationTime", QString().setNum(mp_DigitalInput->GetLifeCycle()))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    PartLifeCycleRecord* pPartLifeCycleRecord = mp_DigitalInput->GetPartLifeCycleRecord();
    if (!pPartLifeCycleRecord)
        return true;

    QString paramName = mp_SubModule->GetSubModuleName() + "_LifeCycle";
    QString strLifeTimeNew = QString().setNum(mp_DigitalInput->GetLifeCycle());
    QMap<QString, QString>::const_iterator iter = pPartLifeCycleRecord->m_ParamMap.find(paramName);
    if (iter != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap[paramName] = strLifeTimeNew;

    return true;
}

} //namespace

