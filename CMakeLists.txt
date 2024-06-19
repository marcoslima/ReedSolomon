cmake_minimum_required(VERSION 3.28)

# Print build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# TODO: Check if needed for macOS
#set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum OS X deployment version")

###########################################################
# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS false) # Don't use GNU++

###########################################################
# Set options for libs
#set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Needed to set options from subprojects inside parent project

###########################################################
# Our project
project("ReedSolomon"
	VERSION 0.1.0
	DESCRIPTION "ReedSolomon error correction library"
	LANGUAGES CXX
)

# Main library
add_library("${PROJECT_NAME}"
	include/GaloisField.hpp
	include/Polynomial.hpp
	include/Utils.hpp
	include/ReedSolomon.hpp
	src/GaloisField.cpp
	src/Polynomial.cpp
	src/ReedSolomon.cpp
)

# The project include directories
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

###########################################################
# Project versioning
configure_file(ReedSolomonVersion.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/ReedSolomonVersion.hpp)

###########################################################
# Example executable
add_subdirectory(example)

#############################################################
# Optimization
#target_compile_options("${PROJECT_NAME}" PRIVATE "-O3")

###########################################################
# Add as many warning as possible
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
	#add_compile_options("-Weverything")
	#add_compile_options("-Wsign-conversion")
	
	#target_compile_options("${PROJECT_NAME}" PRIVATE "-Weverything")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wall")
	#target_compile_options("${PROJECT_NAME}" PRIVATE "-Wsign-conversion")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wextra")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-pedantic")
	#target_compile_options("${PROJECT_NAME}" PRIVATE "-Werror")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wmissing-declarations")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wdeprecated")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wshadow")
endif()
