/****************************************************************************
** Meta object code from reading C++ file 'BaseModule.h'
**
** Created: Thu Jul 25 11:26:03 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/BaseModule.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BaseModule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CBaseModule[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: signature, parameters, type, tag, flags
      57,   28,   27,   27, 0x05,
     126,  107,   27,   27, 0x05,
     163,  107,   27,   27, 0x05,
     197,  107,   27,   27, 0x05,
     239,  107,   27,   27, 0x05,
     312,  284,   27,   27, 0x05,
     418,  361,   27,   27, 0x05,
     548,  488,   27,   27, 0x05,
     660,  618,   27,   27, 0x05,
     724,  107,   27,   27, 0x05,
     763,  107,   27,   27, 0x05,
     805,  107,   27,   27, 0x05,
     844,  107,   27,   27, 0x05,
     882,  107,   27,   27, 0x05,
     923,  107,   27,   27, 0x05,
     964,  107,   27,   27, 0x05,
    1005,  107,   27,   27, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CBaseModule[] = {
    "DeviceControl::CBaseModule\0\0"
    "InstanceID,HdlInfo,NodeState\0"
    "ReportNodeState(quint32,ReturnCode_t,NodeState_t)\0"
    "InstanceID,HdlInfo\0"
    "ReportEmcyStop(quint32,ReturnCode_t)\0"
    "ReportReset(quint32,ReturnCode_t)\0"
    "ReportDataResetAckn(quint32,ReturnCode_t)\0"
    "ReportFormatMemoryAckn(quint32,ReturnCode_t)\0"
    "InstanceID,HdlInfo,serialNb\0"
    "ReportSerialNumber(quint32,ReturnCode_t,QString)\0"
    "InstanceID,HdlInfo,TestResult,TestYear,TestMonth,TestDay\0"
    "ReportEndTestResult(quint32,ReturnCode_t,quint8,quint8,quint8,quint8)\0"
    "InstanceID,HdlInfo,VersionMajor,VersionMinor,Year,Month,Day\0"
    "ReportHWInfo(quint32,ReturnCode_t,quint8,quint8,quint8,quint8,quint8)\0"
    "InstanceID,HdlInfo,Version,Year,Month,Day\0"
    "ReportSWInfo(quint32,ReturnCode_t,quint16,quint8,quint8,quint8)\0"
    "ReportLoaderInfo(quint32,ReturnCode_t)\0"
    "ReportLifeCycleData(quint32,ReturnCode_t)\0"
    "ReportLaunchDate(quint32,ReturnCode_t)\0"
    "ReportBoardName(quint32,ReturnCode_t)\0"
    "ReportBoardOptions(quint32,ReturnCode_t)\0"
    "ReportVoltageState(quint32,ReturnCode_t)\0"
    "ReportCurrentState(quint32,ReturnCode_t)\0"
    "ReportUniqueNumber(quint32,ReturnCode_t)\0"
};

void DeviceControl::CBaseModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CBaseModule *_t = static_cast<CBaseModule *>(_o);
        switch (_id) {
        case 0: _t->ReportNodeState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< NodeState_t(*)>(_a[3]))); break;
        case 1: _t->ReportEmcyStop((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 2: _t->ReportReset((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 3: _t->ReportDataResetAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 4: _t->ReportFormatMemoryAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 5: _t->ReportSerialNumber((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 6: _t->ReportEndTestResult((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6]))); break;
        case 7: _t->ReportHWInfo((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint8(*)>(_a[7]))); break;
        case 8: _t->ReportSWInfo((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6]))); break;
        case 9: _t->ReportLoaderInfo((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 10: _t->ReportLifeCycleData((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 11: _t->ReportLaunchDate((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 12: _t->ReportBoardName((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 13: _t->ReportBoardOptions((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 14: _t->ReportVoltageState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 15: _t->ReportCurrentState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 16: _t->ReportUniqueNumber((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CBaseModule::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CBaseModule::staticMetaObject = {
    { &CModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CBaseModule,
      qt_meta_data_DeviceControl__CBaseModule, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CBaseModule::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CBaseModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CBaseModule::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CBaseModule))
        return static_cast<void*>(const_cast< CBaseModule*>(this));
    return CModule::qt_metacast(_clname);
}

int DeviceControl::CBaseModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CBaseModule::ReportNodeState(quint32 _t1, ReturnCode_t _t2, NodeState_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CBaseModule::ReportEmcyStop(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::CBaseModule::ReportReset(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::CBaseModule::ReportDataResetAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::CBaseModule::ReportFormatMemoryAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::CBaseModule::ReportSerialNumber(quint32 _t1, ReturnCode_t _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DeviceControl::CBaseModule::ReportEndTestResult(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint8 _t4, quint8 _t5, quint8 _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DeviceControl::CBaseModule::ReportHWInfo(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint8 _t4, quint8 _t5, quint8 _t6, quint8 _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void DeviceControl::CBaseModule::ReportSWInfo(quint32 _t1, ReturnCode_t _t2, quint16 _t3, quint8 _t4, quint8 _t5, quint8 _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void DeviceControl::CBaseModule::ReportLoaderInfo(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void DeviceControl::CBaseModule::ReportLifeCycleData(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void DeviceControl::CBaseModule::ReportLaunchDate(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void DeviceControl::CBaseModule::ReportBoardName(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void DeviceControl::CBaseModule::ReportBoardOptions(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void DeviceControl::CBaseModule::ReportVoltageState(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void DeviceControl::CBaseModule::ReportCurrentState(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void DeviceControl::CBaseModule::ReportUniqueNumber(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}
QT_END_MOC_NAMESPACE
