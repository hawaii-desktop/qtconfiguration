INCLUDEPATH += $$PWD

PUBLIC_HEADERS += \
    $$PWD/qconfigurationglobal.h \
    $$PWD/qconfiguration.h

PRIVATE_HEADERS += \
    $$PWD/qconfiguration_p.h \
    $$PWD/qconfigurationschema_p.h

SOURCES += \
    $$PWD/qconfiguration.cpp \
    $$PWD/qconfigurationschema.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
