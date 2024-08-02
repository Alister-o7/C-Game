#include "../game.h"
#include "../util.h"
#include "../array_list.h"
#include "../global.h"
#include "../entity.h"

static Array_List *player_slot_list;
static Array_List *enemy_slot_list;

static Array_List *player_hand_slot_list;
static Array_List *enemy_hand_slot_list;

static Array_List *global_card_list;

static Array_List *player_card_list;
static Array_List *enemy_card_list;

static Array_List *total_player_cardpool_list;

static Array_List *player_deck_list;
static Array_List *enemy_deck_list;

static Array_List *button_list;

static Array_List *opponent_list;

static Array_List *gacha_list;

// Textures
static Sprite_Sheet sprite_sheet_player;
static Sprite_Sheet sprite_sheet_empty;
static Sprite_Sheet sprite_sheet_wall;
static Sprite_Sheet sprite_sheet_treasure;
static Sprite_Sheet sprite_sheet_enemy;
static Sprite_Sheet sprite_sheet_machine;
static Sprite_Sheet sprite_sheet_boss;

// Gacha Art
static Sprite_Sheet sprite_sheet_gachaR_0;
static Sprite_Sheet sprite_sheet_gachaR_1;
static Sprite_Sheet sprite_sheet_gachaR_2;
static Sprite_Sheet sprite_sheet_gachaR_3;
static Sprite_Sheet sprite_sheet_gachaR_4;
static Sprite_Sheet sprite_sheet_gachaR_5;

static Sprite_Sheet sprite_sheet_gacha_G_0;

//! CARDGAME SECTION !//

typedef enum colors {
    RED_COL,
    GREEN_COL,
    YELLOW_COL,
    BLUE_COL,
} Colors;

void game_init(void) {
    if (sodium_init() < 0) {
        ERROR_EXIT("Could not initialize SODIUM!\n");
    }

    global.game.is_menu = true;
    global.game.is_gallery = false;

    global.game.gachart_id =  ((usize)-1);

    global.game.games_played = 0;
    global.game.points = 0;
    global.game.is_cardgame = false;
    global.game.is_enemy = false;
    global.game.is_boss = false;

    player_slot_list = array_list_create(sizeof(Slot), 0); //? Mess with initial capacity here.
    enemy_slot_list = array_list_create(sizeof(Slot), 0); //? Mess with initial capacity here.

    player_hand_slot_list = array_list_create(sizeof(Slot), 0); //? Mess with initial capacity here.
    enemy_hand_slot_list = array_list_create(sizeof(Slot), 0); //? Mess with initial capacity here.

    global_card_list = array_list_create(sizeof(Card), 0);

    player_card_list = array_list_create(sizeof(Card), 0);
    enemy_card_list = array_list_create(sizeof(Card), 0);

    total_player_cardpool_list = array_list_create(sizeof(Card), 0);

    player_deck_list = array_list_create(sizeof(Card), 0);
    enemy_deck_list = array_list_create(sizeof(Card), 0);

    button_list = array_list_create(sizeof(Button), 0);

    opponent_list = array_list_create(sizeof(Enemy), 0);

    gacha_list = array_list_create(sizeof(Gacha), 0);

    //? Roguelite texture init
    render_sprite_sheet_init(&sprite_sheet_player, "assets/textures/roguelite/player_texture_60.png", 60, 60);
    render_sprite_sheet_init(&sprite_sheet_empty, "assets/textures/roguelite/empty_texture_60.png", 60, 60);
    render_sprite_sheet_init(&sprite_sheet_wall, "assets/textures/roguelite/wall_texture_60.png", 60, 60);
    render_sprite_sheet_init(&sprite_sheet_treasure, "assets/textures/roguelite/treasure_texture_60.png", 60, 60);
    render_sprite_sheet_init(&sprite_sheet_enemy, "assets/textures/roguelite/enemy_texture_60.png", 60, 60);
    render_sprite_sheet_init(&sprite_sheet_machine, "assets/textures/roguelite/machine_texture_60.png", 60, 60);
    render_sprite_sheet_init(&sprite_sheet_boss, "assets/textures/roguelite/boss_texture_60.png", 60, 60);
}

