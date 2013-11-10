include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${DConf_INCLUDE_DIRS}
)

set(SOURCES
    qconfigurationbackend.cpp
    qconfiguration.cpp
    qconfigurationutils.cpp
    qdconfconfigurationbackend.cpp
    qgvariantutils.cpp
)

set(HEADERS
    qconfigurationbackend.h
    qconfiguration.h
)

add_library(qtconfiguration SHARED ${SOURCES})
qt5_use_modules(qtconfiguration Core)
target_link_libraries(qtconfiguration ${DConf_LIBRARIES})
set_target_properties(qtconfiguration PROPERTIES
    VERSION ${QT_CONFIGURATION_VERSION_STRING}
    SOVERSION ${QT_CONFIGURATION_VERSION_MAJOR}.${QT_CONFIGURATION_VERSION_MINOR}.${QT_CONFIGURATION_VERSION_PATCH})
install(TARGETS qtconfiguration
        EXPORT QtConfigurationTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QtConfiguration)
