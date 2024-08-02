#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <sodium.h>

#include "engine/global.h"
#include "engine/config.h"
#include "engine/input.h"
#include "engine/mouse.h"
#include "engine/time.h"
#include "engine/physics.h"
#include "engine/util.h"
#include "engine/entity.h"
#include "engine/render.h"
#include "engine/animation.h"
#include "engine/audio.h"
#include "engine/game.h"

#define MAX_PLAYER_CARDS 15

typedef enum collision_layer {
    COLLISION_LAYER_PLAYER = 1,
    COLLISION_LAYER_ENEMY = 1 << 1,
    COLLISION_LAYER_TERRAIN = 1 << 2,
    COLLISION_LAYER_ENEMY_PASSTHROUGH = 1 << 3,
    COLLISION_LAYER_PROJECTILE = 1 << 4,
} Collision_Layer;

static const f32 PLAYER_CARD_MIN_HEIGHT = 0;
static const f32 TIME_DELAY = 500;

static f32 SPEED_PLAYER;
static f32 SPEED_CARD;
static f32 CARD_SIZE_WIDTH;
static f32 CARD_SIZE_HEIGHT;
static f32 ENEMY_CARD_MIN_HEIGHT;
static f32 PLAYER_CARD_MAX_HEIGHT;
static f32 RELATIVE_WIDTH_SCALE;
static f32 RELATIVE_HEIGHT_SCALE;
static bool is_doubleDmg_p;
static bool is_doubleDmg_e;
static f32 render_width, render_height;
static u32 texture_slots[TEX_SLOTS] = {0};
static u32 time_ticker;

// Entities
static usize entity_sigil_1_id;
static usize entity_sigil_2_id;
static usize entity_dragon_card_id;
static usize entity_card_1_id;
static usize entity_card_2_id;
static usize entity_card_3_id;
static usize entity_card_4_id;
static usize entity_card_5_id;
static usize entity_start_id;
static usize entity_gallery_id;
static usize entity_exit_id;
static usize entity_slot_1_id;
static usize entity_slot_2_id;
static usize entity_slot_3_id;
static usize entity_slot_4_id;
static usize entity_slot_5_id;
static usize entity_slot_6_id;
static usize entity_slot_7_id;
static usize entity_slot_8_id;
static usize entity_slot_9_id;

// Buttons
static usize button_sigil_1_id;
static usize button_sigil_2_id;
static usize button_start_id;
static usize button_gallery_id;
static usize button_exit_id;
static usize button_slot_1_id;
static usize button_slot_2_id;
static usize button_slot_3_id;
static usize button_slot_4_id;
static usize button_slot_5_id;
static usize button_slot_6_id;
static usize button_slot_7_id;
static usize button_slot_8_id;
static usize button_slot_9_id;

// Global cards.
static usize card_1_id;
static usize card_2_id;
static usize card_3_id;
static usize card_4_id;
static usize card_5_id;

// Player cards.
static usize p_card_1_id;
static usize p_card_2_id;
static usize p_card_3_id;

// Enemy cards.
static usize e_card_1_id;
static usize e_card_2_id;
static usize e_card_3_id;

// Board slots.
static usize player_slot_1_id;
static usize player_slot_2_id;
static usize player_slot_3_id;

static usize enemy_slot_1_id;
static usize enemy_slot_2_id;
static usize enemy_slot_3_id;

// Hand slots.
static usize player_hand_slot_1_id;
static usize player_hand_slot_2_id;
static usize player_hand_slot_3_id;
static usize player_hand_slot_4_id;
static usize player_hand_slot_5_id;
static usize enemy_hand_slot_1_id;
static usize enemy_hand_slot_2_id;
static usize enemy_hand_slot_3_id;
static usize enemy_hand_slot_4_id;
static usize enemy_hand_slot_5_id;

// Animation ids.
static usize anim_sigil_1_id;
static usize anim_sigil_2_id;
static usize anim_dragon_id;
static usize anim_card_1_id;
static usize anim_card_2_id;
static usize anim_card_3_id;
static usize anim_card_4_id;
static usize anim_card_5_id;

static Player player;
static Enemy enemy;

static u8 player_mask = COLLISION_LAYER_ENEMY | COLLISION_LAYER_TERRAIN;
static u8 enemy_mask = COLLISION_LAYER_PLAYER | COLLISION_LAYER_TERRAIN;

void player_on_hit(Body *self, Body *other, Hit hit) {
    if (other->collision_layer == COLLISION_LAYER_ENEMY) {
        // ???
    }
}

void enemy_on_hit(Body *self, Body *other, Hit hit) {
    if (other->collision_layer == COLLISION_LAYER_PLAYER) {
        // ???
    }
}

void player_on_hit_static(Body *self, Static_Body *other, Hit hit) {
    if (hit.normal[1] > 0) {
        // ???
    }
}

void enemy_on_hit_static(Body *self, Static_Body *other, Hit hit) {
    if (hit.normal[1] > 0) {
        // ???
    }
}

