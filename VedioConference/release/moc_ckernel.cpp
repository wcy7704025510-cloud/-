/****************************************************************************
** Meta object code from reading C++ file 'ckernel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../core/ckernel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ckernel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Ckernel_t {
    QByteArrayData data[16];
    char stringdata0[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Ckernel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Ckernel_t qt_meta_stringdata_Ckernel = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Ckernel"
QT_MOC_LITERAL(1, 8, 13), // "setNetPackMap"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 10), // "initConfig"
QT_MOC_LITERAL(4, 34, 12), // "slot_destroy"
QT_MOC_LITERAL(5, 47, 13), // "slot_dealData"
QT_MOC_LITERAL(6, 61, 4), // "sock"
QT_MOC_LITERAL(7, 66, 5), // "char*"
QT_MOC_LITERAL(8, 72, 3), // "buf"
QT_MOC_LITERAL(9, 76, 4), // "nLen"
QT_MOC_LITERAL(10, 81, 17), // "slot_LoginSuccess"
QT_MOC_LITERAL(11, 99, 6), // "userId"
QT_MOC_LITERAL(12, 106, 4), // "name"
QT_MOC_LITERAL(13, 111, 19), // "slot_SendClientData"
QT_MOC_LITERAL(14, 131, 18), // "slot_SendAudioData"
QT_MOC_LITERAL(15, 150, 18) // "slot_SendVideoData"

    },
    "Ckernel\0setNetPackMap\0\0initConfig\0"
    "slot_destroy\0slot_dealData\0sock\0char*\0"
    "buf\0nLen\0slot_LoginSuccess\0userId\0"
    "name\0slot_SendClientData\0slot_SendAudioData\0"
    "slot_SendVideoData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Ckernel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a /* Public */,
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x0a /* Public */,
       5,    3,   57,    2, 0x0a /* Public */,
      10,    2,   64,    2, 0x0a /* Public */,
      13,    2,   69,    2, 0x0a /* Public */,
      14,    2,   74,    2, 0x0a /* Public */,
      15,    2,   79,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 7, QMetaType::Int,    6,    8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   12,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int,    8,    9,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int,    8,    9,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int,    8,    9,

       0        // eod
};

void Ckernel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Ckernel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setNetPackMap(); break;
        case 1: _t->initConfig(); break;
        case 2: _t->slot_destroy(); break;
        case 3: _t->slot_dealData((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->slot_LoginSuccess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->slot_SendClientData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->slot_SendAudioData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->slot_SendVideoData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Ckernel::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Ckernel.data,
    qt_meta_data_Ckernel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Ckernel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Ckernel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Ckernel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Ckernel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
