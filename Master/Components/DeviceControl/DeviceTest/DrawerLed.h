#ifndef DEVICECONTROL_DRAWERLED_H
#define DEVICECONTROL_DRAWERLED_H

#include "State.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CMockFm;

class CDrawerLed : public CState
{
    Q_OBJECT

public:
    explicit CDrawerLed(CMockFm *p_FmDoLedGreen, bool LedGreenActive,
                        CMockFm *p_FmDoLedRed, bool LedRedActive,
                        const QString &Name, QState *p_Parent = 0);

signals:
    void LedError(ReturnCode_t);

private:
    bool Green(QEvent *p_Event);
    bool Red(QEvent *p_Event);
    bool End(QEvent *p_Event);

    CMockFm *mp_FmDoLedGreen;
    CMockFm *mp_FmDoLedRed;
    quint16 m_LedGreenValue;
    quint16 m_LedRedValue;

};

} //namespace

#endif // DEVICECONTROL_DRAWERLED_H

// vi: set ts=4 sw=4 et:

