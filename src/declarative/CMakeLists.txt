include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/src/configuration
)

set(SOURCES
    plugin.cpp
    qqmlconfiguration.cpp
)

add_library(qmlconfigurationplugin SHARED ${SOURCES})
qt5_use_modules(qmlconfigurationplugin Qml)
target_link_libraries(qmlconfigurationplugin qtconfiguration)
install(TARGETS qmlconfigurationplugin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Configuration)
install(FILES plugins.qmltypes qmldir
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Configuration)
