/****************************************************************************
** Meta object code from reading C++ file 'qtscroller.h'
**
** Created: Tue Mar 12 04:27:51 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtscroller.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtscroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtScroller[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       2,   54, // properties
       1,   62, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   12,   11,   11, 0x05,
      53,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     106,  101,   11,   11, 0x0a,
     154,  150,   11,   11, 0x0a,
     187,  172,   11,   11, 0x0a,
     230,  209,   11,   11, 0x0a,
     296,  264,   11,   11, 0x0a,
     334,   11,   11,   11, 0x0a,

 // properties: name, type, flags
     361,  355, 0x00495009,
     388,  367, 0x0049510b,

 // properties: notify_signal_id
       0,
       1,

 // enums: name, flags, count, data
     355, 0x0,    4,   66,

 // enum data: key, value
     407, uint(QtScroller::Inactive),
     416, uint(QtScroller::Pressed),
     424, uint(QtScroller::Dragging),
     433, uint(QtScroller::Scrolling),

       0        // eod
};

static const char qt_meta_stringdata_QtScroller[] = {
    "QtScroller\0\0newstate\0"
    "stateChanged(QtScroller::State)\0"
    "scrollerPropertiesChanged(QtScrollerProperties)\0"
    "prop\0setScrollerProperties(QtScrollerProperties)\0"
    "pos\0scrollTo(QPointF)\0pos,scrollTime\0"
    "scrollTo(QPointF,int)\0rect,xmargin,ymargin\0"
    "ensureVisible(QRectF,qreal,qreal)\0"
    "rect,xmargin,ymargin,scrollTime\0"
    "ensureVisible(QRectF,qreal,qreal,int)\0"
    "resendPrepareEvent()\0State\0state\0"
    "QtScrollerProperties\0scrollerProperties\0"
    "Inactive\0Pressed\0Dragging\0Scrolling\0"
};

void QtScroller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QtScroller *_t = static_cast<QtScroller *>(_o);
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< QtScroller::State(*)>(_a[1]))); break;
        case 1: _t->scrollerPropertiesChanged((*reinterpret_cast< const QtScrollerProperties(*)>(_a[1]))); break;
        case 2: _t->setScrollerProperties((*reinterpret_cast< const QtScrollerProperties(*)>(_a[1]))); break;
        case 3: _t->scrollTo((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 4: _t->scrollTo((*reinterpret_cast< const QPointF(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->ensureVisible((*reinterpret_cast< const QRectF(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 6: _t->ensureVisible((*reinterpret_cast< const QRectF(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 7: _t->resendPrepareEvent(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QtScroller::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QtScroller::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtScroller,
      qt_meta_data_QtScroller, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtScroller::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtScroller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtScroller::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtScroller))
        return static_cast<void*>(const_cast< QtScroller*>(this));
    return QObject::qt_metacast(_clname);
}

int QtScroller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< State*>(_v) = state(); break;
        case 1: *reinterpret_cast< QtScrollerProperties*>(_v) = scrollerProperties(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setScrollerProperties(*reinterpret_cast< QtScrollerProperties*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QtScroller::stateChanged(QtScroller::State _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtScroller::scrollerPropertiesChanged(const QtScrollerProperties & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
