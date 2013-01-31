#include "DeviceControl/Include/Devices/DeviceExhaust.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
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
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              request & response signals
 *
 *  \iparam     FctModList Function module list of this device, to invoke
 *              function module interface functions.
 *
 *  \iparam     InstanceID Device Instance ID
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
    // TODO: add configuration work
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
    mp_BaseModule = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseExhaustKey));

    mp_FlowSensor = static_cast<CDigitalInput *>(m_DeviceProcessing.GetFunctionModule(
                                                     GetModuleInstanceFromKey(CANObjectKeyLUT::m_FlowSensorKey)));
    mp_CurrentConsumptionFan1 = static_cast<CAnalogInput *>(m_DeviceProcessing.GetFunctionModule(
                                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_CurrentFan1Key)));
    mp_CurrentConsumptionFan2 = static_cast<CAnalogInput *>(m_DeviceProcessing.GetFunctionModule(
                                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_CurrentFan2Key)));
    mp_Fan[0] =  static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                 GetModuleInstanceFromKey(CANObjectKeyLUT::m_ControlFan1Key)));
    mp_Fan[1] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                 GetModuleInstanceFromKey(CANObjectKeyLUT::m_ControlFan2Key)));

    if ((mp_FlowSensor == NULL) || (mp_CurrentConsumptionFan1 == NULL) || (mp_CurrentConsumptionFan2 == NULL) ||
            (mp_Fan[0] == NULL) || (mp_Fan[1] == NULL) )
    {
        //! \todo Throw configuration error
        return false;
    }

    connect(mp_FlowSensor, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,quint16)),
            this, SLOT(FlowSensor(quint32,ReturnCode_t,quint16)));

    connect(mp_CurrentConsumptionFan1, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,qint16)),
            this, SLOT(Current1(quint32,ReturnCode_t,qint16)));

    connect(mp_CurrentConsumptionFan2, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,qint16)),
            this, SLOT(Current2(quint32,ReturnCode_t,qint16)));

    connect(mp_Fan[0], SIGNAL(ReportOutputValueAckn(quint32,ReturnCode_t,quint16)),
            this, SLOT(SetValveAckn(quint32,ReturnCode_t,quint16)));

    connect(mp_Fan[1], SIGNAL(ReportOutputValueAckn(quint32,ReturnCode_t,quint16)),
            this, SLOT(SetValveAckn(quint32,ReturnCode_t,quint16)));

    // No Initialization needed.
    QFinalState *Init_Start = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(Init_Start);

    return true;
}


void CDeviceExhaust::FlowSensor(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    Q_UNUSED(InstanceID)

    emit ReportFlowStatus(HdlInfo, InputValue);
}

void CDeviceExhaust::Current1(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 InputValue)
{
    Q_UNUSED(InstanceID)

    if (m_DeviceStatus)
    {
        if (InputValue < EXHAUSTFAN_MIN_CURRENT)
        {
            emit FanStopped(HdlInfo, FAN_ID_1, InputValue);
        }
        else
        {
            emit FanStarted(HdlInfo, FAN_ID_1, InputValue);
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
            emit FanStopped(HdlInfo, FAN_ID_2, InputValue);
        }
        else
        {
            emit FanStarted(HdlInfo, FAN_ID_2, InputValue);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  The sets valve On when, SetValveOn Command Recieved.
 *
 *  \iparam ValveID = Valve ID.
 *
 */
/****************************************************************************/
void CDeviceExhaust::SetFanOn(FanID_t FanID)
{
    quint8 Index;

    Index = GetIndexFromType(FanID);

    if (Index < MAX_EXHAUST_FANS)
    {
        mp_Fan[Index]->SetOutputValue(1);
    }
}

/****************************************************************************/
/*!
 *  \brief  The sets valve Off when, SetValveOff Command Recieved.
 *
 *  \iparam ValveID = Valve ID.
 *
 */
/****************************************************************************/
void CDeviceExhaust::SetFanOff(FanID_t FanID)
{
    quint8 Index;

    Index = GetIndexFromType(FanID);

    if (Index < MAX_EXHAUST_FANS)
    {
        mp_Fan[Index]->SetOutputValue(0);
    }
}

void CDeviceExhaust::SetValveAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
{
    quint8 FanIndex;
    FanID_t FanID;
    quint8 FctModuleIndex = 0;

    for(FanIndex = 0; FanIndex < MAX_EXHAUST_FANS; FanIndex++)
    {
        if((mp_Fan[FanIndex] != 0) && (mp_Fan[FanIndex]->GetModuleHandle() == InstanceID))
        {
            FctModuleIndex = FanIndex;
            break;
        }
    }

    FanID = GetFanIDFromIndex(FctModuleIndex);

    if(OutputValue)
    {
        emit ReportSetFanOn(HdlInfo, FanID);        ///< Signal to report valve set On.
    }
    else
    {
        emit ReportSetFanOff(HdlInfo, FanID);       ///< Signal to report valve set off.
    }
}


/****************************************************************************/
/*!
 *  \brief   Convert the valve type to function module array index
 *
 *  \iparam   ValveID = valvue identification
 *
 *  \return  requested array index
 *
 ****************************************************************************/
quint8 CDeviceExhaust::GetIndexFromType(FanID_t FanID)
{
    quint8 Index;

    switch(FanID)
    {
    case (FAN_ID_1):
        Index = 0;
        break;
    case (FAN_ID_2):
        Index = 1;
        break;
    default:
        Index = 0;
        break;
    }

    return Index;

}


/****************************************************************************/
/*!
 *  \brief   Convert the valve type to function module array index
 *
 *  \iparam   ValveID = valvue identification
 *
 *  \return  requested array index
 *
 ****************************************************************************/
FanID_t CDeviceExhaust::GetFanIDFromIndex(quint8 Index)
{
    FanID_t FanID;

    switch(Index)
    {
    case (0):
        FanID = FAN_ID_1;
        break;
    case (1):
        FanID = FAN_ID_1;
        break;
    default:
        FanID = FAN_ID_1;
        break;
    }

    return FanID;

}


}
