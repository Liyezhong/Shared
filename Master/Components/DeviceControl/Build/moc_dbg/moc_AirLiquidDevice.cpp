/****************************************************************************
** Meta object code from reading C++ file 'AirLiquidDevice.h'
**
** Created: Thu Jul 25 11:26:13 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Devices/AirLiquidDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AirLiquidDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CAirLiquidDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x08,
      54,   32,   41,   32, 0x08,
      82,   32,   41,   32, 0x08,
     109,   32,   41,   32, 0x08,
     132,   32,   32,   32, 0x08,
     150,   32,   32,   32, 0x08,
     190,  169,   41,   32, 0x08,
     222,   32,  216,   32, 0x08,
     250,  236,   41,   32, 0x08,
     304,  282,   41,   32, 0x08,
     347,  328,   41,   32, 0x08,
     394,   32,   32,   32, 0x08,
     431,  411,   41,   32, 0x08,
     534,  494,   41,   32, 0x08,
     591,  580,  216,   32, 0x08,
     631,  580,   41,   32, 0x08,
     676,   32,   41,   32, 0x08,
     722,  695,   41,   32, 0x08,
     768,  763,  758,   32, 0x08,
     800,  763,  758,   32, 0x08,
     833,  763,  758,   32, 0x08,
     874,  763,  758,   32, 0x08,
     960,  916,  758,   32, 0x08,
    1007,   32,   32,   32, 0x08,
    1025,   32,   32,   32, 0x08,
    1041,   32,   32,   32, 0x08,
    1059,   32,   32,   32, 0x08,
    1084,   32,   32,   32, 0x08,
    1101,   32,   32,   32, 0x08,
    1141, 1120,   32,   32, 0x08,
    1217, 1183,   32,   32, 0x08,
    1291, 1264,   32,   32, 0x08,
    1364, 1340,   32,   32, 0x08,
    1435, 1402,   32,   32, 0x08,
    1547, 1480,   32,   32, 0x08,
    1638, 1614,   32,   32, 0x08,
    1705, 1687,   32,   32, 0x08,
    1793, 1753,   32,   32, 0x08,
    1907, 1875,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CAirLiquidDevice[] = {
    "DeviceControl::CAirLiquidDevice\0\0"
    "Reset()\0ReturnCode_t\0HandleInitializationState()\0"
    "HandleConfigurationState()\0"
    "ConfigureDeviceTasks()\0HandleIdleState()\0"
    "HandleErrorState()\0flag,NominalPressure\0"
    "SetPressure(quint8,qreal)\0qreal\0"
    "GetPressure()\0flag,pressure\0"
    "SetTargetPressure(quint8,qreal)\0"
    "ValveIndex,ValveState\0SetValve(quint8,quint8)\0"
    "PressureCtrlStatus\0"
    "SetPressureControlStatus(PressureCtrlStatus_t)\0"
    "StopCompressor()\0Type,TempCtrlStatus\0"
    "SetTemperatureControlStatus(ALTempCtrlType_t,TempCtrlStatus_t)\0"
    "Type,NominalTemperature,SlopeTempChange\0"
    "SetTemperature(ALTempCtrlType_t,qreal,quint8)\0"
    "Type,Index\0GetTemperature(ALTempCtrlType_t,quint8)\0"
    "GetTemperatureAsync(ALTempCtrlType_t,quint8)\0"
    "GetPressureAsync()\0OutputValue,Duration,Delay\0"
    "SetDOValue(quint16,quint16,quint16)\0"
    "bool\0Type\0IsInsideRange(ALTempCtrlType_t)\0"
    "IsOutsideRange(ALTempCtrlType_t)\0"
    "IsTemperatureControlOn(ALTempCtrlType_t)\0"
    "IsTemperatureControlOff(ALTempCtrlType_t)\0"
    "TargetValue,CurrentValue,Tolerance,Num,Init\0"
    "IsPIDDataSteady(qreal,qreal,qreal,qint32,bool)\0"
    "PressureTimerCB()\0VaccumTimerCB()\0"
    "DrainingTimerCB()\0ReleasePressureTimerCB()\0"
    "SuckingTimerCB()\0CheckSensorsData()\0"
    ",ReturnCode,Pressure\0"
    "OnSetPressure(quint32,ReturnCode_t,qreal)\0"
    ",ReturnCode,ValveIndex,ValveState\0"
    "OnSetValve(quint32,ReturnCode_t,quint8,quint8)\0"
    ",ReturnCode,Index,Pressure\0"
    "OnGetPressure(quint32,ReturnCode_t,quint8,qreal)\0"
    ",ReturnCode,Temperature\0"
    "OnSetTemp(quint32,ReturnCode_t,qreal)\0"
    "InstanceID,ReturnCode,Index,Temp\0"
    "OnGetTemp(quint32,ReturnCode_t,quint8,qreal)\0"
    ",ReturnCode,MaxTemperature,ControllerGain,ResetTime,DerivativeTime\0"
    "OnSetTempPid(quint32,ReturnCode_t,quint16,quint16,quint16,quint16)\0"
    ",ReturnCode,OutputValue\0"
    "OnSetDOOutputValue(quint32,ReturnCode_t,quint16)\0"
    ",ReturnCode,State\0"
    "OnLevelSensorState(quint32,ReturnCode_t,quint8)\0"
    ",ReturnCode,TempCtrlStatus,MainsVoltage\0"
    "OnTempControlStatus(quint32,ReturnCode_t,TempCtrlStatus_t,TempCtrlMain"
    "sVoltage_t)\0"
    ",ReturnCode,LifeTime,LifeCycles\0"
    "OnGetLifeTime(quint32,ReturnCode_t,quint32,quint32)\0"
};

void DeviceControl::CAirLiquidDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CAirLiquidDevice *_t = static_cast<CAirLiquidDevice *>(_o);
        switch (_id) {
        case 0: _t->Reset(); break;
        case 1: { ReturnCode_t _r = _t->HandleInitializationState();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 2: { ReturnCode_t _r = _t->HandleConfigurationState();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 3: { ReturnCode_t _r = _t->ConfigureDeviceTasks();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 4: _t->HandleIdleState(); break;
        case 5: _t->HandleErrorState(); break;
        case 6: { ReturnCode_t _r = _t->SetPressure((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 7: { qreal _r = _t->GetPressure();
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 8: { ReturnCode_t _r = _t->SetTargetPressure((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 9: { ReturnCode_t _r = _t->SetValve((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 10: { ReturnCode_t _r = _t->SetPressureControlStatus((*reinterpret_cast< PressureCtrlStatus_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 11: _t->StopCompressor(); break;
        case 12: { ReturnCode_t _r = _t->SetTemperatureControlStatus((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 13: { ReturnCode_t _r = _t->SetTemperature((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 14: { qreal _r = _t->GetTemperature((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 15: { ReturnCode_t _r = _t->GetTemperatureAsync((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 16: { ReturnCode_t _r = _t->GetPressureAsync();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 17: { ReturnCode_t _r = _t->SetDOValue((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 18: { bool _r = _t->IsInsideRange((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 19: { bool _r = _t->IsOutsideRange((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 20: { bool _r = _t->IsTemperatureControlOn((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 21: { bool _r = _t->IsTemperatureControlOff((*reinterpret_cast< ALTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 22: { bool _r = _t->IsPIDDataSteady((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3])),(*reinterpret_cast< qint32(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 23: _t->PressureTimerCB(); break;
        case 24: _t->VaccumTimerCB(); break;
        case 25: _t->DrainingTimerCB(); break;
        case 26: _t->ReleasePressureTimerCB(); break;
        case 27: _t->SuckingTimerCB(); break;
        case 28: _t->CheckSensorsData(); break;
        case 29: _t->OnSetPressure((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 30: _t->OnSetValve((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4]))); break;
        case 31: _t->OnGetPressure((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 32: _t->OnSetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 33: _t->OnGetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 34: _t->OnSetTempPid((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< quint16(*)>(_a[5])),(*reinterpret_cast< quint16(*)>(_a[6]))); break;
        case 35: _t->OnSetDOOutputValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        case 36: _t->OnLevelSensorState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3]))); break;
        case 37: _t->OnTempControlStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[3])),(*reinterpret_cast< TempCtrlMainsVoltage_t(*)>(_a[4]))); break;
        case 38: _t->OnGetLifeTime((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CAirLiquidDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CAirLiquidDevice::staticMetaObject = {
    { &CBaseDevice::staticMetaObject, qt_meta_stringdata_DeviceControl__CAirLiquidDevice,
      qt_meta_data_DeviceControl__CAirLiquidDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CAirLiquidDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CAirLiquidDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CAirLiquidDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CAirLiquidDevice))
        return static_cast<void*>(const_cast< CAirLiquidDevice*>(this));
    return CBaseDevice::qt_metacast(_clname);
}

int DeviceControl::CAirLiquidDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CBaseDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
