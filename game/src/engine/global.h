#pragma once

#include "render.h"
#include "config.h"
#include "input.h"
#include "time.h"
#include "mouse.h"
#include "game.h"

#define TEX_SLOTS 10

typedef struct global {
    Render_State render;
    Config_State config;
    Input_State input;
    Time_State time;
    Game_State game;
} Global;

extern Global global;
