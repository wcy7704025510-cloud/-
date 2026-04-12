/****************************************************************************
** Meta object code from reading C++ file 'videoconferencedialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../view/main_frame/videoconferencedialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videoconferencedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VedioConferenceDialog_t {
    QByteArrayData data[7];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VedioConferenceDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VedioConferenceDialog_t qt_meta_stringdata_VedioConferenceDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "VedioConferenceDialog"
QT_MOC_LITERAL(1, 22, 9), // "SIG_close"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "SIG_joinRoom"
QT_MOC_LITERAL(4, 46, 14), // "SIG_createRoom"
QT_MOC_LITERAL(5, 61, 20), // "on_tb_create_clicked"
QT_MOC_LITERAL(6, 82, 18) // "on_tb_join_clicked"

    },
    "VedioConferenceDialog\0SIG_close\0\0"
    "SIG_joinRoom\0SIG_createRoom\0"
    "on_tb_create_clicked\0on_tb_join_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VedioConferenceDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   42,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void VedioConferenceDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VedioConferenceDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SIG_close(); break;
        case 1: _t->SIG_joinRoom(); break;
        case 2: _t->SIG_createRoom(); break;
        case 3: _t->on_tb_create_clicked(); break;
        case 4: _t->on_tb_join_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VedioConferenceDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VedioConferenceDialog::SIG_close)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VedioConferenceDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VedioConferenceDialog::SIG_joinRoom)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VedioConferenceDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VedioConferenceDialog::SIG_createRoom)) {
                *result = 2;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject VedioConferenceDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_VedioConferenceDialog.data,
    qt_meta_data_VedioConferenceDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VedioConferenceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VedioConferenceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VedioConferenceDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int VedioConferenceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void VedioConferenceDialog::SIG_close()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VedioConferenceDialog::SIG_joinRoom()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void VedioConferenceDialog::SIG_createRoom()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
