/****************************************************************************
** Meta object code from reading C++ file 'ExternalProcessDevice.h'
**
** Created: Thu Jul 25 11:26:55 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/ExternalProcessDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExternalProcessDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExternalProcessControl__ExternalProcessDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      47,   46,   46,   46, 0x05,

 // slots: signature, parameters, type, tag, flags
      65,   46,   46,   46, 0x0a,
      91,   46,   46,   46, 0x0a,
     116,  114,   46,   46, 0x0a,
     174,   46,   46,   46, 0x0a,
     190,   46,   46,   46, 0x0a,
     208,   46,   46,   46, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ExternalProcessControl__ExternalProcessDevice[] = {
    "ExternalProcessControl::ExternalProcessDevice\0"
    "\0SigLoginTimeout()\0PeerDisconnected(QString)\0"
    "PeerConnected(QString)\0,\0"
    "HandleNetworkError(NetworkBase::DisconnectType_t,QString)\0"
    "LoginTimedout()\0StartLoginGuard()\0"
    "StopLoginGuard()\0"
};

void ExternalProcessControl::ExternalProcessDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExternalProcessDevice *_t = static_cast<ExternalProcessDevice *>(_o);
        switch (_id) {
        case 0: _t->SigLoginTimeout(); break;
        case 1: _t->PeerDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->PeerConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->HandleNetworkError((*reinterpret_cast< NetworkBase::DisconnectType_t(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->LoginTimedout(); break;
        case 5: _t->StartLoginGuard(); break;
        case 6: _t->StopLoginGuard(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ExternalProcessControl::ExternalProcessDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExternalProcessControl::ExternalProcessDevice::staticMetaObject = {
    { &NetworkBase::NetworkServerDevice::staticMetaObject, qt_meta_stringdata_ExternalProcessControl__ExternalProcessDevice,
      qt_meta_data_ExternalProcessControl__ExternalProcessDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExternalProcessControl::ExternalProcessDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExternalProcessControl::ExternalProcessDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExternalProcessControl::ExternalProcessDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExternalProcessControl__ExternalProcessDevice))
        return static_cast<void*>(const_cast< ExternalProcessDevice*>(this));
    typedef NetworkBase::NetworkServerDevice QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int ExternalProcessControl::ExternalProcessDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef NetworkBase::NetworkServerDevice QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
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
void ExternalProcessControl::ExternalProcessDevice::SigLoginTimeout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
