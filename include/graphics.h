#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "globals.h"
#include "logic.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class SDL_FRect;
class Logic;

class Graphics {
public:
    Graphics(Logic& logic);

    ~Graphics();

    void draw();

    void flip();

    void clear();

    void handleEvent(SDL_Event& event);

    SDL_Renderer* getRenderer() const;

private:
    Logic& _logic;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};