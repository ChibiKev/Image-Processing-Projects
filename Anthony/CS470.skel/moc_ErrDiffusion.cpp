/****************************************************************************
** Meta object code from reading C++ file 'ErrDiffusion.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui_src/ErrDiffusion.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ErrDiffusion.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ErrDiffusion_t {
    QByteArrayData data[6];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ErrDiffusion_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ErrDiffusion_t qt_meta_stringdata_ErrDiffusion = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ErrDiffusion"
QT_MOC_LITERAL(1, 13, 12), // "changeMethod"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 16), // "changeSerpentine"
QT_MOC_LITERAL(4, 44, 12), // "changeGammaI"
QT_MOC_LITERAL(5, 57, 12) // "changeGammaD"

    },
    "ErrDiffusion\0changeMethod\0\0changeSerpentine\0"
    "changeGammaI\0changeGammaD"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ErrDiffusion[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x09 /* Protected */,
       3,    1,   37,    2, 0x09 /* Protected */,
       4,    1,   40,    2, 0x09 /* Protected */,
       5,    1,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,

       0        // eod
};

void ErrDiffusion::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ErrDiffusion *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeMethod((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->changeSerpentine((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->changeGammaI((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->changeGammaD((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ErrDiffusion::staticMetaObject = { {
    &ImageFilter::staticMetaObject,
    qt_meta_stringdata_ErrDiffusion.data,
    qt_meta_data_ErrDiffusion,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ErrDiffusion::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ErrDiffusion::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ErrDiffusion.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int ErrDiffusion::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
