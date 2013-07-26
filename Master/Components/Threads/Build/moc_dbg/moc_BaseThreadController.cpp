/****************************************************************************
** Meta object code from reading C++ file 'BaseThreadController.h'
**
** Created: Thu Jul 25 11:28:37 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/BaseThreadController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BaseThreadController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Threads__BaseThreadController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      50,   31,   30,   30, 0x05,

 // slots: signature, parameters, type, tag, flags
      87,   30,   30,   30, 0x08,
     116,  104,   30,   30, 0x08,
     163,   30,   30,   30, 0x0a,
     168,   30,   30,   30, 0x0a,
     183,  175,   30,   30, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Threads__BaseThreadController[] = {
    "Threads::BaseThreadController\0\0"
    "TheHeartBeatSource\0"
    "HeartbeatSignal(Global::gSourceType)\0"
    "HeartbeatTimer()\0Ref,CmdName\0"
    "OnProcessTimeoutSlot(Global::tRefType,QString)\0"
    "Go()\0Stop()\0Timeout\0SetHeartbeatTimeout(int)\0"
};

void Threads::BaseThreadController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BaseThreadController *_t = static_cast<BaseThreadController *>(_o);
        switch (_id) {
        case 0: _t->HeartbeatSignal((*reinterpret_cast< const Global::gSourceType(*)>(_a[1]))); break;
        case 1: _t->HeartbeatTimer(); break;
        case 2: _t->OnProcessTimeoutSlot((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->Go(); break;
        case 4: _t->Stop(); break;
        case 5: _t->SetHeartbeatTimeout((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Threads::BaseThreadController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Threads::BaseThreadController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Threads__BaseThreadController,
      qt_meta_data_Threads__BaseThreadController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Threads::BaseThreadController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Threads::BaseThreadController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Threads::BaseThreadController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Threads__BaseThreadController))
        return static_cast<void*>(const_cast< BaseThreadController*>(this));
    return QObject::qt_metacast(_clname);
}

int Threads::BaseThreadController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Threads::BaseThreadController::HeartbeatSignal(const Global::gSourceType & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
