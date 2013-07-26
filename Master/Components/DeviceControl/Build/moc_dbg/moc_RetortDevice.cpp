/****************************************************************************
** Meta object code from reading C++ file 'RetortDevice.h'
**
** Created: Thu Jul 25 11:26:17 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Devices/RetortDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RetortDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CRetortDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x08,
      51,   29,   38,   29, 0x08,
      79,   29,   38,   29, 0x08,
     106,   29,   38,   29, 0x08,
     129,   29,   29,   29, 0x08,
     147,   29,   29,   29, 0x08,
     186,  166,   38,   29, 0x08,
     289,  249,   38,   29, 0x08,
     352,  341,  335,   29, 0x08,
     392,  341,   38,   29, 0x08,
     437,   29,   38,   29, 0x08,
     485,  458,   38,   29, 0x08,
     531,  526,  521,   29, 0x08,
     563,  526,  521,   29, 0x08,
     596,  526,  521,   29, 0x08,
     637,  526,  521,   29, 0x08,
     679,   29,   29,   29, 0x08,
     722,  698,   29,   29, 0x08,
     793,  760,   29,   29, 0x08,
     905,  838,   29,   29, 0x08,
     996,  972,   29,   29, 0x08,
    1085, 1045,   29,   29, 0x08,
    1190, 1167,   29,   29, 0x08,
    1306, 1233,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CRetortDevice[] = {
    "DeviceControl::CRetortDevice\0\0Reset()\0"
    "ReturnCode_t\0HandleInitializationState()\0"
    "HandleConfigurationState()\0"
    "ConfigureDeviceTasks()\0HandleIdleState()\0"
    "HandleErrorState()\0Type,TempCtrlStatus\0"
    "SetTemperatureControlStatus(RTTempCtrlType_t,TempCtrlStatus_t)\0"
    "Type,NominalTemperature,SlopeTempChange\0"
    "SetTemperature(RTTempCtrlType_t,qreal,quint8)\0"
    "qreal\0Type,Index\0"
    "GetTemperature(RTTempCtrlType_t,quint8)\0"
    "GetTemperatureAsync(RTTempCtrlType_t,quint8)\0"
    "GetLockStatusAsync()\0OutputValue,Duration,Delay\0"
    "SetDOValue(quint16,quint16,quint16)\0"
    "bool\0Type\0IsInsideRange(RTTempCtrlType_t)\0"
    "IsOutsideRange(RTTempCtrlType_t)\0"
    "IsTemperatureControlOn(RTTempCtrlType_t)\0"
    "IsTemperatureControlOff(RTTempCtrlType_t)\0"
    "CheckSensorsData()\0,ReturnCode,Temperature\0"
    "OnSetTemp(quint32,ReturnCode_t,qreal)\0"
    "InstanceID,ReturnCode,Index,Temp\0"
    "OnGetTemp(quint32,ReturnCode_t,quint8,qreal)\0"
    ",ReturnCode,MaxTemperature,ControllerGain,ResetTime,DerivativeTime\0"
    "OnSetTempPid(quint32,ReturnCode_t,quint16,quint16,quint16,quint16)\0"
    ",ReturnCode,OutputValue\0"
    "OnSetDOOutputValue(quint32,ReturnCode_t,quint16)\0"
    ",ReturnCode,TempCtrlStatus,MainsVoltage\0"
    "OnTempControlStatus(quint32,ReturnCode_t,TempCtrlStatus_t,TempCtrlMain"
    "sVoltage_t)\0"
    ",ReturnCode,InputValue\0"
    "OnGetDIValue(quint32,ReturnCode_t,quint16)\0"
    "InstanceID,ReturnCode,Sensors,Fans,Heaters,Pids,Current,HeaterSwitchTy"
    "pe\0"
    "OnGetHardwareStatus(quint32,ReturnCode_t,quint8,quint8,quint8,quint8,q"
    "uint16,quint8)\0"
};

void DeviceControl::CRetortDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CRetortDevice *_t = static_cast<CRetortDevice *>(_o);
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
        case 6: { ReturnCode_t _r = _t->SetTemperatureControlStatus((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 7: { ReturnCode_t _r = _t->SetTemperature((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 8: { qreal _r = _t->GetTemperature((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 9: { ReturnCode_t _r = _t->GetTemperatureAsync((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 10: { ReturnCode_t _r = _t->GetLockStatusAsync();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 11: { ReturnCode_t _r = _t->SetDOValue((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->IsInsideRange((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->IsOutsideRange((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { bool _r = _t->IsTemperatureControlOn((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: { bool _r = _t->IsTemperatureControlOff((*reinterpret_cast< RTTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: _t->CheckSensorsData(); break;
        case 17: _t->OnSetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 18: _t->OnGetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 19: _t->OnSetTempPid((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< quint16(*)>(_a[5])),(*reinterpret_cast< quint16(*)>(_a[6]))); break;
        case 20: _t->OnSetDOOutputValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        case 21: _t->OnTempControlStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[3])),(*reinterpret_cast< TempCtrlMainsVoltage_t(*)>(_a[4]))); break;
        case 22: _t->OnGetDIValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        case 23: _t->OnGetHardwareStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint16(*)>(_a[7])),(*reinterpret_cast< quint8(*)>(_a[8]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CRetortDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CRetortDevice::staticMetaObject = {
    { &CBaseDevice::staticMetaObject, qt_meta_stringdata_DeviceControl__CRetortDevice,
      qt_meta_data_DeviceControl__CRetortDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CRetortDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CRetortDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CRetortDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CRetortDevice))
        return static_cast<void*>(const_cast< CRetortDevice*>(this));
    return CBaseDevice::qt_metacast(_clname);
}

int DeviceControl::CRetortDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CBaseDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
