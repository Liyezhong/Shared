/****************************************************************************
** Meta object code from reading C++ file 'StepperMotor.h'
**
** Created: Thu Jul 25 11:26:10 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/StepperMotor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StepperMotor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CStepperMotor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: signature, parameters, type, tag, flags
      49,   30,   29,   29, 0x05,
     115,   90,   29,   29, 0x05,
     184,  156,   29,   29, 0x05,
     241,   30,   29,   29, 0x05,
     293,   30,   29,   29, 0x05,
     376,  342,   29,   29, 0x05,
     467,  431,   29,   29, 0x05,
     557,  517,   29,   29, 0x05,
     634,  598,   29,   29, 0x05,
     742,  685,   29,   29, 0x05,
     808,   30,   29,   29, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CStepperMotor[] = {
    "DeviceControl::CStepperMotor\0\0"
    "InstanceID,HdlInfo\0"
    "ReportSetStateAckn(quint32,ReturnCode_t)\0"
    "InstanceID,HdlInfo,State\0"
    "ReportState(quint32,ReturnCode_t,quint8)\0"
    "InstanceID,HdlInfo,Position\0"
    "ReportReferenceMovementAckn(quint32,ReturnCode_t,qint32)\0"
    "ReportPositionMovementCmdAckn(quint32,ReturnCode_t)\0"
    "ReportSpeedMovementCmdAckn(quint32,ReturnCode_t)\0"
    "InstanceID,HdlInfo,Position,speed\0"
    "ReportMovementAckn(quint32,ReturnCode_t,qint32,qint16)\0"
    "InstanceID,HdlInfo,Position,PosCode\0"
    "ReportPosition(quint32,ReturnCode_t,qint32,qint8)\0"
    "InstanCANStepperMotorceID,HdlInfo,Speed\0"
    "ReportSpeed(quint32,ReturnCode_t,qint16)\0"
    "InstanceID,HdlInfo,LimitSwitchState\0"
    "ReportLimitSwitchState(quint32,ReturnCode_t,qint8)\0"
    "InstanceID,HdlInfo,StepCounter,TripCounter,OperationTime\0"
    "ReportLifeCycleData(quint32,ReturnCode_t,quint32,quint32,quint32)\0"
    "ReportConfigureDone(quint32,ReturnCode_t)\0"
};

void DeviceControl::CStepperMotor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CStepperMotor *_t = static_cast<CStepperMotor *>(_o);
        switch (_id) {
        case 0: _t->ReportSetStateAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 1: _t->ReportState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3]))); break;
        case 2: _t->ReportReferenceMovementAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint32(*)>(_a[3]))); break;
        case 3: _t->ReportPositionMovementCmdAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 4: _t->ReportSpeedMovementCmdAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 5: _t->ReportMovementAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint32(*)>(_a[3])),(*reinterpret_cast< qint16(*)>(_a[4]))); break;
        case 6: _t->ReportPosition((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint32(*)>(_a[3])),(*reinterpret_cast< qint8(*)>(_a[4]))); break;
        case 7: _t->ReportSpeed((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint16(*)>(_a[3]))); break;
        case 8: _t->ReportLimitSwitchState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< qint8(*)>(_a[3]))); break;
        case 9: _t->ReportLifeCycleData((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4])),(*reinterpret_cast< quint32(*)>(_a[5]))); break;
        case 10: _t->ReportConfigureDone((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CStepperMotor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CStepperMotor::staticMetaObject = {
    { &CFunctionModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CStepperMotor,
      qt_meta_data_DeviceControl__CStepperMotor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CStepperMotor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CStepperMotor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CStepperMotor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CStepperMotor))
        return static_cast<void*>(const_cast< CStepperMotor*>(this));
    return CFunctionModule::qt_metacast(_clname);
}

int DeviceControl::CStepperMotor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CFunctionModule::qt_metacall(_c, _id, _a);
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
void DeviceControl::CStepperMotor::ReportSetStateAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CStepperMotor::ReportState(quint32 _t1, ReturnCode_t _t2, quint8 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::CStepperMotor::ReportReferenceMovementAckn(quint32 _t1, ReturnCode_t _t2, qint32 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::CStepperMotor::ReportPositionMovementCmdAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::CStepperMotor::ReportSpeedMovementCmdAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::CStepperMotor::ReportMovementAckn(quint32 _t1, ReturnCode_t _t2, qint32 _t3, qint16 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DeviceControl::CStepperMotor::ReportPosition(quint32 _t1, ReturnCode_t _t2, qint32 _t3, qint8 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DeviceControl::CStepperMotor::ReportSpeed(quint32 _t1, ReturnCode_t _t2, qint16 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void DeviceControl::CStepperMotor::ReportLimitSwitchState(quint32 _t1, ReturnCode_t _t2, qint8 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void DeviceControl::CStepperMotor::ReportLifeCycleData(quint32 _t1, ReturnCode_t _t2, quint32 _t3, quint32 _t4, quint32 _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void DeviceControl::CStepperMotor::ReportConfigureDone(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
