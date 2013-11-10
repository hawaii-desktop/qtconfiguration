set(PUBLIC_CLASSES
    QConfiguration
    QConfigurationBackend
)

# Create the portable headers
include(CreatePortableHeaders)
create_portable_headers(PORTABLE_HEADERS ${PUBLIC_CLASSES})

install(FILES qconfigurationglobal.h ${PORTABLE_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QtConfiguration)
