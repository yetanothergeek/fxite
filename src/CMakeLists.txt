FILE (GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c)

# resources
SET (
    TXT_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/helptext.txt 
    ${CMAKE_CURRENT_SOURCE_DIR}/help_lua.txt
)
FOREACH (TXT_FILE ${TXT_FILES})
    STRING (REPLACE ".txt" ".h" H_FILE ${TXT_FILE})
    STRING (REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} H_FILE ${H_FILE})
    LIST (APPEND H_FILES ${H_FILE})
    ADD_CUSTOM_COMMAND (
        OUTPUT ${H_FILE}
        COMMAND awk "{if (/\\$$/) {sub(/\\$$/,\"\"); printf(\"%s\",$$0)} else {print}}" "${TXT_FILE}" > "${TXT_FILE}.tmp"
        COMMAND ${RESWRAP} -o "${H_FILE}" "${TXT_FILE}.tmp"
        COMMAND rm "${TXT_FILE}.tmp"
        VERBATIM
    )
ENDFOREACH ()

INCLUDE_DIRECTORIES (
    ${LUA_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/jef
    ${CMAKE_SOURCE_DIR}/mnumgr
    ${CMAKE_SOURCE_DIR}/fxscintilla
    ${X11_X11_INCLUDE_PATH}
    ${FOX_INCLUDE_DIRS}
)

ADD_EXECUTABLE (
    fxite
    ${SOURCES}
    ${H_FILES}
)

TARGET_LINK_LIBRARIES (
    fxite
    fxscintilla
    mnumgr
    jeffx
    ${LUA_LIBRARIES}
    ${FOX_LDFLAGS}
    ${X11_X11_LIB}
)

INSTALL (TARGETS fxite         RUNTIME DESTINATION bin)
INSTALL (FILES   fxite.xpm     DESTINATION share/pixmaps)
INSTALL (FILES   fxite.desktop DESTINATION share/applications)
