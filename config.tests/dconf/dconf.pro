TARGET = dconf
QT = core

!contains(QT_CONFIG, no-pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += dconf
} else {
    LIBS += -ldconf -lgio-2.0 -lgobject-2.0 -lglib-2.0 
}

SOURCES += main.cpp
