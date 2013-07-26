/****************************************************************************
** Meta object code from reading C++ file 'NetworkServer.h'
**
** Created: Thu Jul 25 11:28:10 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/NetworkServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__NetworkServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   28,   27,   27, 0x05,
      58,   28,   27,   27, 0x05,
      97,   86,   27,   27, 0x05,
     158,  152,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
     227,  207,   27,   27, 0x0a,
     303,  289,   27,   27, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__NetworkServer[] = {
    "NetworkBase::NetworkServer\0\0name\0"
    "ClientConnected(QString)\0"
    "ClientDisconnected(QString)\0dtype,name\0"
    "ConnectionError(NetworkBase::DisconnectType_t,QString)\0"
    "dtype\0DestroyConnection(NetworkBase::DisconnectType_t)\0"
    "number,client,dtype\0"
    "DestroyManager(quint32,QString,NetworkBase::DisconnectType_t)\0"
    "number,client\0RegisterConnection(quint32,QString)\0"
};

void NetworkBase::NetworkServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkServer *_t = static_cast<NetworkServer *>(_o);
        switch (_id) {
        case 0: _t->ClientConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->ClientDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->ConnectionError((*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->DestroyConnection((*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[1]))); break;
        case 4: _t->DestroyManager((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[3]))); break;
        case 5: _t->RegisterConnection((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::NetworkServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::NetworkServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_NetworkBase__NetworkServer,
      qt_meta_data_NetworkBase__NetworkServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::NetworkServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::NetworkServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::NetworkServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__NetworkServer))
        return static_cast<void*>(const_cast< NetworkServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int NetworkBase::NetworkServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::NetworkServer::ClientConnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::NetworkServer::ClientDisconnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkBase::NetworkServer::ConnectionError(NetworkBase::DisconnectType_t _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkBase::NetworkServer::DestroyConnection(NetworkBase::DisconnectType_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
