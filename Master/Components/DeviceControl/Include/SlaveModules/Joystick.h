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
    CJoystick(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
              CBaseModule* pParentNode);

    ReturnCode_t Initialize();

    void HandleTasks();
    void HandleCanMessage(can_frame* pCANframe);

    ReturnCode_t SetState(bool Active, bool Scanning = false, bool SetCenter = false);
    ReturnCode_t SetVoltageRange(qint16 MinX, qint16 MaxX, qint16 MinY, qint16 MaxY);
    ReturnCode_t GetDisplacement();
    ReturnCode_t SetMechanicalOffset(quint32 Offset);
    ReturnCode_t GetMechanicalOffset();

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to acknowledge the set state command
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportSetState(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to acknowledge the set voltage range command
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportSetVoltageRange(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the current displacement values
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam DisplacementX = Displacement value of the X-axis
     *  \iparam DisplacementY = Displacement value of the Y-axis
     */
    /****************************************************************************/
    void ReportGetDisplacement(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 DisplacementX, qint16 DisplacementY);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to acknowledge the set mechanical offset command
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportSetMechanicalOffset(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to acknowledge the get mechanical offset command
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Offset = Mechanical offset data
     */
    /****************************************************************************/
    void ReportGetMechanicalOffset(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 Offset);

private:
    ReturnCode_t InitializeCANMessages();
    ReturnCode_t RegisterCANMessages();

    void HandleIdleState();
    void HandleCommandRequestTask();

    ReturnCode_t SendCANMsgSetConfig(bool Active, bool Scanning, bool SetCenter);
    ReturnCode_t SendCANMsgSetVoltageRange(qint16 MinX, qint16 MaxX, qint16 MinY, qint16 MaxY);
    ReturnCode_t SendCANMsgGetDisplacement();
    ReturnCode_t SendCANMsgSetMechanicalOffset(quint32 Offset);
    ReturnCode_t SendCANMsgGetMechanicalOffset();

    void HandleCANMsgGetDisplacement(can_frame* pCANframe);
    void HandleCANMsgGetMechanicalOffset(can_frame* pCANframe);

    /*! Joystick command type definitions */
    typedef enum {
        FM_JSTCK_CMD_TYPE_UNDEF             = 0,  //!< undefined command type
        FM_JSTCK_CMD_TYPE_SET_CONFIG        = 1,  //!< set configuration
        FM_JSTCK_CMD_TYPE_SET_VOLTAGE_RANGE = 2,  //!< set voltage range
        FM_JSTCK_CMD_TYPE_SET_MECH_OFFSET   = 3,  //!< set mechanical offset
        FM_JSTCK_CMD_TYPE_GET_DISPLACEMENT  = 4,  //!< get displacement
        FM_JSTCK_CMD_TYPE_GET_MECH_OFFSET   = 5   //!< get mechanical offset
    } CANJoystickModuleCmdType_t;

    /*! configuration state defines */
    typedef enum {
        FM_JSTCK_SUB_STATE_CONFIG_INIT   = 0x00,    //!< initialisation state
        FM_JSTCK_SUB_STATE_CONFIG_START  = 0x01,    //!< start state
        FM_JSTCK_SUB_STATE_CONFIG_FINISH = 0x02,    //!< finish state, configuration was send successfully
        FM_JSTCK_SUB_STATE_CONFIG_ERROR  = 0x03     //!< error state, configuration was not send successfully
    } CANJoystickSubStateConfig_t;

    CANJoystickSubStateConfig_t  m_subStateConfig;  //!< configuration state

    quint32 m_unCanIDConfig;            //!< CAN-message id of 'JoystickConfig' message
    quint32 m_unCanIDVoltageRangeSet;   //!< CAN-message id of 'JoystickVoltageRangeSet' message
    quint32 m_unCanIDMechOffsetSet;     //!< CAN-message id of 'JoystickMechOffsetSet' message
    quint32 m_unCanIDDisplacementReq;   //!< CAN-message id of 'JoystickDisplacementReq' message
    quint32 m_unCanIDDisplacement;      //!< CAN-message id of 'JoystickDisplacement' message
    quint32 m_unCanIDMechOffsetReq;     //!< CAN-message id of 'JoystickMechOffsetReq' message
    quint32 m_unCanIDMechOffset;        //!< CAN-message id of 'JoystickMechOffset' message

    /*! module command data, used for internal data transfer*/
    typedef struct {
        CANJoystickModuleCmdType_t Type;    //!< command type
        ModuleCmdState_t State;             //!< command state
        Global::MonotonicTime ReqSendTime;  //!< time the command was executed
        qint32 Timeout;                     //!< timeout in ms
        bool Active;                        //!< Enable/disable flag of the module
        bool Scanning;                      //!< Scanning mode or threshold mode
        bool SetCenter;                     //!< Center detection for calibration
        qint16 MinimumX;                    //!< Minimum voltage of the X-axis in mV
        qint16 MaximumX;                    //!< Maximum voltage of the X-axis in mV
        qint16 MinimumY;                    //!< Minimum voltage of the Y-axis in mV
        qint16 MaximumY;                    //!< Maximum voltage of the Y-axis in mV
        quint32 MechanicalOffset;           //!< Mechanical offset stored on the board
    } ModuleCommand_t;

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANJoystickModuleCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANJoystickModuleCmdType_t CommandType);
};

} //namespace

#endif /* CANJOYSTICK_H */
