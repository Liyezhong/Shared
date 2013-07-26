/****************************************************************************
** Meta object code from reading C++ file 'ProtocolTxCommand.h'
**
** Created: Thu Jul 25 11:28:15 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/ProtocolTxCommand.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProtocolTxCommand.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__ProtocolTxCommand[] = {

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
      32,   31,   31,   31, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__ProtocolTxCommand[] = {
    "NetworkBase::ProtocolTxCommand\0\0"
    "HandleAckTimeout()\0"
};

void NetworkBase::ProtocolTxCommand::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProtocolTxCommand *_t = static_cast<ProtocolTxCommand *>(_o);
        switch (_id) {
        case 0: _t->HandleAckTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData NetworkBase::ProtocolTxCommand::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::ProtocolTxCommand::staticMetaObject = {
    { &ProtocolCommand::staticMetaObject, qt_meta_stringdata_NetworkBase__ProtocolTxCommand,
      qt_meta_data_NetworkBase__ProtocolTxCommand, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::ProtocolTxCommand::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::ProtocolTxCommand::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::ProtocolTxCommand::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__ProtocolTxCommand))
        return static_cast<void*>(const_cast< ProtocolTxCommand*>(this));
    return ProtocolCommand::qt_metacast(_clname);
}

int NetworkBase::ProtocolTxCommand::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ProtocolCommand::qt_metacall(_c, _id, _a);
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
