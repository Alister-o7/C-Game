#include <stdio.h>
#include "../mouse.h"
#include "../entity.h"
#include "../global.h"
#include "../game.h"

static usize slot_id;

static void scaleMouseCoordinates(int* mouseX, int* mouseY) {
    *mouseX = (int)(*mouseX / global.render.scale);
    *mouseY = (int)(global.render.render_height - (*mouseY / global.render.scale));
}

static int isMouseOverEntity(Body* self, int mouseX, int mouseY) {
    int x = self->aabb.position[0];
    int y = self->aabb.position[1];
    int w = self->aabb.half_size[0];
    int h = self->aabb.half_size[1];

    int l_wall = x - w;
    int r_wall = x + w;
    int t_wall = y + h;
    int b_wall = y - h;

    return (mouseX < r_wall && mouseX > l_wall && mouseY < t_wall && mouseY > b_wall);
}

void handleMouseEvent(SDL_Event* event) {
    if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;
        scaleMouseCoordinates(&mouseX, &mouseY);

        for (usize i = 0; i < physics_body_count(); ++i) {
            Body* body = physics_body_get(i);
            Entity* entity = entity_get(body->entity_id);

            if (isMouseOverEntity(body, mouseX, mouseY) && event->type == SDL_MOUSEBUTTONDOWN && !global.game.board_game.is_playing_anim) {
                switch(entity->type) {
                    case TYPE_CARD:
                        Card *card = playable_card_get(entity->card_id, entity->is_player);
                        if (entity->is_active && !card->is_dead && !entity->is_button) {
                          
                            if (event->button.button == SDL_BUTTON_LEFT && entity->is_player && !card->is_dead && !global.game.board_game.is_ai_turn) {
                                if (!card->is_deployed && !card->is_moving && is_slot_available(true)) {
                                    card->is_moving = true;
                                    entity->slot_id = available_slot_get(true);
                                    Slot *slot = slot_get(entity->slot_id, true);
                                }
                                else if (is_board_full(true)) {
                                    printf("No space left to deploy -> Draw a card, or end turn.\n");
                                }
                            }
                            else if (event->button.button == SDL_BUTTON_RIGHT && entity->is_player && !card->is_dead && !global.game.board_game.is_ai_turn) {
                                printf("Name: %s | Health: %u | Attack: %u | Archetype: %s\n", card->name, card->health, card->attack, stringify_enum(card->archetype));
                            }
                        }
                        break;
                    case TYPE_BUTTON:
                        Button *button = button_get(body->entity_id);
                        if (event->button.button == SDL_BUTTON_LEFT) {
                            //! Cardgame
                            //? Button 0 logic
                            if (button->button_id == (usize)0) {
                                if (is_hand_slot_available(true) && deck_card_count(true) > 0 && !global.game.board_game.is_ai_turn) {
                                    printf("Drawing a card from deck... (%zu cards left)\n", deck_card_count(true));
                                
                                    spawn_card(random_number(deck_card_count(true)), true);
                                    global.game.board_game.is_ai_turn = true;
                                } else if (!is_hand_slot_available(true) && deck_card_count(true) > 0 && !global.game.board_game.is_ai_turn) {
                                    printf("No hand slots available.\n");
                                } else if (deck_card_count(true) <= 0 && !global.game.board_game.is_ai_turn) {
                                    printf("No cards left in deck.\n");
                                }
                            }
                            //? Button 1 logic
                            else if (button->button_id == (usize)1 && !global.game.board_game.is_ai_turn) {
                                global.game.board_game.is_ai_turn = true;
                            }
                            
                            //! Menu
                            //? Button 2 logic (MENU_START)
                            else if (button->button_id == (usize)2) {
                                printf("START\n");
                                global.game.is_menu = false;
                            }
                            //? Button 3 logic (MENU_GALLERY)
                            else if (button->button_id == (usize)3) {
                                printf("GALLERY\n");
                                global.game.is_gallery = true;
                            }
                            //? Button 4 logic (MENU_EXIT)
                            else if (button->button_id == (usize)4) {
                                printf("EXIT\n");
                                global.game.should_quit = true;
                            }
                            
                            //! Gallery
                            //? Button 5 logic (GALLERY_SLOT_1)
                            else if (button->button_id == (usize)5) {
                                printf("GALLERY_SLOT_1\n");
                                global.game.gachart_id = (usize)0;
                                reset();
                            }
                            //? Button 6 logic (GALLERY_SLOT_2)
                            else if (button->button_id == (usize)6) {
                                printf("GALLERY_SLOT_2\n");
                                global.game.gachart_id = (usize)1;
                                reset();
                            }
                            //? Button 7 logic (GALLERY_SLOT_3)
                            else if (button->button_id == (usize)7) {
                                printf("GALLERY_SLOT_3\n");
                                global.game.gachart_id = (usize)2;
                                reset();
                            }
                            //? Button 8 logic (GALLERY_SLOT_4)
                            else if (button->button_id == (usize)8) {
                                printf("GALLERY_SLOT_4\n");
                                global.game.gachart_id = (usize)3;
                                reset();
                            }
                            //? Button 9 logic (GALLERY_SLOT_5)
                            else if (button->button_id == (usize)9) {
                                printf("GALLERY_SLOT_5\n");
                                global.game.gachart_id = (usize)4;
                                reset();
                            }
                            //? Button 10 logic (GALLERY_SLOT_6)
                            else if (button->button_id == (usize)10) {
                                printf("GALLERY_SLOT_6\n");
                                global.game.gachart_id = (usize)5;
                                reset();
                            }
                            //? Button 11 logic (GALLERY_SLOT_7)
                            else if (button->button_id == (usize)11) {
                                printf("GALLERY_SLOT_7\n");
                                global.game.gachart_id = (usize)6;
                                reset();
                            }
                            //? Button 12 logic (GALLERY_SLOT_8)
                            else if (button->button_id == (usize)12) {
                                printf("GALLERY_SLOT_8\n");
                                global.game.gachart_id = (usize)7;
                                reset();
                            }
                            //? Button 13 logic (GALLERY_SLOT_9)
                            else if (button->button_id == (usize)13) {
                                printf("GALLERY_SLOT_9\n");
                                global.game.gachart_id = (usize)8;
                                reset();
                            }
                        }
                    break;
                }
            }

            else if (isMouseOverEntity(body, mouseX, mouseY) && !global.game.board_game.is_playing_anim) {
                switch(entity->type) {
                    case TYPE_CARD:
                        Card *card = playable_card_get(entity->card_id, entity->is_player);
                        if (entity->is_active && !card->is_dead && !entity->is_button) {
                            card->is_highlighted = true;
                        }
                        break;

                    case TYPE_BUTTON:
                        if (entity->is_button && entity->is_active) {
                            Button *button = button_get(body->entity_id);
                            button->is_highlighted = true;
                        }
                        break;
                }
            }

            else {
                switch(entity->type) {
                    case TYPE_CARD:
                        Card *card = playable_card_get(entity->card_id, entity->is_player);
                        if (entity->is_active && !card->is_dead && !entity->is_button) {
                            card->is_highlighted = false;
                        }
                        break;

                    case TYPE_BUTTON:
                        if (entity->is_button && entity->is_active) {
                            Button *button = button_get(body->entity_id);
                            button->is_highlighted = false;
                        }
                        break;
                }
            }
        }
    }
}
