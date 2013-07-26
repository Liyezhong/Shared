/****************************************************************************
** Meta object code from reading C++ file 'ExternalProcessController.h'
**
** Created: Thu Jul 25 11:26:54 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/ExternalProcessController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExternalProcessController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExternalProcessControl__ExternalProcessController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      53,   51,   50,   50, 0x05,

 // slots: signature, parameters, type, tag, flags
      84,   80,   50,   50, 0x0a,
     118,   80,   50,   50, 0x0a,
     155,   50,   50,   50, 0x0a,
     170,   51,   50,   50, 0x0a,
     205,   50,   50,   50, 0x0a,
     237,   50,   50,   50, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ExternalProcessControl__ExternalProcessController[] = {
    "ExternalProcessControl::ExternalProcessController\0"
    "\0,\0ProcessExited(QString,int)\0str\0"
    "ExternalProcessConnected(QString)\0"
    "ExternalProcessDisconnected(QString)\0"
    "LoginTimeout()\0ExternalProcessExited(QString,int)\0"
    "ExternalProcessStarted(QString)\0"
    "ExternalProcessError(int)\0"
};

void ExternalProcessControl::ExternalProcessController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExternalProcessController *_t = static_cast<ExternalProcessController *>(_o);
        switch (_id) {
        case 0: _t->ProcessExited((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->ExternalProcessConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->ExternalProcessDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->LoginTimeout(); break;
        case 4: _t->ExternalProcessExited((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->ExternalProcessStarted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->ExternalProcessError((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ExternalProcessControl::ExternalProcessController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExternalProcessControl::ExternalProcessController::staticMetaObject = {
    { &Threads::ThreadController::staticMetaObject, qt_meta_stringdata_ExternalProcessControl__ExternalProcessController,
      qt_meta_data_ExternalProcessControl__ExternalProcessController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExternalProcessControl::ExternalProcessController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExternalProcessControl::ExternalProcessController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExternalProcessControl::ExternalProcessController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExternalProcessControl__ExternalProcessController))
        return static_cast<void*>(const_cast< ExternalProcessController*>(this));
    typedef Threads::ThreadController QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int ExternalProcessControl::ExternalProcessController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Threads::ThreadController QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ExternalProcessControl::ExternalProcessController::ProcessExited(const QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
