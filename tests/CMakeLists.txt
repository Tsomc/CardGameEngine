cmake_minimum_required(VERSION 3.28)
project(EngineTests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/../output")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")

set(APP_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(ENGINE_DIR "${APP_DIR}/engine")

# ============================================================
# SDL2 预编译包 (MinGW-w64 x86_64)
# ============================================================
set(SDL2_ROOT "${APP_DIR}/libs/SDL2/SDL2-2.30.10/x86_64-w64-mingw32")
set(SDL2_IMAGE_ROOT "${APP_DIR}/libs/SDL2_image/SDL2_image-2.8.4/x86_64-w64-mingw32")
set(SDL2_TTF_ROOT "${APP_DIR}/libs/SDL2_ttf/SDL2_ttf-2.22.0/x86_64-w64-mingw32")
set(SDL2_MIXER_ROOT "${APP_DIR}/libs/SDL2_mixer/SDL2_mixer-2.8.0/x86_64-w64-mingw32")

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

add_library(SDL2_image::SDL2_image SHARED IMPORTED)
set_target_properties(SDL2_image::SDL2_image PROPERTIES
    IMPORTED_LOCATION "${SDL2_IMAGE_ROOT}/bin/SDL2_image.dll"
    IMPORTED_IMPLIB "${SDL2_IMAGE_ROOT}/lib/libSDL2_image.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_ROOT}/include/SDL2"
    INTERFACE_LINK_LIBRARIES "SDL2::SDL2"
)

add_library(SDL2_ttf::SDL2_ttf SHARED IMPORTED)
set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
    IMPORTED_LOCATION "${SDL2_TTF_ROOT}/bin/SDL2_ttf.dll"
    IMPORTED_IMPLIB "${SDL2_TTF_ROOT}/lib/libSDL2_ttf.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_ROOT}/include/SDL2"
    INTERFACE_LINK_LIBRARIES "SDL2::SDL2"
)

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
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${APP_DIR}/libs/glm/glm-0.9.9.8")

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE "${APP_DIR}/libs/nlohmann")

# ============================================================
# Google Test (本地 libs 目录)
# ============================================================
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${APP_DIR}/libs/googletest" "${CMAKE_BINARY_DIR}/googletest")

# ============================================================
# 测试源码（引擎源文件 + 测试用例）
# ============================================================
set(TEST_SOURCES
    ${ENGINE_DIR}/foundation/math_types.cpp
    ${ENGINE_DIR}/foundation/math_utils.cpp
    ${ENGINE_DIR}/foundation/core_log.cpp
    ${ENGINE_DIR}/core/event_bus.cpp
    ${ENGINE_DIR}/core/module_manager.cpp
    ${ENGINE_DIR}/core/engine.cpp
    ${ENGINE_DIR}/core/game_clock.cpp
    ${ENGINE_DIR}/platform/sdl_platform.cpp
    ${ENGINE_DIR}/render/camera.cpp
    ${ENGINE_DIR}/animation/easing.cpp
    ${ENGINE_DIR}/card/card.cpp
    ${ENGINE_DIR}/card/card_zone.cpp
    ${ENGINE_DIR}/card/card_deck.cpp
    ${ENGINE_DIR}/card/card_hand.cpp
    ${ENGINE_DIR}/card/card_battlefield.cpp
    ${ENGINE_DIR}/card/card_hand_layout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_math.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_event_bus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_module_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_easing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_card.cpp
)

add_executable(EngineTests ${TEST_SOURCES})

target_include_directories(EngineTests PRIVATE
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
    ${SDL2_ROOT}/include/SDL2
    ${SDL2_IMAGE_ROOT}/include/SDL2
    ${SDL2_TTF_ROOT}/include/SDL2
    ${SDL2_MIXER_ROOT}/include/SDL2
    ${APP_DIR}/libs/nlohmann
)

target_link_libraries(EngineTests PRIVATE
    gtest
    gtest_main
    glm
    nlohmann_json
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
)

target_compile_options(EngineTests PRIVATE
    -Wall -Wextra -pedantic
    -Wno-unused-parameter
)

include(CTest)
add_test(NAME EngineTests COMMAND EngineTests)

# 复制 SDL2 DLL 到构建输出目录
add_custom_command(TARGET EngineTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_ROOT}/bin/SDL2.dll"
        "${SDL2_IMAGE_ROOT}/bin/SDL2_image.dll"
        "${SDL2_TTF_ROOT}/bin/SDL2_ttf.dll"
        "${SDL2_MIXER_ROOT}/bin/SDL2_mixer.dll"
        "${OUTPUT_DIR}/bin"
    COMMENT "Copying SDL2 DLLs to output directory"
)
