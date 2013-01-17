#ifndef DEVICECONTROL_SETDRAWERLED_H
#define DEVICECONTROL_SETDRAWERLED_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CSetDrawerLed : public CState
{
    Q_OBJECT

public:
    explicit CSetDrawerLed(CDigitalOutput *p_FmDoLedGreen, bool LedGreenActive,
                        CDigitalOutput *p_FmDoLedRed, bool LedRedActive,
                        const QString &Name, QState *p_Parent = 0);

signals:
    void LedError(ReturnCode_t);

private:
    bool Green(QEvent *p_Event);
    bool Red(QEvent *p_Event);
    bool End(QEvent *p_Event);

    CDigitalOutput *mp_FmDoLedGreen;
    CDigitalOutput *mp_FmDoLedRed;
    quint16 m_LedGreenValue;
    quint16 m_LedRedValue;

};

} //namespace

#endif // DEVICECONTROL_SETDRAWERLED_H

// vi: set ts=4 sw=4 et:

