set(HDRS
jtui.h
)
	
set(SRCS
jtui.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

if (${PDCURSES_TARGET} STREQUAL "sdl2")
add_definitions(-DPDCURSES_WITH_SDL)
endif (${PDCURSES_TARGET} STREQUAL "sdl2")

if (WIN32)
add_definitions(-DPDC_DLL_BUILD)
endif (WIN32)
add_definitions(-DPDC_RGB)
add_definitions(-DPDC_FORCE_UTF8)
add_definitions(-DPDC_WIDE)

add_library(jtui STATIC ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(jtui
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
target_link_libraries(jtui
    PUBLIC	
    pdcurses
    )
