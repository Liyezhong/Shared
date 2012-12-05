/****************************************************************************/
/*! \file   DeviceBase.h
 *
 *  \brief  Definition file for class CDeviceBase
 *
 *  \version  0.1
 *  \date     2012-10-24
 *  \author   Torsten Scheck, Stalin
 *
 *  \b Description:
 *
 *   N/A
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

#ifndef DEVICECONTROL_BASE_H
#define DEVICECONTROL_BASE_H

#include <QStateMachine>
#include <QStringList>
#include <QThread>
#include <QDateTime>

#include "DeviceControl/Include/Devices/DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

typedef QList<CBaseModule*> DeviceBaseModList;

/****************************************************************************/
/*! \brief  Base Device class, to provide functionality required by all
 *          Devices
 *
 *  Signals: \n
 *      Request interface to DCP (incoming signals, used as events by Device
 *      Classes) \n
 *          Configure()\n
 *          Initialize()\n
 *          Stop()\n
 *          Resume()\n
 *          Abort()\n
 *          Disable()\n
 *
 *      Response Interface to DCP (outgoing signals, used as events by upper
 *      layer(s) i.e. DCP) \n
 *          ReportConfigure()\n
 *          ReportInitialize()\n
 *          ReportStop()\n
 *          ReportResume()\n
 *          ReportAbort()\n
 *          ReportDisable()\n
 *
 *      Independent Signals to DCP\n
 *          NeedInitialize()\n
 */
/****************************************************************************/
class CDeviceBase : public QObject
{
    Q_OBJECT

public:
    //CDevice(const DeviceProcessing &DeviceProcessing, 
    //	const DeviceFctModList &SlaveModuleList, 
    //	DevInstanceID_t InstanceId);
    CDeviceBase(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                DevInstanceID_t InstanceID);

    virtual ~CDeviceBase();

    QStringList GetStates();

    static QMap<DevInstanceID_t, QString> m_DeviceName;     ///< only for debug messages, not for any reference

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*! \brief  To trigger device configuration.
     *
     *          Triggers transition from start state to configuring state
     */
    /****************************************************************************/
    void Configure();

    /****************************************************************************/
    /*! \brief  To trigger device initialization
     *
     *          Triggers transition from configured state to initializing state
     */
    /****************************************************************************/
    void Initialize();

    /****************************************************************************/
    /*! \brief  To stop device.
     *
     *          Triggers transition from any working state to stop state
     */
    /****************************************************************************/
    void Stop();

    /****************************************************************************/
    /*! \brief  To resume device
     *
     *          Triggers transition from stop state to working state
     */
    /****************************************************************************/
    void Resume();

    /****************************************************************************/
    /*! \brief  To abort running task
     */
    /****************************************************************************/
    void Abort();

    /****************************************************************************/
    /*! \brief  To disable device
     *
     *          Disable FMs and return to Configured
     */
    /****************************************************************************/
    void Disable();

    // command response interface to DCP
    /****************************************************************************/
    /*! \brief  To acknowledge Configure() command
     *
     *          Acknowledges Configure() command with appropriate return code
     */
    /****************************************************************************/
    void ReportConfigure(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  To acknowledge Initialize() command
     *
     *          Acknowledges Initialize() command with appropriate return code
     */
    /****************************************************************************/
    void ReportInitialize(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  To acknowledge Stop() command
     *
     *          Acknowledges Stop() command with appropriate return code
     */
    /****************************************************************************/
    void ReportStop(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  To acknowledge Resume() command
     *
     *          Acknowledges Resume() command with appropriate return code
     */
    /****************************************************************************/
    void ReportResume(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  To acknowledge Abort() command
     *
     *          Acknowledges Abort() command with appropriate return code
     */
    /****************************************************************************/
    void ReportAbort(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  To acknowledge Disable() command
     *
     *          Acknowledges Disable() command with appropriate return code
     */
    /****************************************************************************/
    void ReportDisable(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  To report error to DCP
     */
    /****************************************************************************/
    void ReportError(DevInstanceID_t InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);

    /****************************************************************************/
    /*! \brief  To request reinitilization of device
     *
     *          Triggers reinitialization of device. It can be triggered when
     *          device does not behave as expected.
     */
    /****************************************************************************/
    void NeedInitialize(ReturnCode_t ReturnCode);

protected slots:
    void OnMachineErrorEntered();

    void Shutdown();

    virtual void ThreadStarted();

    virtual bool Trans_Configure(QEvent *p_Event) = 0;

protected:
    bool Trans_Initializing_Configured(QEvent *p_Event);
    bool Trans_Initializing_Working(QEvent *p_Event);
    bool Initialize_Nack(QEvent *p_Event);
    bool Configure_Ack(QEvent *p_Event);

    quint32 GetModuleInstanceFromKey(const QString &Key);

    QStateMachine m_machine;
    CState *mp_All;
    CState *mp_MachineError;
    CState *mp_Init;
    CState *mp_Start;
    CState *mp_Configuring;
    CState *mp_Configured;
    CState *mp_Initializing;
    CState *mp_Working;

    const DeviceProcessing &m_DeviceProcessing;
    const DevInstanceID_t m_InstanceID;     ///< Instance ID

    QThread m_Thread;

private:
    const DeviceModuleList_t m_ModuleList;  ///< Contains module identifiers
};

} //namespace

#endif /* DEVICECONTROL_DEVICE_H */

// vi: set ts=4 sw=4 et:

