/****************************************************************************
** Meta object code from reading C++ file 'SoftSwitchManager.h'
**
** Created: Thu Jul 25 11:29:06 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/SoftSwitchManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SoftSwitchManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SoftSwitchManager__SoftSwitchMgr[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      34,   33,   33,   33, 0x05,
      54,   33,   33,   33, 0x05,
      81,   33,   33,   33, 0x05,
     102,   33,   33,   33, 0x05,
     117,   33,   33,   33, 0x05,
     147,   33,   33,   33, 0x05,
     174,   33,   33,   33, 0x05,
     199,   33,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
     225,   33,   33,   33, 0x08,
     247,   33,   33,   33, 0x08,
     288,  267,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SoftSwitchManager__SoftSwitchMgr[] = {
    "SoftSwitchManager::SoftSwitchMgr\0\0"
    "SoftSwitchPressed()\0SendSoftSwitchPressedCmd()\0"
    "SystemInitComplete()\0TimerTimeOut()\0"
    "CriticalActionNotInProgress()\0"
    "CriticalActionInProgress()\0"
    "CriticalActionComplete()\0"
    "SendCricitalActionCheck()\0"
    "OnSoftSwitchPressed()\0ResetStateMachine()\0"
    "CriticalActionStatus\0"
    "CriticalActionStatusCheck(NetCommands::CriticalActionStatus_t)\0"
};

void SoftSwitchManager::SoftSwitchMgr::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SoftSwitchMgr *_t = static_cast<SoftSwitchMgr *>(_o);
        switch (_id) {
        case 0: _t->SoftSwitchPressed(); break;
        case 1: _t->SendSoftSwitchPressedCmd(); break;
        case 2: _t->SystemInitComplete(); break;
        case 3: _t->TimerTimeOut(); break;
        case 4: _t->CriticalActionNotInProgress(); break;
        case 5: _t->CriticalActionInProgress(); break;
        case 6: _t->CriticalActionComplete(); break;
        case 7: _t->SendCricitalActionCheck(); break;
        case 8: _t->OnSoftSwitchPressed(); break;
        case 9: _t->ResetStateMachine(); break;
        case 10: _t->CriticalActionStatusCheck((*reinterpret_cast< NetCommands::CriticalActionStatus_t(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SoftSwitchManager::SoftSwitchMgr::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SoftSwitchManager::SoftSwitchMgr::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SoftSwitchManager__SoftSwitchMgr,
      qt_meta_data_SoftSwitchManager__SoftSwitchMgr, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SoftSwitchManager::SoftSwitchMgr::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SoftSwitchManager::SoftSwitchMgr::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SoftSwitchManager::SoftSwitchMgr::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SoftSwitchManager__SoftSwitchMgr))
        return static_cast<void*>(const_cast< SoftSwitchMgr*>(this));
    return QObject::qt_metacast(_clname);
}

int SoftSwitchManager::SoftSwitchMgr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void SoftSwitchManager::SoftSwitchMgr::SoftSwitchPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SoftSwitchManager::SoftSwitchMgr::SendSoftSwitchPressedCmd()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SoftSwitchManager::SoftSwitchMgr::SystemInitComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void SoftSwitchManager::SoftSwitchMgr::TimerTimeOut()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void SoftSwitchManager::SoftSwitchMgr::CriticalActionNotInProgress()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void SoftSwitchManager::SoftSwitchMgr::CriticalActionInProgress()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void SoftSwitchManager::SoftSwitchMgr::CriticalActionComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void SoftSwitchManager::SoftSwitchMgr::SendCricitalActionCheck()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}
QT_END_MOC_NAMESPACE
