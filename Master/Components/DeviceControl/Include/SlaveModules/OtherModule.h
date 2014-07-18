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
     *  \brief  Definition/Declaration of function CUart
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
    ~COtherModule();

    /// function module initialization, can communication initialisation
    ReturnCode_t Initialize();

    /// task handling function
    void HandleTasks();

    /// recall function to receive CAN messages from communication layer
    void HandleCanMessage(can_frame* pCANframe);

    inline void SetLifeTime(quint32 lifeTime) { m_LifeTime = lifeTime;}

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

    quint32 m_LifeTime;
};

} //namespace

#endif // DEVICECONTROL_OTHERPRIVATE_H
