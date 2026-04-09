/****************************************************************************
** Meta object code from reading C++ file 'CRoomManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../manager/CRoomManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CRoomManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CRoomManager_t {
    QByteArrayData data[15];
    char stringdata0[189];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CRoomManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CRoomManager_t qt_meta_stringdata_CRoomManager = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CRoomManager"
QT_MOC_LITERAL(1, 13, 15), // "SIG_RoomQuitted"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 12), // "SIG_SendData"
QT_MOC_LITERAL(4, 43, 5), // "char*"
QT_MOC_LITERAL(5, 49, 3), // "buf"
QT_MOC_LITERAL(6, 53, 4), // "nLen"
QT_MOC_LITERAL(7, 58, 15), // "slot_createRoom"
QT_MOC_LITERAL(8, 74, 13), // "slot_joinRoom"
QT_MOC_LITERAL(9, 88, 13), // "slot_QuitRoom"
QT_MOC_LITERAL(10, 102, 21), // "slot_dealCreateRoomRs"
QT_MOC_LITERAL(11, 124, 4), // "sock"
QT_MOC_LITERAL(12, 129, 19), // "slot_dealJoinRoomRs"
QT_MOC_LITERAL(13, 149, 19), // "slot_dealUserInfoRq"
QT_MOC_LITERAL(14, 169, 19) // "slot_dealQuitRoomRq"

    },
    "CRoomManager\0SIG_RoomQuitted\0\0"
    "SIG_SendData\0char*\0buf\0nLen\0slot_createRoom\0"
    "slot_joinRoom\0slot_QuitRoom\0"
    "slot_dealCreateRoomRs\0sock\0"
    "slot_dealJoinRoomRs\0slot_dealUserInfoRq\0"
    "slot_dealQuitRoomRq"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CRoomManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    2,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   65,    2, 0x0a /* Public */,
       8,    0,   66,    2, 0x0a /* Public */,
       9,    0,   67,    2, 0x0a /* Public */,
      10,    3,   68,    2, 0x0a /* Public */,
      12,    3,   75,    2, 0x0a /* Public */,
      13,    3,   82,    2, 0x0a /* Public */,
      14,    3,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 4, QMetaType::Int,   11,    5,    6,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 4, QMetaType::Int,   11,    5,    6,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 4, QMetaType::Int,   11,    5,    6,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 4, QMetaType::Int,   11,    5,    6,

       0        // eod
};

void CRoomManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CRoomManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SIG_RoomQuitted(); break;
        case 1: _t->SIG_SendData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->slot_createRoom(); break;
        case 3: _t->slot_joinRoom(); break;
        case 4: _t->slot_QuitRoom(); break;
        case 5: _t->slot_dealCreateRoomRs((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: _t->slot_dealJoinRoomRs((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 7: _t->slot_dealUserInfoRq((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: _t->slot_dealQuitRoomRq((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CRoomManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CRoomManager::SIG_RoomQuitted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CRoomManager::*)(char * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CRoomManager::SIG_SendData)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CRoomManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CRoomManager.data,
    qt_meta_data_CRoomManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CRoomManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CRoomManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CRoomManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CRoomManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CRoomManager::SIG_RoomQuitted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CRoomManager::SIG_SendData(char * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