usize button_create(usize entity_id, usize button_id, bool is_interactible) {
    usize id;

    id = button_list->len;
    
    if (id == button_list->len) {
        if (array_list_append(button_list, &(Button){0}) == (usize)-1) {
            ERROR_EXIT("Could not append to BUTTON list\n");
        }
    }
    
    Button *button = button_get(id);

    *button = (Button){
        .entity_id = entity_id,
        .button_id = button_id,
        .is_interactible = is_interactible,
        .is_highlighted = false,
    };

    return id;
}

//! Fix this function
Button *button_get(usize id) {
    return array_list_get(button_list, id);
}

usize button_count() {
    return button_list->len;
}

usize create_deck_card(usize copy_id, bool is_player) {
    usize id;
    const Card *copy_card;


    if (is_player) {
        id = player_deck_list->len;
        for (usize i = 0; i < player_deck_list->len; ++i) {
            Card *card = array_list_get(player_deck_list, i);
            if (card->is_dead) {
                id = i;
                break;
            }
        }
        if (id == player_deck_list->len) {
            if (array_list_append(player_deck_list, &(Card){0}) == (usize)-1) {
                ERROR_EXIT("Could not append card to PLAYER DECK list\n");
            }
        }
    } else {
        id = enemy_deck_list->len;
        for (usize i = 0; i < enemy_deck_list->len; ++i) {
            Card *card = array_list_get(enemy_deck_list, i);
            if (card->is_dead) {
                id = i;
                break;
            }
        }
        if (id == enemy_deck_list->len) {
            if (array_list_append(enemy_deck_list, &(Card){0}) == (usize)-1) {
                ERROR_EXIT("Could not append card to ENEMY DECK list\n");
            }
        }
    }

    if (is_player) {
        copy_card = array_list_get(total_player_cardpool_list, copy_id);
    } else {
        copy_card = array_list_get(global_card_list, copy_id);
    }

    Card *card = deck_card_get(id, is_player);

    *card = (Card){
        .name = copy_card->name,
        .animation_id = copy_card->animation_id,
        .health = copy_card->health,
        .attack = copy_card->attack,
        .archetype = copy_card->archetype,
        .is_dead = false,
        .is_deployed = false,
        .is_highlighted = false,
        .is_moving = false,
    };

    return id;
}

Card *deck_card_get(usize id, bool is_player) {
    if (is_player) {
        return array_list_get(player_deck_list, id);
    } else {
        return array_list_get(enemy_deck_list, id);
    }
}

usize deck_card_count(bool is_player) {
    if (is_player) {
        return player_deck_list->len;
    } else {
        return enemy_deck_list->len;
    }
}

usize slot_create(vec2 position, bool is_player) {
    usize id;
    if (is_player) {
        id = player_slot_list->len;

        if (id == player_slot_list->len) {
            if (array_list_append(player_slot_list, &(Slot){0}) == (usize)-1) {
                ERROR_EXIT("Could not append slot to PLAYER list\n");
            }
        }
    } else {
        id = enemy_slot_list->len;

        if (id == enemy_slot_list->len) {
            if (array_list_append(enemy_slot_list, &(Slot){0}) == (usize)-1) {
                ERROR_EXIT("Could not append slot to ENEMY list\n");
            }
        }
    }
    
    Slot *slot = slot_get(id, is_player);

    *slot = (Slot){
        .is_taken = false,
        .position = { position[0], position[1] },
    };

    return id;
}

Slot *slot_get(usize id, bool is_player) {
    if (is_player) {
        return array_list_get(player_slot_list, id);
    } else {
        return array_list_get(enemy_slot_list, id);
    }
}

bool is_slot_available(bool is_player) {
    if (is_player) {
        for (usize i = 0; i < player_slot_list->len; ++i) {
            Slot *slot = array_list_get(player_slot_list, i);
            if (!slot->is_taken) {
                return true;
            }
        }
    } else {
        for (usize i = 0; i < enemy_slot_list->len; ++i) {
            Slot *slot = array_list_get(enemy_slot_list, i);
            if (!slot->is_taken) {
                return true;
            }
        }
    }
    return false;
}

usize available_slot_get(bool is_player) {
    if (is_player) {
        for (usize i = 0; i < player_slot_list->len; ++i) {
            Slot *slot = array_list_get(player_slot_list, i);
            if (!slot->is_taken) {
                return i;
            }
        }
    } else {
        for (usize i = 0; i < enemy_slot_list->len; ++i) {
            Slot *slot = array_list_get(enemy_slot_list, i);
            if (!slot->is_taken) {
                return i;
            }
        }
    }
    return -1;
}

