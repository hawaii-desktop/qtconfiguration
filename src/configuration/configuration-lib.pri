INCLUDEPATH += $$PWD

!contains(QT_CONFIG, no-pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += dconf dbus-1 dconf-dbus-1
} else {
    LIBS += -ldconf -lgio-2.0 -lgobject-2.0 -ldconf-dbus-1 -ldbus-1 -lglib-2.0
}

PUBLIC_HEADERS += \
    $$PWD/qconfigurationglobal.h \
    $$PWD/qconfiguration.h \
    $$PWD/qconfigurationbackend.h

PRIVATE_HEADERS += \
    $$PWD/qconfiguration_p.h \
    $$PWD/qdconfconfigurationbackend.h \
    $$PWD/qgvarianutils.h

SOURCES += \
    $$PWD/qconfiguration.cpp \
    $$PWD/qconfigurationbackend.cpp \
    $$PWD/qdconfconfigurationbackend.cpp \
    $$PWD/qgvariantutils.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
