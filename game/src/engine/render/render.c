#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../global.h"
#include "../render.h"
#include "render_internal.h"
#include "../array_list.h"
#include "../util.h"

static f32 window_width = 1920;
static f32 window_height = 1080;
static f32 render_width = 1280;
static f32 render_height = 720;
static f32 scale = 1.5;

static i32 texture_num = 10;
static const f32 cw_origin = 39;
static const f32 ch_origin = 45;
static const f32 w_origin = 507;
static const f32 h_origin = 270;

static u32 vao_quad;
static u32 vbo_quad;
static u32 ebo_quad;
static u32 vao_line;
static u32 vbo_line;
static u32 shader_default;
static u32 texture_color;
static u32 vao_batch;
static u32 vbo_batch;
static u32 ebo_batch;
static u32 shader_batch;
static Array_List *list_batch;
static Array_List *font_list;
static Array_List *glyph_list;

Sprite_Sheet atlas;

SDL_Window *render_init(void) {
    global.render.window_width = window_width;
    global.render.window_height = window_height;
    global.render.render_width = render_width;
    global.render.render_height = render_height;
    global.render.scale = scale;

    SDL_Window *window = render_init_window(window_width, window_height);

    render_init_quad(&vao_quad, &vbo_quad, &ebo_quad);
    render_init_batch_quads(&vao_batch, &vbo_batch, &ebo_batch);
    render_init_line(&vao_line, &vbo_line);
    render_init_shaders(&shader_default, &shader_batch, render_width, render_height);
    render_init_color_texture(&texture_color);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    list_batch = array_list_create(sizeof(Batch_Vertex), 10);
    font_list = array_list_create(sizeof(Font), 0);
    glyph_list = array_list_create(sizeof(Glyph), 127);

    for (usize i = 0; i < 127; ++i) {
        array_list_append(glyph_list, &(Glyph){0});
    }

    f32 offset_x = 20;
    f32 offset_y = 0;

    glyph_create(65, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){0, 0});   // A
    glyph_create(66, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){1, 0});   // B
    glyph_create(67, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){2, 0});   // C
    glyph_create(68, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){3, 0});   // D
    glyph_create(69, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){4, 0});   // E
    glyph_create(70, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){5, 0});   // F
    glyph_create(71, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){6, 0});   // G
    glyph_create(72, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){7, 0});   // H
    glyph_create(73, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){8, 0});   // I
    glyph_create(74, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){9, 0});   // J
    glyph_create(75, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){10, 0});  // K
    glyph_create(76, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){11, 0});  // L
    glyph_create(77, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){12, 0});  // M
    glyph_create(78, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){0, 1});   // N
    glyph_create(79, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){1, 1});   // O
    glyph_create(80, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){2, 1});   // P
    glyph_create(81, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){3, 1});   // Q
    glyph_create(82, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){4, 1});   // R
    glyph_create(83, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){5, 1});   // S
    glyph_create(84, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){6, 1});   // T
    glyph_create(85, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){7, 1});   // U
    glyph_create(86, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){8, 1});   // V
    glyph_create(87, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){9, 1});   // W
    glyph_create(88, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){10, 1});  // X
    glyph_create(89, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){11, 1});  // Y
    glyph_create(90, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){12, 1});  // Z
    glyph_create(48, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){0, 2});   // 0
    glyph_create(49, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){1, 2});   // 1
    glyph_create(50, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){2, 2});   // 2
    glyph_create(51, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){3, 2});   // 3
    glyph_create(52, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){4, 2});   // 4
    glyph_create(53, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){5, 2});   // 5
    glyph_create(54, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){6, 2});   // 6
    glyph_create(55, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){7, 2});   // 7
    glyph_create(56, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){8, 2});   // 8
    glyph_create(57, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){9, 2});   // 9
    glyph_create(33, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){10, 2});  // !
    glyph_create(63, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){11, 2});  // ?
    glyph_create(46, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){0, 3});   // .
    glyph_create(58, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){1, 3});   // :
    glyph_create(40, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){6, 3});   // (
    glyph_create(41, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){7, 3});   // )
    glyph_create(124, (vec2){39,45}, (vec2){offset_x,offset_y}, (vec2){9, 4});  // |
    
    render_sprite_sheet_init(&atlas, "assets/fonts/atlas7.png", w_origin,h_origin);

    stbi_set_flip_vertically_on_load(1);

    return window;
}

static i32 find_texture_slot(u32 texture_slots[10], u32 texture_id) {
    for (i32 i = 1; i < texture_num; ++i) {
        if (texture_slots[i] == texture_id) {
            return i;
        }
    }

    return -1;
}