usize slot_count(bool is_player) {
    if (is_player) {
        return player_slot_list->len;
    } else {
        return enemy_slot_list->len;
    }
}

usize hand_slot_create(vec2 position, bool is_player) {
    usize id;
    if (is_player) {
        id = player_hand_slot_list->len;

        if (id == player_hand_slot_list->len) {
            if (array_list_append(player_hand_slot_list, &(Slot){0}) == (usize)-1) {
                ERROR_EXIT("Could not append HAND slot to PLAYER list\n");
            }
        }
    } else {
        id = enemy_hand_slot_list->len;

        if (id == enemy_hand_slot_list->len) {
            if (array_list_append(enemy_hand_slot_list, &(Slot){0}) == (usize)-1) {
                ERROR_EXIT("Could not append HAND slot to ENEMY list\n");
            }
        }
    }

    Slot *slot = hand_slot_get(id, is_player);

    *slot = (Slot){
        .is_taken = false,
        .position = { position[0], position[1] },
    };

    return id;
}

Slot *hand_slot_get(usize id, bool is_player) {
    if (is_player) {
        return array_list_get(player_hand_slot_list, id);
    } else {
        return array_list_get(enemy_hand_slot_list, id);
    }
}

bool is_hand_slot_available(bool is_player) {
    if (is_player) {
        for (usize i = 0; i < player_hand_slot_list->len; ++i) {
            Slot *slot = array_list_get(player_hand_slot_list, i);
            if (!slot->is_taken) {
                return true;
            }
        }
    } else {
        for (usize i = 0; i < enemy_hand_slot_list->len; ++i) {
            Slot *slot = array_list_get(enemy_hand_slot_list, i);
            if (!slot->is_taken) {
                return true;
            }
        }
    }
    return false;
}

usize available_hand_slot_get(bool is_player) {
    if (is_player) {
        for (usize i = 0; i < player_hand_slot_list->len; ++i) {
            Slot *slot = array_list_get(player_hand_slot_list, i);
            if (!slot->is_taken) {
                return i;
            }
        }
    } else {
        for (usize i = 0; i < enemy_hand_slot_list->len; ++i) {
            Slot *slot = array_list_get(enemy_hand_slot_list, i);
            if (!slot->is_taken) {
                return i;
            }
        }
    }
    return -1;
}

usize hand_slot_count(bool is_player) {
    if (is_player) {
        return player_hand_slot_list->len;
    } else {
        return enemy_hand_slot_list->len;
    }
}

usize card_create(const char *name, u8 health, u8 attack, Archetype archetype, usize animation_id) {
    usize id = global_card_list->len;

    if (id == global_card_list->len) {
        if (array_list_append(global_card_list, &(Card){0}) == (usize)-1) {
            ERROR_EXIT("Could not append card to GLOBAL list\n");
        }
    }

    Card *card = array_list_get(global_card_list, id);

    *card = (Card){
        .name = name,
        .animation_id = animation_id,
        .health = health,
        .attack = attack,
        .archetype = archetype,
        .is_dead = false,
        .is_deployed = false,
        .is_highlighted = false,
        .is_moving = false,
    };

    return id;
}

Card *global_card_get(usize id) {
    return array_list_get(global_card_list, id);
}

usize global_card_count(void) {
    return global_card_list->len;
}

Card *playable_card_get(usize id, bool is_player) {
    if (is_player) {
        return array_list_get(player_card_list, id);
    } else {
        return array_list_get(enemy_card_list, id);
    }
}

usize playable_card_count(bool is_player) {
    if (is_player) {
        return player_card_list->len;
    } else {
        return enemy_card_list->len;
    }
}

