QT = core

#QMAKE_DOCS = $$PWD/doc/qtconfiguration.qdocconf
include($$PWD/configuration-lib.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
    load(qt_build_config)
    QT += core-private
    TARGET = QtConfiguration
    load(qt_module)
} else {
    TEMPLATE = lib
    TARGET = $$qtLibraryTarget(QtConfiguration$$QT_LIBINFIX)
    include($$PWD/qt4support/install-helper.pri)
    CONFIG += module create_prl
    mac:QMAKE_FRAMEWORK_BUNDLE_NAME = $$TARGET
}

HEADERS += \
    qconfigurationglobal.h
