cmake_minimum_required(VERSION 3.29)

# TODO: Check if needed for macOS
#set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum OS X deployment version")

###########################################################
# Use C++20
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED true)
#set(CMAKE_CXX_EXTENSIONS false)

###########################################################
# Our project
project("ReedSolomon-SimpleExample"
	VERSION 1.0.0
	DESCRIPTION "ReedSolomon library simple example"
	LANGUAGES CXX
)

# Main executable
add_executable("${PROJECT_NAME}"
	SimpleExample.cpp
)

###########################################################
# Use ReedSolomon lib
target_link_libraries("${PROJECT_NAME}" PRIVATE ReedSolomon)
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/include")

#############################################################
target_compile_options("${PROJECT_NAME}" PRIVATE "-O3")

###########################################################
# Add as many warnings as possible
if (WIN32)
	if (MSVC)
		target_compile_options("${PROJECT_NAME}" PRIVATE "/W3")
		target_compile_options("${PROJECT_NAME}" PRIVATE "/WX")
		target_compile_options("${PROJECT_NAME}" PRIVATE "/wd4244")
		target_compile_options("${PROJECT_NAME}" PRIVATE "/wd4267")
		target_compile_options("${PROJECT_NAME}" PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
	endif()
	# Force Win32 to UNICODE
	target_compile_definitions("${PROJECT_NAME}" PRIVATE UNICODE _UNICODE)
else()
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wall")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wextra")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-pedantic")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wdeprecated")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wshadow")
endif()
