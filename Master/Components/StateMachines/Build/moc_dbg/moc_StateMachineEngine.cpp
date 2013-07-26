/****************************************************************************
** Meta object code from reading C++ file 'StateMachineEngine.h'
**
** Created: Thu Jul 25 11:28:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/StateMachineEngine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StateMachineEngine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_StateMachines__StateMachineEngine[] = {

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
      43,   35,   34,   34, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_StateMachines__StateMachineEngine[] = {
    "StateMachines::StateMachineEngine\0\0"
    "name,et\0"
    "SetState(StateMachines::StateNameType_t,StateMachines::StateEvent)\0"
};

void StateMachines::StateMachineEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        StateMachineEngine *_t = static_cast<StateMachineEngine *>(_o);
        switch (_id) {
        case 0: _t->SetState((*reinterpret_cast< const StateMachines::StateNameType_t(*)>(_a[1])),(*reinterpret_cast< StateMachines::StateEvent(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData StateMachines::StateMachineEngine::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject StateMachines::StateMachineEngine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_StateMachines__StateMachineEngine,
      qt_meta_data_StateMachines__StateMachineEngine, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &StateMachines::StateMachineEngine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *StateMachines::StateMachineEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *StateMachines::StateMachineEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StateMachines__StateMachineEngine))
        return static_cast<void*>(const_cast< StateMachineEngine*>(this));
    return QObject::qt_metacast(_clname);
}

int StateMachines::StateMachineEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
