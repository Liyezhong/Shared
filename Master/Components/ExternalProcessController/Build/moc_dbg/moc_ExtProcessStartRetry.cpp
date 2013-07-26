/****************************************************************************
** Meta object code from reading C++ file 'ExtProcessStartRetry.h'
**
** Created: Thu Jul 25 11:26:58 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/ExternalProcessStates/ExtProcessStartRetry.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExtProcessStartRetry.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExternalProcessControl__ExtProcessStartRetry[] = {

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
      46,   45,   45,   45, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ExternalProcessControl__ExtProcessStartRetry[] = {
    "ExternalProcessControl::ExtProcessStartRetry\0"
    "\0OnWindowTimerTimeout()\0"
};

void ExternalProcessControl::ExtProcessStartRetry::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExtProcessStartRetry *_t = static_cast<ExtProcessStartRetry *>(_o);
        switch (_id) {
        case 0: _t->OnWindowTimerTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ExternalProcessControl::ExtProcessStartRetry::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExternalProcessControl::ExtProcessStartRetry::staticMetaObject = {
    { &StateMachines::State::staticMetaObject, qt_meta_stringdata_ExternalProcessControl__ExtProcessStartRetry,
      qt_meta_data_ExternalProcessControl__ExtProcessStartRetry, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExternalProcessControl::ExtProcessStartRetry::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExternalProcessControl::ExtProcessStartRetry::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExternalProcessControl::ExtProcessStartRetry::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExternalProcessControl__ExtProcessStartRetry))
        return static_cast<void*>(const_cast< ExtProcessStartRetry*>(this));
    typedef StateMachines::State QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int ExternalProcessControl::ExtProcessStartRetry::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef StateMachines::State QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
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
