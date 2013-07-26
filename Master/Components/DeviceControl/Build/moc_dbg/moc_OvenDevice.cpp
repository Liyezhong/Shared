/****************************************************************************
** Meta object code from reading C++ file 'OvenDevice.h'
**
** Created: Thu Jul 25 11:26:16 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Devices/OvenDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OvenDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__COvenDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      28,   27,   27,   27, 0x08,
      49,   27,   36,   27, 0x08,
      77,   27,   36,   27, 0x08,
     104,   27,   36,   27, 0x08,
     127,   27,   27,   27, 0x08,
     145,   27,   27,   27, 0x08,
     184,  164,   36,   27, 0x08,
     289,  249,   36,   27, 0x08,
     354,  343,  337,   27, 0x08,
     396,  343,   36,   27, 0x08,
     451,   27,  443,   27, 0x08,
     466,   27,   36,   27, 0x08,
     496,  491,  486,   27, 0x08,
     530,  491,  486,   27, 0x08,
     565,  491,  486,   27, 0x08,
     608,  491,  486,   27, 0x08,
     652,   27,   27,   27, 0x08,
     695,  671,   27,   27, 0x08,
     766,  733,   27,   27, 0x08,
     878,  811,   27,   27, 0x08,
     985,  945,   27,   27, 0x08,
    1090, 1067,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__COvenDevice[] = {
    "DeviceControl::COvenDevice\0\0Reset()\0"
    "ReturnCode_t\0HandleInitializationState()\0"
    "HandleConfigurationState()\0"
    "ConfigureDeviceTasks()\0HandleIdleState()\0"
    "HandleErrorState()\0Type,TempCtrlStatus\0"
    "SetTemperatureControlStatus(OVENTempCtrlType_t,TempCtrlStatus_t)\0"
    "Type,NominalTemperature,SlopeTempChange\0"
    "SetTemperature(OVENTempCtrlType_t,qreal,quint8)\0"
    "qreal\0Type,Index\0"
    "GetTemperature(OVENTempCtrlType_t,quint8)\0"
    "GetTemperatureAsync(OVENTempCtrlType_t,quint8)\0"
    "quint16\0GetLidStatus()\0GetLidStatusAsync()\0"
    "bool\0Type\0IsInsideRange(OVENTempCtrlType_t)\0"
    "IsOutsideRange(OVENTempCtrlType_t)\0"
    "IsTemperatureControlOn(OVENTempCtrlType_t)\0"
    "IsTemperatureControlOff(OVENTempCtrlType_t)\0"
    "CheckSensorsData()\0,ReturnCode,Temperature\0"
    "OnSetTemp(quint32,ReturnCode_t,qreal)\0"
    "InstanceID,ReturnCode,Index,Temp\0"
    "OnGetTemp(quint32,ReturnCode_t,quint8,qreal)\0"
    ",ReturnCode,MaxTemperature,ControllerGain,ResetTime,DerivativeTime\0"
    "OnSetTempPid(quint32,ReturnCode_t,quint16,quint16,quint16,quint16)\0"
    ",ReturnCode,TempCtrlStatus,MainsVoltage\0"
    "OnTempControlStatus(quint32,ReturnCode_t,TempCtrlStatus_t,TempCtrlMain"
    "sVoltage_t)\0"
    ",ReturnCode,InputValue\0"
    "OnGetDIValue(quint32,ReturnCode_t,quint16)\0"
};

void DeviceControl::COvenDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        COvenDevice *_t = static_cast<COvenDevice *>(_o);
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
        case 6: { ReturnCode_t _r = _t->SetTemperatureControlStatus((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 7: { ReturnCode_t _r = _t->SetTemperature((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 8: { qreal _r = _t->GetTemperature((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 9: { ReturnCode_t _r = _t->GetTemperatureAsync((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 10: { quint16 _r = _t->GetLidStatus();
            if (_a[0]) *reinterpret_cast< quint16*>(_a[0]) = _r; }  break;
        case 11: { ReturnCode_t _r = _t->GetLidStatusAsync();
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->IsInsideRange((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->IsOutsideRange((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { bool _r = _t->IsTemperatureControlOn((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: { bool _r = _t->IsTemperatureControlOff((*reinterpret_cast< OVENTempCtrlType_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: _t->CheckSensorsData(); break;
        case 17: _t->OnSetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 18: _t->OnGetTemp((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 19: _t->OnSetTempPid((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< quint16(*)>(_a[5])),(*reinterpret_cast< quint16(*)>(_a[6]))); break;
        case 20: _t->OnTempControlStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[3])),(*reinterpret_cast< TempCtrlMainsVoltage_t(*)>(_a[4]))); break;
        case 21: _t->OnGetDIValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::COvenDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::COvenDevice::staticMetaObject = {
    { &CBaseDevice::staticMetaObject, qt_meta_stringdata_DeviceControl__COvenDevice,
      qt_meta_data_DeviceControl__COvenDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::COvenDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::COvenDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::COvenDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__COvenDevice))
        return static_cast<void*>(const_cast< COvenDevice*>(this));
    return CBaseDevice::qt_metacast(_clname);
}

int DeviceControl::COvenDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CBaseDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
