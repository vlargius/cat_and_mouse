#pragma once

#define SDL_MAIN_HANDLED

#include <iostream>

#include <SDL.h>
#include <SDL_main.h>

struct Color {
    int8_t r;
    int8_t g;
    int8_t b;

	const static Color red;
	const static Color green;
	const static Color blue;
	const static Color black;
	const static Color white;
};

const Color Color::red = {static_cast<int8_t>(255), 0, 0};
const Color Color::green = {0, static_cast<int8_t>(255), 0};
const Color Color::blue = {0, 0, static_cast<int8_t>(255)};
const Color Color::black = {0, 0, 0};
const Color Color::white = {static_cast<int8_t>(255), static_cast<int8_t>(255), static_cast<int8_t>(255)};


struct Render {
    SDL_Window* SdlWindow;
	SDL_Renderer* SdlRender;

    Render() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        }

        SdlWindow = SDL_CreateWindow(
        "cat and mouse",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        0);

	    SdlRender = SDL_CreateRenderer(SdlWindow, -1, 0);
    }

    void flush() {
        SDL_RenderPresent(SdlRender);
    }

    void clear(const Color background = Color::black) {
        SDL_SetRenderDrawColor(SdlRender, background.r, background.g, background.b, 255);
	    SDL_RenderClear(SdlRender);
    }

    void drawRect(int x, int y, int w, int h, const Color color = Color::white) {
        SDL_SetRenderDrawColor(SdlRender, color.r, color.g, color.b, 255);
        SDL_Rect rect{ x, y, w, h};

        SDL_RenderFillRect(SdlRender, &rect);
        SDL_RenderPresent(SdlRender);
    }
};