cmake_minimum_required(VERSION 3.21)
project(anyany LANGUAGES CXX)

option(STRSWITCH_ENABLE_TESTING "enables testing" OFF)

### strswitchlib ###

add_library(strswitchlib INTERFACE)

target_include_directories(strswitchlib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(strswitchlib PROPERTIES
	CMAKE_CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX
	CMAKE_CXX_STANDARD_REQUIRED ON
	CXX_STANDARD 20)

### tests ###

if(STRSWITCH_ENABLE_TESTING)
	include(CTest)
	add_subdirectory(tests)
endif()
