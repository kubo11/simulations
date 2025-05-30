cmake_minimum_required(VERSION 3.22)
project(simulations VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "No release type specified. Setting to 'Release'.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

if(WIN32)
    add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)
endif()

set(BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(external)