static i32 try_insert_texture(u32 texture_slots[10], u32 texture_id) {
    
    i32 index = find_texture_slot(texture_slots, texture_id);

    if (index > 0) {
        return index;
    }

    for (i32 i = 1; i < texture_num; ++i) {
        if (texture_slots[i] == 0) {
            texture_slots[i] = texture_id;
            return i;
        }
    }

    return -1;
}

void render_begin(vec4 clearcolor) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(clearcolor[0], clearcolor[1], clearcolor[2], clearcolor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    list_batch->len = 0;
}

static void render_batch(Batch_Vertex *vertices, usize count, u32 texture_ids[10]) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_batch);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Batch_Vertex), vertices);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_color);

    for (u32 i = 1; i < 10; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
    }

    glUseProgram(shader_batch);
    glBindVertexArray(vao_batch);

    glDrawElements(GL_TRIANGLES, (count >> 2) * 6, GL_UNSIGNED_INT, NULL);
}

static void append_quad(vec2 position, vec2 size, vec4 texture_coordinates, vec4 color, f32 texture_slot) {
    vec4 uvs = {0, 0, 1, 1};

    if (texture_coordinates != NULL) {
        memcpy(uvs, texture_coordinates, sizeof(vec4));
    }

    array_list_append(list_batch, &(Batch_Vertex){
        .position = { position[0], position[1] },
        .uvs = { uvs[0], uvs[1] },
        .color = { color[0], color[1], color[2], color[3] },
        .texture_slot = texture_slot,
    });

    array_list_append(list_batch, &(Batch_Vertex){
        .position = { position[0] + size[0], position[1] },
        .uvs = { uvs[2], uvs[1] },
        .color = { color[0], color[1], color[2], color[3] },
        .texture_slot = texture_slot,
    });

    array_list_append(list_batch, &(Batch_Vertex){
        .position = { position[0] + size[0], position[1] + size[1] },
        .uvs = { uvs[2], uvs[3] },
        .color = { color[0], color[1], color[2], color[3] },
        .texture_slot = texture_slot,
    });

    array_list_append(list_batch, &(Batch_Vertex){
        .position = { position[0], position[1] + size[1] },
        .uvs = { uvs[0], uvs[3] },
        .color = { color[0], color[1], color[2], color[3] },
        .texture_slot = texture_slot,
    });
}

void render_end(SDL_Window *window, u32 texture_ids[10]) {
    render_batch(list_batch->items, list_batch->len, texture_ids);

    // Reset for next Frame
    font_list->len = 0;

    SDL_GL_SwapWindow(window);
}

