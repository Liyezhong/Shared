/****************************************************************************/
/*! \file   InfoOtherModule.cpp
 *
 *  \brief  Implementaion file for class CInfoOtherModule
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
#include "DeviceControl/Include/Devices/InfoOtherModule.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/OtherModule.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoOtherModule
typedef CSignalTransition<CInfoOtherModule> CInfoInfoOtherModuleTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoOtherModule
 *
 *  \iparam p_DigitalInput = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoOtherModule::CInfoOtherModule(COtherModule *p_OtherModule, DataManager::CSubModule *p_SubModule,
                                       QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_OtherModule(p_OtherModule), mp_SubModule(p_SubModule)
{
    CState *p_Init = new CState("Init", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoInfoOtherModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoOtherModule::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("LifeTime", "Seconds", QString());
    m_LastSaveTime = QDateTime::currentDateTime();
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
bool CInfoOtherModule::Finished(QEvent *p_Event)
{
    PartLifeCycleRecord* pPartLifeCycleRecord = mp_OtherModule->GetPartLifeCycleRecord();
    if (!pPartLifeCycleRecord)
        return true;

    const QString& paramName = mp_SubModule->GetSubModuleName() + "_LifeTime";
    uint lifeTimeOld = pPartLifeCycleRecord->m_ParamMap[paramName].toUInt();
    quint32 elpaseSec = m_LastSaveTime.secsTo(QDateTime::currentDateTime());
    quint32 lifeTimeNew = elpaseSec + lifeTimeOld;
    QString strLifeTimeNew = QString().setNum(lifeTimeNew);
    if (!mp_SubModule->UpdateParameterInfo("LifeTime", strLifeTimeNew)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    pPartLifeCycleRecord->m_ParamMap[paramName] = strLifeTimeNew;
    m_LastSaveTime = QDateTime::currentDateTime();
    return true;
}

} //namespace

