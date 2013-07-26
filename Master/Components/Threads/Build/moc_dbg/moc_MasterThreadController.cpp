/****************************************************************************
** Meta object code from reading C++ file 'MasterThreadController.h'
**
** Created: Thu Jul 25 11:28:39 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/MasterThreadController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MasterThreadController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Threads__MasterThreadController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,
      42,   32,   32,   32, 0x05,
      65,   32,   32,   32, 0x05,
      76,   32,   32,   32, 0x05,
      92,   32,   32,   32, 0x05,
     121,   32,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
     143,   32,   32,   32, 0x08,
     160,   32,   32,   32, 0x08,
     206,  187,   32,   32, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Threads__MasterThreadController[] = {
    "Threads::MasterThreadController\0\0"
    "SendGo()\0SendGoToBasicThreads()\0"
    "SendStop()\0RequestFinish()\0"
    "CreateExportProcess(QString)\0"
    "CheckLoggingEnabled()\0HeartbeatCheck()\0"
    "ExternalMemShutdownCheck()\0"
    "TheHeartBeatSource\0"
    "HeartbeatSlot(Global::gSourceType)\0"
};

void Threads::MasterThreadController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MasterThreadController *_t = static_cast<MasterThreadController *>(_o);
        switch (_id) {
        case 0: _t->SendGo(); break;
        case 1: _t->SendGoToBasicThreads(); break;
        case 2: _t->SendStop(); break;
        case 3: _t->RequestFinish(); break;
        case 4: _t->CreateExportProcess((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->CheckLoggingEnabled(); break;
        case 6: _t->HeartbeatCheck(); break;
        case 7: _t->ExternalMemShutdownCheck(); break;
        case 8: _t->HeartbeatSlot((*reinterpret_cast< const Global::gSourceType(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Threads::MasterThreadController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Threads::MasterThreadController::staticMetaObject = {
    { &BaseThreadController::staticMetaObject, qt_meta_stringdata_Threads__MasterThreadController,
      qt_meta_data_Threads__MasterThreadController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Threads::MasterThreadController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Threads::MasterThreadController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Threads::MasterThreadController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Threads__MasterThreadController))
        return static_cast<void*>(const_cast< MasterThreadController*>(this));
    return BaseThreadController::qt_metacast(_clname);
}

int Threads::MasterThreadController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseThreadController::qt_metacall(_c, _id, _a);
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
void Threads::MasterThreadController::SendGo()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Threads::MasterThreadController::SendGoToBasicThreads()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Threads::MasterThreadController::SendStop()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Threads::MasterThreadController::RequestFinish()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Threads::MasterThreadController::CreateExportProcess(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Threads::MasterThreadController::CheckLoggingEnabled()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
