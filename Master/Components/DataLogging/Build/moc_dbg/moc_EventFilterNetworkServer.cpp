/****************************************************************************
** Meta object code from reading C++ file 'EventFilterNetworkServer.h'
**
** Created: Thu Jul 25 11:20:33 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/EventFilterNetworkServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EventFilterNetworkServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataLogging__EventFilterNetworkServer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      39,   38,   38,   38, 0x08,
      61,   38,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DataLogging__EventFilterNetworkServer[] = {
    "DataLogging::EventFilterNetworkServer\0"
    "\0AcceptNewConnection()\0ConnectionDisconnected()\0"
};

const QMetaObject DataLogging::EventFilterNetworkServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DataLogging__EventFilterNetworkServer,
      qt_meta_data_DataLogging__EventFilterNetworkServer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataLogging::EventFilterNetworkServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataLogging::EventFilterNetworkServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataLogging::EventFilterNetworkServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataLogging__EventFilterNetworkServer))
        return static_cast<void*>(const_cast< EventFilterNetworkServer*>(this));
    return QObject::qt_metacast(_clname);
}

int DataLogging::EventFilterNetworkServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: AcceptNewConnection(); break;
        case 1: ConnectionDisconnected(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