void render_quad(vec2 pos, vec2 size, vec4 color) {
    glUseProgram(shader_default);

    mat4x4 model;
    mat4x4_identity(model);

    mat4x4_translate(model, pos[0], pos[1], 0);
    mat4x4_scale_aniso(model, model, size[0], size[1], 1);

    glUniformMatrix4fv(glGetUniformLocation(shader_default, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform4fv(glGetUniformLocation(shader_default, "color"), 1, color);
    
    glBindVertexArray(vao_quad);

    glBindTexture(GL_TEXTURE_2D, texture_color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}

void render_line_segment(vec2 start, vec2 end, vec4 color) {
    glUseProgram(shader_default);
    glLineWidth(3);

    f32 x = end[0] - start[0];
    f32 y = end[1] - start[1];
    f32 line[6] = {0, 0, 0, x, y, 0};

    mat4x4 model;
    mat4x4_translate(model, start[0], start[1], 0);

    glUniformMatrix4fv(glGetUniformLocation(shader_default, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform4fv(glGetUniformLocation(shader_default, "color"), 1, color);
    
    glBindTexture(GL_TEXTURE_2D, texture_color);
    glBindVertexArray(vao_line);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line), line);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
}

void render_quad_line(vec2 pos, vec2 size, vec4 color) {
    vec2 points[4] = {
        {pos[0] - size[0] * 0.5, pos[1] - size[1] * 0.5},
        {pos[0] + size[0] * 0.5, pos[1] - size[1] * 0.5},
        {pos[0] + size[0] * 0.5, pos[1] + size[1] * 0.5},
        {pos[0] - size[0] * 0.5, pos[1] + size[1] * 0.5},
    };

    render_line_segment(points[0], points[1], color);
    render_line_segment(points[1], points[2], color);
    render_line_segment(points[2], points[3], color);
    render_line_segment(points[3], points[0], color);
}

void render_aabb(f32 *aabb, vec4 color) {
    vec2 size;
    vec2_scale(size, &aabb[2], 2);
    render_quad_line(&aabb[0], size, color);
}

void render_sprite_sheet_init(Sprite_Sheet *sprite_sheet, const char *path, f32 cell_width, f32 cell_height) {
    glGenTextures(1, &sprite_sheet->texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_sheet->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, channel_count;
    u8 *image_data = stbi_load(path, &width, &height, &channel_count, 0);
    if (!image_data) {
        ERROR_EXIT("Failed to load image: %s\n", path);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    sprite_sheet->width = (f32)width;
    sprite_sheet->height = (f32)height;
    sprite_sheet->cell_width = cell_width;
    sprite_sheet->cell_height = cell_height;
}

static void calculate_sprite_texture_coordinates(vec4 result, f32 row, f32 column, f32 texture_width, f32 texture_height, f32 cell_width, f32 cell_heigth) {
    f32 w = 1.0 / (texture_width / cell_width);
    f32 h = 1.0 / (texture_height / cell_heigth);
    f32 x = row * w;
    f32 y = column * h;
    result[0] = x;
    result[1] = y;
    result[2] = x + w;
    result[3] = y + h;
}

void render_sprite_sheet_frame(Sprite_Sheet *sprite_sheet, f32 row, f32 column, vec2 position, bool is_flipped, vec4 color, u32 texture_slots[10]) {
    vec4 uvs;
    calculate_sprite_texture_coordinates(uvs, row, column, sprite_sheet->width, 
    sprite_sheet->height, sprite_sheet->cell_width, sprite_sheet->cell_height);

    if (is_flipped) {
        f32 tmp = uvs[1];
        uvs[1] = uvs[3];
        uvs[3] = tmp;
    }

    vec2 size = {sprite_sheet->cell_width, sprite_sheet->cell_height};
    vec2 bottom_left = {position[0] - size[0] * 0.5, position[1] - size[1] * 0.5};
    i32 texture_slot = try_insert_texture(texture_slots, sprite_sheet->texture_id);
    if (texture_slot == -1) {
        // TODO flush buffer here.
    }

    append_quad(bottom_left, size, uvs, color, (f32)texture_slot);
}

usize font_create(vec2 pos, vec2 size, vec2 advance, vec4 color) {
    usize id = font_list->len;

    // Find inactive font
    for (usize i = 0; i < font_list->len; ++i) {
        Font *font = array_list_get(font_list, i);
        if (!font->is_active) {
            id = i;
            break;
        }
    }

    if (id == font_list->len) {
        if (array_list_append(font_list, &(Font){0}) == (usize)-1) {
            ERROR_EXIT("Could not append font to list\n");
        }
    }

    Font *font = font_get(id);
    
    *font = (Font){
        .is_active = true,
        .pos = { pos[0], pos[1] },
        .size = { size[0], size[1] },
        .advance = { advance[0], advance[1] },
        .color = { color[0], color[1], color[2], color[3] },
    };

    return id;
}

Font *font_get(usize id) {
    return array_list_get(font_list, id);
}

usize font_count() {
    return font_list->len;
}

void glyph_create(usize index, vec2 size, vec2 offset, vec2 advance) {
    Glyph *glyph = glyph_get(index);
    *glyph = (Glyph) {
        .size = { size[0], size[1] },
        .offset = { offset[0], offset[1] },
        .advance = { advance[0], advance[1] },
    };
}

Glyph *glyph_get(usize id) {
    return array_list_get(glyph_list, id);
}

usize glyph_count() {
    return glyph_list->len;
}

void render_text(const char* text, vec2 pos, Text_Data textData, f32 scale_factor, bool is_flipped, u32 texture_slots[10]) {
    assert(text);
    if(!text)
    {
        ERROR_EXIT("No Text Supplied!");
        return;
    }

    is_flipped = !is_flipped;

    atlas.cell_width = cw_origin*scale_factor;
    atlas.cell_height = ch_origin*scale_factor;
    atlas.width = w_origin*scale_factor;
    atlas.height = h_origin*scale_factor;

    vec2 origin = { pos[0], pos[1] };

    char c;
    while(c = *(text++))
    {
        if(c == '\n')
        {
            pos[0] = origin[0];
            pos[1] -= 40*scale_factor;
            continue;
        } else if (c == ' ') {
            pos[0] += 10*scale_factor;
            continue;
        }

        c = SDL_toupper(c);

        Glyph *glyph = glyph_get(c);

        usize font_id = font_create((vec2){pos[0] + glyph->offset[0], pos[1] - glyph->offset[1]},
                                    (vec2){glyph->size[0], glyph->size[1]},
                                    (vec2){glyph->advance[0], glyph->advance[1]},
                                    (vec4){textData.color[0], textData.color[1], textData.color[2], textData.color[3]});

        Font *font = font_get(font_id);

        render_sprite_sheet_frame(&atlas, font->advance[0], font->advance[1], pos, is_flipped, font->color, texture_slots);

        // Advance the Glyph
        pos[0] += glyph->offset[0]*scale_factor;
    }

    atlas.cell_width = cw_origin;
    atlas.cell_height = ch_origin;
    atlas.width = w_origin;
    atlas.height = h_origin;
}
