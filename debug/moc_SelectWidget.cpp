/****************************************************************************
** Meta object code from reading C++ file 'SelectWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SelectWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SelectWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SelectWidget_t {
    QByteArrayData data[15];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SelectWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SelectWidget_t qt_meta_stringdata_SelectWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SelectWidget"
QT_MOC_LITERAL(1, 13, 13), // "sendBuildMode"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 9), // "buildMode"
QT_MOC_LITERAL(4, 38, 9), // "widgetAct"
QT_MOC_LITERAL(5, 48, 3), // "num"
QT_MOC_LITERAL(6, 52, 5), // "aiAct"
QT_MOC_LITERAL(7, 58, 7), // "actName"
QT_MOC_LITERAL(8, 66, 11), // "Coordinate*"
QT_MOC_LITERAL(9, 78, 4), // "self"
QT_MOC_LITERAL(10, 83, 11), // "frameUpdate"
QT_MOC_LITERAL(11, 95, 8), // "initActs"
QT_MOC_LITERAL(12, 104, 8), // "getBuild"
QT_MOC_LITERAL(13, 113, 6), // "BlockL"
QT_MOC_LITERAL(14, 120, 6) // "BlockU"

    },
    "SelectWidget\0sendBuildMode\0\0buildMode\0"
    "widgetAct\0num\0aiAct\0actName\0Coordinate*\0"
    "self\0frameUpdate\0initActs\0getBuild\0"
    "BlockL\0BlockU"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SelectWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   47,    2, 0x0a /* Public */,
       6,    2,   50,    2, 0x0a /* Public */,
      10,    0,   55,    2, 0x0a /* Public */,
      11,    0,   56,    2, 0x0a /* Public */,
      12,    3,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Int, QMetaType::Int, 0x80000000 | 8,    7,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   13,   14,    5,

       0        // eod
};

void SelectWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SelectWidget *_t = static_cast<SelectWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendBuildMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->widgetAct((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: { int _r = _t->aiAct((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Coordinate*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->frameUpdate(); break;
        case 4: _t->initActs(); break;
        case 5: _t->getBuild((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SelectWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectWidget::sendBuildMode)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject SelectWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SelectWidget.data,
      qt_meta_data_SelectWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SelectWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SelectWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SelectWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SelectWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SelectWidget::sendBuildMode(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
