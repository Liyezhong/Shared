/****************************************************************************/
/*! \file Joystick.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 04.10.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CJoystick
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DEVICECONTROL_JOYSTICKPRIVATE_H
#define DEVICECONTROL_JOYSTICKPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

/****************************************************************************/
/*!
*   \brief  Joystick function module class
*
*       This class implements the functionality to configure and control a
*       slave's 'joystick' function module (used for adjustemnt)
*/
/****************************************************************************/
class CJoystick : public CFunctionModule
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CJoystick
     *
     *  \param mp_MessageConfiguration
     *  \param pCANCommunicator
     *  \param pParentNode
     *
     *  \return from CJoystick
     */
    /****************************************************************************/
    CJoystick(const CANMessageConfiguration *mp_MessageConfiguration, CANCommunicator* pCANCommunicator,
              CBaseModule* pParentNode);
    ~CJoystick();

    ReturnCode_t Initialize();

    void HandleTasks();
    void HandleCanMessage(can_frame* pCANframe);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ReqActInputValue
     *
     *  \return from ReqActInputValue
     */
    /****************************************************************************/
    ReturnCode_t ReqActInputValue();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetActInputValue
     *
     *  \param inputValue = quint16 type parameter
     *
     *  \return from GetActInputValue
     */
    /****************************************************************************/
    ReturnCode_t GetActInputValue(quint16& inputValue);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetContactLimitValue
     *
     *  \param contactValue = quint16 type parameter
     *
     *  \return from SetContactLimitValue
     */
    /****************************************************************************/
    ReturnCode_t SetContactLimitValue(quint16 contactValue);

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the actual input value
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam InputValue = Actual input value
     */
    /****************************************************************************/
    void ReportActInputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue);

private:
    ReturnCode_t InitializeCANMessages();
    ReturnCode_t RegisterCANMessages();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendConfiguration
     *
     *  \return from SendConfiguration
     */
    /****************************************************************************/
    void SendConfiguration();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleIdleState
     *
     *  \return from HandleIdleState
     */
    /****************************************************************************/
    void HandleIdleState();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgConfig
     *
     *  \return from SendCANMsgConfig
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgConfig();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgMechanicalOffsetReq
     *
     *  \return from SendCANMsgMechanicalOffsetReq
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgMechanicalOffsetReq();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgDisplacementReq
     *
     *  \return from SendCANMsgDisplacementReq
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgDisplacementReq();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCANMsgInputValueResp
     *
     *  \param pCANframe = can_frame type parameter
     *
     *  \return from HandleCANMsgInputValueResp
     */
    /****************************************************************************/
    void HandleCANMsgInputValueResp(can_frame* pCANframe);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleDisplacmentRequestTask
     *
     *  \return from HandleDisplacmentRequestTask
     */
    /****************************************************************************/
    void HandleDisplacmentRequestTask();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleContactLimitSetTask
     *
     *  \return from HandleContactLimitSetTask
     */
    /****************************************************************************/
    void HandleContactLimitSetTask();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetTask
     *
     *  \return from ResetTask
     */
    /****************************************************************************/
    void ResetTask();

    /*! configuration state defines */
    typedef enum {
        FM_JSTCK_SUB_STATE_CONFIG_INIT   = 0x00,    //!< initialisation state
        FM_JSTCK_SUB_STATE_CONFIG_START  = 0x01,    //!< start state
        FM_JSTCK_SUB_STATE_CONFIG_FINISH = 0x02,    //!< finish state, configuration was send successfully
        FM_JSTCK_SUB_STATE_CONFIG_ERROR  = 0x03     //!< error state, configuration was not send successfully
    } CANJoystickSubStateConfig_t;

    CANJoystickSubStateConfig_t  m_subStateConfig;  //!< configuration state
    ModuleCmdState_t m_taskState;                   //!< general task state

    quint32 m_unCanIDConfig;            //!< CAN-message id of 'JoystickConfig' message
    quint32 m_unCanIDMechOffsetSet;     //!< CAN-message id of 'JoystickSetMechanicalOffset' message
    quint32 m_unCanIDDisplacementReq;   //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDDisplacement;      //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDMechOffsetReq;     //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDMechOffset;        //!< CAN-message id of 'TBD' message

    Global::MonotonicTime m_timeAction;       ///<  Definition/Declaration of variable m_timeAction
    qint16 m_aktionTimespan;       ///<  Definition/Declaration of variable m_aktionTimespan

    quint16 m_actInputValue;       ///<  Definition/Declaration of variable m_actInputValue
    quint16 m_contactLimit;       ///<  Definition/Declaration of variable m_contactLimit
};

} //namespace

#endif /* CANJOYSTICK_H */
