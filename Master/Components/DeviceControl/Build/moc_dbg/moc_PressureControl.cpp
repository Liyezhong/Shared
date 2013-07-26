/****************************************************************************
** Meta object code from reading C++ file 'PressureControl.h'
**
** Created: Thu Jul 25 11:26:08 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/PressureControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PressureControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CPressureControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: signature, parameters, type, tag, flags
      67,   33,   32,   32, 0x05,
     148,  120,   32,   32, 0x05,
     228,  194,   32,   32, 0x05,
     340,  314,   32,   32, 0x05,
     436,  403,   32,   32, 0x05,
     509,  403,   32,   32, 0x05,
     611,  586,   32,   32, 0x05,
     708,  669,   32,   32, 0x05,
     803,  769,   32,   32, 0x05,
     906,  855,   32,   32, 0x05,
    1021,  985,   32,   32, 0x05,
    1115, 1074,   32,   32, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CPressureControl[] = {
    "DeviceControl::CPressureControl\0\0"
    "InstanceID,HdlInfo,Index,Pressure\0"
    "ReportActPressure(quint32,ReturnCode_t,quint8,qreal)\0"
    "InstanceID,HdlInfo,Pressure\0"
    "ReportRefPressure(quint32,ReturnCode_t,qreal)\0"
    "InstanceID,HdlInfo,Status,Voltage\0"
    "ReportActStatus(quint32,ReturnCode_t,PressureCtrlStatus_t,PressureCtrl"
    "MainsVoltage_t)\0"
    "InstanceID,HdlInfo,Status\0"
    "ReportSetStatusAckn(quint32,ReturnCode_t,PressureCtrlStatus_t)\0"
    "InstanceID,HdlInfo,OperatingMode\0"
    "ReportActOperatingMode(quint32,ReturnCode_t,PressureCtrlOperatingMode_"
    "t)\0"
    "ReportSetOperatingModeAckn(quint32,ReturnCode_t,PressureCtrlOperatingM"
    "ode_t)\0"
    "InstanceID,HdlInfo,Index\0"
    "ReportResetPumpOperatingTime(quint32,ReturnCode_t,quint8)\0"
    "InstanceID,HdlInfo,Index,OperatingTime\0"
    "ReportPumpOperatingTime(quint32,ReturnCode_t,quint8,quint32)\0"
    "InstanceID,HdlInfo,Index,FanSpeed\0"
    "ReportFanSpeed(quint32,ReturnCode_t,quint8,quint16)\0"
    "InstanceID,HdlInfo,Sensors,Fans,Pumps,Pids,Current\0"
    "ReportHardwareStatus(quint32,ReturnCode_t,quint8,quint8,quint8,quint8,"
    "quint16)\0"
    "InstanceID,HdlInfo,InRange,Pressure\0"
    "ReportPressureRange(quint32,ReturnCode_t,bool,qint8)\0"
    "InstanceID,HdlInfo,ValveIndex,ValveState\0"
    "ReportRefValveState(quint32,ReturnCode_t,quint8,quint8)\0"
};

void DeviceControl::CPressureControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CPressureControl *_t = static_cast<CPressureControl *>(_o);
        switch (_id) {
        case 0: _t->ReportActPressure((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 1: _t->ReportRefPressure((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 2: _t->ReportActStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< PressureCtrlStatus_t(*)>(_a[3])),(*reinterpret_cast< PressureCtrlMainsVoltage_t(*)>(_a[4]))); break;
        case 3: _t->ReportSetStatusAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< PressureCtrlStatus_t(*)>(_a[3]))); break;
        case 4: _t->ReportActOperatingMode((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< PressureCtrlOperatingMode_t(*)>(_a[3]))); break;
        case 5: _t->ReportSetOperatingModeAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< PressureCtrlOperatingMode_t(*)>(_a[3]))); break;
        case 6: _t->ReportResetPumpOperatingTime((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3]))); break;
        case 7: _t->ReportPumpOperatingTime((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        case 8: _t->ReportFanSpeed((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4]))); break;
        case 9: _t->ReportHardwareStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint16(*)>(_a[7]))); break;
        case 10: _t->ReportPressureRange((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< qint8(*)>(_a[4]))); break;
        case 11: _t->ReportRefValveState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CPressureControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CPressureControl::staticMetaObject = {
    { &CFunctionModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CPressureControl,
      qt_meta_data_DeviceControl__CPressureControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CPressureControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CPressureControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CPressureControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CPressureControl))
        return static_cast<void*>(const_cast< CPressureControl*>(this));
    return CFunctionModule::qt_metacast(_clname);
}

int DeviceControl::CPressureControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CFunctionModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CPressureControl::ReportActPressure(quint32 _t1, ReturnCode_t _t2, quint8 _t3, qreal _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CPressureControl::ReportRefPressure(quint32 _t1, ReturnCode_t _t2, qreal _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::CPressureControl::ReportActStatus(quint32 _t1, ReturnCode_t _t2, PressureCtrlStatus_t _t3, PressureCtrlMainsVoltage_t _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::CPressureControl::ReportSetStatusAckn(quint32 _t1, ReturnCode_t _t2, PressureCtrlStatus_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::CPressureControl::ReportActOperatingMode(quint32 _t1, ReturnCode_t _t2, PressureCtrlOperatingMode_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::CPressureControl::ReportSetOperatingModeAckn(quint32 _t1, ReturnCode_t _t2, PressureCtrlOperatingMode_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DeviceControl::CPressureControl::ReportResetPumpOperatingTime(quint32 _t1, ReturnCode_t _t2, quint8 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DeviceControl::CPressureControl::ReportPumpOperatingTime(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint32 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void DeviceControl::CPressureControl::ReportFanSpeed(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint16 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void DeviceControl::CPressureControl::ReportHardwareStatus(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint8 _t4, quint8 _t5, quint8 _t6, quint16 _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void DeviceControl::CPressureControl::ReportPressureRange(quint32 _t1, ReturnCode_t _t2, bool _t3, qint8 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void DeviceControl::CPressureControl::ReportRefValveState(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint8 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_END_MOC_NAMESPACE
