/******************************************************************/
/*! \file AdjustmentService.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 11.10.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CAdjustmentService
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

#ifndef ADJUSTMENTSERVICE_H
#define ADJUSTMENTSERVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"

namespace DeviceControl
{

class CCANMsgCommunicationBase;
class CANCommunicator;

/****************************************************************************/
/*!
 *  \brief  This is the class to control the adjustment procedure
 *
 *      The class provides functionality to control the adjustment sequence
 *       - checking the preconditions
 *       - controlling the movement of the grappler
 *       - performing plausibility checks
 *       - store the adjustment data to a file
 */
/****************************************************************************/
class CAdjustmentService
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CAdjustmentService
     *
     *  \param pDeviceProcessing = DeviceProcessing type parameter
     *  \param pCANCommunicator =  CANCommunicator type parameter
     *  \param pObjectTree =  ObjectTree type parameter
     *
     *  \return from CAdjustmentService
     */
    /****************************************************************************/
    CAdjustmentService(DeviceProcessing* pDeviceProcessing, CANCommunicator* pCANCommunicator, ObjectTree* pObjectTree);
    virtual ~CAdjustmentService();

    //! The main states of the adjustment service
    typedef enum {
        ADJUST_MAIN_STATE_UNDEF  = 0x00,    //!< Undefined state
        ADJUST_MAIN_STATE_INIT   = 0x01,    //!< Initialization state
        ADJUST_MAIN_STATE_CONFIG = 0x02,    //!< Configuration state
        ADJUST_MAIN_STATE_IDLE   = 0x03     //!< Idle state
    } AdjustmentServiceMainState_t;

    /****************************************************************************/
    /*!
     *  \brief  Returns the main state of the adjustment service
     *
     *  \return Main state
     */
    /****************************************************************************/
    AdjustmentServiceMainState_t GetState() { return m_mainState; }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleTasks
     *
     *  \return from HandleTasks
     */
    /****************************************************************************/
    void HandleTasks();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ThrowErrorSignal
     *
     *  \param usNodeID = quint16 type parameter
     *  \param ulModulInfo =  quint32 type parameter
     *  \param usErrorGroup =  quint16 type parameter
     *  \param usErrorID =  quint16 type parameter
     *  \param sErrorData =  qint16 type parameter
     *
     *  \return from ThrowErrorSignal
     */
    /****************************************************************************/
    void ThrowErrorSignal(quint16 usNodeID, quint32 ulModulInfo, quint16 usErrorGroup, quint16 usErrorID, qint16 sErrorData);

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleInitializationState
     *
     *  \return from HandleInitializationState
     */
    /****************************************************************************/
    void HandleInitializationState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleConfigurationState
     *
     *  \return from HandleConfigurationState
     */
    /****************************************************************************/
    void HandleConfigurationState();
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
     *  \brief  Definition/Declaration of function HandleCANNodesTask
     *
     *  \return from HandleCANNodesTask
     */
    /****************************************************************************/
    void HandleCANNodesTask();

    DeviceProcessing* m_pDeviceProcessing;  //!< Global device processing
    CANCommunicator* m_pCANCommunicator;    //!< Global CAN communicator
    ObjectTree* m_pObjectTree;              //!< Global object tree

    AdjustmentServiceMainState_t m_mainState;   //!< Main state of the service
};

}  //namespace

#endif /* ADJUSTMENTSERVICE_H */
