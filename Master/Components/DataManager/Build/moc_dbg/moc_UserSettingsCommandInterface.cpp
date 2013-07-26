/****************************************************************************
** Meta object code from reading C++ file 'UserSettingsCommandInterface.h'
**
** Created: Thu Jul 25 11:25:11 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CommandInterface/Include/UserSettingsCommandInterface.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UserSettingsCommandInterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataManager__CUserSettingsCommandInterface[] = {

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
      60,   44,   43,   43, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DataManager__CUserSettingsCommandInterface[] = {
    "DataManager::CUserSettingsCommandInterface\0"
    "\0LanguageChanged\0UserSettingsChanged(bool)\0"
};

void DataManager::CUserSettingsCommandInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CUserSettingsCommandInterface *_t = static_cast<CUserSettingsCommandInterface *>(_o);
        switch (_id) {
        case 0: _t->UserSettingsChanged((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DataManager::CUserSettingsCommandInterface::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DataManager::CUserSettingsCommandInterface::staticMetaObject = {
    { &CCommandInterfaceBase::staticMetaObject, qt_meta_stringdata_DataManager__CUserSettingsCommandInterface,
      qt_meta_data_DataManager__CUserSettingsCommandInterface, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataManager::CUserSettingsCommandInterface::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataManager::CUserSettingsCommandInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataManager::CUserSettingsCommandInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataManager__CUserSettingsCommandInterface))
        return static_cast<void*>(const_cast< CUserSettingsCommandInterface*>(this));
    return CCommandInterfaceBase::qt_metacast(_clname);
}

int DataManager::CUserSettingsCommandInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CCommandInterfaceBase::qt_metacall(_c, _id, _a);
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
void DataManager::CUserSettingsCommandInterface::UserSettingsChanged(const bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
