#include "HoodDevice.h"
#include "SignalTransition.h"

#include <QFinalState>

namespace DeviceControl
{

typedef CSignalTransition<CHoodDevice> HoodDeviceTransition;

CHoodDevice::CHoodDevice(CMockFm *fm):CDevice(fm)
{
    CState *Configuring_Start = new CState("Configuring_Start", mp_Configuring);
    mp_Configuring->setInitialState(Configuring_Start);
    QFinalState *Configuring_End = new QFinalState(mp_Configuring);
    // TODO: add configuration work
    Configuring_Start->addTransition(Configuring_End);


    CState *Init_Start = new CState("Init_Start", mp_Initializing);
    mp_Initializing->setInitialState(Configuring_Start);
    QFinalState *Init_End = new QFinalState(mp_Initializing);
    // TODO: add configuration work
    Init_Start->addTransition(Init_End);


    mp_Idle = new CState("Hood_Idle", mp_Working);
    mp_Read = new CState("Hood_Read", mp_Working);

    mp_Working->setChildMode(QState::ExclusiveStates);

    mp_Working->setInitialState(mp_Idle);

    // Idle to Reading state when DCP requests for status
    mp_Idle->addTransition(new HoodDeviceTransition(this, SIGNAL(ReadHoodStatus()),
                                                    *this, &CHoodDevice::Trans_Read, mp_Read));

    // Idle to Read State when hood opened
    mp_Idle->addTransition(new HoodDeviceTransition(this, SIGNAL(ReportDigInputValue(quint32,ReturnCode_t,quint16)),
                                                    *this, &CHoodDevice::Trans_Idle, mp_Idle));

    // Reading to Read state when msg received from FM
    mp_Read->addTransition(new HoodDeviceTransition(mp_fm, SIGNAL(ReportDigInputValue(quint32,ReturnCode_t,quint16)),
                                                    *this, &CHoodDevice::Trans_Idle, mp_Idle));

    m_machine.start();
}

CHoodDevice::~CHoodDevice()
{

}

bool CHoodDevice::Trans_Idle(QEvent *p_Event)
{
    ReturnCode_t HdlInfo;
    quint16 InputValue;

    if (!HoodDeviceTransition::GetEventValue(p_Event, 1, HdlInfo)) {
        emit HoodError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!HoodDeviceTransition::GetEventValue(p_Event, 2, InputValue)) {
        emit HoodError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if (InputValue == 0)
        {
            emit ReportHoodStatus(HOOD_STATE_OPEN);
        }
        else
        {
            emit ReportHoodStatus(HOOD_STATE_CLOSED);
        }
    }
    else
    {
        emit ReportHoodStatus(HOOD_STATE_UNDEF);

        emit HoodError(HdlInfo);

        return false;
    }

    return true;
}

bool CHoodDevice::Trans_Read(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = mp_fm->ReqActInputValue();

    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit HoodError(ReturnCode);
        return false;
    }

    return true;
}

} //namespace
