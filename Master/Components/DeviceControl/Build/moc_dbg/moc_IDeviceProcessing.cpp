/****************************************************************************
** Meta object code from reading C++ file 'IDeviceProcessing.h'
**
** Created: Thu Jul 25 11:26:19 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/Interface/IDeviceProcessing.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IDeviceProcessing.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__IDeviceProcessing[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      36,   34,   33,   33, 0x05,
      95,   34,   33,   33, 0x05,
     153,   34,   33,   33, 0x05,
     270,  214,   33,   33, 0x05,
     402,  333,   33,   33, 0x05,
     481,   33,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
     505,   33,   33,   33, 0x08,
     519,   33,   33,   33, 0x08,
     535,   33,   33,   33, 0x08,
     574,   33,   33,   33, 0x08,
     612,   33,   33,   33, 0x08,
     653,  214,   33,   33, 0x08,
     712,  333,   33,   33, 0x08,
     804,  787,   33,   33, 0x08,
     838,   33,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__IDeviceProcessing[] = {
    "DeviceControl::IDeviceProcessing\0\0,\0"
    "ReportInitializationFinished(DevInstanceID_t,ReturnCode_t)\0"
    "ReportConfigurationFinished(DevInstanceID_t,ReturnCode_t)\0"
    "ReportStartNormalOperationMode(DevInstanceID_t,ReturnCode_t)\0"
    "instanceID,usErrorGroup,usErrorID,usErrorData,timeStamp\0"
    "ReportError(DevInstanceID_t,quint16,quint16,quint16,QDateTime)\0"
    "instanceID,usErrorGroup,usErrorID,usErrorData,timeStamp,strErrorInfo\0"
    "ReportErrorWithInfo(DevInstanceID_t,quint16,quint16,quint16,QDateTime,"
    "QString)\0"
    "ReportDestroyFinished()\0HandleTasks()\0"
    "ThreadStarted()\0OnInitializationFinished(ReturnCode_t)\0"
    "OnConfigurationFinished(ReturnCode_t)\0"
    "OnStartNormalOperationMode(ReturnCode_t)\0"
    "OnError(DevInstanceID_t,quint16,quint16,quint16,QDateTime)\0"
    "OnErrorWithInfo(DevInstanceID_t,quint16,quint16,quint16,QDateTime,QStr"
    "ing)\0"
    "DiagnosticResult\0OnDiagnosticServiceClosed(qint16)\0"
    "OnDestroyFinished()\0"
};

void DeviceControl::IDeviceProcessing::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        IDeviceProcessing *_t = static_cast<IDeviceProcessing *>(_o);
        switch (_id) {
        case 0: _t->ReportInitializationFinished((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 1: _t->ReportConfigurationFinished((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 2: _t->ReportStartNormalOperationMode((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 3: _t->ReportError((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5]))); break;
        case 4: _t->ReportErrorWithInfo((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6]))); break;
        case 5: _t->ReportDestroyFinished(); break;
        case 6: _t->HandleTasks(); break;
        case 7: _t->ThreadStarted(); break;
        case 8: _t->OnInitializationFinished((*reinterpret_cast< ReturnCode_t(*)>(_a[1]))); break;
        case 9: _t->OnConfigurationFinished((*reinterpret_cast< ReturnCode_t(*)>(_a[1]))); break;
        case 10: _t->OnStartNormalOperationMode((*reinterpret_cast< ReturnCode_t(*)>(_a[1]))); break;
        case 11: _t->OnError((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5]))); break;
        case 12: _t->OnErrorWithInfo((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6]))); break;
        case 13: _t->OnDiagnosticServiceClosed((*reinterpret_cast< qint16(*)>(_a[1]))); break;
        case 14: _t->OnDestroyFinished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::IDeviceProcessing::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::IDeviceProcessing::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DeviceControl__IDeviceProcessing,
      qt_meta_data_DeviceControl__IDeviceProcessing, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::IDeviceProcessing::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::IDeviceProcessing::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::IDeviceProcessing::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__IDeviceProcessing))
        return static_cast<void*>(const_cast< IDeviceProcessing*>(this));
    return QObject::qt_metacast(_clname);
}

int DeviceControl::IDeviceProcessing::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::IDeviceProcessing::ReportInitializationFinished(DevInstanceID_t _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::IDeviceProcessing::ReportConfigurationFinished(DevInstanceID_t _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::IDeviceProcessing::ReportStartNormalOperationMode(DevInstanceID_t _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::IDeviceProcessing::ReportError(DevInstanceID_t _t1, quint16 _t2, quint16 _t3, quint16 _t4, QDateTime _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::IDeviceProcessing::ReportErrorWithInfo(DevInstanceID_t _t1, quint16 _t2, quint16 _t3, quint16 _t4, QDateTime _t5, QString _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::IDeviceProcessing::ReportDestroyFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
