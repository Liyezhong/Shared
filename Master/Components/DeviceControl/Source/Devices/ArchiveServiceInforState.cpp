/****************************************************************************/
/*! \file   ArchiveServiceInforState.cpp
 *
 *  \brief  Implementaion file for class CArchiveServiceInforState
 *
  *  \version  0.1
 *  \date     2014-07-012
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


#include "DeviceControl/Include/Devices/ArchiveServiceInforState.h"
#include <DeviceControl/Include/Devices/BaseDevice.h>
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CArchiveServiceInforState
typedef CSignalTransition<CArchiveServiceInforState> CArchiveServiceInforTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CArchiveServiceInforState
 *
 *  \iparam pBaseDevice = Function module used for communication
 *
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CArchiveServiceInforState::CArchiveServiceInforState(CBaseDevice *pBaseDevice, QState *p_Parent) :
    CState(pBaseDevice->GetType(), p_Parent), m_pBaseDevice(pBaseDevice)
{
    CState *p_Init = new CState("Init", this);
    CState *p_EmitGetService = new CState("EmitGetService", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CArchiveServiceInforTransition(
        p_Init, SIGNAL(entered()),
        *this, &CArchiveServiceInforState::EmitGetServiceInfor,
        p_EmitGetService));

    p_EmitGetService->addTransition(new CArchiveServiceInforTransition(
        m_pBaseDevice, SIGNAL(ReportSavedServiceInfor()),
        *this, &CArchiveServiceInforState::Finished,
        p_Final));
}

/****************************************************************************/
/*!
 *  \brief  Emits the signal of GetServiceInfor
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CArchiveServiceInforState::EmitGetServiceInfor(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    m_pBaseDevice->OnGetServiceInfor();
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives the signal of SavedServiceInfor after save the
 *     InstrumentHistrory.xml
 *
 *  \iparam p_Event = Parameters of the signal ReportLifeTimeData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CArchiveServiceInforState::Finished(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
