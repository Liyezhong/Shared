/****************************************************************************
** Meta object code from reading C++ file 'PeripheryDevice.h'
**
** Created: Thu Jul 25 11:26:17 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Devices/PeripheryDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PeripheryDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CPeripheryDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
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
     201,  169,   41,   32, 0x08,
     273,  249,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CPeripheryDevice[] = {
    "DeviceControl::CPeripheryDevice\0\0"
    "Reset()\0ReturnCode_t\0HandleInitializationState()\0"
    "HandleConfigurationState()\0"
    "ConfigureDeviceTasks()\0HandleIdleState()\0"
    "HandleErrorState()\0Type,OutputValue,Duration,Delay\0"
    "SetDOValue(PerDOType_t,quint16,quint16,quint16)\0"
    ",ReturnCode,OutputValue\0"
    "OnSetDOOutputValue(quint32,ReturnCode_t,quint16)\0"
};

void DeviceControl::CPeripheryDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CPeripheryDevice *_t = static_cast<CPeripheryDevice *>(_o);
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
        case 6: { ReturnCode_t _r = _t->SetDOValue((*reinterpret_cast< PerDOType_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< ReturnCode_t*>(_a[0]) = _r; }  break;
        case 7: _t->OnSetDOOutputValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CPeripheryDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CPeripheryDevice::staticMetaObject = {
    { &CBaseDevice::staticMetaObject, qt_meta_stringdata_DeviceControl__CPeripheryDevice,
      qt_meta_data_DeviceControl__CPeripheryDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CPeripheryDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CPeripheryDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CPeripheryDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CPeripheryDevice))
        return static_cast<void*>(const_cast< CPeripheryDevice*>(this));
    return CBaseDevice::qt_metacast(_clname);
}

int DeviceControl::CPeripheryDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CBaseDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
