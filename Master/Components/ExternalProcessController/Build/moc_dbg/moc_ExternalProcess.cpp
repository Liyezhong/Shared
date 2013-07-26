/****************************************************************************
** Meta object code from reading C++ file 'ExternalProcess.h'
**
** Created: Thu Jul 25 11:26:53 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/ExternalProcess.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExternalProcess.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExternalProcessControl__ExternalProcess[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      55,   41,   40,   40, 0x05,
      87,   82,   40,   40, 0x05,
     117,  111,   40,   40, 0x05,

 // slots: signature, parameters, type, tag, flags
     135,   40,   40,   40, 0x08,
     148,   40,   40,   40, 0x08,
     167,  163,   40,   40, 0x08,
     220,  211,   40,   40, 0x08,
     241,   40,   40,   40, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ExternalProcessControl__ExternalProcess[] = {
    "ExternalProcessControl::ExternalProcess\0"
    "\0name,exitcode\0ProcessExited(QString,int)\0"
    "name\0ProcessStarted(QString)\0error\0"
    "ProcessError(int)\0ReadStdOut()\0"
    "ReadStdError()\0err\0"
    "ProcessErrorOccured(QProcess::ProcessError)\0"
    "exitcode\0ProcessFinished(int)\0"
    "ProcessRuns()\0"
};

void ExternalProcessControl::ExternalProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExternalProcess *_t = static_cast<ExternalProcess *>(_o);
        switch (_id) {
        case 0: _t->ProcessExited((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->ProcessStarted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->ProcessError((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->ReadStdOut(); break;
        case 4: _t->ReadStdError(); break;
        case 5: _t->ProcessErrorOccured((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 6: _t->ProcessFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->ProcessRuns(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ExternalProcessControl::ExternalProcess::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExternalProcessControl::ExternalProcess::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ExternalProcessControl__ExternalProcess,
      qt_meta_data_ExternalProcessControl__ExternalProcess, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExternalProcessControl::ExternalProcess::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExternalProcessControl::ExternalProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExternalProcessControl::ExternalProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExternalProcessControl__ExternalProcess))
        return static_cast<void*>(const_cast< ExternalProcess*>(this));
    return QObject::qt_metacast(_clname);
}

int ExternalProcessControl::ExternalProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void ExternalProcessControl::ExternalProcess::ProcessExited(const QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ExternalProcessControl::ExternalProcess::ProcessStarted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ExternalProcessControl::ExternalProcess::ProcessError(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
