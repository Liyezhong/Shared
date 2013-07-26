/****************************************************************************
** Meta object code from reading C++ file 'NetworkClientDevice.h'
**
** Created: Thu Jul 25 11:28:08 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/NetworkClientDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkClientDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__NetworkClientDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      37,   34,   33,   33, 0x05,
      76,   63,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
     145,  128,  123,   33, 0x0a,
     196,   33,   33,   33, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__NetworkClientDevice[] = {
    "NetworkBase::NetworkClientDevice\0\0dt\0"
    "SigDateAndTime(QDateTime)\0name,payload\0"
    "ForwardMessageToUpperLayer(QString,QByteArray)\0"
    "bool\0cmdname,ref,bArr\0"
    "ProcessIncomingMessage(QString,QString,QByteArray)\0"
    "HandleNetworkError(NetworkBase::NetworkClientErrorType_t)\0"
};

void NetworkBase::NetworkClientDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkClientDevice *_t = static_cast<NetworkClientDevice *>(_o);
        switch (_id) {
        case 0: _t->SigDateAndTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 1: _t->ForwardMessageToUpperLayer((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 2: { bool _r = _t->ProcessIncomingMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QByteArray(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->HandleNetworkError((*reinterpret_cast< NetworkBase::NetworkClientErrorType_t(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::NetworkClientDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::NetworkClientDevice::staticMetaObject = {
    { &NetworkDevice::staticMetaObject, qt_meta_stringdata_NetworkBase__NetworkClientDevice,
      qt_meta_data_NetworkBase__NetworkClientDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::NetworkClientDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::NetworkClientDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::NetworkClientDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__NetworkClientDevice))
        return static_cast<void*>(const_cast< NetworkClientDevice*>(this));
    return NetworkDevice::qt_metacast(_clname);
}

int NetworkBase::NetworkClientDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NetworkDevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::NetworkClientDevice::SigDateAndTime(const QDateTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::NetworkClientDevice::ForwardMessageToUpperLayer(const QString & _t1, const QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
