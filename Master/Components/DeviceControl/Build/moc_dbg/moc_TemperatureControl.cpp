/****************************************************************************
** Meta object code from reading C++ file 'TemperatureControl.h'
**
** Created: Thu Jul 25 11:26:11 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/TemperatureControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TemperatureControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CTemperatureControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: signature, parameters, type, tag, flags
      73,   36,   35,   35, 0x05,
     160,  129,   35,   35, 0x05,
     243,  209,   35,   35, 0x05,
     347,  321,   35,   35, 0x05,
     439,  406,   35,   35, 0x05,
     508,  406,   35,   35, 0x05,
     606,  581,   35,   35, 0x05,
     705,  666,   35,   35, 0x05,
     802,  768,   35,   35, 0x05,
     925,  854,   35,   35, 0x05,
    1050, 1011,   35,   35, 0x05,
    1131, 1106,   35,   35, 0x05,
    1257, 1183,   35,   35, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CTemperatureControl[] = {
    "DeviceControl::CTemperatureControl\0\0"
    "InstanceID,HdlInfo,Index,Temperature\0"
    "ReportActTemperature(quint32,ReturnCode_t,quint8,qreal)\0"
    "InstanceID,HdlInfo,Temperature\0"
    "ReportRefTemperature(quint32,ReturnCode_t,qreal)\0"
    "InstanceID,HdlInfo,Status,Voltage\0"
    "ReportActStatus(quint32,ReturnCode_t,TempCtrlStatus_t,TempCtrlMainsVol"
    "tage_t)\0"
    "InstanceID,HdlInfo,Status\0"
    "ReportSetStatusAckn(quint32,ReturnCode_t,TempCtrlStatus_t)\0"
    "InstanceID,HdlInfo,OperatingMode\0"
    "ReportActOperatingMode(quint32,ReturnCode_t,TempCtrlOperatingMode_t)\0"
    "ReportSetOperatingModeAckn(quint32,ReturnCode_t,TempCtrlOperatingMode_"
    "t)\0"
    "InstanceID,HdlInfo,Index\0"
    "ReportResetHeaterOperatingTime(quint32,ReturnCode_t,quint8)\0"
    "InstanceID,HdlInfo,Index,OperatingTime\0"
    "ReportHeaterOperatingTime(quint32,ReturnCode_t,quint8,quint32)\0"
    "InstanceID,HdlInfo,Index,FanSpeed\0"
    "ReportFanSpeed(quint32,ReturnCode_t,quint8,quint16)\0"
    "InstanceID,HdlInfo,Sensors,Fans,Heaters,Pids,Current,HeaterSwitchState\0"
    "ReportHardwareStatus(quint32,ReturnCode_t,quint8,quint8,quint8,quint8,"
    "quint16,quint8)\0"
    "InstanceID,HdlInfo,InRange,Temperature\0"
    "ReportTemperatureRange(quint32,ReturnCode_t,bool,qreal)\0"
    "InstanceID,HdlInfo,State\0"
    "ReportLevelSensorState(quint32,ReturnCode_t,quint8)\0"
    "InstanceID,HdlInfo,MaxTemperature,ControllerGain,ResetTime,DerivativeT"
    "ime\0"
    "ReportSetPidAckn(quint32,ReturnCode_t,quint16,quint16,quint16,quint16)\0"
};

void DeviceControl::CTemperatureControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CTemperatureControl *_t = static_cast<CTemperatureControl *>(_o);
        switch (_id) {
        case 0: _t->ReportActTemperature((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 1: _t->ReportRefTemperature((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 2: _t->ReportActStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[3])),(*reinterpret_cast< TempCtrlMainsVoltage_t(*)>(_a[4]))); break;
        case 3: _t->ReportSetStatusAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlStatus_t(*)>(_a[3]))); break;
        case 4: _t->ReportActOperatingMode((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlOperatingMode_t(*)>(_a[3]))); break;
        case 5: _t->ReportSetOperatingModeAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< TempCtrlOperatingMode_t(*)>(_a[3]))); break;
        case 6: _t->ReportResetHeaterOperatingTime((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3]))); break;
        case 7: _t->ReportHeaterOperatingTime((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        case 8: _t->ReportFanSpeed((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4]))); break;
        case 9: _t->ReportHardwareStatus((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint8(*)>(_a[4])),(*reinterpret_cast< quint8(*)>(_a[5])),(*reinterpret_cast< quint8(*)>(_a[6])),(*reinterpret_cast< quint16(*)>(_a[7])),(*reinterpret_cast< quint8(*)>(_a[8]))); break;
        case 10: _t->ReportTemperatureRange((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 11: _t->ReportLevelSensorState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3]))); break;
        case 12: _t->ReportSetPidAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< quint16(*)>(_a[5])),(*reinterpret_cast< quint16(*)>(_a[6]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CTemperatureControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CTemperatureControl::staticMetaObject = {
    { &CFunctionModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CTemperatureControl,
      qt_meta_data_DeviceControl__CTemperatureControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CTemperatureControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CTemperatureControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CTemperatureControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CTemperatureControl))
        return static_cast<void*>(const_cast< CTemperatureControl*>(this));
    return CFunctionModule::qt_metacast(_clname);
}

int DeviceControl::CTemperatureControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CFunctionModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CTemperatureControl::ReportActTemperature(quint32 _t1, ReturnCode_t _t2, quint8 _t3, qreal _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CTemperatureControl::ReportRefTemperature(quint32 _t1, ReturnCode_t _t2, qreal _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::CTemperatureControl::ReportActStatus(quint32 _t1, ReturnCode_t _t2, TempCtrlStatus_t _t3, TempCtrlMainsVoltage_t _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::CTemperatureControl::ReportSetStatusAckn(quint32 _t1, ReturnCode_t _t2, TempCtrlStatus_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::CTemperatureControl::ReportActOperatingMode(quint32 _t1, ReturnCode_t _t2, TempCtrlOperatingMode_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::CTemperatureControl::ReportSetOperatingModeAckn(quint32 _t1, ReturnCode_t _t2, TempCtrlOperatingMode_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DeviceControl::CTemperatureControl::ReportResetHeaterOperatingTime(quint32 _t1, ReturnCode_t _t2, quint8 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DeviceControl::CTemperatureControl::ReportHeaterOperatingTime(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint32 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void DeviceControl::CTemperatureControl::ReportFanSpeed(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint16 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void DeviceControl::CTemperatureControl::ReportHardwareStatus(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint8 _t4, quint8 _t5, quint8 _t6, quint16 _t7, quint8 _t8)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void DeviceControl::CTemperatureControl::ReportTemperatureRange(quint32 _t1, ReturnCode_t _t2, bool _t3, qreal _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void DeviceControl::CTemperatureControl::ReportLevelSensorState(quint32 _t1, ReturnCode_t _t2, quint8 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void DeviceControl::CTemperatureControl::ReportSetPidAckn(quint32 _t1, ReturnCode_t _t2, quint16 _t3, quint16 _t4, quint16 _t5, quint16 _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_END_MOC_NAMESPACE
