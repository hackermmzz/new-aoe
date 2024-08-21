/****************************************************************************
** Meta object code from reading C++ file 'Option.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Option.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Option.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Option_t {
    QByteArrayData data[20];
    char stringdata0[328];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Option_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Option_t qt_meta_stringdata_Option = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Option"
QT_MOC_LITERAL(1, 7, 11), // "changeMusic"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 22), // "request_ClearDebugText"
QT_MOC_LITERAL(4, 43, 18), // "request_exportHtml"
QT_MOC_LITERAL(5, 62, 17), // "request_exportTxt"
QT_MOC_LITERAL(6, 80, 19), // "request_exportClear"
QT_MOC_LITERAL(7, 100, 16), // "on_music_clicked"
QT_MOC_LITERAL(8, 117, 16), // "on_sound_clicked"
QT_MOC_LITERAL(9, 134, 10), // "paintEvent"
QT_MOC_LITERAL(10, 145, 12), // "QPaintEvent*"
QT_MOC_LITERAL(11, 158, 5), // "event"
QT_MOC_LITERAL(12, 164, 17), // "on_select_clicked"
QT_MOC_LITERAL(13, 182, 19), // "on_showLine_clicked"
QT_MOC_LITERAL(14, 202, 19), // "on_clickPos_clicked"
QT_MOC_LITERAL(15, 222, 18), // "on_overlap_clicked"
QT_MOC_LITERAL(16, 241, 20), // "on_textClear_clicked"
QT_MOC_LITERAL(17, 262, 21), // "on_exportHtml_clicked"
QT_MOC_LITERAL(18, 284, 20), // "on_exportTxt_clicked"
QT_MOC_LITERAL(19, 305, 22) // "on_exportClear_clicked"

    },
    "Option\0changeMusic\0\0request_ClearDebugText\0"
    "request_exportHtml\0request_exportTxt\0"
    "request_exportClear\0on_music_clicked\0"
    "on_sound_clicked\0paintEvent\0QPaintEvent*\0"
    "event\0on_select_clicked\0on_showLine_clicked\0"
    "on_clickPos_clicked\0on_overlap_clicked\0"
    "on_textClear_clicked\0on_exportHtml_clicked\0"
    "on_exportTxt_clicked\0on_exportClear_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Option[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,
       5,    0,   97,    2, 0x06 /* Public */,
       6,    0,   98,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    1,  101,    2, 0x08 /* Private */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    0,  106,    2, 0x08 /* Private */,
      15,    0,  107,    2, 0x08 /* Private */,
      16,    0,  108,    2, 0x08 /* Private */,
      17,    0,  109,    2, 0x08 /* Private */,
      18,    0,  110,    2, 0x08 /* Private */,
      19,    0,  111,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Option::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Option *_t = static_cast<Option *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeMusic(); break;
        case 1: _t->request_ClearDebugText(); break;
        case 2: _t->request_exportHtml(); break;
        case 3: _t->request_exportTxt(); break;
        case 4: _t->request_exportClear(); break;
        case 5: _t->on_music_clicked(); break;
        case 6: _t->on_sound_clicked(); break;
        case 7: _t->paintEvent((*reinterpret_cast< QPaintEvent*(*)>(_a[1]))); break;
        case 8: _t->on_select_clicked(); break;
        case 9: _t->on_showLine_clicked(); break;
        case 10: _t->on_clickPos_clicked(); break;
        case 11: _t->on_overlap_clicked(); break;
        case 12: _t->on_textClear_clicked(); break;
        case 13: _t->on_exportHtml_clicked(); break;
        case 14: _t->on_exportTxt_clicked(); break;
        case 15: _t->on_exportClear_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Option::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Option::changeMusic)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Option::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Option::request_ClearDebugText)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Option::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Option::request_exportHtml)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Option::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Option::request_exportTxt)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Option::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Option::request_exportClear)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject Option::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Option.data,
      qt_meta_data_Option,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Option::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Option::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Option.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Option::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void Option::changeMusic()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Option::request_ClearDebugText()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Option::request_exportHtml()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Option::request_exportTxt()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Option::request_exportClear()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
