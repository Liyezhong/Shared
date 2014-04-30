/******************************************************************/
/*! \file DeviceProcTask.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.05.2010
 *   $Author:  $ M.Scherer
 *
 *  \b Description:
 *
 *       This module contains the definition of the DeviceProcTask class
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

#ifndef DEVICEPROCTASK_H
#define DEVICEPROCTASK_H

/****************************************************************************/
/*! \class  DeviceProcTask
 *
 *  \brief  This class defines a task of the device control layer.
 *
 *      The classed is used to store severel parameter of a task to control
 *      the device control layer functionality.
 */
/****************************************************************************/
class DeviceProcTask
{
private:
    DeviceProcTask();                                           ///< Not implemented.
    DeviceProcTask(const DeviceProcTask &);                     ///< Not implemented.
    //const DeviceProcTask & operator = (const DeviceProcTask &); ///< Not implemented.

public:
    /*! Task identification definitions */
    typedef enum {
        TASK_ID_DP_UNDEF            = 0x00, //!< undefined task
        TASK_ID_DP_CONFIG           = 0x01, //!< configuration task
        TASK_ID_DP_NORMAL_OPERATION = 0x02, //!< normal operation task
        TASK_ID_DP_ADJUST           = 0x03, //!< adjustment
        TASK_ID_DP_SHUTDOWN         = 0x04, //!< shutdown
        TASK_ID_DP_DIAGNOSTIC       = 0x05, //!< service
        TASK_ID_DP_FW_UPDATE        = 0x06, //!< firmware update
        TASK_ID_DP_STOP             = 0x07, //!< stop,
        TASK_ID_DP_DESTROY          = 0x08  //!< destroy, finishs all operation
    } TaskID_t;

    /*! Task priority definitions */
    typedef enum {
        TASK_PRIO_LOW       = 0x01, //!< low priority
        TASK_PRIO_MIDDLE    = 0x02, //!< middle priority
        TASK_PRIO_HIGH      = 0x03, //!< high priority
        TASK_PRIO_VERYHIGH  = 0x04  //!< very high priority
    } TaskPrio_t;

    /*! Task state definitions */
    typedef enum {
        TASK_STATE_UNDEF        = 0x00, //!< undef, invalid state
        TASK_STATE_SLEEP        = 0x01, //!< the task sleeps
        TASK_STATE_STARTUP      = 0x02, //!< startup, wait for becoming active
        TASK_STATE_ACTIVE       = 0x03, //!< currently executed, just one state at one time can be active
        TASK_STATE_ACTIVE_BRK   = 0x04, //!< still active, but requested to break (e.g. high priority state is pending
        TASK_STATE_ABBORT       = 0x05, //!< abort, signal after a brake request was executed (state is ready for finished)
        TASK_STATE_PAUSE        = 0x06, //!< paused, signal after a brake request was executed, but the state need to be continued later
        TASK_STATE_FINSHED      = 0x07  //!< state has finished
    } TaskState_t;

    /****************************************************************************/
    /*!
     *  \brief  Constructor for the DeviceProcTask
     *
     *  \iparam TaskID = Task identification
     *  \iparam Priority = Task priority
     */
    /****************************************************************************/
    DeviceProcTask(TaskID_t TaskID, TaskPrio_t  Priority) {
        m_taskID = TaskID;
        m_priority = Priority;
        m_state = TASK_STATE_SLEEP;
        m_sParameter1 = 0;
        m_sParameter2 = 0;
    }

    TaskID_t    m_taskID;       //!< task identification
    TaskPrio_t  m_priority;     //!< task priority
    TaskState_t m_state;        //!< task state
    quint16     m_sParameter1;  //!< parameter
    quint16     m_sParameter2;  //!< parameter
};

#endif // DEVICEPROCTASK_H
