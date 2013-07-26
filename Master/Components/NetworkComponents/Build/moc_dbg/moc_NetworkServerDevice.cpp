/****************************************************************************
** Meta object code from reading C++ file 'NetworkServerDevice.h'
**
** Created: Thu Jul 25 11:28:11 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/NetworkServerDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkServerDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__NetworkServerDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      36,   34,   33,   33, 0x0a,
      94,   33,   33,   33, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__NetworkServerDevice[] = {
    "NetworkBase::NetworkServerDevice\0\0,\0"
    "HandleNetworkError(NetworkBase::DisconnectType_t,QString)\0"
    "SyncDateAndTime()\0"
};

void NetworkBase::NetworkServerDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkServerDevice *_t = static_cast<NetworkServerDevice *>(_o);
        switch (_id) {
        case 0: _t->HandleNetworkError((*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->SyncDateAndTime(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::NetworkServerDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::NetworkServerDevice::staticMetaObject = {
    { &NetworkDevice::staticMetaObject, qt_meta_stringdata_NetworkBase__NetworkServerDevice,
      qt_meta_data_NetworkBase__NetworkServerDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::NetworkServerDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::NetworkServerDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::NetworkServerDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__NetworkServerDevice))
        return static_cast<void*>(const_cast< NetworkServerDevice*>(this));
    return NetworkDevice::qt_metacast(_clname);
}

int NetworkBase::NetworkServerDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NetworkDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
