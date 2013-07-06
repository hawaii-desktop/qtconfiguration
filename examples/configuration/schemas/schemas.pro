greaterThan(QT_MAJOR_VERSION, 4) {
    QT += core configuration
} else {
    include($$QTCONFIGURATION_PROJECT_ROOT/src/configuration/qt4support/configuration.prf)
}

TARGET = schemas
TEMPLATE = app

SOURCES += \
    main.cpp
