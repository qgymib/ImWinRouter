cmake_minimum_required(VERSION 3.5)
project(ImWinRouter)

###############################################################################
# Executable
###############################################################################

add_executable(${PROJECT_NAME}
    "src/main.cpp"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /utf-8)

###############################################################################
# Dependency
###############################################################################

include(third_party/imgui.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
