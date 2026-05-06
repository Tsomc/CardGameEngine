cmake_minimum_required(VERSION 3.28)
project(MyFirstAiGameEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/output")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")

# ============================================================
# SDL2 预编译包 (MinGW-w64 x86_64)
# ============================================================
set(SDL2_ROOT "${CMAKE_SOURCE_DIR}/libs/SDL2/SDL2-2.30.10/x86_64-w64-mingw32")
set(SDL2_IMAGE_ROOT "${CMAKE_SOURCE_DIR}/libs/SDL2_image/SDL2_image-2.8.4/x86_64-w64-mingw32")
set(SDL2_TTF_ROOT "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/SDL2_ttf-2.22.0/x86_64-w64-mingw32")
set(SDL2_MIXER_ROOT "${CMAKE_SOURCE_DIR}/libs/SDL2_mixer/SDL2_mixer-2.8.0/x86_64-w64-mingw32")

set(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/engine")

# --- SDL2 ---
add_library(SDL2::SDL2 SHARED IMPORTED)
set_target_properties(SDL2::SDL2 PROPERTIES
    IMPORTED_LOCATION "${SDL2_ROOT}/bin/SDL2.dll"
    IMPORTED_IMPLIB "${SDL2_ROOT}/lib/libSDL2.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_ROOT}/include/SDL2"
)

add_library(SDL2::SDL2main STATIC IMPORTED)
set_target_properties(SDL2::SDL2main PROPERTIES
    IMPORTED_LOCATION "${SDL2_ROOT}/lib/libSDL2main.a"
    INTERFACE_LINK_OPTIONS "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:-Wl,--undefined=WinMain>"
)

# --- SDL2_image ---
add_library(SDL2_image::SDL2_image SHARED IMPORTED)
set_target_properties(SDL2_image::SDL2_image PROPERTIES
    IMPORTED_LOCATION "${SDL2_IMAGE_ROOT}/bin/SDL2_image.dll"
    IMPORTED_IMPLIB "${SDL2_IMAGE_ROOT}/lib/libSDL2_image.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_ROOT}/include/SDL2"
    INTERFACE_LINK_LIBRARIES "SDL2::SDL2"
)

# --- SDL2_ttf ---
add_library(SDL2_ttf::SDL2_ttf SHARED IMPORTED)
set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
    IMPORTED_LOCATION "${SDL2_TTF_ROOT}/bin/SDL2_ttf.dll"
    IMPORTED_IMPLIB "${SDL2_TTF_ROOT}/lib/libSDL2_ttf.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_ROOT}/include/SDL2"
    INTERFACE_LINK_LIBRARIES "SDL2::SDL2"
)

# --- SDL2_mixer ---
add_library(SDL2_mixer::SDL2_mixer SHARED IMPORTED)
set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
    IMPORTED_LOCATION "${SDL2_MIXER_ROOT}/bin/SDL2_mixer.dll"
    IMPORTED_IMPLIB "${SDL2_MIXER_ROOT}/lib/libSDL2_mixer.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_ROOT}/include/SDL2"
    INTERFACE_LINK_LIBRARIES "SDL2::SDL2"
)

# ============================================================
# 仅头文件库
# ============================================================

# glm 0.9.9.8
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_SOURCE_DIR}/libs/glm/glm-0.9.9.8")

# nlohmann/json 3.11.3
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE "${CMAKE_SOURCE_DIR}/libs/nlohmann")

# sol2 v3.5.0
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE "${CMAKE_SOURCE_DIR}/libs/sol2/sol2-3.5.0/include")
target_link_libraries(sol2 INTERFACE lua)

# ============================================================
# Lua 5.4.7 静态库
# ============================================================
add_library(lua STATIC IMPORTED)
set_target_properties(lua PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/lua-5.4.7/build/liblua.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/lua-5.4.7/src"
)

# ============================================================
# Dear ImGui v1.91.0
# ============================================================
add_library(imgui STATIC
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/backends/imgui_impl_sdl2.cpp"
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/backends/imgui_impl_sdlrenderer2.cpp"
)
target_include_directories(imgui PUBLIC
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0"
    "${CMAKE_SOURCE_DIR}/libs/imgui/imgui-1.91.0/backends"
    "${SDL2_ROOT}/include/SDL2"
)
target_link_libraries(imgui PUBLIC SDL2::SDL2)
target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

