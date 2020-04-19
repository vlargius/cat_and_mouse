#pragma once

struct InputState {
    bool working = true;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
};

void do_input(InputState& state) {
    SDL_Event sevent;
    while (SDL_PollEvent(&sevent)) {
        switch (sevent.type) {
            case SDL_KEYDOWN:
            {
                if (sevent.key.keysym.sym == SDLK_ESCAPE)
                    state.working = false;
                if (sevent.key.keysym.sym == SDLK_w)
                    state.up = true;
                if (sevent.key.keysym.sym == SDLK_s)
                    state.down = true;
                if (sevent.key.keysym.sym == SDLK_a)
                    state.left = true;
                if (sevent.key.keysym.sym == SDLK_d)
                    state.right = true;
                break;
            }
            case SDL_KEYUP:
            {
                if (sevent.key.keysym.sym == SDLK_w)
                    state.up = false;
                if (sevent.key.keysym.sym == SDLK_s)
                    state.down = false;
                if (sevent.key.keysym.sym == SDLK_a)
                    state.left = false;
                if (sevent.key.keysym.sym == SDLK_d)
                    state.right = false;
                break;
            }
            case SDL_QUIT:
            {
                state.working = false;
                break;
            }
        }
    }
}