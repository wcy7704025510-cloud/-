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
    QByteArrayData data[28];
    char stringdata0[355];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CMediaManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CMediaManager_t qt_meta_stringdata_CMediaManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CMediaManager"
QT_MOC_LITERAL(1, 14, 13), // "SIG_sendVideo"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 5), // "char*"
QT_MOC_LITERAL(4, 35, 3), // "buf"
QT_MOC_LITERAL(5, 39, 9), // "nPackSize"
QT_MOC_LITERAL(6, 49, 17), // "SIG_SendAudioData"
QT_MOC_LITERAL(7, 67, 4), // "nLen"
QT_MOC_LITERAL(8, 72, 17), // "SIG_SendVideoData"
QT_MOC_LITERAL(9, 90, 17), // "slot_audioFrameRq"
QT_MOC_LITERAL(10, 108, 4), // "sock"
QT_MOC_LITERAL(11, 113, 17), // "slot_videoFrameRq"
QT_MOC_LITERAL(12, 131, 15), // "slot_audioFrame"
QT_MOC_LITERAL(13, 147, 2), // "ba"
QT_MOC_LITERAL(14, 150, 19), // "slot_sendVedioFrame"
QT_MOC_LITERAL(15, 170, 3), // "img"
QT_MOC_LITERAL(16, 174, 20), // "slot_sendScreenFrame"
QT_MOC_LITERAL(17, 195, 15), // "slot_AudioPause"
QT_MOC_LITERAL(18, 211, 15), // "slot_AudioStart"
QT_MOC_LITERAL(19, 227, 15), // "slot_VideoPause"
QT_MOC_LITERAL(20, 243, 15), // "slot_VideoStart"
QT_MOC_LITERAL(21, 259, 16), // "slot_ScreenPause"
QT_MOC_LITERAL(22, 276, 16), // "slot_ScreenStart"
QT_MOC_LITERAL(23, 293, 17), // "slot_refreshVideo"
QT_MOC_LITERAL(24, 311, 2), // "id"
QT_MOC_LITERAL(25, 314, 7), // "QImage&"
QT_MOC_LITERAL(26, 322, 14), // "slot_sendVideo"
QT_MOC_LITERAL(27, 337, 17) // "slot_clearDevices"

    },
    "CMediaManager\0SIG_sendVideo\0\0char*\0"
    "buf\0nPackSize\0SIG_SendAudioData\0nLen\0"
    "SIG_SendVideoData\0slot_audioFrameRq\0"
    "sock\0slot_videoFrameRq\0slot_audioFrame\0"
    "ba\0slot_sendVedioFrame\0img\0"
    "slot_sendScreenFrame\0slot_AudioPause\0"
    "slot_AudioStart\0slot_VideoPause\0"
    "slot_VideoStart\0slot_ScreenPause\0"
    "slot_ScreenStart\0slot_refreshVideo\0"
    "id\0QImage&\0slot_sendVideo\0slot_clearDevices"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMediaManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   99,    2, 0x06 /* Public */,
       6,    2,  104,    2, 0x06 /* Public */,
       8,    2,  109,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    3,  114,    2, 0x0a /* Public */,
      11,    3,  121,    2, 0x0a /* Public */,
      12,    1,  128,    2, 0x0a /* Public */,
      14,    1,  131,    2, 0x0a /* Public */,
      16,    1,  134,    2, 0x0a /* Public */,
      17,    0,  137,    2, 0x0a /* Public */,
      18,    0,  138,    2, 0x0a /* Public */,
      19,    0,  139,    2, 0x0a /* Public */,
      20,    0,  140,    2, 0x0a /* Public */,
      21,    0,  141,    2, 0x0a /* Public */,
      22,    0,  142,    2, 0x0a /* Public */,
      23,    2,  143,    2, 0x0a /* Public */,
      26,    2,  148,    2, 0x0a /* Public */,
      27,    0,  153,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    7,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 3, QMetaType::Int,   10,    4,    7,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 3, QMetaType::Int,   10,    4,    7,
    QMetaType::Void, QMetaType::QByteArray,   13,
    QMetaType::Void, QMetaType::QImage,   15,
    QMetaType::Void, QMetaType::QImage,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 25,   24,   15,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void,

       0        // eod
};

void CMediaManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CMediaManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SIG_sendVideo((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->SIG_SendAudioData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->SIG_SendVideoData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->slot_audioFrameRq((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->slot_videoFrameRq((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->slot_audioFrame((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 6: _t->slot_sendVedioFrame((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 7: _t->slot_sendScreenFrame((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 8: _t->slot_AudioPause(); break;
        case 9: _t->slot_AudioStart(); break;
        case 10: _t->slot_VideoPause(); break;
        case 11: _t->slot_VideoStart(); break;
        case 12: _t->slot_ScreenPause(); break;
        case 13: _t->slot_ScreenStart(); break;
        case 14: _t->slot_refreshVideo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2]))); break;
        case 15: _t->slot_sendVideo((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: _t->slot_clearDevices(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CMediaManager::*)(char * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CMediaManager::SIG_sendVideo)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CMediaManager::*)(char * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CMediaManager::SIG_SendAudioData)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CMediaManager::*)(char * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CMediaManager::SIG_SendVideoData)) {
                *result = 2;
                return;
            }
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
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void CMediaManager::SIG_sendVideo(char * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CMediaManager::SIG_SendAudioData(char * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CMediaManager::SIG_SendVideoData(char * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
