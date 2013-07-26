/****************************************************************************
** Meta object code from reading C++ file 'MasterLinkDevice.h'
**
** Created: Thu Jul 25 11:28:03 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/MasterLinkDevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MasterLinkDevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__CMasterLinkDevice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      48,   32,   31,   31, 0x05,
      91,   88,   31,   31, 0x05,
     122,  117,   31,   31, 0x05,
     150,  117,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
     181,   32,   31,   31, 0x08,
     243,  228,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__CMasterLinkDevice[] = {
    "NetworkBase::CMasterLinkDevice\0\0"
    "Ref,CommandName\0SigCmdTimeout(Global::tRefType,QString)\0"
    "dt\0SigDateAndTime(QDateTime)\0name\0"
    "SigMasterConnected(QString)\0"
    "SigMasterDisconnected(QString)\0"
    "OnCommandTimeoutSlot(Global::tRefType,QString)\0"
    "msgname,barray\0ProcessNetMessage(QString,QByteArray)\0"
};

void NetworkBase::CMasterLinkDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CMasterLinkDevice *_t = static_cast<CMasterLinkDevice *>(_o);
        switch (_id) {
        case 0: _t->SigCmdTimeout((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->SigDateAndTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 2: _t->SigMasterConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->SigMasterDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->OnCommandTimeoutSlot((*reinterpret_cast< Global::tRefType(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->ProcessNetMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::CMasterLinkDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::CMasterLinkDevice::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkBase__CMasterLinkDevice,
      qt_meta_data_NetworkBase__CMasterLinkDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::CMasterLinkDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::CMasterLinkDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::CMasterLinkDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__CMasterLinkDevice))
        return static_cast<void*>(const_cast< CMasterLinkDevice*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkBase::CMasterLinkDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::CMasterLinkDevice::SigCmdTimeout(Global::tRefType _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::CMasterLinkDevice::SigDateAndTime(const QDateTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkBase::CMasterLinkDevice::SigMasterConnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkBase::CMasterLinkDevice::SigMasterDisconnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
