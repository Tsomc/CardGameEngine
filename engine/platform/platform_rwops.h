#ifndef PLATFORM_RWOPS_H
#define PLATFORM_RWOPS_H

#include <SDL.h>

class RwopsGuard {
public:
    explicit RwopsGuard(SDL_RWops* rw) : rw(rw) {}

    ~RwopsGuard()
    {
        if (rw != nullptr) {
            SDL_RWclose(rw);
        }
    }

    RwopsGuard(const RwopsGuard&) = delete;
    RwopsGuard& operator=(const RwopsGuard&) = delete;

    RwopsGuard(RwopsGuard&& other) noexcept : rw(other.rw)
    {
        other.rw = nullptr;
    }

    RwopsGuard& operator=(RwopsGuard&& other) noexcept
    {
        if (this != &other) {
            if (rw != nullptr) {
                SDL_RWclose(rw);
            }
            rw = other.rw;
            other.rw = nullptr;
        }
        return *this;
    }

    SDL_RWops* Get() const { return rw; }
    bool IsValid() const { return rw != nullptr; }

private:
    SDL_RWops* rw;
};

#endif
