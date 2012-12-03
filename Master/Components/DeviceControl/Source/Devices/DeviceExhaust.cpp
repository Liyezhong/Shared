#include "DeviceControl/Include/Devices/DeviceExhaust.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

#include <QFinalState>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceHood
 *
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceExhaust> CExhaustTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceHood
 *
 *  \iparam     fm      Function module pointer of Digital Input that signals
 *                      change in hood device.
 */
/****************************************************************************/
CDeviceExhaust::CDeviceExhaust(const DeviceProcessing &DeviceProc, const DeviceModuleList_t& FctModList,
                         DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, FctModList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      Invoked when its thread is started. All child objects of this
 *              devices shall instantiated after this event.
 *
 *  \iparam     none
 */
/****************************************************************************/
void CDeviceExhaust::ThreadStarted()
{
    // Configure step might be needed for ProtoTest in order to
    // allow reconfiguration (FM housekeeping and connects).
    // => automatically trigger configuration from constructor
    //    in order to keep interface lean at first.

    /////////////////////////////////////////////////////////////////
    // Configuring composite states
    /////////////////////////////////////////////////////////////////
    CState *ConfigStart = new CState("ConfigStart", mp_Configuring);
    QFinalState *ConfigEnd = new QFinalState(mp_Configuring);
    mp_Configuring->setInitialState(ConfigStart);

    ConfigStart->addTransition(new CExhaustTransition(ConfigStart, SIGNAL(entered()),
                                                      *this, &CDeviceExhaust::Trans_Configure, ConfigEnd));

    m_machine.start();
}

/****************************************************************************/
/*!
 *  \brief      To configure device with appropriate function modules &
 *              definition of working state flow for exhaust device.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true, if configuration is successful, else false
 */
/****************************************************************************/
bool CDeviceExhaust::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    // Get function module instances
    mp_FlowSensor = static_cast<CDigitalInput *>(m_DeviceProcessing.GetFunctionModule(
                                                     GetModuleInstanceFromKey(CANObjectKeyLUT::m_FlowSensorKey)));

    mp_CurrentConsumptionFan1 = static_cast<CAnalogInput *>(m_DeviceProcessing.GetFunctionModule(
                                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_CurrentFan1Key)));

    mp_CurrentConsumptionFan2 = static_cast<CAnalogInput *>(m_DeviceProcessing.GetFunctionModule(
                                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_CurrentFan2Key)));

    if (mp_FlowSensor == NULL || mp_CurrentConsumptionFan1 == NULL || mp_CurrentConsumptionFan2 == NULL)
    {
        return false;
    }

    connect(mp_FlowSensor, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,quint16)),
            this, SLOT(FlowSensor(quint32,ReturnCode_t,quint16)));

    connect(mp_CurrentConsumptionFan1, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,qint16)),
            this, SLOT(Current1(quint32,ReturnCode_t,qint16)));

    connect(mp_CurrentConsumptionFan2, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,qint16)),
            this, SLOT(Current2(quint32,ReturnCode_t,qint16)));
}

void CDeviceExhaust::ActivateSlot()
{
    m_DeviceStatus = true;
}

void CDeviceExhaust::DeactivateSlot()
{
    m_DeviceStatus = false;
}

void CDeviceExhaust::GetFlowStatusSlot()
{
    ReturnCode_t ReturnCode;

    ReturnCode = mp_FlowSensor->ReqActInputValue();

    if (ReturnCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
        emit NeedInitialize(ReturnCode);
    }
}

void CDeviceExhaust::FlowSensor(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    Q_UNUSED(InstanceID)

    emit ReportFlowStatus(HdlInfo, InputValue);
}

void CDeviceExhaust::Current1(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 InputValue)
{
    Q_UNUSED(InstanceID)
    Q_UNUSED(HdlInfo)

    if (m_DeviceStatus)
    {
        if (InputValue < EXHAUSTFAN_MIN_CURRENT)
        {
            emit FanStopped(EXHAUST_FAN_1);
        }
        else
        {
            emit FanStarted(EXHAUST_FAN_1);
        }
    }
}

void CDeviceExhaust::Current2(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 InputValue)
{
    Q_UNUSED(InstanceID)
    Q_UNUSED(HdlInfo)

    if (m_DeviceStatus)
    {
        if (InputValue < EXHAUSTFAN_MIN_CURRENT)
        {
            emit FanStopped(EXHAUST_FAN_1);
        }
        else
        {
            emit FanStarted(EXHAUST_FAN_1);
        }
    }
}

}
