/****************************************************************************
** Meta object code from reading C++ file 'roomdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../view/room/roomdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'roomdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RoomDialog_t {
    QByteArrayData data[14];
    char stringdata0[212];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RoomDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RoomDialog_t qt_meta_stringdata_RoomDialog = {
    {
QT_MOC_LITERAL(0, 0, 10), // "RoomDialog"
QT_MOC_LITERAL(1, 11, 9), // "SIG_close"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 14), // "SIG_AudioPause"
QT_MOC_LITERAL(4, 37, 14), // "SIG_AudioStart"
QT_MOC_LITERAL(5, 52, 14), // "SIG_VideoStart"
QT_MOC_LITERAL(6, 67, 14), // "SIG_VideoPause"
QT_MOC_LITERAL(7, 82, 15), // "SIG_ScreenStart"
QT_MOC_LITERAL(8, 98, 15), // "SIG_ScreenPause"
QT_MOC_LITERAL(9, 114, 19), // "on_pb_close_clicked"
QT_MOC_LITERAL(10, 134, 18), // "on_pb_exit_clicked"
QT_MOC_LITERAL(11, 153, 19), // "on_cb_audio_clicked"
QT_MOC_LITERAL(12, 173, 19), // "on_cb_vedio_clicked"
QT_MOC_LITERAL(13, 193, 18) // "on_cb_desk_clicked"

    },
    "RoomDialog\0SIG_close\0\0SIG_AudioPause\0"
    "SIG_AudioStart\0SIG_VideoStart\0"
    "SIG_VideoPause\0SIG_ScreenStart\0"
    "SIG_ScreenPause\0on_pb_close_clicked\0"
    "on_pb_exit_clicked\0on_cb_audio_clicked\0"
    "on_cb_vedio_clicked\0on_cb_desk_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RoomDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    0,   76,    2, 0x06 /* Public */,
       5,    0,   77,    2, 0x06 /* Public */,
       6,    0,   78,    2, 0x06 /* Public */,
       7,    0,   79,    2, 0x06 /* Public */,
       8,    0,   80,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   81,    2, 0x08 /* Private */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,
      12,    0,   84,    2, 0x08 /* Private */,
      13,    0,   85,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RoomDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RoomDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SIG_close(); break;
        case 1: _t->SIG_AudioPause(); break;
        case 2: _t->SIG_AudioStart(); break;
        case 3: _t->SIG_VideoStart(); break;
        case 4: _t->SIG_VideoPause(); break;
        case 5: _t->SIG_ScreenStart(); break;
        case 6: _t->SIG_ScreenPause(); break;
        case 7: _t->on_pb_close_clicked(); break;
        case 8: _t->on_pb_exit_clicked(); break;
        case 9: _t->on_cb_audio_clicked(); break;
        case 10: _t->on_cb_vedio_clicked(); break;
        case 11: _t->on_cb_desk_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_close)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_AudioPause)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_AudioStart)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_VideoStart)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_VideoPause)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_ScreenStart)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (RoomDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoomDialog::SIG_ScreenPause)) {
                *result = 6;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject RoomDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_RoomDialog.data,
    qt_meta_data_RoomDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RoomDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RoomDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RoomDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int RoomDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void RoomDialog::SIG_close()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RoomDialog::SIG_AudioPause()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RoomDialog::SIG_AudioStart()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RoomDialog::SIG_VideoStart()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RoomDialog::SIG_VideoPause()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RoomDialog::SIG_ScreenStart()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void RoomDialog::SIG_ScreenPause()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
