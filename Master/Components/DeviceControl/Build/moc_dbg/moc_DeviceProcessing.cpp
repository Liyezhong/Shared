/****************************************************************************
** Meta object code from reading C++ file 'DeviceProcessing.h'
**
** Created: Thu Jul 25 11:26:12 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/DeviceProcessing/DeviceProcessing.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DeviceProcessing.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__DeviceProcessing[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,
      76,   32,   32,   32, 0x05,
     118,   32,   32,   32, 0x05,
     219,  163,   32,   32, 0x05,
     351,  282,   32,   32, 0x05,
     447,  430,   32,   32, 0x05,
     485,   32,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
     564,  509,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__DeviceProcessing[] = {
    "DeviceControl::DeviceProcessing\0\0"
    "ReportInitializationFinished(ReturnCode_t)\0"
    "ReportConfigurationFinished(ReturnCode_t)\0"
    "ReportStartNormalOperationMode(ReturnCode_t)\0"
    "instanceID,usErrorGroup,usErrorID,usErrorData,timeStamp\0"
    "ReportError(DevInstanceID_t,quint16,quint16,quint16,QDateTime)\0"
    "instanceID,usErrorGroup,usErrorID,usErrorData,timeStamp,strErrorInfo\0"
    "ReportErrorWithInfo(DevInstanceID_t,quint16,quint16,quint16,QDateTime,"
    "QString)\0"
    "DiagnosticResult\0ReportDiagnosticServiceClosed(qint16)\0"
    "ReportDestroyFinished()\0"
    "ID,data0,data1,data2,data3,data4,data5,data6,data7,dlc\0"
    "ReceiveCANMessage(quint32,quint8,quint8,quint8,quint8,quint8,quint8,qu"
    "int8,quint8,quint8)\0"
};

void DeviceControl::DeviceProcessing::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DeviceProcessing *_t = static_cast<DeviceProcessing *>(_o);
        switch (_id) {
        case 0: _t->ReportInitializationFinished((*reinterpret_cast< ReturnCode_t(*)>(_a[1]))); break;
        case 1: _t->ReportConfigurationFinished((*reinterpret_cast< ReturnCode_t(*)>(_a[1]))); break;
        case 2: _t->ReportStartNormalOperationMode((*reinterpret_cast< ReturnCode_t(*)>(_a[1]))); break;
        case 3: _t->ReportError((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5]))); break;
        case 4: _t->ReportErrorWithInfo((*reinterpret_cast< DevInstanceID_t(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6]))); break;
        case 5: _t->ReportDiagnosticServiceClosed((*reinterpret_cast< qint16(*)>(_a[1]))); break;
        case 6: _t->ReportDestroyFinished(); break;
        case 7: _t->ReceiveCANMessage((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint8(*)>(_a[7])),(*reinterpret_cast< quint8(*)>(_a[8])),(*reinterpret_cast< quint8(*)>(_a[9])),(*reinterpret_cast< quint8(*)>(_a[10]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::DeviceProcessing::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::DeviceProcessing::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DeviceControl__DeviceProcessing,
      qt_meta_data_DeviceControl__DeviceProcessing, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::DeviceProcessing::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::DeviceProcessing::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::DeviceProcessing::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__DeviceProcessing))
        return static_cast<void*>(const_cast< DeviceProcessing*>(this));
    return QObject::qt_metacast(_clname);
}

int DeviceControl::DeviceProcessing::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void DeviceControl::DeviceProcessing::ReportInitializationFinished(ReturnCode_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::DeviceProcessing::ReportConfigurationFinished(ReturnCode_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::DeviceProcessing::ReportStartNormalOperationMode(ReturnCode_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::DeviceProcessing::ReportError(DevInstanceID_t _t1, quint16 _t2, quint16 _t3, quint16 _t4, QDateTime _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::DeviceProcessing::ReportErrorWithInfo(DevInstanceID_t _t1, quint16 _t2, quint16 _t3, quint16 _t4, QDateTime _t5, QString _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::DeviceProcessing::ReportDiagnosticServiceClosed(qint16 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DeviceControl::DeviceProcessing::ReportDestroyFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
