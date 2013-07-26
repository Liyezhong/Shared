/****************************************************************************
** Meta object code from reading C++ file 'BootLoader.h'
**
** Created: Thu Jul 25 11:26:04 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SlaveModules/BootLoader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BootLoader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceControl__CBootLoader[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      47,   28,   27,   27, 0x05,
      90,   28,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
     129,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceControl__CBootLoader[] = {
    "DeviceControl::CBootLoader\0\0"
    "InstanceID,HdlInfo\0"
    "ReportUpdateFirmware(quint32,ReturnCode_t)\0"
    "ReportUpdateInfo(quint32,ReturnCode_t)\0"
    "TimeoutHandler()\0"
};

void DeviceControl::CBootLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CBootLoader *_t = static_cast<CBootLoader *>(_o);
        switch (_id) {
        case 0: _t->ReportUpdateFirmware((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 1: _t->ReportUpdateInfo((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< ReturnCode_t(*)>(_a[2]))); break;
        case 2: _t->TimeoutHandler(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DeviceControl::CBootLoader::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DeviceControl::CBootLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DeviceControl__CBootLoader,
      qt_meta_data_DeviceControl__CBootLoader, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceControl::CBootLoader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceControl::CBootLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceControl::CBootLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceControl__CBootLoader))
        return static_cast<void*>(const_cast< CBootLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int DeviceControl::CBootLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void DeviceControl::CBootLoader::ReportUpdateFirmware(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceControl::CBootLoader::ReportUpdateInfo(quint32 _t1, ReturnCode_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
