/******************************************************************/
/*! \file ShutdownService.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CShutdownService
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

#ifndef SHUTDOWNSERVICE_H
#define SHUTDOWNSERVICE_H


#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"

#include <QHash>

class QDomElement;

namespace DeviceControl
{

class DeviceProcessing;

/****************************************************************************/
/*! \brief This class implements the functionality to shutdown the object tree conponents
*          of the device control layer
*/
/****************************************************************************/
class CShutdownService
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CShutdownService
     *
     *  \param pDeviceProcessing = DeviceProcessing type parameter
     *  \param pCANCommunicator =  CANCommunicator type parameter
     *  \param pObjectTree =  ObjectTree type parameter
     *
     *  \return from CShutdownService
     */
    /****************************************************************************/
    CShutdownService(DeviceProcessing* pDeviceProcessing, CANCommunicator* pCANCommunicator, ObjectTree* pObjectTree);
    virtual ~CShutdownService();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleTasks
     *
     *  \return from HandleTasks
     */
    /****************************************************************************/
    void HandleTasks();

    //! Configuration service main states definitions
    typedef enum  {
        CS_MAIN_STATE_UNDEF            = 0x00,  ///< undefined
        CS_MAIN_STATE_INIT             = 0x01,  ///< initialisation state
        CS_MAIN_STATE_REQ_NODE_STATE   = 0x02,  ///< request the shutdown node state
        CS_MAIN_STATE_WAIT_NODE_STATES = 0x03,  ///< wait for all nedes acknowledged the shutdown node state
        CS_MAIN_STATE_FINISHED         = 0x04,  ///< finished
        CS_MAIN_STATE_ERROR            = 0x07   ///< error state
    } ShutdownServiceMainState_t;

    /*****************************************************************************/
    /*!
     *  \brief  Return the main state
     *
     *  \return Actual main state
     */
    /*****************************************************************************/
    ShutdownServiceMainState_t GetState() { return m_MainState; }

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RequestNodeStateShutdown
     *
     *  \return from RequestNodeStateShutdown
     */
    /****************************************************************************/
    ReturnCode_t RequestNodeStateShutdown();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CheckNodeState
     *
     *  \return from CheckNodeState
     */
    /****************************************************************************/
    ReturnCode_t CheckNodeState();

    ShutdownServiceMainState_t m_MainState; ///< main state

    DeviceProcessing* m_pDeviceProcessing;  ///< pointer to the device processing instance
    CANCommunicator*  m_pCANCommunicator;   ///< pointer to the CAN communicator
    ObjectTree*       m_pObjectTree;        ///< pointer to object tree, which keeps all CAN objects

    ReturnCode_t m_LastErrorHdlInfo;        ///< return value after a failed function call
    quint16      m_LastErrorGroup;          ///< error group of the error causing functionality
    quint16      m_LastErrorCode;           ///< error code of the error causing functionality
    quint16      m_LastErrorData;           ///< additional error data, filled by the error causing functionality
    QDateTime    m_LastErrorTime;           ///< time of error detection

    Global::MonotonicTime m_StateTimer;     ///< timer for timeout observation
    qint16 m_StateTimespan;                 ///< max. time delay of current active timeout observation
};

}  //namespace

#endif // SHUTDOWNSERVICE_H
