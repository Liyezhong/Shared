/****************************************************************************
** Meta object code from reading C++ file 'Rfid11785.h'
**
** Created: Thu Jul 25 11:26:09 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/Rfid11785.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Rfid11785.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CRfid11785[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      46,   27,   26,   26, 0x05,
      84,   27,   26,   26, 0x05,
     145,  122,   26,   26, 0x05,
     221,  185,   26,   26, 0x05,
     300,  273,   26,   26, 0x05,
     345,   27,   26,   26, 0x05,
     387,   27,   26,   26, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CRfid11785[] = {
    "DeviceControl::CRfid11785\0\0"
    "InstanceID,HdlInfo\0"
    "ReportStateAckn(quint32,ReturnCode_t)\0"
    "ReportLoginAckn(quint32,ReturnCode_t)\0"
    "InstanceID,HdlInfo,UID\0"
    "ReportUID(quint32,ReturnCode_t,quint32)\0"
    "InstanceID,HdlInfo,Address,UserData\0"
    "ReportUserData(quint32,ReturnCode_t,quint8,quint32)\0"
    "InstanceID,HdlInfo,Address\0"
    "ReportWriteData(quint32,ReturnCode_t,quint8)\0"
    "ReportWritePassword(quint32,ReturnCode_t)\0"
    "ReportWriteConfiguration(quint32,ReturnCode_t)\0"
};

void DeviceControl::CRfid11785::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CRfid11785 *_t = static_cast<CRfid11785 *>(_o);
        switch (_id) {
        case 0: _t->ReportStateAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 1: _t->ReportLoginAckn((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 2: _t->ReportUID((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3]))); break;
        case 3: _t->ReportUserData((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        case 4: _t->ReportWriteData((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3]))); break;
        case 5: _t->ReportWritePassword((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 6: _t->ReportWriteConfiguration((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CRfid11785::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CRfid11785::staticMetaObject = {
    { &CFunctionModule::staticMetaObject, qt_meta_stringdata_DeviceControl__CRfid11785,
      qt_meta_data_DeviceControl__CRfid11785, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CRfid11785::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CRfid11785::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CRfid11785::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CRfid11785))
        return static_cast<void*>(const_cast< CRfid11785*>(this));
    return CFunctionModule::qt_metacast(_clname);
}

int DeviceControl::CRfid11785::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CFunctionModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void DeviceControl::CRfid11785::ReportStateAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CRfid11785::ReportLoginAckn(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceControl::CRfid11785::ReportUID(quint32 _t1, ReturnCode_t _t2, quint32 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceControl::CRfid11785::ReportUserData(quint32 _t1, ReturnCode_t _t2, quint8 _t3, quint32 _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceControl::CRfid11785::ReportWriteData(quint32 _t1, ReturnCode_t _t2, quint8 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceControl::CRfid11785::ReportWritePassword(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DeviceControl::CRfid11785::ReportWriteConfiguration(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
