FILE (GLOB SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp *.c)

INCLUDE_DIRECTORIES (${FOX_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${X11_X11_INCLUDE_PATH})

ADD_LIBRARY (
    jeffx
    STATIC
    ${SOURCES}
)

