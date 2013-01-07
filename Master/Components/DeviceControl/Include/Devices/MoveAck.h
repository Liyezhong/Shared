/****************************************************************************/
/*! \file   MoveAck.h
 *
 *  \brief  Definition file for class CMoveAck. Handle acknowledgement of
 *          axis (stepper) movement for XYZ Transfer System
 *
 *  \version  0.1
 *  \date     2012-11-19
 *  \author   Stalin
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
#ifndef DEVICECONTROL_MOVEACK_H
#define DEVICECONTROL_MOVEACK_H

#include "DeviceState.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

/****************************************************************************/
/*!
 * \brief   Definition file for class CMoveAck. Handle acknowledgement of
 *          axis (stepper) movement for Xyz transfer system
 */
/****************************************************************************/
class CMoveAck : public CState
{
    Q_OBJECT

public:
    CMoveAck(CStepperMotor &StepperMotor, const QString &Name, QState *p_Parent);

signals:
    /****************************************************************************/
    /*! \brief Error Response to signal movement error
     *
     *  \iparam ReturnCode Reason for Failure / Error
     */
    /****************************************************************************/
    void MoveError(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Independent response to report current position
     *
     *  \iparam Position Current position
     */
    /****************************************************************************/
    void ReportPosition(quint32 Position);

protected:
    bool CheckMovementAckn(QEvent *p_Event);

private:
    CStepperMotor &m_StepperMotor;
};

}

#endif // MOVEACK_H