usize move_card_to_playable_list(usize copy_id, bool is_player) {
    usize id;
    if (is_player) {
        id = player_card_list->len;
        if (id == player_card_list->len) {
            if (array_list_append(player_card_list, &(Card){0}) == (usize)-1) {
                ERROR_EXIT("Could not append card to PLAYER list\n");
            }
        }
    } else {
        id = enemy_card_list->len;
        if (id == enemy_card_list->len) {
            if (array_list_append(enemy_card_list, &(Card){0}) == (usize)-1) {
                ERROR_EXIT("Could not append card to ENEMY list\n");
            }
        }
    }

    const Card *copy_card = deck_card_get(copy_id, is_player);

    Card *card = playable_card_get(id, is_player);

    *card = (Card){
        .name = copy_card->name,
        .animation_id = copy_card->animation_id,
        .health = copy_card->health,
        .attack = copy_card->attack,
        .archetype = copy_card->archetype,
        .is_dead = false,
        .is_deployed = false,
        .is_highlighted = false,
        .is_moving = false,
    };

    if (is_player) {
        array_list_remove(player_deck_list, copy_id);
    } else {
        array_list_remove(enemy_deck_list, copy_id);
    }

    return id;
}

void gacha_create(const char *path_small, f32 cell_width_small, f32 cell_height_small, const char *path_large, f32 cell_width_large, f32 cell_height_large) {
    usize id;

    id = gacha_list->len;
    
    if (id == gacha_list->len) {
        if (array_list_append(gacha_list, &(Gacha){0}) == (usize)-1) {
            ERROR_EXIT("Could not append to GACHA list\n");
        }
    }

    Gacha *gacha = gacha_get(id);
    
    *gacha = (Gacha){
        .is_unlocked = false,
    };

    render_sprite_sheet_init(&gacha->sprite_sheet_small, path_small, cell_width_small, cell_height_small);
    render_sprite_sheet_init(&gacha->sprite_sheet_large, path_large, cell_width_large, cell_height_large);
}

usize gacha_count(void) {
    return gacha_list->len;
}

Gacha *gacha_get(usize id) {
    return array_list_get(gacha_list, id);
}

usize gacha_get_anim_id(usize id) {
    const Gacha *gacha = gacha_get(id);
    return gacha->anim_id;
}

bool is_gacha_unlocked(usize animation_id) {
    for (usize i = 0; i < gacha_count(); i++) {
        const Gacha *gacha = gacha_get(i);
        if (gacha->anim_id == animation_id) {
            return gacha->is_unlocked;
        }
    }

    return false;
}

void total_player_cardpool_append(usize copy_id) {
    usize id;

    id = total_player_cardpool_list->len;

    if (id == total_player_cardpool_list->len) {
        if (array_list_append(total_player_cardpool_list, &(Card){0}) == (usize)-1) {
            ERROR_EXIT("Could not append card to TOTAL PLAYER CARDPOOL list\n");
        }
    }
    
    const Card *copy_card = array_list_get(global_card_list, copy_id);
    
    Card *card = array_list_get(total_player_cardpool_list, id);

    *card = (Card){
        .name = copy_card->name,
        .animation_id = copy_card->animation_id,
        .health = copy_card->health,
        .attack = copy_card->attack,
        .archetype = copy_card->archetype,
        .is_dead = false,
        .is_deployed = false,
        .is_highlighted = false,
        .is_moving = false,
    };
}

void total_player_cardpool_remove(usize id) {
    array_list_remove(total_player_cardpool_list, id);
}

usize total_player_cardpool_count() {
    return total_player_cardpool_list->len;
}

bool is_board_empty(bool is_player) {
    for (usize i = 0; i < slot_count(is_player); ++i) {
        Slot *slot = slot_get(i, is_player);
        if (slot->is_taken) {
            return false;
        }
    }

    return true;
}

bool is_board_full(bool is_player) {
    for (usize i = 0; i < slot_count(is_player); ++i) {
        Slot *slot = slot_get(i, is_player);
        if (!slot->is_taken) {
            return false;
        }
    }

    return true;
}

bool is_hand_empty(bool is_player) {
    for (usize i = 0; i < hand_slot_count(is_player); ++i) {
        Slot *slot = hand_slot_get(i, is_player);
        if (slot->is_taken) {
            return false;
        }
    }

    return true;
}

bool is_hand_full (bool is_player) {
    for (usize i = 0; i < hand_slot_count(is_player); ++i) {
        Slot *slot = hand_slot_get(i, is_player);
        if (!slot->is_taken) {
            return false;
        }
    }

    return true;
}

usize enemy_create(u8 health) {
    
    usize id = opponent_list->len;

    if (id == opponent_list->len) {
        if (array_list_append(opponent_list, &(Enemy){0}) == (usize)-1) {
            ERROR_EXIT("Could not append to OPPONENT list\n");
        }
    }
    
    Enemy *enemy = enemy_get(id);

    *enemy = (Enemy){
        .health = health,
    };

    return id;
}

