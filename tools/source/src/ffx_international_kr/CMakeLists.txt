file(GLOB_RECURSE ASSETS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
cmrc_add_resource_library(
    cmrc
    NAMESPACE embed
    WHENCE "${CMAKE_CURRENT_SOURCE_DIR}"
    ${ASSETS}
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

ENABLE_LANGUAGE(RC)
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources.rc ${SOURCES})
add_executable(ffx_international_kr WIN32 ${SOURCES})

target_include_directories(ffx_international_kr PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ffx_international_kr PRIVATE core cmrc xdelta3 wx::core)

if(MINGW)
    target_compile_options(ffx_international_kr PRIVATE
        -Wall
        -Wextra
        -Wno-comment
        -Wno-parentheses
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-function
    )

    target_link_options(ffx_international_kr PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(ffx_international_kr PRIVATE -s)

        find_program(UPX_EXECUTABLE upx)
        if(UPX_EXECUTABLE)
            add_custom_command(TARGET ffx_international_kr POST_BUILD
                COMMAND "${UPX_EXECUTABLE}"
                -q
                --compress-icons=0
                "$<$<CONFIG:Release>:$<TARGET_FILE:ffx_international_kr>>"
            )
        endif()
    endif()

endif()
