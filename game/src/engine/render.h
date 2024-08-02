#pragma once

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <linmath.h>

#include "types.h"

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct render_state {
    f32 window_width;
    f32 window_height;
    f32 render_width;
    f32 render_height;
    f32 scale;
} Render_State;

typedef struct batch_vertex {
    vec2 position;
    vec2 uvs;
    vec4 color;
    f32 texture_slot;
} Batch_Vertex;

typedef struct sprite_sheet {
    f32 width;
    f32 height;
    f32 cell_width;
    f32 cell_height;
    u32 texture_id;
} Sprite_Sheet;

typedef struct glyph {
    vec2 size;
    vec2 offset;
    vec2 advance;
    usize id;
} Glyph;

typedef struct render_data {
    f32 font_height;
    Glyph glyphs[127];
} Render_Data;

typedef struct font {
    bool is_active;
    vec2 pos; // This is top left.
    vec2 size;
    vec2 advance;
    vec4 color;
} Font;

typedef struct text_data {
    f32 font_size;
    vec4 color;
} Text_Data;

#define MAX_BATCH_QUADS 10000
#define MAX_BATCH_VERTICES 40000
#define MAX_BATCH_ELEMENTS 60000

SDL_Window *render_init(void);
void render_begin(vec4 clearcolor);
void render_end(SDL_Window *window, u32 texture_ids[10]);
void render_quad(vec2 pos, vec2 size, vec4 color);
void render_quad_line(vec2 pos, vec2 size, vec4 color);
void render_line_segment(vec2 start, vec2 end, vec4 color);
void render_aabb(f32 *aabb, vec4 color);

void render_sprite_sheet_init(Sprite_Sheet *sprite_sheet, const char *path, f32 cell_width, f32 cell_height);
void render_sprite_sheet_frame(Sprite_Sheet *sprite_sheet, f32 row, f32 column, vec2 position, bool is_flipped, vec4 color, u32 texture_slots[10]);

usize font_create(vec2 pos, vec2 size, vec2 advance, vec4 color);
Font *font_get(usize id);
usize font_count();
void glyph_create(usize index, vec2 size, vec2 offset, vec2 advance);
Glyph *glyph_get(usize id);
usize glyph_count();
void render_text(const char* text, vec2 pos, Text_Data textData, f32 scale_factor, bool is_flipped, u32 texture_slots[10]);
