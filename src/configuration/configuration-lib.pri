INCLUDEPATH += $$PWD

!contains(QT_CONFIG, no-pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += dconf
} else {
    LIBS += -ldconf -lgio-2.0 -lgobject-2.0 -lglib-2.0
}

PUBLIC_HEADERS += \
    $$PWD/qconfigurationglobal.h \
    $$PWD/qconfiguration.h \
    $$PWD/qconfigurationbackend.h

PRIVATE_HEADERS += \
    $$PWD/qconfiguration_p.h \
    $$PWD/qdconfconfigurationbackend.h \
    $$PWD/qgvarianutils.h \
    $$PWD/qconfigurationutils.h

SOURCES += \
    $$PWD/qconfiguration.cpp \
    $$PWD/qconfigurationbackend.cpp \
    $$PWD/qdconfconfigurationbackend.cpp \
    $$PWD/qgvariantutils.cpp \
    $$PWD/qconfigurationutils.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
