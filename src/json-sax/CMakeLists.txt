project(json-sax)

set(json-sax_HEADERS
	json.h
)

set(json-sax_SOURCES
	json.c
)

add_library(json-sax SHARED
	${json-sax_SOURCES}
)

SET_TARGET_PROPERTIES(json-sax PROPERTIES LINKER_LANGUAGE C)

install(TARGETS json-sax DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
