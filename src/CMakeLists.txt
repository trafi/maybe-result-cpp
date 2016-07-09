set(TARGET "maybe_result")

add_library(${TARGET} INTERFACE)

SET(EXPERIMENTAL_OPTIONAL_INCLUDE "" CACHE STRING "Path to experimental optional include directory.")

target_include_directories(${TARGET}
        INTERFACE .
        INTERFACE ${EXPERIMENTAL_OPTIONAL_INCLUDE}
        )