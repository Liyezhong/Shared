#ifndef DEVICECONTROL_DRAWERMOTION_H
#define DEVICECONTROL_DRAWERMOTION_H

#include "State.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CMockFm;

class CDrawerMotion : public CState
{
    Q_OBJECT

public:
    explicit CDrawerMotion(
	    CMockFm *p_FmMotor, 
        CMockFm *p_FmDiButton, 
        CMockFm *p_FmDoLedGreen, 
        CMockFm *p_FmDoLedRed, 
        const QString &Name, 
        QState *p_Parent = 0);

signals:
    // command request interface to DCP
    void Block(); 
    void Unblock(); 

    // command response interface to DCP
    void ReportBlock(ReturnCode_t ReturnCode); 
    void ReportUnblock(ReturnCode_t ReturnCode); 

    // Independent signals to DCP
    void Open();
    void Closed();
    
    // to this
    
private:
    bool AckBlock(QEvent *p_Event);
    bool AckUnblock(QEvent *p_Event);
    bool NackBlock(QEvent *p_Event);
    bool NackUnblock(QEvent *p_Event);

    CMockFm *mp_FmMotor;
    CMockFm *mp_FmDiButton;
    CMockFm *mp_FmDoLedGreen;
    CMockFm *mp_FmDoLedRed;
};

} //namespace

#endif // DEVICECONTROL_DRAWERMOTION_H

// vi: set ts=4 sw=4 et:

