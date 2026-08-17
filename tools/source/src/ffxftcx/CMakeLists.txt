file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(ffxftcx ${SOURCES})
target_include_directories(ffxftcx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ffxftcx PRIVATE core xdelta3)

if(MINGW)
    target_compile_options(ffxftcx PRIVATE
        -Wall
        -Wextra
        -Wno-comment
        -Wno-parentheses
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-function
    )

    target_link_options(ffxftcx PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(ffxftcx PRIVATE -s)

        find_program(UPX_EXECUTABLE upx)
        if(UPX_EXECUTABLE)
            add_custom_command(TARGET ffxftcx POST_BUILD
                COMMAND "${UPX_EXECUTABLE}"
                -q
                --compress-icons=0
                "$<$<CONFIG:Release>:$<TARGET_FILE:ffxftcx>>"
            )
        endif()
    endif()

endif()
