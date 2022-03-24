cmake_minimum_required (VERSION 3.4)

set (ANSI OFF CACHE BOOL "Make ANSI version")
set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

project (C3Bridge)

if (MSVC)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")

	if (MSVC_VERSION LESS 1600)
		message (STATUS "Using compatibility <stdint.h> header for Visual Studio 2008 and earlier")
		include_directories(${CMAKE_SOURCE_DIR}/dependencies/compatibility)
		add_definitions (-DRC_LEGACY)
		project (C3BCOMPT)
	endif ()

	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
	)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
endif ()

if (ANSI)
	message (STATUS "Making ANSI version of the application")
	remove_definitions (-D_UNICODE -DUNICODE)
	add_definitions (-D_MBCS)
	project (C3BANSI)
else ()
	message (STATUS "Making UNICODE version of the application")
	remove_definitions (-D_MBCS)
	add_definitions (-D_UNICODE -DUNICODE)
endif ()

if (MSVC)
	set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif ()

include_directories (${CMAKE_SOURCE_DIR}/dependencies/win32xx)
include_directories (${CMAKE_SOURCE_DIR}/include)
include_directories (${CMAKE_SOURCE_DIR}/source)

set (SOURCES_APPLICATION
	source/aboutdialog.cpp
	source/aboutdialog.h
	source/application.cpp
	source/application.h
	source/log.cpp
	source/log.h
	source/mainwindow.cpp
	source/mainwindow.h
	source/settings.cpp
	source/settings.h
	source/usermessage.cpp
	source/usermessage.h
	source/view.cpp
	source/view.h
)

set (SOURCES_CROSS3
	source/callbackerrorimplementation.cpp
	source/callbackerrorimplementation.h
	source/cross3.cpp
	source/cross3.h
	source/cross3krc.cpp
	source/cross3krc.h
)

set (SOURCES_MEMORY
	source/bstring.cpp
	source/bstring.h
	source/memoryallocator.cpp
	source/memoryallocator.h
)

set (SOURCES_NETWORK
	source/client.cpp
	source/client.h
	source/tcpserver.cpp
	source/tcpserver.h
	source/udpserver.cpp
	source/udpserver.h
)

set (SOURCES_PROXY
	source/messagebuilder.cpp
	source/messagebuilder.h
	source/messagereader.cpp
	source/messagereader.h
	source/proxy.cpp
	source/proxy.h
)

set (SOURCES_SYSTEM
	source/main.cpp
	source/resource.h
	source/safe.cpp
	source/safe.h
	source/stdafx.cpp
	source/stdafx.h
	source/targetver.h
)

set (SOURCES_HANDLERS
	source/handlers/client_control.cpp
	source/handlers/client_cross.cpp
	source/handlers/client_file.cpp
	source/handlers/client_service.cpp
	source/handlers/client_variables.cpp
)

set (RESOURCES
	source/resources.rc
)

source_group ("Resource Files" FILES ${RESOURCES})
source_group ("Source Files\\Application" FILES ${SOURCES_APPLICATION})
source_group ("Source Files\\Cross 3" FILES ${SOURCES_CROSS3})
source_group ("Source Files\\Memory" FILES ${SOURCES_MEMORY})
source_group ("Source Files\\Network" FILES ${SOURCES_NETWORK})
source_group ("Source Files\\Network Handlers" FILES ${SOURCES_HANDLERS})
source_group ("Source Files\\Proxy" FILES ${SOURCES_PROXY})
source_group ("Source Files\\System" FILES ${SOURCES_SYSTEM})

set (SOURCES
	${SOURCES_APPLICATION}
	${SOURCES_CROSS3}
	${SOURCES_MEMORY}
	${SOURCES_NETWORK}
	${SOURCES_HANDLERS}
	${SOURCES_PROXY}
	${SOURCES_SYSTEM}
)

add_executable (${PROJECT_NAME} WIN32 ${SOURCES} ${RESOURCES})
target_link_libraries (${PROJECT_NAME} "comctl32.lib" "ws2_32.lib")
