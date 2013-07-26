/****************************************************************************
** Meta object code from reading C++ file 'ConnectionManager.h'
**
** Created: Thu Jul 25 11:28:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/ConnectionManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ConnectionManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__ConnectionManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      50,   32,   31,   31, 0x05,
     119,  107,   31,   31, 0x05,
     161,  152,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
     210,  193,   31,   31, 0x0a,
     235,   31,   31,   31, 0x0a,
     254,  248,   31,   31, 0x0a,
     311,  303,   31,   31, 0x0a,
     345,   31,   31,   31, 0x0a,
     370,   31,   31,   31, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__ConnectionManager[] = {
    "NetworkBase::ConnectionManager\0\0"
    "number,name,dtype\0"
    "DestroyMe(quint32,QString,NetworkBase::DisconnectType_t)\0"
    "number,name\0ClientConnected(quint32,QString)\0"
    "type,msg\0MsgReceived(quint8,QByteArray&)\0"
    "socketDescriptor\0HandleNewConnection(int)\0"
    "ReadSocket()\0dtype\0"
    "DestroyConnection(NetworkBase::DisconnectType_t)\0"
    "type,ba\0ForwardMessage(quint8,QByteArray)\0"
    "HandleSocketDisconnect()\0HandleTimeout()\0"
};

void NetworkBase::ConnectionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ConnectionManager *_t = static_cast<ConnectionManager *>(_o);
        switch (_id) {
        case 0: _t->DestroyMe((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[3]))); break;
        case 1: _t->ClientConnected((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->MsgReceived((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        case 3: _t->HandleNewConnection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->ReadSocket(); break;
        case 5: _t->DestroyConnection((*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[1]))); break;
        case 6: _t->ForwardMessage((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 7: _t->HandleSocketDisconnect(); break;
        case 8: _t->HandleTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::ConnectionManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::ConnectionManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkBase__ConnectionManager,
      qt_meta_data_NetworkBase__ConnectionManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::ConnectionManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::ConnectionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::ConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__ConnectionManager))
        return static_cast<void*>(const_cast< ConnectionManager*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkBase::ConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::ConnectionManager::DestroyMe(quint32 _t1, const QString & _t2, NetworkBase::DisconnectType_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::ConnectionManager::ClientConnected(quint32 _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkBase::ConnectionManager::MsgReceived(quint8 _t1, QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
