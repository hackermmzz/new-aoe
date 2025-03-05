/****************************************************************************
** Meta object code from reading C++ file 'AI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../AI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AI_t {
    QByteArrayData data[7];
    char stringdata0[61];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AI_t qt_meta_stringdata_AI = {
    {
QT_MOC_LITERAL(0, 0, 2), // "AI"
QT_MOC_LITERAL(1, 3, 8), // "cheatRes"
QT_MOC_LITERAL(2, 12, 0), // ""
QT_MOC_LITERAL(3, 13, 11), // "cheatAttack"
QT_MOC_LITERAL(4, 25, 4), // "wave"
QT_MOC_LITERAL(5, 30, 15), // "startProcessing"
QT_MOC_LITERAL(6, 46, 14) // "stopProcessing"

    },
    "AI\0cheatRes\0\0cheatAttack\0wave\0"
    "startProcessing\0stopProcessing"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AI[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    1,   35,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   38,    2, 0x0a /* Public */,
       6,    0,   39,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AI *_t = static_cast<AI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cheatRes(); break;
        case 1: _t->cheatAttack((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->startProcessing(); break;
        case 3: _t->stopProcessing(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AI::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AI::cheatRes)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (AI::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AI::cheatAttack)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject AI::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_AI.data,
      qt_meta_data_AI,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AI.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int AI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void AI::cheatRes()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AI::cheatAttack(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
