/****************************************************************************
** Meta object code from reading C++ file 'PendingCmdDescriptor.h'
**
** Created: Thu Jul 25 11:27:24 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Commands/PendingCmdDescriptor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PendingCmdDescriptor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Global__PendingCmdDescriptor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      39,   30,   29,   29, 0x05,

 // slots: signature, parameters, type, tag, flags
      80,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Global__PendingCmdDescriptor[] = {
    "Global::PendingCmdDescriptor\0\0Ref,Name\0"
    "TimeoutOccured(Global::tRefType,QString)\0"
    "OnTimeout()\0"
};

void Global::PendingCmdDescriptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PendingCmdDescriptor *_t = static_cast<PendingCmdDescriptor *>(_o);
        switch (_id) {
        case 0: _t->TimeoutOccured((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->OnTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Global::PendingCmdDescriptor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Global::PendingCmdDescriptor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Global__PendingCmdDescriptor,
      qt_meta_data_Global__PendingCmdDescriptor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Global::PendingCmdDescriptor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Global::PendingCmdDescriptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Global::PendingCmdDescriptor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Global__PendingCmdDescriptor))
        return static_cast<void*>(const_cast< PendingCmdDescriptor*>(this));
    return QObject::qt_metacast(_clname);
}

int Global::PendingCmdDescriptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Global::PendingCmdDescriptor::TimeoutOccured(Global::tRefType _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
