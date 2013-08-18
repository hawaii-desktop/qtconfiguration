TARGET = dconf-dbus-1
QT = core

!contains(QT_CONFIG, no-pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += dconf dconf-dbus-1
} else {
    LIBS += -ldconf -lgio-2.0 -lgobject-2.0 -ldconf-dbus-1 -ldbus-1 -lglib-2.0
}

SOURCES += main.cpp
