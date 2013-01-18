/****************************************************************************/
/*! \file   MoveXyz.h
 *
 *  \brief  Definition file for class CMoveXYZ. To handle 3 axis movement
 *          & acknowledgement for XYZ Transfer System
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
#ifndef DEVICECONTROL_MOVEXYZ_H
#define DEVICECONTROL_MOVEXYZ_H

#include "DeviceControl/Include/Global/DeviceControl.h"
#include "DeviceState.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CStepperMotor;

#define NO_CHANGE ((quint32)0xFFFFFFFF)

/****************************************************************************/
/*!
 *  \brief  Definition file for class CMoveXYZ. To handle 3 axis movement
 *          & acknowledgement for XYZ Transfer System
 */
/****************************************************************************/
class CMoveXYZ : public CState
{
Q_OBJECT

public:
    CMoveXYZ(CStepperMotor &XAxisMotor,
             CStepperMotor &YAxisMotor,
             CStepperMotor &ZAxisMotor,
             const QString &Name,
             QState *p_Parent = 0);

    quint32 GetPositionX();
    quint32 GetPositionY();
    quint32 GetPositionZ();

signals:
    // Request Interface
    /****************************************************************************/
    /*! \brief  Request interface to initiate all axis movement
     *
     *  \iparam PositionX   Target X position
     *  \iparam ProfileX    Motion profile for X axis
     *  \iparam PositionY   Target Y position
     *  \iparam ProfileY    Motion profile for Y axis
     *  \iparam PositionZ   Target Z position
     *  \iparam ProfileZ    Motion profile for Z axis
     */
    /****************************************************************************/
    void Move(quint32 PositionX, quint8 ProfileX,
              quint32 PositionY, quint8 ProfileY,
              quint32 PositionZ, quint8 ProfileZ);

    /****************************************************************************/
    /*!
     *  \brief  Request interface to abort movement in progress
     */
    /****************************************************************************/
    void Abort();

    // Response Interface
    /****************************************************************************/
    /*! \brief Response to Move() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportMove(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to Abort() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportAbort(ReturnCode_t ReturnCode);

protected:
    bool Trans_Idle_Moving(QEvent *p_Event);
    bool Trans_Moving_Idle(QEvent *p_Event);
    bool Trans_Moving(QEvent *p_Event);

    bool Trans_Moving_Aborting(QEvent *p_Event);
    bool Trans_Aborting_Idle(QEvent *p_Event);

private slots:
    void SetPositionX(quint32 Positon);
    void SetPositionY(quint32 Positon);
    void SetPositionZ(quint32 Positon);

    void MoveError(ReturnCode_t ReturnCode);

private:
    CStepperMotor &m_XAxisMotor;
    CStepperMotor &m_YAxisMotor;
    CStepperMotor &m_ZAxisMotor;

    quint32 m_CurrentPositionX;
    quint32 m_CurrentPositionY;
    quint32 m_CurrentPositionZ;

    ReturnCode_t m_ReturnCode;
};

}

#endif // CMOVEXYZ_H
