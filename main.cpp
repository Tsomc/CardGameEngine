#include <cstdio>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sol/sol.hpp>

#include <nlohmann/json.hpp>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("=== MyFirstAiGameEngine - Dependency Verification ===\n");

    /* SDL2 */
    SDL_version sdlVer;
    SDL_GetVersion(&sdlVer);
    printf("[SDL2]    version %d.%d.%d\n", sdlVer.major, sdlVer.minor, sdlVer.patch);

    /* SDL_image */
    const SDL_version* imgVer = IMG_Linked_Version();
    printf("[SDL_img] version %d.%d.%d\n", imgVer->major, imgVer->minor, imgVer->patch);

    /* SDL_ttf */
    const SDL_version* ttfVer = TTF_Linked_Version();
    printf("[SDL_ttf] version %d.%d.%d\n", ttfVer->major, ttfVer->minor, ttfVer->patch);

    /* SDL_mixer */
    const SDL_version* mixVer = Mix_Linked_Version();
    printf("[SDL_mix] version %d.%d.%d\n", mixVer->major, mixVer->minor, mixVer->patch);

    /* glm */
    glm::vec2 v(1.0f, 2.0f);
    printf("[glm]     vec2(%.1f, %.1f) length=%.3f\n", v.x, v.y, glm::length(v));

    /* nlohmann/json */
    nlohmann::json j = { {"engine", "MyFirstAiGameEngine"}, {"version", "0.1.0"} };
    printf("[json]    %s\n", j.dump().c_str());

    /* Lua */
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushstring(L, "Hello from Lua!");
    lua_setglobal(L, "greeting");
    luaL_dostring(L, "print(greeting)");
    printf("[Lua]     state created, version: %s\n", LUA_VERSION);
    lua_close(L);

    /* sol2 */
    sol::state solState;
    solState.open_libraries();
    solState.script("print('[sol2]   C++17 binding working')");

    /* Dear ImGui */
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    printf("[ImGui]   version %s, %d fonts\n", IMGUI_VERSION, io.Fonts->Fonts.Size);
    ImGui::DestroyContext();

    printf("\n=== All dependencies verified successfully! ===\n");
    return 0;
}
