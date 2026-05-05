#ifndef SPRITE_BATCHER_H
#define SPRITE_BATCHER_H

#include "sprite.h"
#include "resource_handle.h"
#include <vector>
#include <cstdint>

struct BatchedVertex {
    float x, y;
    float u, v;
    uint8_t r, g, b, a;
};

struct BatchGroup {
    ResourceId textureId;
    int layer;
    std::vector<BatchedVertex> vertices;
    std::vector<int> indices;
};

class SpriteBatcher {
public:
    SpriteBatcher();
    ~SpriteBatcher();

    void RenderDrawSprite(const Sprite& sprite);
    void RenderFlush(void* sdlRenderer);
    void RenderClear();

    int RenderGetDrawCallCount() const;
    int RenderGetSpriteCount() const;

private:
    void ComputeVertices(const Sprite& sprite, BatchedVertex* outVerts);

    std::vector<BatchGroup> groups;
    int drawCallCount;
    int spriteCount;
};

#endif
