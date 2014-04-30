/****************************************************************************/
/*! \file FunctionModule.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.12.2011
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class FunctionModule
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

#ifndef DEVICECONTROL_FUNCTIONMODULEPRIVATE_H
#define DEVICECONTROL_FUNCTIONMODULEPRIVATE_H

#include <QObject>
#include <QDateTime>

#include "DeviceControl/Include/SlaveModules/Module.h"
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"

namespace DeviceControl
{

class CANCommunicator;
class CBaseModule;
class CFunctionModule;

/****************************************************************************/
/*!
 *  \brief  This class is the base class for all function modules
 *
 *      The class provides some function module specific basic functionality,
 *      like channel and main state access. It provides some virtual methods
 *      for task handling and CAN message receipt
 */
/****************************************************************************/
class CFunctionModule : public CModule
{
    Q_OBJECT

public:
    CFunctionModule(CModuleConfig::CANObjectType_t eObjectType, const CANMessageConfiguration *MessageConfiguration,
                    CANCommunicator* pCANCommunicator, CBaseModule* pParentNode);
    virtual ~CFunctionModule();

    /****************************************************************************/
    /*!
     *  \brief  Returns the function module's channel
     *
     *  \return Channel number
     */
    /****************************************************************************/
    quint8 GetChannelNo() const { return m_pCANObjectConfig->m_sChannel; }

    /*! Main state type definition */
    typedef enum {
        FM_MAIN_STATE_UNDEF     = 0x00, //!< undefined
        FM_MAIN_STATE_BOOTUP    = 0x01, //!< bootup, after instance was dcreeted, set at constuctor
        FM_MAIN_STATE_INIT      = 0x02, //!< initialisation done, set if Initialize() was called
        FM_MAIN_STATE_CONFIRMED = 0x03, //!< function module was confirmed by base module, CAN-msg 'm_unCanIDConfig was received, set by Confirm()
        FM_MAIN_STATE_CONFIG    = 0x04, //!< coniguration, the configuration massges are sento slave
        FM_MAIN_STATE_IDLE      = 0x05, //!< idle state. ready for requests
        FM_MAIN_STATE_ERROR     = 0x06  //!< error state
    } CANFctModMainState_t;

    /****************************************************************************/
    /*!
     *  \brief  Returns the main state
     *
     *  \return Main state
     */
    /****************************************************************************/
    CANFctModMainState_t GetMainState() const { return m_mainState; }
    quint32 GetNodeID() const;

    /****************************************************************************/
    /*!
     *  \brief  Set the main state to 'confirmed'
     *
     *      Done by CANNode, when 'm_unCanIDConfig' was received.
     */
    /****************************************************************************/
    void Confirm() { m_mainState = FM_MAIN_STATE_CONFIRMED; }

protected:
    /****************************************************************************/
    /*!
     *  \brief  Initialisation of event CAN message IDs
     *
     *  \param ModuleID
     *
     *  \return from InitializeEventCANMessages
     */
    /****************************************************************************/
    ReturnCode_t InitializeEventCANMessages(quint8 ModuleID);

    /****************************************************************************/
    /*!
     *  \brief  Registers the can messages to communication layer
     *
     *  \return from RegisterEventCANMessages
     */
    /****************************************************************************/
    ReturnCode_t RegisterEventCANMessages();

    CANFctModMainState_t m_mainState;   //!< Main state
    CBaseModule* m_pParent;             //!< Pointer to CANNode this module is assigned to

private:
    CFunctionModule();                                              ///< Not implemented.
    CFunctionModule(const CFunctionModule &);                       ///< Not implemented.
    //const CFunctionModule & operator = (const CFunctionModule &);   ///< Not implemented.
};

} //namespace

#endif /* DEVICECONTROL_FUNCTIONMODULEPRIVATE_H */
