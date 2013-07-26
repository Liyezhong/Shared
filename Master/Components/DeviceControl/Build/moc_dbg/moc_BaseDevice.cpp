/****************************************************************************
** Meta object code from reading C++ file 'BaseDevice.h'
**
** Created: Thu Jul 25 11:26:14 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Devices/BaseDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BaseDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CBaseDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      80,   28,   27,   27, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CBaseDevice[] = {
    "DeviceControl::CBaseDevice\0\0"
    "InstanceID,ErrorGroup,ErrorCode,ErrorData,ErrorTime\0"
    "OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime)\0"
};

void DeviceControl::CBaseDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CBaseDevice *_t = static_cast<CBaseDevice *>(_o);
        switch (_id) {
        case 0: _t->OnFunctionModuleError((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CBaseDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CBaseDevice::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DeviceControl__CBaseDevice,
      qt_meta_data_DeviceControl__CBaseDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CBaseDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CBaseDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CBaseDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CBaseDevice))
        return static_cast<void*>(const_cast< CBaseDevice*>(this));
    return QObject::qt_metacast(_clname);
}

int DeviceControl::CBaseDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