Enemy *enemy_get(usize id) {
    return array_list_get(opponent_list, id);
}

usize enemy_count(void) {
    return opponent_list->len;
}

static bool is_critical(Archetype subject, Archetype target) {
    if (subject == MYSTIC && target == REALITY) {
        return true;
    } else if (subject == TECH && target == MYSTIC) {
        return true;
    } else if (subject == DREAM && target == TECH) {
        return true;
    } else if (subject == REALITY && target == DREAM) {
        return true;
    }

    return false;
}

const char* stringify_enum(Archetype archetype) {
    switch (archetype) {
        case MYSTIC: return "Mystic";
        case TECH: return "Tech";
        case DREAM: return "Dream";
        case REALITY: return "Reality";
        default: ERROR_EXIT("Unknown Archetype Enum Value.\n");
    }
}

void game_update(void) {
    for (usize i = 0; i < playable_card_count(true); ++i) {
        Card *p_card = playable_card_get(i, true);
        if (p_card->is_dead) {
            array_list_remove(player_card_list, i);
        }
    }
    for (usize j = 0; j < playable_card_count(false); ++j) {
        Card *e_card = playable_card_get(j, false);
        if (e_card->is_dead) {
            array_list_remove(enemy_card_list, j);
        }
    }
}

static void print_color(Colors color, const char *text) {
    // ANSI escape codes
    if (color == RED_COL) {
        printf("\033[31m%s\033[0m", text);
    } else if (color == GREEN_COL) {
        printf("\033[32m%s\033[0m", text);
    } else if (color == YELLOW_COL) {
        printf("\033[33m%s\033[0m", text);
    } else if (color == BLUE_COL) {
        printf("\033[34m%s\033[0m", text);
    }
}

void game_loop(void) {
    char formattedString[200];
    usize i = 0;
    usize j = 0;

    while (i < slot_count(true) &&  j < slot_count(false)) {
        Slot *player_slot = slot_get(i, true);
        Slot *enemy_slot = slot_get(j, false);
        if (player_slot->is_taken && enemy_slot->is_taken) {
            Card *player_card = playable_card_get(player_slot->card_id, true);
            Card *enemy_card = playable_card_get(enemy_slot->card_id, false);
            u8 player_attack = player_card->attack;
            u8 enemy_attack = enemy_card->attack;

            if (is_critical(player_card->archetype, enemy_card->archetype)) {
                player_attack *= 2;
                player_card->has_advantage = true;
                sprintf(formattedString, "Player Card: %s has ADVANTAGE over Enemy Card: %s\n", player_card->name, enemy_card->name);
                print_color(YELLOW_COL, formattedString);
            } else if (is_critical(enemy_card->archetype, player_card->archetype)) {
                enemy_attack *= 2;
                enemy_card->has_advantage = true;
                sprintf(formattedString, "Enemy Card: %s has ADVANTAGE over Player Card: %s\n", enemy_card->name, player_card->name);
                print_color(YELLOW_COL, formattedString);
            }

            if (player_attack >= enemy_card->health && enemy_attack >= player_card->health) {
                sprintf(formattedString, "Player Card: %s and Enemy Card: %s have eliminated EACHOTHER!\n", player_card->name, enemy_card->name);
                print_color(YELLOW_COL, formattedString);
                enemy_card->is_dead = true;
                player_card->is_dead = true;
                enemy_slot->is_taken = false;
                player_slot->is_taken = false;
                enemy_card->has_advantage = false;
                player_card->has_advantage = false;
            } else {
                if (player_attack >= enemy_card->health && !player_card->is_dead) {
                    sprintf(formattedString, "Enemy Card: %s has been eliminated by Player Card: %s !\n", enemy_card->name, player_card->name);
                    print_color(BLUE_COL, formattedString);
                    enemy_card->is_dead = true;
                    enemy_slot->is_taken = false;
                    player_card->has_advantage = false;
                } else if (!player_card->is_dead) {
                    enemy_card->health -= player_attack;
                    sprintf(formattedString, "Player Card: %s is attacking Enemy Card: %s dealing %u damage. (Enemy Card HP: %u)\n", player_card->name, enemy_card->name, player_attack, enemy_card->health);
                    print_color(BLUE_COL, formattedString);
                }

                if (enemy_attack >= player_card->health && !enemy_card->is_dead) {
                    sprintf(formattedString, "Player Card: %s has been eliminated by Enemy Card: %s !\n", player_card->name, enemy_card->name);
                    print_color(RED_COL, formattedString);
                    player_card->is_dead = true;
                    player_slot->is_taken = false;
                    enemy_card->has_advantage = false;
                } else if (!enemy_card->is_dead){
                    player_card->health -= enemy_attack;
                    sprintf(formattedString, "Enemy Card: %s is attacking Player Card: %s dealing %u damage. (Player Card HP: %u)\n", enemy_card->name, player_card->name, enemy_attack, player_card->health);
                    print_color(RED_COL, formattedString);
                }
            }
        } 
        
        else if (player_slot->is_taken && !enemy_slot->is_taken) {
            Card *player_card = playable_card_get(player_slot->card_id, true);
            if (player_card->attack >= global.game.board_game.enemy.health) {
                global.game.board_game.is_match_over = true;
                global.game.board_game.enemy.health = (u8)0;
                sprintf(formattedString, "Player Victory ! -> Enemy lost all HP.\n");
                print_color(GREEN_COL, formattedString);
                return;
            } 
            else {
                global.game.board_game.enemy.health -= player_card->attack;
                sprintf(formattedString, "Player Card: %s is attacking Enemy HERO dealing %u damage. (Enemy HERO HP: %u)\n", player_card->name, player_card->attack, global.game.board_game.enemy.health);
                print_color(BLUE_COL, formattedString);
            }
        } 
        
        else if (!player_slot->is_taken && enemy_slot->is_taken) {
            Card *enemy_card = playable_card_get(player_slot->card_id, false);
            if (enemy_card->attack >= global.game.board_game.player.health) {
                global.game.board_game.is_match_over = true;
                global.game.board_game.player.health = (u8)0;
                sprintf(formattedString, "Player losses... -> Player lost all HP.\n");
                print_color(GREEN_COL, formattedString);
                return;
            } else {
                global.game.board_game.player.health -= enemy_card->attack;
                sprintf(formattedString, "Enemy Card: %s is attacking Player HERO dealing %u damage. (Player HERO HP: %u)\n", enemy_card->name, enemy_card->attack, global.game.board_game.player.health);
                print_color(RED_COL, formattedString);
            }
        } 
        ++i;
        ++j;
    }

    global.game.board_game.turn += 1;
    global.game.board_game.is_combat_phase = false;
    printf("Turn: %u\n", global.game.board_game.turn);
}

