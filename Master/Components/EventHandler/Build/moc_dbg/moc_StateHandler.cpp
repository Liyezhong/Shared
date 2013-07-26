/****************************************************************************
** Meta object code from reading C++ file 'StateHandler.h'
**
** Created: Thu Jul 25 11:26:33 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/StateHandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StateHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EventHandler__StateHandler[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   27,   27,   27, 0x05,
      46,   27,   27,   27, 0x05,
      65,   27,   27,   27, 0x05,
      82,   27,   27,   27, 0x05,
      99,   27,   27,   27, 0x05,
     119,   27,   27,   27, 0x05,
     134,   27,   27,   27, 0x05,
     159,   27,   27,   27, 0x05,
     181,   27,   27,   27, 0x05,
     200,   27,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
     232,   27,   27,   27, 0x08,
     261,   27,   27,   27, 0x08,
     287,   27,   27,   27, 0x08,
     308,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EventHandler__StateHandler[] = {
    "EventHandler::StateHandler\0\0"
    "enterErrorState()\0enterNormalState()\0"
    "enterIdleState()\0enterBusyState()\0"
    "softSwitchPressed()\0initComplete()\0"
    "softSwitchMonitorStart()\0stateChanged(QString)\0"
    "enteredInitState()\0enteredSoftSwitchMonitorState()\0"
    "onAvailabilityStateChanged()\0"
    "onOperationStateChanged()\0"
    "onInitStateEntered()\0"
    "onSoftSwitchMonitorStateEntered()\0"
};

void EventHandler::StateHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        StateHandler *_t = static_cast<StateHandler *>(_o);
        switch (_id) {
        case 0: _t->enterErrorState(); break;
        case 1: _t->enterNormalState(); break;
        case 2: _t->enterIdleState(); break;
        case 3: _t->enterBusyState(); break;
        case 4: _t->softSwitchPressed(); break;
        case 5: _t->initComplete(); break;
        case 6: _t->softSwitchMonitorStart(); break;
        case 7: _t->stateChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->enteredInitState(); break;
        case 9: _t->enteredSoftSwitchMonitorState(); break;
        case 10: _t->onAvailabilityStateChanged(); break;
        case 11: _t->onOperationStateChanged(); break;
        case 12: _t->onInitStateEntered(); break;
        case 13: _t->onSoftSwitchMonitorStateEntered(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EventHandler::StateHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventHandler::StateHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EventHandler__StateHandler,
      qt_meta_data_EventHandler__StateHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventHandler::StateHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventHandler::StateHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventHandler::StateHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventHandler__StateHandler))
        return static_cast<void*>(const_cast< StateHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int EventHandler::StateHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void EventHandler::StateHandler::enterErrorState()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void EventHandler::StateHandler::enterNormalState()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void EventHandler::StateHandler::enterIdleState()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void EventHandler::StateHandler::enterBusyState()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void EventHandler::StateHandler::softSwitchPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void EventHandler::StateHandler::initComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void EventHandler::StateHandler::softSwitchMonitorStart()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void EventHandler::StateHandler::stateChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void EventHandler::StateHandler::enteredInitState()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void EventHandler::StateHandler::enteredSoftSwitchMonitorState()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}
QT_END_MOC_NAMESPACE
