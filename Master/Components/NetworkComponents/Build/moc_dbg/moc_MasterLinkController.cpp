/****************************************************************************
** Meta object code from reading C++ file 'MasterLinkController.h'
**
** Created: Thu Jul 25 11:28:01 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/MasterLinkController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MasterLinkController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkBase__MasterLinkController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      52,   35,   34,   34, 0x05,
     125,  112,   34,   34, 0x05,
     173,  170,   34,   34, 0x05,
     204,  199,   34,   34, 0x05,
     232,  199,   34,   34, 0x05,

 // slots: signature, parameters, type, tag, flags
     263,   34,   34,   34, 0x0a,
     268,   34,   34,   34, 0x0a,
     286,  280,  275,   34, 0x0a,
     358,  343,   34,   34, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkBase__MasterLinkController[] = {
    "NetworkBase::MasterLinkController\0\0"
    "name,payload,Ref\0"
    "SigSendMessageToMaster(QString,QByteArray,Global::tRefType)\0"
    "name,payload\0SigForwardMsgToRecipient(QString,QByteArray)\0"
    "dt\0SigDateAndTime(QDateTime)\0name\0"
    "SigMasterConnected(QString)\0"
    "SigMasterDisconnected(QString)\0Go()\0"
    "Stop()\0bool\0,,Ref\0"
    "SendMessageToMaster(QString,QByteArray,Global::tRefType)\0"
    "cmdname,barray\0ForwardMsgToRecipient(QString,QByteArray)\0"
};

void NetworkBase::MasterLinkController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MasterLinkController *_t = static_cast<MasterLinkController *>(_o);
        switch (_id) {
        case 0: _t->SigSendMessageToMaster((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2])),(*reinterpret_cast< Global::tRefType(*)>(_a[3]))); break;
        case 1: _t->SigForwardMsgToRecipient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 2: _t->SigDateAndTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 3: _t->SigMasterConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->SigMasterDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->Go(); break;
        case 6: _t->Stop(); break;
        case 7: { bool _r = _t->SendMessageToMaster((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2])),(*reinterpret_cast< Global::tRefType(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: _t->ForwardMsgToRecipient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetworkBase::MasterLinkController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkBase::MasterLinkController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkBase__MasterLinkController,
      qt_meta_data_NetworkBase__MasterLinkController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkBase::MasterLinkController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkBase::MasterLinkController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkBase::MasterLinkController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkBase__MasterLinkController))
        return static_cast<void*>(const_cast< MasterLinkController*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkBase::MasterLinkController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void NetworkBase::MasterLinkController::SigSendMessageToMaster(const QString & _t1, const QByteArray & _t2, Global::tRefType _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkBase::MasterLinkController::SigForwardMsgToRecipient(const QString & _t1, const QByteArray & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkBase::MasterLinkController::SigDateAndTime(const QDateTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkBase::MasterLinkController::SigMasterConnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NetworkBase::MasterLinkController::SigMasterDisconnected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
