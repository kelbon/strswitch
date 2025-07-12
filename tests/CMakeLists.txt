cmake_minimum_required(VERSION 3.21)

add_executable(strswitch ${CMAKE_CURRENT_SOURCE_DIR}/test_strswitch.cpp)
target_link_libraries(strswitch PUBLIC strswitchlib)
add_test(NAME test_strswitch COMMAND strswitch)
set_target_properties(strswitch PROPERTIES
	CMAKE_CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX
	CXX_STANDARD 20
	CMAKE_CXX_STANDARD_REQUIRED ON
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    include(FetchContent)
    
    # disable build breaking useless things from gbench
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
      googlebenchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.9.4
    )
    
    FetchContent_MakeAvailable(googlebenchmark)
    
    add_executable(strswitch_bench ${CMAKE_CURRENT_SOURCE_DIR}/bench_strswitch.cpp)
    target_link_libraries(strswitch_bench PUBLIC benchmark::benchmark strswitchlib)
    
	add_test(NAME test_strswitch_faster COMMAND strswitch_bench)

    set_target_properties(strswitch_bench PROPERTIES
    	CMAKE_CXX_EXTENSIONS OFF
    	LINKER_LANGUAGE CXX
    	CMAKE_CXX_STANDARD_REQUIRED ON
    	CXX_STANDARD 20)
endif()
