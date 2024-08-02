#pragma once

#include <stdbool.h>
#include <linmath.h>
#include <sodium.h>
#include "types.h"
#include "render.h"

#define ROGUE_TEXTURE_DIMENSIONS 60

//! CARDGAME SECTION !//

typedef enum archetype {
    MYSTIC = 1,
    TECH = 1 << 1,
    DREAM = 1 << 2,
    REALITY = 1 << 3,
} Archetype;

typedef struct card {
    const char* name;
    usize animation_id;
    usize hand_slot_id;
    usize entity_id;
    u8 health;
    u8 attack;
    Archetype archetype;
    bool is_dead;
    bool is_highlighted;
    bool is_moving;
    bool is_deployed;
    bool has_advantage;
} Card;

typedef struct slot {
    vec2 position;
    usize card_id;
    bool is_taken;
} Slot;

typedef struct button {
    usize entity_id;
    usize button_id;
    bool is_interactible;
    bool is_highlighted;
} Button;

typedef struct gacha {
    Sprite_Sheet sprite_sheet_small;
    Sprite_Sheet sprite_sheet_large;
    usize anim_id;
    bool is_unlocked;
} Gacha;

typedef struct player {
    u8 health;
    int x, y;
} Player;

typedef struct enemy {
    u8 health;
} Enemy;

typedef struct game {
    u8 turn;
    Player player;
    Enemy enemy;
    bool is_match_over;
    bool is_combat_phase;
    bool is_ai_turn;
    bool is_playing_anim;
} Game;

typedef struct game_state {
    u8 games_played;
    u8 points;
    Game board_game;
    usize gacha_roll;
    usize gachart_id;
    bool should_quit;
    bool is_cardgame;
    bool is_menu;
    bool is_gallery;
    bool is_enemy;
    bool is_boss;
    bool is_machine_used;
} Game_State;

//-------------------------------------------------//

//! ROGUELITE SECTION !//

typedef enum {
    EMPTY,
    WALL,
    TREASURE,
    ENEMY,
    MACHINE,
    BOSS,
} TileType;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
} MovementDirection;

// Struct for the dungeon
typedef struct {
    int width, height;
    TileType **tiles;
} Dungeon;

//-------------------------------------------------//

//? FUNCTIONS //

void game_init(void);
usize button_create(usize entity_id, usize button_id, bool is_interactible);
Button *button_get(usize id);
usize button_count();
usize create_deck_card(usize copy_id, bool is_player);
Card *deck_card_get(usize id, bool is_player);
usize deck_card_count(bool is_player);
usize slot_create(vec2 position, bool is_player);
Slot *slot_get(usize id, bool is_player);
bool is_slot_available(bool is_player);
usize available_slot_get(bool is_player);
usize slot_count(bool is_player);
usize hand_slot_create(vec2 position, bool is_player);
Slot *hand_slot_get(usize id, bool is_player);
bool is_hand_slot_available(bool is_player);
usize available_hand_slot_get(bool is_player);
usize hand_slot_count(bool is_player);
usize card_create(const char *name, u8 health, u8 attack, Archetype archetype, usize animation_id);
Card *global_card_get(usize id);
usize global_card_count(void);
Card *playable_card_get(usize id, bool is_player);
usize playable_card_count(bool is_player);
usize move_card_to_playable_list(usize copy_id, bool is_player);

void gacha_create(const char *path_small, f32 cell_width_small, f32 cell_height_small, const char *path_large, f32 cell_width_large, f32 cell_height_large);
usize gacha_count(void);
Gacha *gacha_get(usize id);
usize gacha_get_anim_id(usize id);
bool is_gacha_unlocked(usize animation_id);

void total_player_cardpool_append(usize copy_id);
void total_player_cardpool_remove(usize id);
usize total_player_cardpool_count();

bool is_board_empty(bool is_player);
bool is_board_full(bool is_player);
bool is_hand_empty(bool is_player);
bool is_hand_full(bool is_player);
usize enemy_create(u8 health);
Enemy *enemy_get(usize id);
usize enemy_count(void);
const char* stringify_enum(Archetype archetype);
void game_update(void);
void game_loop(void);
void game_reset(const Player player, const Enemy enemy);

TileType charToTileType(char c);
void load_dungeon(Dungeon *dungeon, Player *player, const char *filename) ;
void free_dungeon(Dungeon *dungeon);
u32 random_number(u32 size);
void move_player(Dungeon *dungeon, Player *player, MovementDirection direction);
void render_dungeon(const Dungeon *dungeon, const Player *player, u32 texture_slots[10]);
