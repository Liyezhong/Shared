/****************************************************************************
** Meta object code from reading C++ file 'CommandChannel.h'
**
** Created: Tue Mar 12 09:18:57 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/CommandChannel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommandChannel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Threads__CommandChannel[] = {

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
      33,   25,   24,   24, 0x05,
     106,   98,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
     164,   98,   24,   24, 0x0a,
     222,   25,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Threads__CommandChannel[] = {
    "Threads::CommandChannel\0\0Ref,Ack\0"
    "CommandChannelRxAck(Global::tRefType,Global::AcknowledgeShPtr_t)\0"
    "Ref,Cmd\0"
    "CommandChannelTx(Global::tRefType,Global::CommandShPtr_t)\0"
    "CommandChannelRx(Global::tRefType,Global::CommandShPtr_t)\0"
    "CommandChannelTxAck(Global::tRefType,Global::AcknowledgeShPtr_t)\0"
};

void Threads::CommandChannel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CommandChannel *_t = static_cast<CommandChannel *>(_o);
        switch (_id) {
        case 0: _t->CommandChannelRxAck((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< const Global::AcknowledgeShPtr_t(*)>(_a[2]))); break;
        case 1: _t->CommandChannelTx((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< const Global::CommandShPtr_t(*)>(_a[2]))); break;
        case 2: _t->CommandChannelRx((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< const Global::CommandShPtr_t(*)>(_a[2]))); break;
        case 3: _t->CommandChannelTxAck((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< const Global::AcknowledgeShPtr_t(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Threads::CommandChannel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Threads::CommandChannel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Threads__CommandChannel,
      qt_meta_data_Threads__CommandChannel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Threads::CommandChannel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Threads::CommandChannel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Threads::CommandChannel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Threads__CommandChannel))
        return static_cast<void*>(const_cast< CommandChannel*>(this));
    return QObject::qt_metacast(_clname);
}

int Threads::CommandChannel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Threads::CommandChannel::CommandChannelRxAck(Global::tRefType _t1, const Global::AcknowledgeShPtr_t & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Threads::CommandChannel::CommandChannelTx(Global::tRefType _t1, const Global::CommandShPtr_t & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
