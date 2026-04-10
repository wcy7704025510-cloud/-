/****************************************************************************
** Meta object code from reading C++ file 'CMediaManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../manager/CMediaManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CMediaManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CMediaManager_t {
    QByteArrayData data[24];
    char stringdata0[317];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CMediaManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CMediaManager_t qt_meta_stringdata_CMediaManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CMediaManager"
QT_MOC_LITERAL(1, 14, 22), // "slot_startMediaEngines"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 15), // "slot_AudioPause"
QT_MOC_LITERAL(4, 54, 15), // "slot_AudioStart"
QT_MOC_LITERAL(5, 70, 15), // "slot_VideoPause"
QT_MOC_LITERAL(6, 86, 15), // "slot_VideoStart"
QT_MOC_LITERAL(7, 102, 16), // "slot_ScreenPause"
QT_MOC_LITERAL(8, 119, 16), // "slot_ScreenStart"
QT_MOC_LITERAL(9, 136, 17), // "slot_refreshVideo"
QT_MOC_LITERAL(10, 154, 2), // "id"
QT_MOC_LITERAL(11, 157, 7), // "QImage&"
QT_MOC_LITERAL(12, 165, 3), // "img"
QT_MOC_LITERAL(13, 169, 17), // "slot_clearDevices"
QT_MOC_LITERAL(14, 187, 17), // "slot_audioFrameRq"
QT_MOC_LITERAL(15, 205, 4), // "sock"
QT_MOC_LITERAL(16, 210, 5), // "char*"
QT_MOC_LITERAL(17, 216, 3), // "buf"
QT_MOC_LITERAL(18, 220, 4), // "nLen"
QT_MOC_LITERAL(19, 225, 17), // "slot_videoFrameRq"
QT_MOC_LITERAL(20, 243, 25), // "slot_dispatchDecodedVideo"
QT_MOC_LITERAL(21, 269, 6), // "userId"
QT_MOC_LITERAL(22, 276, 17), // "slot_userLeftRoom"
QT_MOC_LITERAL(23, 294, 22) // "slot_localVideoPreview"

    },
    "CMediaManager\0slot_startMediaEngines\0"
    "\0slot_AudioPause\0slot_AudioStart\0"
    "slot_VideoPause\0slot_VideoStart\0"
    "slot_ScreenPause\0slot_ScreenStart\0"
    "slot_refreshVideo\0id\0QImage&\0img\0"
    "slot_clearDevices\0slot_audioFrameRq\0"
    "sock\0char*\0buf\0nLen\0slot_videoFrameRq\0"
    "slot_dispatchDecodedVideo\0userId\0"
    "slot_userLeftRoom\0slot_localVideoPreview"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMediaManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x0a /* Public */,
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    0,   88,    2, 0x0a /* Public */,
       7,    0,   89,    2, 0x0a /* Public */,
       8,    0,   90,    2, 0x0a /* Public */,
       9,    2,   91,    2, 0x0a /* Public */,
      13,    0,   96,    2, 0x0a /* Public */,
      14,    3,   97,    2, 0x0a /* Public */,
      19,    3,  104,    2, 0x0a /* Public */,
      20,    2,  111,    2, 0x0a /* Public */,
      22,    1,  116,    2, 0x0a /* Public */,
      23,    1,  119,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,   10,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 16, QMetaType::Int,   15,   17,   18,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 16, QMetaType::Int,   15,   17,   18,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,   21,   12,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::QImage,   12,

       0        // eod
};

void CMediaManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CMediaManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_startMediaEngines(); break;
        case 1: _t->slot_AudioPause(); break;
        case 2: _t->slot_AudioStart(); break;
        case 3: _t->slot_VideoPause(); break;
        case 4: _t->slot_VideoStart(); break;
        case 5: _t->slot_ScreenPause(); break;
        case 6: _t->slot_ScreenStart(); break;
        case 7: _t->slot_refreshVideo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2]))); break;
        case 8: _t->slot_clearDevices(); break;
        case 9: _t->slot_audioFrameRq((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 10: _t->slot_videoFrameRq((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: _t->slot_dispatchDecodedVideo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2]))); break;
        case 12: _t->slot_userLeftRoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->slot_localVideoPreview((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CMediaManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CMediaManager.data,
    qt_meta_data_CMediaManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CMediaManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CMediaManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CMediaManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CMediaManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
