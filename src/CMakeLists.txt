set(TARGET "maybe_result")

add_library(${TARGET} INTERFACE)

target_include_directories(${TARGET}
        INTERFACE .
        )