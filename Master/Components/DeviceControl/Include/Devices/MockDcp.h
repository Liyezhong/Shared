#ifndef DEVICECONTROL_MOCKDCL_H
#define DEVICECONTROL_MOCKDCL_H

//#include <QDateTime>
#include <QObject>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

class CMockDcp : public QObject
{
Q_OBJECT
public:
    CMockDcp(QObject *parent = 0) : QObject(parent) {}
    void EmitConfigure() { emit Configure(); }
    void EmitInitialize() { emit Initialize(); }
    void EmitReadRfid(LoaderRFIDChannel_t Channel) { emit ReadRfid( Channel ); }
    void EmitBlock() { emit Block(); }
    void EmitUnblock() { emit Unblock(); }
    void EmitIsClosed() { emit IsClosed(); }

    void EmitOpen() { emit Open(); }
    void EmitClose() { emit Close(); }

    void EmitStop() { emit Stop(); }
    void EmitResume() { emit Resume(); }
    void EmitAbort() { emit Abort(); }
    void EmitDisable() { emit Disable(); }
    void EmitRotate(quint32 speed,quint8 MotionProfileIdx){ emit Rotate(speed,MotionProfileIdx); }
    void EmitMove(quint32 TargetPosition, quint8 MotionProfileIdx){emit Move(TargetPosition,MotionProfileIdx);}

Q_SIGNALS:
    void Configure();
    void Initialize();

    void ReadRfid(LoaderRFIDChannel_t);
    void Block();
    void Unblock();
    void IsClosed();

    void Open();
    void Close();

    void Stop();
    void Resume();
    void Abort();
    void Disable();
    void Rotate(quint32,quint8);
    void Move(quint32,quint8);
};


} //namespace

#endif /* DEVICECONTROL_MOCKDCL_H */

// vi: set ts=4 sw=4 et:

