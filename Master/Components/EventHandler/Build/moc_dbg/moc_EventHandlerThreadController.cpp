/****************************************************************************
** Meta object code from reading C++ file 'EventHandlerThreadController.h'
**
** Created: Thu Jul 25 11:26:31 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/EventHandlerThreadController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EventHandlerThreadController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EventHandler__EventHandlerThreadController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      65,   44,   43,   43, 0x05,
     137,  107,   43,   43, 0x05,
     202,  195,   43,   43, 0x05,
     224,  195,   43,   43, 0x05,

 // slots: signature, parameters, type, tag, flags
     345,  241,   43,   43, 0x0a,
     572,  486,   43,   43, 0x2a,
     770,  705,   43,   43, 0x2a,
     876,  871,   43,   43, 0x0a,
     910,  896,   43,   43, 0x0a,
     960,  936,   43,   43, 0x0a,
    1009,  996,   43,   43, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EventHandler__EventHandlerThreadController[] = {
    "EventHandler::EventHandlerThreadController\0"
    "\0TheDayOperationEntry\0"
    "LogEventEntry(DataLogging::DayEventEntry)\0"
    "TheDayOperationEntry,EventKey\0"
    "ForwardToErrorHandler(DataLogging::DayEventEntry,quint64)\0"
    "active\0GuiAvailability(bool)\0"
    "GuiMsgHint(bool)\0"
    "ErrorCode,EventStringList,IsResolved,EventKeyRef,AltStringUsuage,Event"
    "StringListForRD,EventCodeScenario\0"
    "ProcessEvent(quint32,Global::tTranslatableStringList,bool,quint32,Glob"
    "al::AlternateEventStringUsage,Global::tTranslatableStringList,quint64)\0"
    "ErrorCode,EventStringList,IsResolved,EventKeyRef,AltStringUsuage,Event"
    "StringListForRD\0"
    "ProcessEvent(quint32,Global::tTranslatableStringList,bool,quint32,Glob"
    "al::AlternateEventStringUsage,Global::tTranslatableStringList)\0"
    "ErrorCode,EventStringList,IsResolved,EventKeyRef,AltStringUsuage\0"
    "ProcessEvent(quint32,Global::tTranslatableStringList,bool,quint32,Glob"
    "al::AlternateEventStringUsage)\0"
    "info\0ProcessLog(QString)\0OperatingMode\0"
    "SetOperatingMode(QString)\0"
    "EventLoggerBaseFileName\0"
    "SetEventLoggerBaseFileName(QString)\0"
    "SerialNumber\0SetSerialNumber(QString)\0"
};

void EventHandler::EventHandlerThreadController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EventHandlerThreadController *_t = static_cast<EventHandlerThreadController *>(_o);
        switch (_id) {
        case 0: _t->LogEventEntry((*reinterpret_cast< const DataLogging::DayEventEntry(*)>(_a[1]))); break;
        case 1: _t->ForwardToErrorHandler((*reinterpret_cast< const DataLogging::DayEventEntry(*)>(_a[1])),(*reinterpret_cast< const quint64(*)>(_a[2]))); break;
        case 2: _t->GuiAvailability((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->GuiMsgHint((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->ProcessEvent((*reinterpret_cast< const quint32(*)>(_a[1])),(*reinterpret_cast< const Global::tTranslatableStringList(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3])),(*reinterpret_cast< const quint32(*)>(_a[4])),(*reinterpret_cast< const Global::AlternateEventStringUsage(*)>(_a[5])),(*reinterpret_cast< Global::tTranslatableStringList(*)>(_a[6])),(*reinterpret_cast< quint64(*)>(_a[7]))); break;
        case 5: _t->ProcessEvent((*reinterpret_cast< const quint32(*)>(_a[1])),(*reinterpret_cast< const Global::tTranslatableStringList(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3])),(*reinterpret_cast< const quint32(*)>(_a[4])),(*reinterpret_cast< const Global::AlternateEventStringUsage(*)>(_a[5])),(*reinterpret_cast< Global::tTranslatableStringList(*)>(_a[6]))); break;
        case 6: _t->ProcessEvent((*reinterpret_cast< const quint32(*)>(_a[1])),(*reinterpret_cast< const Global::tTranslatableStringList(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3])),(*reinterpret_cast< const quint32(*)>(_a[4])),(*reinterpret_cast< const Global::AlternateEventStringUsage(*)>(_a[5]))); break;
        case 7: _t->ProcessLog((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->SetOperatingMode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->SetEventLoggerBaseFileName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->SetSerialNumber((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EventHandler::EventHandlerThreadController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventHandler::EventHandlerThreadController::staticMetaObject = {
    { &Threads::ThreadController::staticMetaObject, qt_meta_stringdata_EventHandler__EventHandlerThreadController,
      qt_meta_data_EventHandler__EventHandlerThreadController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventHandler::EventHandlerThreadController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventHandler::EventHandlerThreadController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventHandler::EventHandlerThreadController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventHandler__EventHandlerThreadController))
        return static_cast<void*>(const_cast< EventHandlerThreadController*>(this));
    typedef Threads::ThreadController QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int EventHandler::EventHandlerThreadController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Threads::ThreadController QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void EventHandler::EventHandlerThreadController::LogEventEntry(const DataLogging::DayEventEntry & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EventHandler::EventHandlerThreadController::ForwardToErrorHandler(const DataLogging::DayEventEntry & _t1, const quint64 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EventHandler::EventHandlerThreadController::GuiAvailability(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EventHandler::EventHandlerThreadController::GuiMsgHint(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
