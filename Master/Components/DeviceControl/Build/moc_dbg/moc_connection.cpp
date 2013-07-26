/****************************************************************************
** Meta object code from reading C++ file 'connection.h'
**
** Created: Thu Jul 25 11:26:01 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/CanCommunication/CanTcpCommunication/connection.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__Connection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,
      96,   41,   26,   26, 0x05,

 // slots: signature, parameters, type, tag, flags
     182,   26,   26,   26, 0x0a,
     200,   26,   26,   26, 0x08,
     219,   26,   26,   26, 0x08,
     230,   26,   26,   26, 0x08,
     252,   26,   26,   26, 0x08,
     300,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__Connection[] = {
    "DeviceControl::Connection\0\0readyForUse()\0"
    "ID,data0,data1,data2,data3,data4,data5,data6,data7,dlc\0"
    "newCANMessage(quint32,quint8,quint8,quint8,quint8,quint8,quint8,quint8"
    ",quint8,quint8)\0"
    "connectToServer()\0processReadyRead()\0"
    "sendPing()\0sendGreetingMessage()\0"
    "handleSocketError(QAbstractSocket::SocketError)\0"
    "disconnectFromServer()\0"
};

void DeviceControl::Connection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Connection *_t = static_cast<Connection *>(_o);
        switch (_id) {
        case 0: _t->readyForUse(); break;
        case 1: _t->newCANMessage((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint8(*)>(_a[7])),(*reinterpret_cast< quint8(*)>(_a[8])),(*reinterpret_cast< quint8(*)>(_a[9])),(*reinterpret_cast< quint8(*)>(_a[10]))); break;
        case 2: _t->connectToServer(); break;
        case 3: _t->processReadyRead(); break;
        case 4: _t->sendPing(); break;
        case 5: _t->sendGreetingMessage(); break;
        case 6: _t->handleSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 7: _t->disconnectFromServer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::Connection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::Connection::staticMetaObject = {
    { &QTcpSocket::staticMetaObject, qt_meta_stringdata_DeviceControl__Connection,
      qt_meta_data_DeviceControl__Connection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::Connection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::Connection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::Connection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__Connection))
        return static_cast<void*>(const_cast< Connection*>(this));
    return QTcpSocket::qt_metacast(_clname);
}

int DeviceControl::Connection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::Connection::readyForUse()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void DeviceControl::Connection::newCANMessage(quint32 _t1, quint8 _t2, quint8 _t3, quint8 _t4, quint8 _t5, quint8 _t6, quint8 _t7, quint8 _t8, quint8 _t9, quint8 _t10)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)), const_cast<void*>(reinterpret_cast<const void*>(&_t9)), const_cast<void*>(reinterpret_cast<const void*>(&_t10)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