void game_reset(const Player player, const Enemy enemy) {
    global.game.board_game.is_combat_phase = false;
    global.game.board_game.is_ai_turn = false;
    global.game.board_game.turn = 1;
    global.game.board_game = (Game){
        .player = player,
        .enemy = enemy,
        .is_match_over = false,
        .is_playing_anim = false,
    };

    player_slot_list->len = 0;
    enemy_slot_list-> len = 0;

    player_hand_slot_list->len = 0;
    enemy_hand_slot_list->len = 0;

    //global_card_list->len = 0;
    player_card_list->len = 0;
    enemy_card_list->len = 0;

    player_deck_list->len = 0;
    enemy_deck_list->len = 0;

    button_list->len = 0;
}

//-------------------------------------------------//

//! ROGUELITE SECTION !//

TileType charToTileType(char c) {
    switch (c) {
        case '.': return EMPTY;
        case '#': return WALL;
        case 'T': return TREASURE;
        case 'E': return ENEMY;
        case 'M': return MACHINE;
        case 'B': return BOSS;
        default: return EMPTY;
    }
}

void load_dungeon(Dungeon *dungeon, Player *player, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    fscanf(file, "%d %d %d %d\n", &dungeon->width, &dungeon->height, &player->x, &player->y);

    dungeon->tiles = malloc(dungeon->height * sizeof(TileType *));
    for (int i = 0; i < dungeon->height; i++) {
        dungeon->tiles[i] = malloc(dungeon->width * sizeof(TileType));
        for (int j = 0; j < dungeon->width; j++) {
            char c;
            fscanf(file, "%c", &c);
            dungeon->tiles[i][j] = charToTileType(c);
        }
        fscanf(file, "\n"); // Skip to the next line
    }

    fclose(file);
}

