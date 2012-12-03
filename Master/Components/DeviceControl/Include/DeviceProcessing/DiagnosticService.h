/****************************************************************************/
/*! \file DiagnosticService.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CDiagnosticService
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

#ifndef DIAGNOSTICSERVICE_H
#define DIAGNOSTICSERVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"

namespace DeviceControl
{

class CCANMsgCommunicationBase;
class CANCommunicator;

/****************************************************************************/
/*!
 *  \brief This class implements the functionality to handle the diagnostic task
 */
/****************************************************************************/
class CDiagnosticService
{
public:
    CDiagnosticService(DeviceProcessing* pDeviceProcessing, CANCommunicator* pCANCommunicator, ObjectTree* pObjectTree);
    virtual ~CDiagnosticService();

    void HandleTasks();

    //! Main states of the diagnostics service
    typedef enum  {
        DIAG_MAIN_STATE_UNDEF      = 0x00,  //!< undefined
        DIAG_MAIN_STATE_INIT       = 0x01,  //!< initialisation
        DIAG_MAIN_STATE_CONFIG     = 0x02,  //!< configuration
        DIAG_MAIN_STATE_IDLE       = 0x03,  //!< idle state
        DIAG_MAIN_STATE_ERROR      = 0x04   //!< idle state
    } DiagnosticServiceMainState_t;

    /****************************************************************************/
    /*!
     *  \brief  Returns the main state
     *
     *  \return Main state
     */
    /****************************************************************************/
    DiagnosticServiceMainState_t GetState() { return m_MainState; }

    void ThrowErrorSignal(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime ErrorTime);

private:
    void HandleConfigurationTask();
    ReturnCode_t CheckNodeState();
    ReturnCode_t RequestNodeStateAssembly();

    DeviceProcessing* m_pDeviceProcessing;  //!< Device processing
    CANCommunicator* m_pCANCommunicator;    //!< CAN-Communicator
    ObjectTree* m_pObjectTree;              //!< Object tree

    DiagnosticServiceMainState_t m_MainState;   //!< Main state

    ReturnCode_t m_LastErrorHdlInfo;    //!< last errorcode, e.g. received by a fuction call
    quint16      m_LastErrorGroup;      //!< last error's group id
    quint16      m_LastErrorCode;       //!< last error's error code
    quint16      m_LastErrorData;       //!< last error's data
    QDateTime    m_LastErrorTime;       //!< last error's time
    QString      m_LastErrorString;     //!< last error information string
};

}  //namespace

#endif /* DIAGNOSTICSERVICE_H */
