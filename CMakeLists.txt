cmake_minimum_required(VERSION 3.20)
project(game LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8-docking
    GIT_SHALLOW TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib imgui)

find_package(OpenGL REQUIRED)

# ImGui static library (Dear ImGui doesn't ship a CMakeLists.txt)
# Uses the GLFW that raylib already fetched
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

# Game
add_executable(game
    src/main.cpp
    src/lib/osScaling.cpp
)
target_include_directories(game PRIVATE src)
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(game PRIVATE raylib imgui_lib X11::X11 X11::Xrandr)
else()
    target_link_libraries(game PRIVATE raylib imgui_lib)
endif()
target_compile_definitions(game PRIVATE $<$<CONFIG:Debug>:DEBUG>)
