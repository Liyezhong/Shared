/****************************************************************************/
/*! \file   InfoFunctionModule.cpp
 *
 *  \brief  Implementaion file for class CInfoFunctionModule
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
#include "DeviceControl/Include/Devices/InfoFunctionModule.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoFunctionModule
typedef CSignalTransition<CInfoFunctionModule> CInfoFunctionModuleTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoFunctionModule
 *
 *  \iparam p_FunctionModule = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoFunctionModule::CInfoFunctionModule(CFunctionModule *p_FunctionModule,DataManager::CSubModule *p_SubModule,
                                         QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_FunctionModule(p_FunctionModule), mp_SubModule(p_SubModule)
{
    //lint -esym(1524, CInfoFunctionModule)
    CState *p_Init = new CState("Init", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoFunctionModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoFunctionModule::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
}

/****************************************************************************/
/*!
 *  \brief  Receives the life time data from the function module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoFunctionModule::Finished(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    QString Version = QString().setNum(mp_FunctionModule->GetBaseModule()->GetModuleSWVersion(mp_FunctionModule->GetType()));

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
