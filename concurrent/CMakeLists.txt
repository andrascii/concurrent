cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

set(ProjectName concurrent)

project(${ProjectName})

set(ConcurrentSourcesDirectory src)
set(ConcurrentIncludeDirectory include)

aux_source_directory(${ConcurrentSourcesDirectory} ConcurrentSourcesList)
file(GLOB_RECURSE ConcurrentIncludeList "include/*.h")

list(REMOVE_ITEM ConcurrentSourcesList src/stdafx.cpp)
add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" ConcurrentSourcesList)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

if(WIN32)
	source_group(WinResource FILES ${AppWin32Resoure})
endif()
	
configure_msvc_runtime()
add_executable(
	${ProjectName}
	${ConcurrentSourcesList}
	${ConcurrentIncludeList}
)

add_definitions(${QT_DEFINITIONS})

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/std:c++latest
		/Zi
		/W4
		/WX
	)
	
	set_target_properties(
		${ProjectName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${ProjectName} 
		PROPERTIES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP"
	)
endif()

include_directories(
	${ConcurrentIncludeDirectory}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${ProjectName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
)