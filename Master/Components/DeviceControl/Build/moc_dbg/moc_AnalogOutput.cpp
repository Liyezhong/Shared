/****************************************************************************
** Meta object code from reading C++ file 'AnalogOutput.h'
**
** Created: Thu Jul 25 11:26:02 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/AnalogOutput.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalogOutput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CAnalogOutput[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      61,   30,   29,   29, 0x05,
     113,   30,   29,   29, 0x05,
     203,  164,   29,   29, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CAnalogOutput[] = {
    "DeviceControl::CAnalogOutput\0\0"
    "InstanceID,HdlInfo,OutputValue\0"
    "ReportOutputValueAckn(quint32,ReturnCode_t,quint16)\0"
    "ReportActOutputValue(quint32,ReturnCode_t,quint16)\0"
    "InstanceID,HdlInfo,LifeTime,LifeCycles\0"
    "ReportLifeTimeData(quint32,ReturnCode_t,quint32,quint32)\0"
};

void DeviceControl::CAnalogOutput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CAnalogOutput *_t = static_cast<CAnalogOutput *>(_o);
        switch (_id) {
        case 0: _t->ReportOutputValueAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        case 1: _t->ReportActOutputValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        case 2: _t->ReportLifeTimeData((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CAnalogOutput::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CAnalogOutput::staticMetaObject = {
    { &CFunctionModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CAnalogOutput,
      qt_meta_data_DeviceControl__CAnalogOutput, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CAnalogOutput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CAnalogOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CAnalogOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CAnalogOutput))
        return static_cast<void*>(const_cast< CAnalogOutput*>(this));
    return CFunctionModule::qt_metacast(_clname);
}

int DeviceControl::CAnalogOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CFunctionModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CAnalogOutput::ReportOutputValueAckn(quint32 _t1, ReturnCode_t _t2, quint16 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CAnalogOutput::ReportActOutputValue(quint32 _t1, ReturnCode_t _t2, quint16 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::CAnalogOutput::ReportLifeTimeData(quint32 _t1, ReturnCode_t _t2, quint32 _t3, quint32 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
