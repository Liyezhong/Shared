/****************************************************************************
** Meta object code from reading C++ file 'RotaryValveDevice.h'
**
** Created: Thu Jul 25 11:26:18 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Devices/RotaryValveDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RotaryValveDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CRotaryValveDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      58,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      59,   35,   34,   34, 0x05,

 // slots: signature, parameters, type, tag, flags
     106,  100,   94,   34, 0x08,
     158,  143,  130,   34, 0x08,
     254,  204,   34,   34, 0x08,
     376,  341,  336,   34, 0x08,
     405,   34,   34,   34, 0x08,
     424,  100,  336,   34, 0x08,
     457,  452,   34,   34, 0x08,
     518,  452,   34,   34, 0x08,
     578,  575,   34,   34, 0x08,
     653,   34,  336,   34, 0x08,
     669,   34,  336,   34, 0x08,
     686,   34,  336,   34, 0x08,
     711,   34,  336,   34, 0x08,
     737,   34,  130,   34, 0x08,
     765,   34,  130,   34, 0x08,
     792,   34,  130,   34, 0x08,
     840,  809,   34,   34, 0x08,
     896,  884,   34,   34, 0x08,
     963,  934,   34,   34, 0x08,
    1012,  884,   34,   34, 0x08,
    1076, 1053,   34,   34, 0x08,
    1145, 1121,   34,   34, 0x08,
    1250, 1183,   34,   34, 0x08,
    1355, 1317,  130,   34, 0x08,
    1392,   34,  130,   34, 0x08,
    1411,   34,  130,   34, 0x08,
    1438,   34, 1431,   34, 0x08,
    1471, 1461,   34,   34, 0x08,
    1519, 1508, 1500,   34, 0x08,
    1604, 1560, 1552,   34, 0x08,
    1687, 1560, 1552,   34, 0x08,
    1778, 1770,   34,   34, 0x08,
    1805,   34, 1431,   34, 0x08,
    1833, 1826,   34,   34, 0x08,
    1860,   34, 1431,   34, 0x08,
    1881, 1826,   34,   34, 0x08,
    1908,   34, 1431,   34, 0x08,
    1935,   34, 1929,   34, 0x08,
    1974, 1961,   34,   34, 0x08,
    2011, 2005,   34,   34, 0x08,
    2037, 2005,   34,   34, 0x08,
    2063,   34, 1431,   34, 0x08,
    2083, 2005,   34,   34, 0x08,
    2109,   34, 1431,   34, 0x08,
    2129,   34, 1431,   34, 0x08,
    2149,   34, 1500,   34, 0x08,
    2183,   34, 2170,   34, 0x08,
    2208, 2199,   34,   34, 0x08,
    2236,   34,   34,   34, 0x08,
    2253,   34, 1500,   34, 0x08,
    2267,   34,   34,   34, 0x08,
    2298,   34,   34,   34, 0x08,
    2328, 2306,  130,   34, 0x08,
    2378, 2373,  130,   34, 0x08,
    2398,   34,  130,   34, 0x08,
    2421,   34,   34,   34, 0x08,
    2439,   34,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CRotaryValveDevice[] = {
    "DeviceControl::CRotaryValveDevice\0\0"
    "SensorIndex,Temperature\0"
    "ReportActTempeature(quint32,qreal)\0"
    "qreal\0Index\0GetTemperature(quint32)\0"
    "ReturnCode_t\0TempCtrlStatus\0"
    "SetTemperatureControlStatus(TempCtrlStatus_t)\0"
    "InstanceID,ReturnCode,TempCtrlStatus,MainsVoltage\0"
    "OnTempControlStatus(quint32,ReturnCode_t,TempCtrlStatus_t,TempCtrlMain"
    "sVoltage_t)\0"
    "bool\0NominalTemperature,SlopeTempChange\0"
    "SetTemperature(qreal,quint8)\0"
    "CheckSensorsData()\0GetTemperatureAsync(quint8)\0"
    ",,,,\0StepperMotorError(quint32,quint16,quint16,quint16,QDateTime)\0"
    "TempCtrlError(quint32,quint16,quint16,quint16,QDateTime)\0"
    ",,\0"
    "TempCtrlSetOperatingModeAckn(quint32,ReturnCode_t,TempCtrlOperatingMod"
    "e_t)\0"
    "IsInsideRange()\0IsOutsideRange()\0"
    "IsTemperatureControlOn()\0"
    "IsTemperatureControlOff()\0"
    "HandleInitializationState()\0"
    "HandleConfigurationState()\0DoReferenceRun()\0"
    "InstanceID,ReturnCode,Position\0"
    "OnReferenceRun(quint32,ReturnCode_t,qint32)\0"
    ",ReturnCode\0OnSetMotorState(quint32,ReturnCode_t)\0"
    ",ReturnCode,Position,PosCode\0"
    "OnGetPosition(quint32,ReturnCode_t,qint32,qint8)\0"
    "OnSetConfiguration(quint32,ReturnCode_t)\0"
    ",ReturnCode,Index,Temp\0"
    "OnGetTemp(quint32,ReturnCode_t,quint8,qreal)\0"
    ",ReturnCode,Temperature\0"
    "OnSetTemp(quint32,ReturnCode_t,qreal)\0"
    ",ReturnCode,MaxTemperature,ControllerGain,ResetTime,DerivativeTime\0"
    "OnSetTempPid(quint32,ReturnCode_t,quint16,quint16,quint16,quint16)\0"
    "changeParameter,LowerLimit,UpperLimit\0"
    "MoveToNextPort(bool,quint32,quint32)\0"
    "MoveToNextPortCW()\0MoveToNextPortCCW()\0"
    "quint8\0GetRotationDirection()\0direction\0"
    "SetRotationDirection(quint8)\0QString\0"
    "EDPosition\0TranslateFromEDPosition(quint32)\0"
    "quint32\0CurrentEDPosition,Direction,ChangeDirection\0"
    "GetUpperLimit(quint32,DeviceControl::CANFctModuleStepperMotor::Rotatio"
    "nDir_t,bool)\0"
    "GetLowerLimit(quint32,DeviceControl::CANFctModuleStepperMotor::Rotatio"
    "nDir_t,bool)\0"
    "bExists\0SetConfigLS2Exists(quint8)\0"
    "GetConfigLS2Exists()\0bValid\0"
    "SetConfigPos2Value(quint8)\0"
    "GetConfigPos2Value()\0SetConfigPos3Value(quint8)\0"
    "GetConfigPos3Value()\0qint8\0"
    "GetConfigRefRunPosition()\0refRunRefPos\0"
    "SetConfigRefRunPosition(qint8)\0bStop\0"
    "SetConfigPos1Stop(quint8)\0"
    "SetConfigPos2Stop(quint8)\0GetConfigPos2Stop()\0"
    "SetConfigPos3Stop(quint8)\0GetConfigPos3Stop()\0"
    "GetConfigPos1Stop()\0GetLimitSwitchCode()\0"
    "RVPosition_t\0GetEDPosition()\0position\0"
    "SetEDPosition(RVPosition_t)\0"
    "InitEDPosition()\0GetPosition()\0"
    "RestoreLastValidParameterSet()\0Reset()\0"
    "LowerLimit,UpperLimit\0"
    "DoReferenceRunWithStepCheck(quint32,quint32)\0"
    "flag\0SetMotorState(bool)\0"
    "ApplyNewParameterSet()\0HandleIdleState()\0"
    "HandleErrorState()\0"
};

void DeviceControl::CRotaryValveDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CRotaryValveDevice *_t = static_cast<CRotaryValveDevice *>(_o);
        switch (_id) {
        case 0: _t->ReportActTempeature((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 1: { qreal _r = _t->GetTemperature((*reinterpret_cast< quint32(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 2: { ReturnCode_t _r = _t->SetTemperatureControlStatus((*reinterpret_cast< TempCtrlStatus_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 3: _t->OnTempControlStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[3])),(*reinterpret_cast< TempCtrlMainsVoltage_t(*)>(_a[4]))); break;
        case 4: { bool _r = _t->SetTemperature((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: _t->CheckSensorsData(); break;
        case 6: { bool _r = _t->GetTemperatureAsync((*reinterpret_cast< quint8(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: _t->StepperMotorError((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5]))); break;
        case 8: _t->TempCtrlError((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5]))); break;
        case 9: _t->TempCtrlSetOperatingModeAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlOperatingMode_t(*)>(_a[3]))); break;
        case 10: { bool _r = _t->IsInsideRange();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = _t->IsOutsideRange();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->IsTemperatureControlOn();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->IsTemperatureControlOff();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { ReturnCode_t _r = _t->HandleInitializationState();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 15: { ReturnCode_t _r = _t->HandleConfigurationState();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 16: { ReturnCode_t _r = _t->DoReferenceRun();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 17: _t->OnReferenceRun((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint32(*)>(_a[3]))); break;
        case 18: _t->OnSetMotorState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 19: _t->OnGetPosition((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint32(*)>(_a[3])),(*reinterpret_cast< qint8(*)>(_a[4]))); break;
        case 20: _t->OnSetConfiguration((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 21: _t->OnGetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 22: _t->OnSetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 23: _t->OnSetTempPid((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< quint16(*)>(_a[5])),(*reinterpret_cast< quint16(*)>(_a[6]))); break;
        case 24: { ReturnCode_t _r = _t->MoveToNextPort((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 25: { ReturnCode_t _r = _t->MoveToNextPortCW();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 26: { ReturnCode_t _r = _t->MoveToNextPortCCW();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 27: { quint8 _r = _t->GetRotationDirection();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 28: _t->SetRotationDirection((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 29: { QString _r = _t->TranslateFromEDPosition((*reinterpret_cast< quint32(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 30: { quint32 _r = _t->GetUpperLimit((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< DeviceControl::CANFctModuleStepperMotor::RotationDir_t(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< quint32*>(_a[0]) = _r; }  break;
        case 31: { quint32 _r = _t->GetLowerLimit((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< DeviceControl::CANFctModuleStepperMotor::RotationDir_t(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< quint32*>(_a[0]) = _r; }  break;
        case 32: _t->SetConfigLS2Exists((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 33: { quint8 _r = _t->GetConfigLS2Exists();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 34: _t->SetConfigPos2Value((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 35: { quint8 _r = _t->GetConfigPos2Value();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 36: _t->SetConfigPos3Value((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 37: { quint8 _r = _t->GetConfigPos3Value();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 38: { qint8 _r = _t->GetConfigRefRunPosition();
            if (_a[0]) *reinterpret_cast< qint8*>(_a[0]) = _r; }  break;
        case 39: _t->SetConfigRefRunPosition((*reinterpret_cast< qint8(*)>(_a[1]))); break;
        case 40: _t->SetConfigPos1Stop((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 41: _t->SetConfigPos2Stop((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 42: { quint8 _r = _t->GetConfigPos2Stop();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 43: _t->SetConfigPos3Stop((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 44: { quint8 _r = _t->GetConfigPos3Stop();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 45: { quint8 _r = _t->GetConfigPos1Stop();
            if (_a[0]) *reinterpret_cast< quint8*>(_a[0]) = _r; }  break;
        case 46: { QString _r = _t->GetLimitSwitchCode();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 47: { RVPosition_t _r = _t->GetEDPosition();
            if (_a[0]) *reinterpret_cast< RVPosition_t*>(_a[0]) = _r; }  break;
        case 48: _t->SetEDPosition((*reinterpret_cast< RVPosition_t(*)>(_a[1]))); break;
        case 49: _t->InitEDPosition(); break;
        case 50: { QString _r = _t->GetPosition();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 51: _t->RestoreLastValidParameterSet(); break;
        case 52: _t->Reset(); break;
        case 53: { ReturnCode_t _r = _t->DoReferenceRunWithStepCheck((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 54: { ReturnCode_t _r = _t->SetMotorState((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 55: { ReturnCode_t _r = _t->ApplyNewParameterSet();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 56: _t->HandleIdleState(); break;
        case 57: _t->HandleErrorState(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CRotaryValveDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CRotaryValveDevice::staticMetaObject = {
    { &CBaseDevice::staticMetaObject, qt_meta_stringdata_DeviceControl__CRotaryValveDevice,
      qt_meta_data_DeviceControl__CRotaryValveDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CRotaryValveDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CRotaryValveDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CRotaryValveDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CRotaryValveDevice))
        return static_cast<void*>(const_cast< CRotaryValveDevice*>(this));
    return CBaseDevice::qt_metacast(_clname);
}

int DeviceControl::CRotaryValveDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CBaseDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 58)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 58;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CRotaryValveDevice::ReportActTempeature(quint32 _t1, qreal _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
