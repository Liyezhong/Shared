#include "DrawerLed.h"
#include "SignalTransition.h"
#include <QFinalState>

#include "MockFm.h"

namespace DeviceControl
{

typedef CSignalTransition<CDrawerLed> CDrawerLedTransition;

CDrawerLed::CDrawerLed(
    CMockFm *p_FmDoLedGreen, bool LedGreenActive,
    CMockFm *p_FmDoLedRed,   bool LedRedActive,
    const QString &Name, QState *p_Parent) :
        CState(Name, p_Parent), 
        mp_FmDoLedGreen(p_FmDoLedGreen), 
        mp_FmDoLedRed(p_FmDoLedRed),
        m_LedGreenValue(0),
        m_LedRedValue(0)
{
    if (LedGreenActive) {
        m_LedGreenValue = 1;
    }
    if (LedRedActive) {
        m_LedRedValue = 1;
    }

    CState *p_Start = new CState("LedStart", this);
    CState *p_Green = new CState("LedGreen", this);
    CState *p_Red = new CState("LedRed", this);
    QFinalState *p_End = new QFinalState(this);
    setInitialState(p_Start);

    p_Start->addTransition( new CDrawerLedTransition(
        p_Start, SIGNAL(entered()),
        *this, &CDrawerLed::Green,
        p_Green) );

    p_Green->addTransition( new CDrawerLedTransition(
        p_FmDoLedGreen, SIGNAL(ReportActOutputValue(quint32, ReturnCode_t, quint16)),
        *this, &CDrawerLed::Red,
        p_Red) );

    p_Red->addTransition( new CDrawerLedTransition(
        p_FmDoLedRed, SIGNAL(ReportActOutputValue(quint32, ReturnCode_t, quint16)),
        *this, &CDrawerLed::End,
        p_End) );
}


bool CDrawerLed::Green(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_FmDoLedGreen->SetOutputValue(m_LedGreenValue);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit LedError(ReturnCode);
        return false;
    }
    return true;
}

bool CDrawerLed::Red(QEvent *p_Event)
{
    // ReportActOutputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
    ReturnCode_t ReturnCode = CDrawerLedTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit LedError(ReturnCode);
        return false;
    }

    ReturnCode = mp_FmDoLedRed->SetOutputValue(m_LedRedValue);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit LedError(ReturnCode);
        return false;
    }
    return true;
}

bool CDrawerLed::End(QEvent *p_Event)
{
    // ReportActOutputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
    ReturnCode_t ReturnCode = CDrawerLedTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit LedError(ReturnCode);
        return false;
    }
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
