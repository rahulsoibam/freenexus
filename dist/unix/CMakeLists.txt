if (SYSTEMD)
    find_package(Systemd)
    if (SYSTEMD_FOUND)
        set(EXPAND_BINDIR ${CMAKE_INSTALL_FULL_BINDIR})
        configure_file(systemd/freenexus-nox@.service.in ${CMAKE_CURRENT_BINARY_DIR}/freenexus-nox@.service @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/freenexus-nox@.service
            DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR}
            COMPONENT data)
    endif(SYSTEMD_FOUND)
endif(SYSTEMD)


if (GUI)
    list(APPEND MAN_FILES ${freeNexus_SOURCE_DIR}/doc/freenexus.1)
else (GUI)
    list(APPEND MAN_FILES ${freeNexus_SOURCE_DIR}/doc/freenexus-nox.1)
endif (GUI)

install(FILES ${MAN_FILES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
        COMPONENT doc)

if (GUI)
    install(DIRECTORY menuicons/
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor
            FILES_MATCHING PATTERN "*.png")

    install(FILES ${freeNexus_SOURCE_DIR}/src/icons/freenexus.desktop
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications/
            COMPONENT data)

    install(FILES freenexus.appdata.xml
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/appdata/
            COMPONENT data)

    install(FILES
            ${freeNexus_SOURCE_DIR}/src/icons/skin/freenexus-tray.svg
            ${freeNexus_SOURCE_DIR}/src/icons/skin/freenexus-tray-dark.svg
            ${freeNexus_SOURCE_DIR}/src/icons/skin/freenexus-tray-light.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/status
        COMPONENT data)
endif()
