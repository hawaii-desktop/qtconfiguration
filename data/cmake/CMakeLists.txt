include(CMakePackageConfigHelpers)

set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/QtConfiguration")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/QtConfigurationConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/QtConfigurationConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS CMAKE_INSTALL_PREFIX CMAKE_INSTALL_INCLUDEDIR)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/QtConfigurationConfigVersion.cmake"
                                 VERSION ${QT_CONFIGURATION_VERSION_STRING}
                                 COMPATIBILITY AnyNewerVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/QtConfigurationConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/QtConfigurationConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)
install(EXPORT QtConfigurationTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE QtConfigurationTargets.cmake
        NAMESPACE QtConfiguration::)
