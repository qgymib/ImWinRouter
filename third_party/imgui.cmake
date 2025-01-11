set(IMGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)

add_library(imgui
    ${IMGUI_ROOT}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
)

target_include_directories(imgui
    PUBLIC
        $<BUILD_INTERFACE:${IMGUI_ROOT}>
        $<BUILD_INTERFACE:${IMGUI_ROOT}/backends>
        $<BUILD_INTERFACE:${IMGUI_ROOT}/misc/cpp>
)

# This is the helper tool for embed fonts
add_executable(binary_to_compressed_c
    ${IMGUI_ROOT}/misc/fonts/binary_to_compressed_c.cpp
)

###############################################################################
# Require Freetype
###############################################################################
find_package(Freetype QUIET)
if (FREETYPE_FOUND)
    target_sources(imgui PRIVATE ${IMGUI_ROOT}/misc/freetype/imgui_freetype.cpp)
    target_compile_options(imgui PUBLIC -DIMGUI_ENABLE_FREETYPE)
    target_link_libraries(imgui PUBLIC Freetype::Freetype)
endif()

###############################################################################
# Require Directx9
###############################################################################
target_sources(imgui
    PRIVATE
        ${IMGUI_ROOT}/backends/imgui_impl_dx9.cpp
        ${IMGUI_ROOT}/backends/imgui_impl_win32.cpp
)
target_link_libraries(imgui PRIVATE d3d9)

if (MINGW)
    target_link_libraries(imgui PRIVATE dwmapi)
endif ()
