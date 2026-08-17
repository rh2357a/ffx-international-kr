file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(font_adjustment ${SOURCES})
target_include_directories(font_adjustment PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(font_adjustment PRIVATE core xdelta3)

if(MINGW)
    target_compile_options(font_adjustment PRIVATE
        -Wall
        -Wextra
        -Wno-comment
        -Wno-parentheses
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-function
    )

    target_link_options(font_adjustment PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(font_adjustment PRIVATE -s)

        find_program(UPX_EXECUTABLE upx)
        if(UPX_EXECUTABLE)
            add_custom_command(TARGET font_adjustment POST_BUILD
                COMMAND "${UPX_EXECUTABLE}"
                -q
                --compress-icons=0
                "$<$<CONFIG:Release>:$<TARGET_FILE:font_adjustment>>"
            )
        endif()
    endif()

endif()
