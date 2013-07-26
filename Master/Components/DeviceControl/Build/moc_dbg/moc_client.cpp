/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created: Thu Jul 25 11:26:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/CanCommunication/CanTcpCommunication/client.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__Can2TcpClient[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      43,   30,   29,   29, 0x05,
     113,   71,   29,   29, 0x05,
     204,  199,   29,   29, 0x05,

 // slots: signature, parameters, type, tag, flags
     240,  228,   29,   29, 0x08,
     286,   29,   29,   29, 0x08,
     301,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__Can2TcpClient[] = {
    "DeviceControl::Can2TcpClient\0\0"
    "From,Message\0newMessage(QString,QString)\0"
    "CANID,DB0,DB1,DB2,DB3,DB4,DB5,DB6,DB7,DLC\0"
    "newCANMessage(quint32,quint8,quint8,quint8,quint8,quint8,quint8,quint8"
    ",quint8,quint8)\0"
    "nick\0newParticipant(QString)\0socketError\0"
    "connectionError(QAbstractSocket::SocketError)\0"
    "disconnected()\0readyForUse()\0"
};

void DeviceControl::Can2TcpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Can2TcpClient *_t = static_cast<Can2TcpClient *>(_o);
        switch (_id) {
        case 0: _t->newMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->newCANMessage((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint8(*)>(_a[7])),(*reinterpret_cast< quint8(*)>(_a[8])),(*reinterpret_cast< quint8(*)>(_a[9])),(*reinterpret_cast< quint8(*)>(_a[10]))); break;
        case 2: _t->newParticipant((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->connectionError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 4: _t->disconnected(); break;
        case 5: _t->readyForUse(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::Can2TcpClient::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::Can2TcpClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DeviceControl__Can2TcpClient,
      qt_meta_data_DeviceControl__Can2TcpClient, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::Can2TcpClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::Can2TcpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::Can2TcpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__Can2TcpClient))
        return static_cast<void*>(const_cast< Can2TcpClient*>(this));
    return QObject::qt_metacast(_clname);
}

int DeviceControl::Can2TcpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void DeviceControl::Can2TcpClient::newMessage(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::Can2TcpClient::newCANMessage(quint32 _t1, quint8 _t2, quint8 _t3, quint8 _t4, quint8 _t5, quint8 _t6, quint8 _t7, quint8 _t8, quint8 _t9, quint8 _t10)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)), const_cast<void*>(reinterpret_cast<const void*>(&_t9)), const_cast<void*>(reinterpret_cast<const void*>(&_t10)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::Can2TcpClient::newParticipant(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
