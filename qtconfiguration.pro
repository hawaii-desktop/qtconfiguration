load(configure)
qtCompileTest(dconf)

lessThan(QT_MAJOR_VERSION, 5) {
    TEMPLATE = subdirs
    #SUBDIRS = src examples tests
    SUBDIRS = src examples
    CONFIG += ordered

    !infile($$OUT_PWD/.qmake.cache, QTCONFIGURATION_PROJECT_ROOT) {
        system("echo QTCONFIGURATION_PROJECT_ROOT = $$PWD >> $$OUT_PWD/.qmake.cache")
        system("echo QTCONFIGURATION_BUILD_ROOT = $$OUT_PWD >> $$OUT_PWD/.qmake.cache")
    }
} else {
    load(qt_parts)
}

!config_dconf {
    error("QtConfiguration requires dconf")
}
