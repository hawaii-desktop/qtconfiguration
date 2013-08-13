INCLUDEPATH += $$PWD

PUBLIC_HEADERS += \
    $$PWD/qconfigurationglobal.h \
    $$PWD/qconfiguration.h \
    $$PWD/qconfigurationbackend.h

PRIVATE_HEADERS += \
    $$PWD/qconfiguration_p.h

SOURCES += \
    $$PWD/qconfiguration.cpp \
    $$PWD/qconfigurationbackend.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
