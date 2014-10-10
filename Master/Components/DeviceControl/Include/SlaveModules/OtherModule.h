/******************************************************************/
/*! \file Uart.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CUart
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
/******************************************************************/

#ifndef DEVICECONTROL_OTHERPRIVATE_H
#define DEVICECONTROL_OTHERPRIVATE_H

#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/SlaveModules/FunctionModule.h"

namespace DeviceControl
{

class CANCommunicator;

/****************************************************************************/
/*!
 *  \brief Other function module class
 *
 *      This class implements the functionality to configure and control a
 *      slave's 'Other' function module
 */
/****************************************************************************/
class COtherModule : public CFunctionModule
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function constructor
     *
     *  \param p_MessageConfiguration
     *  \param pCANCommunicator
     *  \param pParentNode
     *
     *  \return from COtherModule
     */
    /****************************************************************************/
    COtherModule(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
          CBaseModule* pParentNode);
    /****************************************************************************/
    /*!
     *  \brief  destructor
     */
    /****************************************************************************/
    ~COtherModule();

    /****************************************************************************/
    /*!
     *  \brief  function module initialization, can communication initialisation
     *  \return return code
     */
    /****************************************************************************/    
    ReturnCode_t Initialize();

    /****************************************************************************/
    /*!
     *  \brief  task handling function
     *  \return return code
     */
    /****************************************************************************/ 
    void HandleTasks();

    /****************************************************************************/
    /*!
     *  \brief  recall function to receive CAN messages from communication layer
     *  \param  pCANframe can frame
     */
    /****************************************************************************/     
    void HandleCanMessage(can_frame* pCANframe);

    /****************************************************************************/
    /*!
     *  \brief  recall function SetLifeTime
     *  \param  lifeTime life time
     */
    /****************************************************************************/   
    inline void SetLifeTime(quint32 lifeTime) { m_LifeTime = lifeTime;}

    /****************************************************************************/
    /*!
     *  \brief  recall function GetLifeTime
     *  \return life time
     */
    /****************************************************************************/   
    inline quint32 GetLifeTime() {return m_LifeTime;}

private:
    /// CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    /// Register receive CAN messages
    ReturnCode_t RegisterCANMessages();

    /// Handle the configuration state
    void HandleConfigurationState();

    /// Handle the idle state
    void HandleIdleState();

    quint32 m_LifeTime; //!< life time
};

} //namespace

#endif // DEVICECONTROL_OTHERPRIVATE_H
