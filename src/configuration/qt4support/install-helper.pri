QTCONFIGURATION_PROJECT_INCLUDEDIR = $$QTCONFIGURATION_BUILD_ROOT/include/QtConfiguration
QTCONFIGURATION_PROJECT_INCLUDEDIR ~=s,/,$$QMAKE_DIR_SEP,

system("$$QMAKE_MKDIR $$QTCONFIGURATION_PROJECT_INCLUDEDIR")

for(header_file, PUBLIC_HEADERS) {
   header_file ~=s,/,$$QMAKE_DIR_SEP,
   system("$$QMAKE_COPY \"$${header_file}\" \"$$QTCONFIGURATION_PROJECT_INCLUDEDIR\"")
}

# This is a quick workaround for generating forward header with Qt4.

unix {
    system("echo \'$${LITERAL_HASH}include \"qconfiguration.h\"\' > \"$$QTCONFIGURATION_PROJECT_INCLUDEDIR/QConfiguration\"")
} win32 {
    system("echo $${LITERAL_HASH}include \"qconfiguration.h\" > \"$$QTCONFIGURATION_PROJECT_INCLUDEDIR/QConfiguration\"")
}

PUBLIC_HEADERS += \
     $$PUBLIC_HEADERS \
     \"$$QTCONFIGURATION_PROJECT_INCLUDEDIR/QConfiguration\"

target_headers.files  = $$PUBLIC_HEADERS
target_headers.path   = $$[QT_INSTALL_PREFIX]/include/QtConfiguration
INSTALLS              += target_headers

mkspecs_features.files    = $$QTCONFIGURATION_PROJECT_ROOT/src/configuration/qt4support/configuration.prf
mkspecs_features.path     = $$[QT_INSTALL_DATA]/mkspecs/features
INSTALLS                  += mkspecs_features

win32 {
   dlltarget.path = $$[QT_INSTALL_BINS]
   INSTALLS += dlltarget
}

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

INCLUDEPATH += $$QTCONFIGURATION_BUILD_ROOT/include $$QTCONFIGURATION_BUILD_ROOT/include/QtConfiguration
DEFINES += QT_BUILD_CONFIGURATION_LIB
