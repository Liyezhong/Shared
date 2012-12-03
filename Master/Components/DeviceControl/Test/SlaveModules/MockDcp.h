#ifndef DEVICECONTROL_MOCKDCL_H
#define DEVICECONTROL_MOCKDCL_H

#include <QObject>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "utGlobal.h"

namespace DeviceControl {

class CMockDcp : public QObject
{
Q_OBJECT
public:
    CMockDcp(QObject *parent = 0) : QObject(parent) {}

    void EmitConfigure() { emit Configure();    THREAD_PROCESS(); }
    void EmitInitialize() { emit Initialize();  THREAD_PROCESS(); }

    void EmitOpen() { emit Open();      THREAD_PROCESS();}
    void EmitClose() { emit Close();    THREAD_PROCESS();}

    void EmitReadRfid(LoaderRFIDChannel_t Channel) { emit ReadRfid(Channel); THREAD_PROCESS(); }
    void EmitBlock() { emit Block();        THREAD_PROCESS();}
    void EmitUnblock() { emit Unblock();    THREAD_PROCESS();}
    void EmitIsClosed() { emit IsClosed();  THREAD_PROCESS();}

    void EmitStop() { emit Stop();          THREAD_PROCESS();}
    void EmitResume() { emit Resume();      THREAD_PROCESS();}
    void EmitAbort() { emit Abort();        THREAD_PROCESS();}
    void EmitDisable() { emit Disable();    THREAD_PROCESS();}

    void EmitRotate(quint32 speed,quint8 MotionProfileIdx){ emit Rotate(speed,MotionProfileIdx); THREAD_PROCESS(); }
    void EmitMove(quint32 TargetPosition, quint8 MotionProfileIdx){emit Move(TargetPosition,MotionProfileIdx); THREAD_PROCESS(); }
    void EmitSetLightOn() { emit SetLightOn(); THREAD_PROCESS(); }
    void EmitSetLightOff() { emit SetLightOff(); THREAD_PROCESS(); }
    void EmitSetValveOn(WaterValveID_t ValveID){emit SetValveOn( ValveID);}
    void EmitSetValveOff(WaterValveID_t ValveID){emit SetValveOff( ValveID);}
    void EmitMoveEmptyTo(quint16 StationColumn, quint16 StationRow) {emit MoveEmptyTo(StationColumn, StationRow);   THREAD_PROCESS(); }
    void EmitReadLiquidLevel(){emit ReadLiquidLevel();}

    void EmitTransportRack(quint16 StationColumn, quint16 StationRow) {emit TransportRackTo(StationColumn, StationRow);   THREAD_PROCESS(); }

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
    void SetLightOn();
    void SetLightOff();
    void SetValveOn(WaterValveID_t);
    void SetValveOff(WaterValveID_t);
    void ReadLiquidLevel();

    // Xyz Device signals
    void MoveEmptyTo(quint16 StationColumn, quint16 StationRow);
    void TransportRackTo(quint16 StationColumn, quint16 StationRow);
};


} //namespace

#endif /* DEVICECONTROL_MOCKDCL_H */

// vi: set ts=4 sw=4 et:

