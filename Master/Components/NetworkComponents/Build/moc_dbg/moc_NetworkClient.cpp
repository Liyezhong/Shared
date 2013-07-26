/****************************************************************************
** Meta object code from reading C++ file 'NetworkClient.h'
**
** Created: Thu Jul 25 11:28:06 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/NetworkClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__NetworkClient[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   28,   27,   27, 0x05,
      64,   28,   27,   27, 0x05,
      94,   88,   27,   27, 0x05,
     154,  150,   27,   27, 0x05,
     196,  187,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
     240,   27,   27,   27, 0x0a,
     256,   27,   27,   27, 0x0a,
     304,   27,   27,   27, 0x0a,
     328,  324,   27,   27, 0x0a,
     359,   27,   27,   27, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__NetworkClient[] = {
    "NetworkBase::NetworkClient\0\0name\0"
    "ConnectionEstablished(QString)\0"
    "ConnectionLost(QString)\0error\0"
    "ConnectionFailed(NetworkBase::NetworkClientErrorType_t)\0"
    "msg\0ForwardToMessageHandler(QString)\0"
    "type,msg\0ForwardToMessageHandler(quint8,QByteArray&)\0"
    "ReadRawSocket()\0"
    "HandleSocketError(QAbstractSocket::SocketError)\0"
    "HandleAuthTimeout()\0,ba\0"
    "SendMessage(quint8,QByteArray)\0"
    "ConnectToServer()\0"
};

void NetworkBase::NetworkClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkClient *_t = static_cast<NetworkClient *>(_o);
        switch (_id) {
        case 0: _t->ConnectionEstablished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->ConnectionLost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->ConnectionFailed((*reinterpret_cast< NetworkBase::NetworkClientErrorType_t(*)>(_a[1]))); break;
        case 3: _t->ForwardToMessageHandler((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->ForwardToMessageHandler((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        case 5: _t->ReadRawSocket(); break;
        case 6: _t->HandleSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 7: _t->HandleAuthTimeout(); break;
        case 8: _t->SendMessage((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 9: _t->ConnectToServer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::NetworkClient::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::NetworkClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkBase__NetworkClient,
      qt_meta_data_NetworkBase__NetworkClient, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::NetworkClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::NetworkClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::NetworkClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__NetworkClient))
        return static_cast<void*>(const_cast< NetworkClient*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkBase::NetworkClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::NetworkClient::ConnectionEstablished(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::NetworkClient::ConnectionLost(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkBase::NetworkClient::ConnectionFailed(NetworkBase::NetworkClientErrorType_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkBase::NetworkClient::ForwardToMessageHandler(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NetworkBase::NetworkClient::ForwardToMessageHandler(quint8 _t1, QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
