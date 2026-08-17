file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(ffxname ${SOURCES})
target_include_directories(ffxname PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ffxname PRIVATE core xdelta3)

if(MINGW)
    target_compile_options(ffxname PRIVATE
        -Wall
        -Wextra
        -Wno-comment
        -Wno-parentheses
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-function
    )

    target_link_options(ffxname PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(ffxname PRIVATE -s)

        find_program(UPX_EXECUTABLE upx)
        if(UPX_EXECUTABLE)
            add_custom_command(TARGET ffxname POST_BUILD
                COMMAND "${UPX_EXECUTABLE}"
                -q
                --compress-icons=0
                "$<$<CONFIG:Release>:$<TARGET_FILE:ffxname>>"
            )
        endif()
    endif()

endif()
