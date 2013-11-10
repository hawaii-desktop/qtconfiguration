set(SOURCES
    main.cpp
    settings.cpp
    settingstest.cpp
)

add_executable(metaobject ${SOURCES})
qt5_use_modules(metaobject Core)
target_link_libraries(metaobject qtconfiguration)
