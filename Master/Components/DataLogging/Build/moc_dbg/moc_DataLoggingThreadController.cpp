/****************************************************************************
** Meta object code from reading C++ file 'DataLoggingThreadController.h'
**
** Created: Thu Jul 25 11:20:32 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/DataLoggingThreadController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataLoggingThreadController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataLogging__DataLoggingThreadController[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      48,   42,   41,   41, 0x0a,
      97,   41,   41,   41, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DataLogging__DataLoggingThreadController[] = {
    "DataLogging::DataLoggingThreadController\0"
    "\0Entry\0SendToDayEventLogger(DataLogging::DayEventEntry)\0"
    "CheckLoggingEnabled()\0"
};

const QMetaObject DataLogging::DataLoggingThreadController::staticMetaObject = {
    { &Threads::ThreadController::staticMetaObject, qt_meta_stringdata_DataLogging__DataLoggingThreadController,
      qt_meta_data_DataLogging__DataLoggingThreadController, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataLogging::DataLoggingThreadController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataLogging::DataLoggingThreadController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataLogging::DataLoggingThreadController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataLogging__DataLoggingThreadController))
        return static_cast<void*>(const_cast< DataLoggingThreadController*>(this));
    typedef Threads::ThreadController QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int DataLogging::DataLoggingThreadController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Threads::ThreadController QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: SendToDayEventLogger((*reinterpret_cast< const DataLogging::DayEventEntry(*)>(_a[1]))); break;
        case 1: CheckLoggingEnabled(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
