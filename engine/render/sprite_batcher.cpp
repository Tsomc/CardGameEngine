#include "sprite_batcher.h"
#include "core_log.h"
#include <algorithm>
#include <cmath>
#include <SDL.h>
using namespace engine;
SpriteBatcher::SpriteBatcher()
    : drawCallCount(0)
    , spriteCount(0)
{
}

SpriteBatcher::~SpriteBatcher()
{
}

void SpriteBatcher::ComputeVertices(
    const Sprite& sprite, BatchedVertex* outVerts)
{
    float left = sprite.dstRect.x;
    float top = sprite.dstRect.y;
    float right = sprite.dstRect.x + sprite.dstRect.w;
    float bottom = sprite.dstRect.y + sprite.dstRect.h;

    float u0, v0, u1, v1;
    if (sprite.srcRect.w > 0.0f && sprite.srcRect.h > 0.0f) {
        u0 = sprite.srcRect.x;
        v0 = sprite.srcRect.y;
        u1 = sprite.srcRect.x + sprite.srcRect.w;
        v1 = sprite.srcRect.y + sprite.srcRect.h;
    } else {
        u0 = 0.0f; v0 = 0.0f;
        u1 = 1.0f; v1 = 1.0f;
    }

    float cosA = 1.0f, sinA = 0.0f;
    if (std::fabs(sprite.angle) > 0.001f) {
        float rad = sprite.angle * 3.14159265f / 180.0f;
        cosA = std::cos(rad);
        sinA = std::sin(rad);
    }

    float px = sprite.dstRect.x + sprite.pivot.x;
    float py = sprite.dstRect.y + sprite.pivot.y;

    float corners[4][2] = {
        {left, top}, {right, top}, {right, bottom}, {left, bottom}
    };

    for (int i = 0; i < 4; ++i) {
        float dx = corners[i][0] - px;
        float dy = corners[i][1] - py;
        corners[i][0] = dx * cosA - dy * sinA + px;
        corners[i][1] = dx * sinA + dy * cosA + py;
    }

    bool flipH = (sprite.flip == FlipMode::FLIP_HORIZONTAL ||
                  sprite.flip == FlipMode::FLIP_VERTICAL);

    int uvOrder[4] = {0, 1, 2, 3};
    if (flipH) {
        std::swap(uvOrder[0], uvOrder[1]);
        std::swap(uvOrder[2], uvOrder[3]);
    }

    float texU[4] = {u0, u1, u1, u0};
    float texV[4] = {v0, v0, v1, v1};

    for (int i = 0; i < 4; ++i) {
        int idx = uvOrder[i];
        outVerts[i].x = corners[idx][0];
        outVerts[i].y = corners[idx][1];
        outVerts[i].u = texU[i];
        outVerts[i].v = texV[i];
        outVerts[i].r = sprite.colorMod.r;
        outVerts[i].g = sprite.colorMod.g;
        outVerts[i].b = sprite.colorMod.b;
        outVerts[i].a = sprite.colorMod.a;
    }
}

void SpriteBatcher::RenderDrawSprite(const Sprite& sprite)
{
    if (sprite.textureId == INVALID_RESOURCE_ID) {
        return;
    }

    BatchGroup* targetGroup = nullptr;
    for (auto& group : groups) {
        if (group.textureId == sprite.textureId && group.layer == sprite.layer) {
            targetGroup = &group;
            break;
        }
    }

    if (targetGroup == nullptr) {
        BatchGroup newGroup;
        newGroup.textureId = sprite.textureId;
        newGroup.layer = sprite.layer;
        groups.push_back(newGroup);
        targetGroup = &groups.back();
    }

    BatchedVertex verts[4];
    ComputeVertices(sprite, verts);

    int baseIndex = (int)targetGroup->vertices.size() / 4;
    for (int i = 0; i < 4; ++i) {
        targetGroup->vertices.push_back(verts[i]);
    }

    targetGroup->indices.push_back(baseIndex * 4 + 0);
    targetGroup->indices.push_back(baseIndex * 4 + 1);
    targetGroup->indices.push_back(baseIndex * 4 + 2);
    targetGroup->indices.push_back(baseIndex * 4 + 2);
    targetGroup->indices.push_back(baseIndex * 4 + 3);
    targetGroup->indices.push_back(baseIndex * 4 + 0);

    spriteCount++;
}

void SpriteBatcher::RenderFlush(void* sdlRenderer)
{
    SDL_Renderer* renderer = (SDL_Renderer*)sdlRenderer;
    if (renderer == nullptr) {
        RenderClear();
        return;
    }

    std::sort(groups.begin(), groups.end(),
        [](const BatchGroup& a, const BatchGroup& b) {
            if (a.layer != b.layer) return a.layer < b.layer;
            return a.textureId < b.textureId;
        });

    drawCallCount = 0;

    for (auto& group : groups) {
        if (group.vertices.empty()) {
            continue;
        }

        SDL_Texture* tex = nullptr;
        drawCallCount++;

        int vertCount = (int)group.vertices.size();

        if (SDL_RenderGeometry(renderer, tex,
                (SDL_Vertex*)group.vertices.data(), vertCount,
                group.indices.data(), (int)group.indices.size()) < 0) {
            LOG_WARN("Render", "SDL_RenderGeometry 失败: %s", SDL_GetError());
        }
    }

    RenderClear();
}

void SpriteBatcher::RenderClear()
{
    groups.clear();
    spriteCount = 0;
}

int SpriteBatcher::RenderGetDrawCallCount() const
{
    return drawCallCount;
}

int SpriteBatcher::RenderGetSpriteCount() const
{
    return spriteCount;
}
