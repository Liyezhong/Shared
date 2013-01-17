/****************************************************************************/
/*! \file   MoveOpenClosed.cpp
 *
 *  \brief  Definition file for class CMoveOpenClosed
 *          To perform move, open and closed operations for various devices
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
#ifndef DEVICECONTROL_DRAWERMOTION_H
#define DEVICECONTROL_DRAWERMOTION_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

#define DEFAULT_OPEN_POS            1000
#define DEFAULT_CLOSE_POS           20
#define DEFAULT_OPEN_PROFILE        0
#define DEFAULT_CLOSE_PROFILE       0

#define BUTTON_RELEASED             0
#define BUTTON_PRESSED              1

class CMoveOpenClosed : public CState
{
    Q_OBJECT

public:
    explicit CMoveOpenClosed(
        CStepperMotor *p_FmMotor,
        CDigitalInput *p_FmDiButton,
        CDigitalOutput *p_FmDoLedGreen,
        CDigitalOutput *p_FmDoLedRed,
        const QString &Name, 
        QState *p_Parent = 0);

    explicit CMoveOpenClosed(
        CStepperMotor *p_FmMotor,
        const QString &Name,
        QState *p_Parent = 0);

    // Getters & Setters
    void SetOpenPosition(quint32 Position);
    void SetClosePosition(quint32 Position);
    void SetOpenProfile(quint32 Profile);
    void SetCloseProfile(quint32 Profile);

signals:
    // command request interface to DCP
    void Block(); 
    void Unblock(); 
    void Open();
    void Close();

    // command response interface to DCP
    void ReportBlock(ReturnCode_t ReturnCode); 
    void ReportUnblock(ReturnCode_t ReturnCode); 
    void ReportOpen(ReturnCode_t ReturnCode); 
    void ReportClose(ReturnCode_t ReturnCode); 

    // Independent signals to DCP
    void Opened();
    void Closed();
    
    // command request interface of Stepper
    void Move(quint32 TargetPosition, quint8 MotionProfileIdx);
    // command response interface of Stepper
    void MoveAck();
    void ReportAbort(ReturnCode_t ReturnCode);
    void StepperError(ReturnCode_t ReturnCode);

    
private:
    bool AckBlock(QEvent *p_Event);
    bool AckUnblock(QEvent *p_Event);
    bool NackBlock(QEvent *p_Event);
    bool NackUnblock(QEvent *p_Event);

    bool Trans_Closed_Opening_Button(QEvent *p_Event);
    bool Trans_Closed_Opening_Command(QEvent *p_Event);
    bool Trans_Closed_Opening(QEvent *p_Event);
    bool Trans_Open_Closing_Button(QEvent *p_Event);
    bool Trans_Open_Closing_Command(QEvent *p_Event);
    bool Trans_Open_Closing(QEvent *p_Event);
    bool Trans_Opening_Open(QEvent *p_Event);
    bool Trans_Closing_Closed(QEvent *p_Event);
    bool NackOpen(QEvent *p_Event);
    bool NackClose(QEvent *p_Event);
    bool DefaultNackOpen(QEvent *p_Event);
    bool DefaultNackClose(QEvent *p_Event);

    bool ButtonPressed(QEvent *p_Event);
    bool ConnectStepper();

    bool CommandActive;

    qint32 m_PositionOpen;
    qint32 m_PositionClosed;
    quint8 m_MotionProfileOpen;     
    quint8 m_MotionProfileClosed;     

    CStepperMotor *mp_FmMotor;
    CDigitalInput *mp_FmDiButton;
    CDigitalOutput *mp_FmDoLedGreen;
    CDigitalOutput *mp_FmDoLedRed;
};

} //namespace

#endif // DEVICECONTROL_DRAWERMOTION_H

// vi: set ts=4 sw=4 et:

