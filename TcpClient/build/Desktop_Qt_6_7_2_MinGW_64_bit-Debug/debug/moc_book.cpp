/****************************************************************************
** Meta object code from reading C++ file 'book.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../book.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'book.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSBookENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBookENDCLASS = QtMocHelpers::stringData(
    "Book",
    "createDir",
    "",
    "flushFile",
    "deleteDir",
    "renameFile",
    "enterDir",
    "QModelIndex",
    "index",
    "returnPreDir",
    "deleteRegFile",
    "uploadFile",
    "uploadFileData",
    "downloadFile",
    "shareFile",
    "moveFile",
    "selectDestDir"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBookENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x0a,    1 /* Public */,
       3,    0,   93,    2, 0x0a,    2 /* Public */,
       4,    0,   94,    2, 0x0a,    3 /* Public */,
       5,    0,   95,    2, 0x0a,    4 /* Public */,
       6,    1,   96,    2, 0x0a,    5 /* Public */,
       9,    0,   99,    2, 0x0a,    7 /* Public */,
      10,    0,  100,    2, 0x0a,    8 /* Public */,
      11,    0,  101,    2, 0x0a,    9 /* Public */,
      12,    0,  102,    2, 0x0a,   10 /* Public */,
      13,    0,  103,    2, 0x0a,   11 /* Public */,
      14,    0,  104,    2, 0x0a,   12 /* Public */,
      15,    0,  105,    2, 0x0a,   13 /* Public */,
      16,    0,  106,    2, 0x0a,   14 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
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

Q_CONSTINIT const QMetaObject Book::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSBookENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBookENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBookENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Book, std::true_type>,
        // method 'createDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'flushFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'deleteDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'renameFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enterDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'returnPreDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'deleteRegFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'uploadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'uploadFileData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'downloadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shareFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'moveFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectDestDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Book::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Book *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->createDir(); break;
        case 1: _t->flushFile(); break;
        case 2: _t->deleteDir(); break;
        case 3: _t->renameFile(); break;
        case 4: _t->enterDir((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 5: _t->returnPreDir(); break;
        case 6: _t->deleteRegFile(); break;
        case 7: _t->uploadFile(); break;
        case 8: _t->uploadFileData(); break;
        case 9: _t->downloadFile(); break;
        case 10: _t->shareFile(); break;
        case 11: _t->moveFile(); break;
        case 12: _t->selectDestDir(); break;
        default: ;
        }
    }
}

const QMetaObject *Book::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Book::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBookENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Book::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
