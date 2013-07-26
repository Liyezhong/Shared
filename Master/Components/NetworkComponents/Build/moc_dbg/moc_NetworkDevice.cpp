/****************************************************************************
** Meta object code from reading C++ file 'NetworkDevice.h'
**
** Created: Thu Jul 25 11:28:09 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/NetworkDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__NetworkDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   28,   27,   27, 0x05,
      59,   28,   27,   27, 0x05,
      92,   88,   27,   27, 0x05,
     122,  113,   27,   27, 0x05,
     153,   27,   27,   27, 0x05,
     187,  176,   27,   27, 0x05,
     239,  233,   27,   27, 0x05,
     266,  233,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
     301,   27,  296,   27, 0x0a,
     315,   27,   27,   27, 0x0a,
     333,  331,  296,   27, 0x0a,
     381,  378,  296,   27, 0x0a,
     432,   27,  296,   27, 0x0a,
     453,  331,  296,   27, 0x0a,
     494,   27,   27,   27, 0x0a,
     520,   27,   27,   27, 0x0a,
     547,  543,   27,   27, 0x0a,
     601,  582,   27,   27, 0x0a,
     658,   27,   27,   27, 0x09,
     676,   27,   27,   27, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__NetworkDevice[] = {
    "NetworkBase::NetworkDevice\0\0name\0"
    "SigPeerConnected(QString)\0"
    "SigPeerDisconnected(QString)\0msg\0"
    "SendMessage(QString)\0type,msg\0"
    "SendMessage(quint8,QByteArray)\0"
    "MessageParsingFailed()\0Ref,status\0"
    "SigMsgSendingResult(Global::tRefType,QString)\0"
    "error\0SigHeartBeatError(QString)\0"
    "SigDateTimeSyncError(QString)\0bool\0"
    "StartDevice()\0DestroyDevice()\0,\0"
    "ProcessIncomingMessage(QString,QDomDocument)\0"
    ",,\0ProcessIncomingMessage(QString,QString,QByteArray)\0"
    "SendCommand(QString)\0"
    "SendCommand(NetMessageType_t,QByteArray)\0"
    "PeerDisconnected(QString)\0"
    "PeerConnected(QString)\0,ba\0"
    "GetIncomingMsg(quint8,QByteArray&)\0"
    "cmdname,bArray,Ref\0"
    "SendOutgoingCommand(QString,QByteArray,Global::tRefType)\0"
    "HandleHeartBeat()\0HandleHeartBeatTimeout()\0"
};

void NetworkBase::NetworkDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkDevice *_t = static_cast<NetworkDevice *>(_o);
        switch (_id) {
        case 0: _t->SigPeerConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->SigPeerDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->SendMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->SendMessage((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 4: _t->MessageParsingFailed(); break;
        case 5: _t->SigMsgSendingResult((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->SigHeartBeatError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->SigDateTimeSyncError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: { bool _r = _t->StartDevice();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: _t->DestroyDevice(); break;
        case 10: { bool _r = _t->ProcessIncomingMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QDomDocument(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = _t->ProcessIncomingMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QByteArray(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->SendCommand((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->SendCommand((*reinterpret_cast< NetMessageType_t(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: _t->PeerDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->PeerConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->GetIncomingMsg((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        case 17: _t->SendOutgoingCommand((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2])),(*reinterpret_cast< Global::tRefType(*)>(_a[3]))); break;
        case 18: _t->HandleHeartBeat(); break;
        case 19: _t->HandleHeartBeatTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::NetworkDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::NetworkDevice::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkBase__NetworkDevice,
      qt_meta_data_NetworkBase__NetworkDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::NetworkDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::NetworkDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::NetworkDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__NetworkDevice))
        return static_cast<void*>(const_cast< NetworkDevice*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkBase::NetworkDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::NetworkDevice::SigPeerConnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::NetworkDevice::SigPeerDisconnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkBase::NetworkDevice::SendMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkBase::NetworkDevice::SendMessage(quint8 _t1, const QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NetworkBase::NetworkDevice::MessageParsingFailed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void NetworkBase::NetworkDevice::SigMsgSendingResult(Global::tRefType _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NetworkBase::NetworkDevice::SigHeartBeatError(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void NetworkBase::NetworkDevice::SigDateTimeSyncError(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