void free_dungeon(Dungeon *dungeon) {
    for (int i = 0; i < dungeon->height; i++) {
        free(dungeon->tiles[i]);
    }
    free(dungeon->tiles);
}

static bool canMoveToTile(Dungeon *dungeon, int x, int y) {
    if (x < 0 || x >= dungeon->width || y < 0 || y >= dungeon->height) {
        return false; // Out of bounds
    }

    else if (dungeon->tiles[y][x] == MACHINE) {
        return false;
    }

    else {
        return dungeon->tiles[y][x] != WALL; // Can move if it's not a wall
    }
}

u32 random_number(u32 size) {
    return randombytes_uniform(size);
}

void move_player(Dungeon *dungeon, Player *player, MovementDirection direction) {
    int newX = player->x;
    int newY = player->y;

    switch (direction) {
        case MOVE_UP:    newY--; break;
        case MOVE_DOWN:  newY++; break;
        case MOVE_LEFT:  newX--; break;
        case MOVE_RIGHT: newX++; break;
    }

    if (canMoveToTile(dungeon, newX, newY)) {
        player->x = newX;
        player->y = newY;
        if (dungeon->tiles[newY][newX] == TREASURE) {
            printf("Got Treasure!\n");
            global.game.points += 10;
            dungeon->tiles[newY][newX] = EMPTY;
        }

        else if (dungeon->tiles[newY][newX] == ENEMY) {
            printf("Encountered an enemy!\n");
            dungeon->tiles[newY][newX] = EMPTY;
            global.game.is_enemy = true;
        }

        else if (dungeon->tiles[newY][newX] == BOSS) {
            printf("Encountered BOSS!\n");
            dungeon->tiles[newY][newX] = EMPTY;
            global.game.is_boss = true;
        }
    } 
    
    else if (dungeon->tiles[newY][newX] == MACHINE) {
        printf("Reached Gacha-machine!\n");
        if (global.game.points >= 5) {
            printf("Spend 5 gacha points to roll a card.\n");
            global.game.points -= 5;
            global.game.is_machine_used = true;
            global.game.gacha_roll = (usize)random_number((u32)8);
        } else {
            printf("Not enough points to roll the dice...\n");
        }
    }

    else {
        printf("Cannot move there!\n");
    }
}

void render_dungeon(const Dungeon *dungeon, const Player *player, u32 texture_slots[10])
{
    int mx,my;
    const int start_sx = (global.render.render_width - (dungeon->width*ROGUE_TEXTURE_DIMENSIONS)) / 2;
    int sx = start_sx;
    int sy = global.render.render_height;

    if (global.game.is_machine_used) {
        Gacha *gacha = gacha_get(global.game.gacha_roll);
        render_sprite_sheet_frame(&gacha->sprite_sheet_large, 0, 0, (vec2){global.render.render_width / 2, global.render.render_height / 2}, false, WHITE, texture_slots);
        render_quad_line((vec2){global.render.render_width / 2, global.render.render_height / 2}, (vec2){gacha->sprite_sheet_large.cell_width,gacha->sprite_sheet_large.cell_height}, RED);
        gacha->is_unlocked = true;
    }

    else {
        for (my = 0; my < dungeon->height; my++) {
            for (mx = 0; mx < dungeon->width; mx++) {
                if (player->y == my && player->x == mx) {
                    render_sprite_sheet_frame(&sprite_sheet_player, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                } else {
                    switch (dungeon->tiles[my][mx]) {
                        case EMPTY:
                            render_sprite_sheet_frame(&sprite_sheet_empty, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                            break;
                        case WALL:
                            render_sprite_sheet_frame(&sprite_sheet_wall, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                            break;
                        case TREASURE:
                            render_sprite_sheet_frame(&sprite_sheet_treasure, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                            break;
                        case ENEMY:
                            render_sprite_sheet_frame(&sprite_sheet_enemy, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                            break;
                        case MACHINE:
                            render_sprite_sheet_frame(&sprite_sheet_machine, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                            break;
                        case BOSS:
                            render_sprite_sheet_frame(&sprite_sheet_boss, 0, 0, (vec2){sx, sy}, false, WHITE, texture_slots);
                            break;
                    }
                }
                sx+=ROGUE_TEXTURE_DIMENSIONS;
            }
            sy-=ROGUE_TEXTURE_DIMENSIONS;
            sx=start_sx;
        }
    }
}
