/****************************************************************************
** Meta object code from reading C++ file 'RemoteCareHandler.h'
**
** Created: Thu Jul 25 11:26:32 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/RemoteCareHandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteCareHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EventHandler__RemoteCareHandler[] = {

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
      33,   32,   32,   32, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EventHandler__RemoteCareHandler[] = {
    "EventHandler::RemoteCareHandler\0\0"
    "ForwardEventToRemoteCare(DataLogging::DayEventEntry)\0"
};

void EventHandler::RemoteCareHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RemoteCareHandler *_t = static_cast<RemoteCareHandler *>(_o);
        switch (_id) {
        case 0: _t->ForwardEventToRemoteCare((*reinterpret_cast< const DataLogging::DayEventEntry(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EventHandler::RemoteCareHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventHandler::RemoteCareHandler::staticMetaObject = {
    { &Threads::ThreadController::staticMetaObject, qt_meta_stringdata_EventHandler__RemoteCareHandler,
      qt_meta_data_EventHandler__RemoteCareHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventHandler::RemoteCareHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventHandler::RemoteCareHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventHandler::RemoteCareHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventHandler__RemoteCareHandler))
        return static_cast<void*>(const_cast< RemoteCareHandler*>(this));
    typedef Threads::ThreadController QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int EventHandler::RemoteCareHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Threads::ThreadController QMocSuperClass;
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
