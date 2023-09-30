## Configure Doxygen CMake Targets

IF(BUILD_DOCS)
    # Find Doxygen
    FIND_PACKAGE(Doxygen)

    IF(DOXYGEN_FOUND)
        # General Doxygen Options
        SET(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR})
        SET(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR})
        SET(DOXYGEN_VERSION ${LIBCHAOS_DESCRIBE})
        IF(DEBUG)
            SET(DOXYGEN_DEBUG "YES")
        ELSE()
            SET(DOXYGEN_DEBUG "NO")
        ENDIF()
        SET(DOXYGEN_README_IN ${DOXYGEN_INPUT}/doc/README.md)
        SET(DOXYGEN_README_OUT ${DOXYGEN_OUTPUT}/README.md)
        SET(DOXYGEN_CONFIG_IN ${DOXYGEN_INPUT}/doc/Doxyfile.in)
        SET(DOXYGEN_CONFIG_OUT ${DOXYGEN_OUTPUT}/Doxyfile)
        SET(DOXYGEN_CONFIG_HELP_IN ${DOXYGEN_INPUT}/doc/Doxyfile-help.in)
        SET(DOXYGEN_CONFIG_HELP_OUT ${DOXYGEN_OUTPUT}/Doxyfile-help)
        SET(DOXYGEN_HTML ${DOXYGEN_OUTPUT}/html/index.html)
        SET(DOXYGEN_PDF ${DOXYGEN_OUTPUT}/${PROJECT_NAME}.pdf)
        SET(DOXYGEN_HELP ${DOXYGEN_OUTPUT}/${PROJECT_NAME}.chm)

        # Get Sources to Document
        GET_PROPERTY(LibChaosAllSources GLOBAL PROPERTY LibChaosAllSources)

        # Configure Mainpage
        CONFIGURE_FILE("${DOXYGEN_README_IN}" "${DOXYGEN_README_OUT}")

        # Build Compressed HTML on Windows
        INCLUDE(cmake/FindHTMLHelp.cmake)
        IF(HTML_HELP_COMPILER)
            SET(DOXYGEN_HHC ${HTML_HELP_COMPILER})
            CONFIGURE_FILE("${DOXYGEN_CONFIG_HELP_IN}" "${DOXYGEN_CONFIG_HELP_OUT}" @ONLY)
            ADD_CUSTOM_COMMAND(OUTPUT ${DOXYGEN_HELP}
                DEPENDS ${LibChaosAllSources} ${DOXYGEN_CONFIG_HELP_OUT} ${DOXYGEN_README_IN}
                WORKING_DIRECTORY ${DOXYGEN_OUTPUT}
                COMMAND ${DOXYGEN_EXECUTABLE} "${DOXYGEN_CONFIG_HELP_OUT}"
                COMMAND ${CMAKE_COMMAND} -E copy chm/LibChaos.chm "${DOXYGEN_HELP}"
            )
            ADD_CUSTOM_TARGET(doxygen_help
                DEPENDS ${DOXYGEN_HELP}
            )
        ENDIF()

        # Find Latex tool
        FIND_PACKAGE(LATEX)

        IF(LATEX_FOUND)
            SET(DOXYGEN_LATEX "YES")
        ELSE()
            SET(DOXYGEN_LATEX "NO")
        ENDIF()

        # Build Normal HTML
        CONFIGURE_FILE("${DOXYGEN_CONFIG_IN}" "${DOXYGEN_CONFIG_OUT}" @ONLY)
        ADD_CUSTOM_COMMAND(OUTPUT ${DOXYGEN_HTML}
            DEPENDS ${LibChaosAllSources} ${DOXYGEN_CONFIG_OUT} ${DOXYGEN_README_IN}
            WORKING_DIRECTORY ${DOXYGEN_OUTPUT}
            COMMAND ${DOXYGEN_EXECUTABLE} "${DOXYGEN_CONFIG_OUT}"
        )
        ADD_CUSTOM_TARGET(doxygen
            DEPENDS ${DOXYGEN_HTML}
        )

        # Build PDF
        IF(LATEX_FOUND)
            ADD_CUSTOM_COMMAND(OUTPUT ${DOXYGEN_PDF}
                DEPENDS ${DOXYGEN_HTML}
                WORKING_DIRECTORY ${DOXYGEN_OUTPUT}/latex
                COMMAND make pdf
                COMMAND ${CMAKE_COMMAND} -E copy refman.pdf "${DOXYGEN_PDF}"
            )
            ADD_CUSTOM_TARGET(doxygen_pdf
                DEPENDS ${DOXYGEN_PDF}
            )
        ENDIF()

    ENDIF()
ENDIF()
