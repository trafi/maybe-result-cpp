set(TARGET "tests")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(${TARGET}
        main.cpp
        result_tests.cpp
        result_map_tests.cpp
        result_map_err_tests.cpp
        result_and_then_tests.cpp
        )

target_include_directories(${TARGET}
        PUBLIC $<TARGET_PROPERTY:maybe_result,INTERFACE_INCLUDE_DIRECTORIES>
        )