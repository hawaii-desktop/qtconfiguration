include_directories(
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/src/configuration
)

set(SOURCES
    main.cpp
    settings.cpp
    settingstest.cpp
)

add_executable(metaobject ${SOURCES})
qt5_use_modules(metaobject Core)
target_link_libraries(metaobject qtconfiguration)