# ============================================================
# Engine 源码
# ============================================================

set(ENGINE_SOURCES
    # Foundation
    ${ENGINE_DIR}/foundation/math_types.cpp
    ${ENGINE_DIR}/foundation/math_utils.cpp
    ${ENGINE_DIR}/foundation/core_log.cpp

    # Core
    ${ENGINE_DIR}/core/event_bus.cpp
    ${ENGINE_DIR}/core/game_clock.cpp
    ${ENGINE_DIR}/core/module_manager.cpp
    ${ENGINE_DIR}/core/engine.cpp
    ${ENGINE_DIR}/core/turn_timer.cpp

    # Platform
    ${ENGINE_DIR}/platform/sdl_platform.cpp

    # Resource
    ${ENGINE_DIR}/resource/resource_manager.cpp

    # Render
    ${ENGINE_DIR}/render/camera.cpp
    ${ENGINE_DIR}/render/render_system.cpp
    ${ENGINE_DIR}/render/sprite_batcher.cpp

    # Input
    ${ENGINE_DIR}/input/input_system.cpp

    # Audio
    ${ENGINE_DIR}/audio/audio_system.cpp

    # Script
    ${ENGINE_DIR}/script/script_system.cpp
    ${ENGINE_DIR}/script/script_binder.cpp

    # Animation
    ${ENGINE_DIR}/animation/animation_system.cpp
    ${ENGINE_DIR}/animation/easing.cpp

    # Card
    ${ENGINE_DIR}/card/card.cpp
    ${ENGINE_DIR}/card/card_zone.cpp
    ${ENGINE_DIR}/card/card_deck.cpp
    ${ENGINE_DIR}/card/card_hand.cpp
    ${ENGINE_DIR}/card/card_battlefield.cpp
    ${ENGINE_DIR}/card/card_system.cpp
    ${ENGINE_DIR}/card/card_hand_layout.cpp

    # Scene
    ${ENGINE_DIR}/scene/scene_manager.cpp

    # UI
    ${ENGINE_DIR}/ui/ui_element.cpp
    ${ENGINE_DIR}/ui/ui_button.cpp
    ${ENGINE_DIR}/ui/ui_label.cpp
    ${ENGINE_DIR}/ui/ui_image.cpp
    ${ENGINE_DIR}/ui/ui_panel.cpp
    ${ENGINE_DIR}/ui/ui_system.cpp

    # Debug
    ${ENGINE_DIR}/debug/debug_system.cpp
)

# ============================================================
# 主可执行文件
# ============================================================
add_executable(MyFirstAiGameEngine
    main.cpp
    ${ENGINE_SOURCES}
)

target_include_directories(MyFirstAiGameEngine PRIVATE
    ${ENGINE_DIR}
    ${ENGINE_DIR}/foundation
    ${ENGINE_DIR}/core
    ${ENGINE_DIR}/platform
    ${ENGINE_DIR}/resource
    ${ENGINE_DIR}/render
    ${ENGINE_DIR}/input
    ${ENGINE_DIR}/audio
    ${ENGINE_DIR}/script
    ${ENGINE_DIR}/animation
    ${ENGINE_DIR}/card
    ${ENGINE_DIR}/scene
    ${ENGINE_DIR}/ui
    ${ENGINE_DIR}/debug
)

target_link_libraries(MyFirstAiGameEngine PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    glm
    nlohmann_json
    lua
    sol2
    imgui
)

target_compile_options(MyFirstAiGameEngine PRIVATE
    -Wall -Wextra -pedantic
    -Wno-unused-parameter
)

# 复制 SDL2 DLL 到构建输出目录
add_custom_command(TARGET MyFirstAiGameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_ROOT}/bin/SDL2.dll"
        "${SDL2_IMAGE_ROOT}/bin/SDL2_image.dll"
        "${SDL2_TTF_ROOT}/bin/SDL2_ttf.dll"
        "${SDL2_MIXER_ROOT}/bin/SDL2_mixer.dll"
        "${OUTPUT_DIR}/bin"
    COMMENT "Copying SDL2 DLLs to output directory"
)
