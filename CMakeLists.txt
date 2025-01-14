cmake_minimum_required(VERSION 3.5)
project(ImWinRouter)

###############################################################################
# Functions
###############################################################################

# Enable all reasonable warnings and make all warnings into errors.
function(setup_target_wall name)
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${name} PRIVATE /W4 /WX /utf-8)
    else ()
        target_compile_options(${name} PRIVATE -Wall -Wextra -Werror)
    endif ()
endfunction()

###############################################################################
# Executable
###############################################################################

add_executable(${PROJECT_NAME}
    "src/i18n/__init__.c"
    "src/i18n/en_US.c"
    "src/i18n/zh_CN.c"
    "src/widgets/__init__.cpp"
    "src/widgets/about.cpp"
    "src/widgets/debug.cpp"
    "src/widgets/notify.cpp"
    "src/widgets/preferences.cpp"
    "src/widgets/router.cpp"
    "src/utils/font.cpp"
    "src/utils/ip.cpp"
    "src/utils/string.cpp"
    "src/utils/time.cpp"
    "src/utils/title_builder.cpp"
    "src/main.cpp"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_options(${PROJECT_NAME} PRIVATE -D_WIN32_WINNT=0x0600)
target_link_libraries(${PROJECT_NAME} PRIVATE Iphlpapi Ws2_32)

setup_target_wall(${PROJECT_NAME})

###############################################################################
# Dependency
###############################################################################

include(third_party/imgui.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

include(third_party/json.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${JSON_INCLUDE_DIRS})