void spawn_card(usize id, bool is_player) {
    usize copy_id = move_card_to_playable_list(id, is_player);
    Card *card = playable_card_get(copy_id, is_player);

    if (is_hand_slot_available(is_player)) {
        usize hand_slot_id = available_hand_slot_get(is_player);
        Slot *hand_slot = hand_slot_get(hand_slot_id, is_player);
        hand_slot->is_taken = true;
        card->hand_slot_id = hand_slot_id;

        vec2 position = {hand_slot->position[0], hand_slot->position[1]};
        vec2 size = {CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT};
        vec2 sprite_offset = {0, 0};
        usize animation_id = card->animation_id;
        On_Hit_Static on_hit_static = enemy_on_hit_static;
        vec2 velocity = {0, 0};
        usize entity_id = entity_create(position, size, sprite_offset, velocity, COLLISION_LAYER_ENEMY, enemy_mask, true, animation_id, copy_id, is_player, TYPE_CARD, NULL, on_hit_static);
        card->entity_id = entity_id;
    } else {
        printf("No HAND slot.\n");
    }
}

void reset(void) {
    if (global.game.is_menu){
        physics_reset();
        entity_reset();
        game_reset(player, enemy);

        // Init menu buttons
        entity_start_id = entity_create((vec2){render_width/2-40, render_height/2}, (vec2){600, 70}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, NULL, NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
        Entity *start_entity = entity_get(entity_start_id);
        start_entity->is_button = true;
        button_start_id = button_create(entity_start_id, (usize)2, true);

        entity_gallery_id = entity_create((vec2){render_width/2-40, render_height/2 - 40*2}, (vec2){600, 70}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, NULL, NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
        Entity *gallery_entity = entity_get(entity_gallery_id);
        gallery_entity->is_button = true;
        button_gallery_id = button_create(entity_gallery_id, (usize)3, true);

        entity_exit_id = entity_create((vec2){render_width/2-40, render_height/2 - 80*2}, (vec2){600, 70}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, NULL, NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
        Entity *exit_entity = entity_get(entity_exit_id);
        exit_entity->is_button = true;
        button_exit_id = button_create(entity_exit_id, (usize)4, true);
    
    }

    else if (!global.game.is_menu && global.game.is_gallery) {
        physics_reset();
        entity_reset();
        game_reset(player, enemy);
      
        if (global.game.gachart_id ==  ((usize)-1)) {
            vec2 pos = {340, 210};
            entity_slot_1_id = entity_create((vec2){pos[0], render_height-pos[1]}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(0), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_1_entity = entity_get(entity_slot_1_id);
            slot_1_entity->is_button = true;
            button_slot_1_id = button_create(entity_slot_1_id, (usize)5, true);

            entity_slot_2_id = entity_create((vec2){pos[0]+150, render_height-pos[1]}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(1), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_2_entity = entity_get(entity_slot_2_id);
            slot_2_entity->is_button = true;
            button_slot_2_id = button_create(entity_slot_2_id, (usize)6, true);

            entity_slot_3_id = entity_create((vec2){pos[0]+300, render_height-pos[1]}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(2), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_3_entity = entity_get(entity_slot_3_id);
            slot_3_entity->is_button = true;
            button_slot_3_id = button_create(entity_slot_3_id, (usize)7, true);

            entity_slot_4_id = entity_create((vec2){pos[0]+450, render_height-pos[1]}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(3), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_4_entity = entity_get(entity_slot_4_id);
            slot_4_entity->is_button = true;
            button_slot_4_id = button_create(entity_slot_4_id, (usize)8, true);

            entity_slot_5_id = entity_create((vec2){pos[0]+600, render_height-pos[1]}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(4), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_5_entity = entity_get(entity_slot_5_id);
            slot_5_entity->is_button = true;
            button_slot_5_id = button_create(entity_slot_5_id, (usize)9, true);

            entity_slot_6_id = entity_create((vec2){pos[0], render_height-pos[1]*2.1}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(5), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_6_entity = entity_get(entity_slot_6_id);
            slot_6_entity->is_button = true;
            button_slot_6_id = button_create(entity_slot_6_id, (usize)10, true);

            entity_slot_7_id = entity_create((vec2){pos[0]+150, render_height-pos[1]*2.1}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(6), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_7_entity = entity_get(entity_slot_7_id);
            slot_7_entity->is_button = true;
            button_slot_7_id = button_create(entity_slot_7_id, (usize)11, true);

            entity_slot_8_id = entity_create((vec2){pos[0]+300, render_height-pos[1]*2.1}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(7), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_8_entity = entity_get(entity_slot_8_id);
            slot_8_entity->is_button = true;
            button_slot_8_id = button_create(entity_slot_8_id, (usize)12, true);

            entity_slot_9_id = entity_create((vec2){pos[0]+450, render_height-pos[1]*2.1}, (vec2){CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, gacha_get_anim_id(8), NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
            Entity *slot_9_entity = entity_get(entity_slot_9_id);
            slot_9_entity->is_button = true;
            button_slot_9_id = button_create(entity_slot_9_id, (usize)13, true);
        }
    }

    else if (!global.game.is_menu && !global.game.is_cardgame) {
        physics_reset();
        entity_reset();
        game_reset(player, enemy);
    }

    else if (!global.game.is_menu && global.game.is_cardgame) {
        physics_reset();
        entity_reset();

        u32 enemy_roll;

        if (global.game.is_enemy) {
            enemy_roll = random_number(3);
            const Enemy *enemy_point = enemy_get(enemy_roll);
            enemy = (Enemy) {
                .health = enemy_point->health,
            };
        }
        else if (global.game.is_boss) {
            enemy_roll = (u32)5;
            const Enemy *enemy_point = enemy_get(enemy_roll);
            enemy = (Enemy) {
                .health = enemy_point->health,
            };
        } else {ERROR_EXIT("Is not enemy or boss!\n");}

        printf("Enemy Roll: %u\n", enemy_roll);

        game_reset(player, enemy);

        // Init buttons
        entity_sigil_1_id = entity_create((vec2){840, 335}, (vec2){145, 145}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, anim_sigil_1_id, NULL, true, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
        Entity *sigil_1 = entity_get(entity_sigil_1_id);
        sigil_1->is_button = true;
        button_sigil_1_id = button_create(entity_sigil_1_id, (usize)0, true);

        entity_sigil_2_id = entity_create((vec2){440, 335}, (vec2){145, 145}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_ENEMY, enemy_mask, true, anim_sigil_2_id, NULL, false, TYPE_BUTTON, enemy_on_hit, enemy_on_hit_static);
        Entity *sigil_2 = entity_get(entity_sigil_2_id);
        sigil_2->is_button = true;
        button_sigil_2_id = button_create(entity_sigil_2_id, (usize)1, true);

        // Init board slots
        player_slot_1_id = slot_create((vec2){110*RELATIVE_WIDTH_SCALE, 110*RELATIVE_HEIGHT_SCALE}, true);
        player_slot_2_id = slot_create((vec2){320*RELATIVE_WIDTH_SCALE, 120*RELATIVE_HEIGHT_SCALE}, true);
        player_slot_3_id = slot_create((vec2){520*RELATIVE_WIDTH_SCALE, 110*RELATIVE_HEIGHT_SCALE}, true);

        enemy_slot_1_id = slot_create((vec2){110*RELATIVE_WIDTH_SCALE, 250*RELATIVE_HEIGHT_SCALE}, false);
        enemy_slot_2_id = slot_create((vec2){320*RELATIVE_WIDTH_SCALE, 240*RELATIVE_HEIGHT_SCALE}, false);
        enemy_slot_3_id = slot_create((vec2){520*RELATIVE_WIDTH_SCALE, 250*RELATIVE_HEIGHT_SCALE}, false);

        // Init hand slots
        player_hand_slot_1_id = hand_slot_create((vec2){140*RELATIVE_WIDTH_SCALE, PLAYER_CARD_MIN_HEIGHT}, true);
        player_hand_slot_2_id = hand_slot_create((vec2){230*RELATIVE_WIDTH_SCALE, PLAYER_CARD_MIN_HEIGHT}, true);
        player_hand_slot_3_id = hand_slot_create((vec2){320*RELATIVE_WIDTH_SCALE, PLAYER_CARD_MIN_HEIGHT}, true);
        player_hand_slot_4_id = hand_slot_create((vec2){410*RELATIVE_WIDTH_SCALE, PLAYER_CARD_MIN_HEIGHT}, true);
        player_hand_slot_5_id = hand_slot_create((vec2){500*RELATIVE_WIDTH_SCALE, PLAYER_CARD_MIN_HEIGHT}, true);

        enemy_hand_slot_1_id = hand_slot_create((vec2){140*RELATIVE_WIDTH_SCALE, ENEMY_CARD_MIN_HEIGHT}, false);
        enemy_hand_slot_2_id = hand_slot_create((vec2){230*RELATIVE_WIDTH_SCALE, ENEMY_CARD_MIN_HEIGHT}, false);
        enemy_hand_slot_3_id = hand_slot_create((vec2){320*RELATIVE_WIDTH_SCALE, ENEMY_CARD_MIN_HEIGHT}, false);
        enemy_hand_slot_4_id = hand_slot_create((vec2){410*RELATIVE_WIDTH_SCALE, ENEMY_CARD_MIN_HEIGHT}, false);
        enemy_hand_slot_5_id = hand_slot_create((vec2){500*RELATIVE_WIDTH_SCALE, ENEMY_CARD_MIN_HEIGHT}, false);

        int j;
        
        // Init decks
        for (usize i = 0; i < total_player_cardpool_count(); i++) {
            create_deck_card(i, true);
        }

        if (enemy_roll == 0) {
            for (j = 0; j < 8; ++j) {
                create_deck_card(card_1_id, false);
            }
            for (j = 0; j < 2; ++j) {
                create_deck_card(card_4_id, false);
            }
        } else if (enemy_roll == 1) {
            for (j = 0; j < 5; ++j) {
                create_deck_card(card_2_id, false);
            }
            for (j = 0; j < 5; ++j) {
                create_deck_card(card_5_id, false);
            }
        } else if (enemy_roll == 2) {
            for (j = 0; j < 4; ++j) {
                create_deck_card(card_4_id, false);
            }
            for (j = 0; j < 3; ++j) {
                create_deck_card(card_2_id, false);
            }
            for (j = 0; j < 3; ++j) {
                create_deck_card(card_5_id, false);
            }
        } else if (enemy_roll == 3) {
            for (j = 0; j < 2; ++j) {
                create_deck_card(card_3_id, false);
            }
            for (j = 0; j < 6; ++j) {
                create_deck_card(card_1_id, false);
            }
            for (j = 0; j < 2; ++j) {
                create_deck_card(card_4_id, false);
            }
        } else if (enemy_roll == 4) {
            for (j = 0; j < 4; ++j) {
                create_deck_card(card_3_id, false);
            }
            for (j = 0; j < 2; ++j) {
                create_deck_card(card_2_id, false);
            }
            for (j = 0; j < 4; ++j) {
                create_deck_card(card_4_id, false);
            }
        } else if (enemy_roll == 5) {
            for (j = 0; j < 10; ++j) {
                create_deck_card(card_3_id, false);
            }
        } else {ERROR_EXIT("WRONG ENEMY ROLL!\n");}

        // Deploy random cards
        while (is_hand_slot_available(false) && !deck_card_count(false) < 1) {
            spawn_card(random_number(deck_card_count(false)), false);
        }
        while (is_hand_slot_available(true) && !deck_card_count(true) < 1) {
            spawn_card(random_number(deck_card_count(true)), true);
        }

        printf("Your Health: %u\n", player.health);
        printf("Opponent Health: %u\n", enemy.health);
    }
}

static void cardSelection(Body *self) {
    if (global.input.menu) {
        render_aabb((f32*)self, RED);
        int x = self->aabb.position[0];
        int y = self->aabb.position[1];
        printf("X coord: %d\nY coord: %d\n", x, y);
        int w = self->aabb.size[0];
        int h = self->aabb.size[1];
        printf("Width: %d\nHeight: %d\n", w, h);
    }
}

static void moveToTarget(Body* self, f32 targetX, f32 targetY) {
    // Calculate direction towards target
    f32 dirX = targetX - self->aabb.position[0];
    f32 dirY = targetY - self->aabb.position[1];

    f32 difX = abs(targetX - self->aabb.position[0]);
    f32 difY = abs(targetY - self->aabb.position[1]);

    f32 margX = (f32)2 * RELATIVE_WIDTH_SCALE;
    f32 margY = (f32)2 * RELATIVE_HEIGHT_SCALE;

    // Normalize direction
    f32 length = sqrt(dirX * dirX + dirY * dirY);
    if (length > 0) {
        dirX /= length;
        dirY /= length;
    }

    // Set velocity in the direction of the target
    self->velocity[0] = dirX * SPEED_CARD;
    self->velocity[1] = dirY * SPEED_CARD;

    if (difX < margX && difY < margY) {
        Entity* entity = entity_get(self->entity_id);
        Card *card = playable_card_get(entity->card_id, entity->is_player);
        card->is_deployed = true;
        self->velocity[0] = (f32)0;
        self->velocity[1] = (f32)0;
    }
}

static void enemies_init(void) {
    enemy_create(100);
    enemy_create(105);
    enemy_create(110);
    enemy_create(200);
    enemy_create(200);
    enemy_create(200);

}

static void ai_turn(void) {
    if (!global.game.board_game.is_playing_anim) {
        if (global.game.board_game.is_ai_turn) {
            printf("Ending turn... \n");
            if (!is_hand_empty(false) && !is_board_full(false)) {
                for (usize i = 0; i < playable_card_count(false); ++i) {
                    Card *backup_card = playable_card_get(i, false);
                    if (!backup_card->is_deployed) {
                        backup_card->is_moving = true;
                        usize slot_id = available_slot_get(false);
                        Slot *slot = slot_get(slot_id, false);
                        Entity *entity = entity_get(backup_card->entity_id);
                        entity->slot_id = slot_id;
                        break;
                    }
                }
            }
            else if (is_hand_empty(false)) {
                printf("Enemy hand is empty!\n");
                if (deck_card_count(false) > 0) {
                    printf("Enemy draws a card from deck.[HAND_EMPTY]\n");
                    SDL_Delay(TIME_DELAY);
                    spawn_card(random_number(deck_card_count(false)), false);
                }
                game_loop();
                global.game.board_game.is_ai_turn = false;
            }
            else if (is_board_full(false)) {
                printf("Enemy board is full!\n");
                if (is_hand_slot_available(false) && deck_card_count(false) > 0) {
                    printf("Enemy draws a card from deck.[BOARD_FULL]\n");
                    SDL_Delay(TIME_DELAY);
                    spawn_card(random_number(deck_card_count(false)), false);
                }
                game_loop();
                global.game.board_game.is_ai_turn = false;
            }
            else {
                printf("Enemy skipping deployment...\n");
            }
        }
    }
}

static void clear_texture_slots(void) {
    for (u32 i = 1; i < TEX_SLOTS; ++i) {
        texture_slots[i] = 0;
    }
}

int main(int argc, char *argv[]) {
    time_init(60);
    config_init();
    SDL_Window *window = render_init();
    physics_init();
    entity_init();
    animation_init();
    audio_init();
    game_init();

    RELATIVE_WIDTH_SCALE = (global.render.render_width / (f32)640);
    RELATIVE_HEIGHT_SCALE = (global.render.render_height / (f32)360);
    PLAYER_CARD_MAX_HEIGHT = (f32)60 * RELATIVE_HEIGHT_SCALE;
    ENEMY_CARD_MIN_HEIGHT = global.render.render_height;
    CARD_SIZE_WIDTH = (f32)71 * RELATIVE_WIDTH_SCALE;
    CARD_SIZE_HEIGHT = (f32)105 * RELATIVE_HEIGHT_SCALE;
    SPEED_PLAYER = (f32)250 * RELATIVE_HEIGHT_SCALE;
    SPEED_CARD = (f32)200 * RELATIVE_HEIGHT_SCALE;

    vec4 background_color = {0.08, 0.1, 0.1, 1};
    vec4 gallery_color = {0.23, 0.27, 0.31, 1};

    time_ticker = 0;
    u32 card_roll = (u32)-1;
    char health_string[50];
    char points_string[50];
    char pool_string[50];
    char card_name_string[100];

    char enemy_hp_string[50];
    char player_hp_string[50];

    Text_Data text_cardgame;
    text_cardgame = (Text_Data) {
        .font_size = 2,
        .color = {1, 0, 0, 1},
    };

    Text_Data text_roguelite;
    text_roguelite = (Text_Data) {
        .font_size = 2,
        .color = {1, 0.56, 0.91, 1},
    };

    Dungeon dungeon;

    // Player Init.
    player = (Player) {
        .health = 100,
        .x = 0,
        .y = 0,
    };

    load_dungeon(&dungeon, &player, "assets/maps/final_dungeon_map.txt");

    enemies_init();

    SDL_ShowCursor(true);

    i32 window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    render_width = global.render.render_width;
    render_height = global.render.render_height;

    Entity_Type entity_type_e;

    Sprite_Sheet sprite_sheet_sigil_1;
    Sprite_Sheet sprite_sheet_sigil_2;
    Sprite_Sheet sprite_sheet_card_1;
    Sprite_Sheet sprite_sheet_card_2;
    Sprite_Sheet sprite_sheet_card_3;
    Sprite_Sheet sprite_sheet_card_4;
    Sprite_Sheet sprite_sheet_card_5;

    render_sprite_sheet_init(&sprite_sheet_sigil_1, "assets/textures/cardgame/sigil_1.png", 158, 158);
    render_sprite_sheet_init(&sprite_sheet_sigil_2, "assets/textures/cardgame/sigil_2.png", 158, 158);
    render_sprite_sheet_init(&sprite_sheet_card_1, "assets/textures/cardgame/card_oracle.png", CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT);
    render_sprite_sheet_init(&sprite_sheet_card_2, "assets/textures/cardgame/card_engineer.png", CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT);
    render_sprite_sheet_init(&sprite_sheet_card_3, "assets/textures/cardgame/card_dreamweaver.png", CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT);
    render_sprite_sheet_init(&sprite_sheet_card_4, "assets/textures/cardgame/card_guardian.png", CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT);
    render_sprite_sheet_init(&sprite_sheet_card_5, "assets/textures/cardgame/card_technician.png", CARD_SIZE_WIDTH, CARD_SIZE_HEIGHT);

    gacha_create("assets/textures/gacha/GS_00.png", 142, 210, "assets/textures/gacha/GL_00.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_01.png", 142, 210, "assets/textures/gacha/GL_01.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_02.png", 142, 210, "assets/textures/gacha/GL_02.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_03.png", 142, 210, "assets/textures/gacha/GL_03.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_04.png", 142, 210, "assets/textures/gacha/GL_04.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_05.png", 142, 210, "assets/textures/gacha/GL_05.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_06.png", 142, 210, "assets/textures/gacha/GL_06.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_07.png", 142, 210, "assets/textures/gacha/GL_07.png", 413, 600);
    gacha_create("assets/textures/gacha/GS_08.png", 142, 210, "assets/textures/gacha/GL_08.png", 413, 600);

    for (usize i = 0; i < gacha_count(); i++) {
        Gacha *gacha = gacha_get(i);
        usize adef = animation_definition_create(&gacha->sprite_sheet_small, 0, 0, (u8[]){0}, (u8*)1);
        gacha->anim_id = animation_create(adef, false);
    }

    usize adef_sigil_1_id = animation_definition_create(&sprite_sheet_sigil_1, 0, 0, (u8[]){0}, (u8*)1);
    usize adef_sigil_2_id = animation_definition_create(&sprite_sheet_sigil_2, 0, 0, (u8[]){0}, (u8*)1);
    usize adef_card_1_id = animation_definition_create(&sprite_sheet_card_1, 0, 0, (u8[]){0}, (u8*)1);
    usize adef_card_2_id = animation_definition_create(&sprite_sheet_card_2, 0, 0, (u8[]){0}, (u8*)1);
    usize adef_card_3_id = animation_definition_create(&sprite_sheet_card_3, 0, 0, (u8[]){0}, (u8*)1);
    usize adef_card_4_id = animation_definition_create(&sprite_sheet_card_4, 0, 0, (u8[]){0}, (u8*)1);
    usize adef_card_5_id = animation_definition_create(&sprite_sheet_card_5, 0, 0, (u8[]){0}, (u8*)1);

    anim_sigil_1_id = animation_create(adef_sigil_1_id, false);
    anim_sigil_2_id = animation_create(adef_sigil_2_id, false);
    anim_card_1_id = animation_create(adef_card_1_id, false);
    anim_card_2_id = animation_create(adef_card_2_id, false);
    anim_card_3_id = animation_create(adef_card_3_id, false);
    anim_card_4_id = animation_create(adef_card_4_id, false);
    anim_card_5_id = animation_create(adef_card_5_id, false);

    // Init cards
    card_1_id = card_create("Oracle", 27, 3, MYSTIC, anim_card_1_id);
    card_2_id = card_create("Engineer", 26, 4, TECH, anim_card_2_id);
    card_3_id = card_create("Dreamweaver", 21, 9, DREAM, anim_card_3_id);
    card_4_id = card_create("Guardian", 29, 1, REALITY, anim_card_4_id);
    card_5_id = card_create("Technician", 26, 4, TECH, anim_card_5_id);

    for (int i = 0; i < 2; ++i) {
        total_player_cardpool_append(card_1_id);
    }
    for (int i = 0; i < 2; ++i) {
        total_player_cardpool_append(card_2_id);
    }
    for (int i = 0; i < 2; ++i) {
        total_player_cardpool_append(card_3_id);
    }
    for (int i = 0; i < 2; ++i) {
        total_player_cardpool_append(card_4_id);
    }
    for (int i = 0; i < 2; ++i) {
        total_player_cardpool_append(card_5_id);
    }

    reset();

    while(!global.game.should_quit) {

        time_update();

        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            if (global.input.up) {
                move_player(&dungeon, &player, MOVE_UP);
            }
            if (global.input.down) {
                move_player(&dungeon, &player, MOVE_DOWN);
            }
            if (global.input.left) {
                move_player(&dungeon, &player, MOVE_LEFT);
            }
            if (global.input.right) {
                move_player(&dungeon, &player, MOVE_RIGHT);
            }
            if (global.input.menu) {
                if (!global.game.is_cardgame) {
                    global.game.is_menu = true;
                    if (global.game.is_gallery) {
                        global.game.is_gallery=false; 
                        global.game.gachart_id = ((usize)-1);
                    }
                    reset();
                } else {printf("Cannot open menu when in battle!\n");}
            }
            if (global.input.escape) {
                global.game.should_quit = true;
                printf("QUIT");
            }

            switch(event.type) {
                case SDL_QUIT:
                    global.game.should_quit = true;
                    printf("QUIT");
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_b:
                            if (global.game.is_gallery && global.game.gachart_id != ((usize)-1)) {
                                global.game.gachart_id = ((usize)-1);
                                reset();
                            }
                            break;
                        case SDLK_1:
                            total_player_cardpool_remove(random_number(total_player_cardpool_count()));
                            break;
                        case SDLK_2:
                            global.game.board_game.is_match_over = true;
                            break;
                        case SDLK_3:
                            break;
                        case SDLK_4:
                            break;
                        case SDLK_5:
                            break;
                        case SDLK_6:
                            break;
                        case SDLK_7:
                            break;
                        case SDLK_8:
                            break;
                        case SDLK_9:
                            break;
                    }
                    break;
                default:
                    handleMouseEvent(&event);
                    break;
            }
        }

        input_update();

        //! Menu loop !//
        if (global.game.is_menu && !global.game.should_quit) {
            render_begin(background_color);
            render_text("Start | Continue", (vec2){render_width/4, render_height/2}, text_roguelite, 2, false, texture_slots);
            render_text("Gallery", (vec2){render_width/4, render_height/2 - 40*2}, text_roguelite, 2, false, texture_slots);
            render_text("Exit", (vec2){render_width/4, render_height/2 - 80*2}, text_roguelite, 2, false, texture_slots);

            for (usize i = 0; i < entity_count(); ++i) {
                Entity *entity = entity_get(i);
                if (entity->is_button) {
                    Body *body = physics_body_get(entity->body_id);
                    Button *button = button_get(i);
                    f32 default_y = entity->sprite_offset[1];
                    if (button->is_highlighted && button->is_interactible) {
                        render_aabb((f32*)body, RED);
                        f32 offset_y = entity->sprite_offset[1] + (f32)5;
                        entity->sprite_offset[1] = offset_y;
                    } else {
                        render_aabb((f32*)body, CYAN);
                    }
                }
            }
        }

        //! Gallery loop !//
        else if (!global.game.is_menu && global.game.is_gallery && !global.game.should_quit) {
            render_begin(gallery_color);
            if (global.game.gachart_id != ((usize)-1)) {
                Gacha *gacha = gacha_get(global.game.gachart_id);
                if (gacha->is_unlocked) {
                    render_sprite_sheet_frame(&gacha->sprite_sheet_large, 0, 0, (vec2){render_width/2, render_height/2}, false, WHITE, texture_slots);
                    render_text("Press b to go back.", (vec2){40,40}, text_roguelite, 1, false, texture_slots);
                } else {
                    global.game.gachart_id = ((usize)-1);
                    reset();
                }
                
            } else {
                for (usize i = 0; i < entity_count(); ++i) {
                    Entity *entity = entity_get(i);

                    if (!entity->is_active || entity->animation_id == (usize)-1 || !entity->is_button) {
                        continue;
                    }
                    
                    Body *body = physics_body_get(entity->body_id);

                    Button *button = button_get(i);
                    if (button->is_highlighted && button->is_interactible) {
                        render_aabb((f32*)body, MAGENTA);
                    } else {
                        render_aabb((f32*)body, CYAN);
                    }

                    if (is_gacha_unlocked(entity->animation_id)) {
                        Animation *anim = animation_get(entity->animation_id);
                        Animation_Definition *adef = anim->animation_definition_id;
                        Animation_Frame *aframe = &adef->frames[anim->current_frame_index];

                        vec2 pos;

                        vec2_add(pos, body->aabb.position, entity->sprite_offset);
                        animation_render(anim, pos, WHITE, texture_slots);
                    }
                }
            }
        }

        //! Roguelite loop !//
        else if (!global.game.is_menu && !global.game.is_cardgame && !global.game.should_quit) {
            render_begin(background_color);
            render_dungeon(&dungeon, &player, texture_slots);
            sprintf(points_string, "Gacha Coins: %u", global.game.points);
            sprintf(pool_string, "Card Pool: %zu", total_player_cardpool_count());
            render_text(points_string, (vec2) {40,40}, text_roguelite, 1, false, texture_slots);
            render_text(pool_string, (vec2) {1000,40}, text_roguelite, 1, false, texture_slots);
            if (global.game.is_enemy || global.game.is_boss) {
                global.game.is_cardgame = true;
                reset();
            }

            else if (global.game.is_machine_used) {
                //
                time_ticker++;
                if (time_ticker >= 200) {
                    global.game.is_machine_used = false;
                    time_ticker = 0;
                }
            } else if (card_roll != (u32)-1) {
                render_text(card_name_string, (vec2){330,80}, text_roguelite, 1, false, texture_slots);
                time_ticker++;
                if (time_ticker >= 200) {
                    card_roll = (u32)-1;
                    time_ticker = 0;
                }
            }
        }

        //! Cardgame loop !//
        else if (!global.game.is_menu && global.game.is_cardgame && !global.game.should_quit) {

            ai_turn();

            physics_update();
            animation_update(global.time.delta);

            render_begin(background_color);

            //? Debug render bounding boxes & cardlogic
            for (usize i = 0; i < entity_count(); ++i) {
                Entity *entity = entity_get(i);
                Body *body = physics_body_get(entity->body_id);
                Card *card = playable_card_get(entity->card_id, entity->is_player);
                f32 vely = 0;

                if (!entity->is_active || card->is_dead || entity->is_button) {
                    continue;
                }

                if (body->collision_mask == player_mask) {
                    render_aabb((f32*)body, MAGENTA);
                }
                else if (card->has_advantage) {
                    render_aabb((f32*)body, YELLOW);
                }
                else if (card->is_highlighted) {
                    //render_aabb((f32*)body, RED);
                }
                else if (body->is_active) {
                    //render_aabb((f32*)body, TURQUOISE);
                }
                else {
                    //render_aabb((f32*)body, GREEN);
                }
                if (entity->is_player && !global.game.board_game.is_ai_turn) {
                    Slot *slot = slot_get(entity->slot_id, true);
                    if (card->is_highlighted && !card->is_moving && !card->is_deployed) {
                        if (body->aabb.position[1] < PLAYER_CARD_MAX_HEIGHT) {
                            vely += SPEED_CARD;
                        }
                        
                        body->velocity[1] = vely;
                    } 
                    else if (!card->is_highlighted && !card->is_moving && !card->is_deployed) {
                        if (body->aabb.position[1] > PLAYER_CARD_MIN_HEIGHT) {
                            vely -= SPEED_CARD;
                        }
                        
                        body->velocity[1] = vely;
                    }
                    if (!card->is_deployed && card->is_moving) {
                        global.game.board_game.is_playing_anim = true;
                        moveToTarget(body, slot->position[0], slot->position[1]);
                    } else if (card->is_deployed && card->is_moving) {
                        card->is_moving = false;
                        slot->is_taken = true;
                        slot->card_id = entity->card_id;
                        Slot *hand_slot = hand_slot_get(card->hand_slot_id, true);
                        hand_slot->is_taken = false;
                        global.game.board_game.is_ai_turn = true;
                        global.game.board_game.is_playing_anim = false;
                    }
                }
                else if (!entity->is_player && global.game.board_game.is_ai_turn){
                    Slot *slot = slot_get(entity->slot_id, false);
                    if (!card->is_deployed && card->is_moving) {
                        global.game.board_game.is_playing_anim = true;
                        moveToTarget(body, slot->position[0], slot->position[1]);
                    } else if (card->is_deployed && card->is_moving) {
                        card->is_moving = false;
                        slot->is_taken = true;
                        slot->card_id = entity->card_id;
                        Slot *hand_slot = hand_slot_get(card->hand_slot_id, false);
                        hand_slot->is_taken = false;
                        global.game.board_game.is_playing_anim = false;
                        game_loop();
                        global.game.board_game.is_ai_turn = false;
                    }
                    else if (is_hand_empty(entity->is_player) && !global.game.board_game.is_playing_anim) {
                        game_loop();
                        global.game.board_game.is_ai_turn = false;
                    }
                    else if (is_board_full(entity->is_player) && !global.game.board_game.is_playing_anim) {
                        game_loop();
                        global.game.board_game.is_ai_turn = false;
                    }
                }
            }

            for (usize i = 0; i < physics_static_body_count(); ++i) {
                render_aabb((f32*)physics_static_body_get(i), WHITE);
            }

            //? Render animated entities
            for (usize i = 0; i < entity_count(); ++i) {
                Entity *entity = entity_get(i);
                Card *card = playable_card_get(entity->card_id, entity->is_player);

                if (!entity->is_active || entity->animation_id == (usize)-1 || card->is_dead || entity->is_button) {
                    continue;
                }

                Body *body = physics_body_get(entity->body_id);
                Animation *anim = animation_get(entity->animation_id);
                Animation_Definition *adef = anim->animation_definition_id;
                Animation_Frame *aframe = &adef->frames[anim->current_frame_index];

                if (!entity->is_player && !card->is_deployed) {
                    anim->is_flipped = true;
                } else {
                    anim->is_flipped = false;
                }

                vec2 pos;

                vec2_add(pos, body->aabb.position, entity->sprite_offset);
                animation_render(anim, pos, WHITE, texture_slots);

                // Render Text (Card_health)
                sprintf(health_string, "%u", card->health);
                render_text(health_string, (vec2){pos[0]-5, pos[1]}, text_cardgame, 0.7, anim->is_flipped, texture_slots);
            }

            //? Render buttons
            for (usize i = 0; i < entity_count(); ++i) {
                Entity *entity = entity_get(i);
                if (entity->is_button) {
                    Body *body = physics_body_get(entity->body_id);
                    Button *button = button_get(i);
                    f32 default_y = entity->sprite_offset[1];
                    if (button->is_highlighted && button->is_interactible) {
                        f32 offset_y = entity->sprite_offset[1] + (f32)5;
                        entity->sprite_offset[1] = offset_y;
                    } else {
                        //render_aabb((f32*)body, CYAN);
                    }
                    Animation *anim = animation_get(entity->animation_id);
                    Animation_Definition *adef = anim->animation_definition_id;
                    Animation_Frame *aframe = &adef->frames[anim->current_frame_index];
                    vec2 pos;

                    vec2_add(pos, body->aabb.position, entity->sprite_offset);
                    animation_render(anim, pos, WHITE, texture_slots);
                    entity->sprite_offset[1] = default_y;
                }
            }

            sprintf(enemy_hp_string, "HP: %d", global.game.board_game.enemy.health);
            sprintf(player_hp_string, "HP: %d", global.game.board_game.player.health);
            render_text(enemy_hp_string, (vec2){40,680}, text_cardgame, 1, false, texture_slots);
            render_text(player_hp_string, (vec2){40,40}, text_cardgame, 1, false, texture_slots);

            if (global.game.board_game.is_match_over) {
                if (global.game.board_game.player.health == 0) {
                    printf("Game over...\n");
                    global.game.should_quit = true;
                } else {
                    global.game.games_played += 1;
                    global.game.is_cardgame = false;
                    reset();
                    global.game.board_game.is_match_over = false;
                    if (global.game.is_enemy && total_player_cardpool_count() < MAX_PLAYER_CARDS) {
                        card_roll = random_number(global_card_count());
                        const Card* card = global_card_get(card_roll);
                        sprintf(card_name_string, "| You received a %s card! |", card->name);
                        total_player_cardpool_append(card_roll);
                        global.game.is_enemy = false;
                    } else if (global.game.is_enemy && total_player_cardpool_count() >= MAX_PLAYER_CARDS) {
                        card_roll = 0;
                        sprintf(card_name_string, "| You have reached max card limit! ( %d cards ) |", MAX_PLAYER_CARDS);
                        global.game.is_enemy = false;
                    }
                    else if (global.game.is_boss) {
                        card_roll = 0;
                        sprintf(card_name_string, "| You received a %s special card! The game is over! |", global.game.is_boss ? "BOSS" : "BOSS");
                        Gacha *gacha = gacha_get((usize)8);
                        gacha->is_unlocked = true;
                        global.game.is_boss = false;
                    }
                }
            }
        }

        render_end(window, texture_slots);

        clear_texture_slots();

        if (global.game.is_menu && global.game.is_gallery) {global.game.is_menu = false; reset();}

        time_update_late();
    }

    free_dungeon(&dungeon);
    return 0;
}
