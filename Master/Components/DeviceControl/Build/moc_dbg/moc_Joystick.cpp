/****************************************************************************
** Meta object code from reading C++ file 'Joystick.h'
**
** Created: Thu Jul 25 11:26:07 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/Joystick.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Joystick.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CJoystick[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      56,   26,   25,   25, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CJoystick[] = {
    "DeviceControl::CJoystick\0\0"
    "InstanceID,HdlInfo,InputValue\0"
    "ReportActInputValue(quint32,ReturnCode_t,quint16)\0"
};

void DeviceControl::CJoystick::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CJoystick *_t = static_cast<CJoystick *>(_o);
        switch (_id) {
        case 0: _t->ReportActInputValue((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CJoystick::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CJoystick::staticMetaObject = {
    { &CFunctionModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CJoystick,
      qt_meta_data_DeviceControl__CJoystick, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CJoystick::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CJoystick::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CJoystick::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CJoystick))
        return static_cast<void*>(const_cast< CJoystick*>(this));
    return CFunctionModule::qt_metacast(_clname);
}

int DeviceControl::CJoystick::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CFunctionModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CJoystick::ReportActInputValue(quint32 _t1, ReturnCode_t _t2, quint16 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
