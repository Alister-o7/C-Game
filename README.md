# C-GAME

A simple cardgame build upon a custom game engine.\
Written in C using 'SDL2' and 'glad'.

## How to run

Use Visual Studio Developer Command Prompt,\
navigate to ```game``` folder and\
run ```clean & build & cgame```.

Modify ```config.ini``` to change keybindings.

## Dungeon

Explore the dungeon, exchange coins for collectible cards by collecting treasure and winning enemy and boss encounters.

Each victory expands player's deck by one random playable card.

![cGame_dungeon_s](https://github.com/user-attachments/assets/39698287-9fbc-41da-8a6f-613a783d98e5)


## Cardgame

Deploy cards from your hand onto the board, up to three of player's cards can be on the board.

Each card has its attack, defence and health value, aswell as an archetype\
that will determine it's weakness and advantage over other cards.

If a card has an advantage over opposing card it is highlighted yellow.

Green sigil ends the turn.\
Brown sigil draws a card from deck (if there are any left), and ends the turn.

Game ends when either of the sides loses its total HP.

![gamePreview3](https://github.com/user-attachments/assets/81521c8f-2081-4fa1-b3fb-de59ad6dcf8d)


## Custom Dungeons

Create ```final_dungeon_map.txt```\
First line: width of the map,\
Second line: height of the map,\
Third line: x of player origin [index, from 0 -> map_width-1],\
Fourth line: y of player origin [index, from 0 -> map_height-1],\
Fifth line: draw the map.

Example:

```
17
6
4
1
..#####..........
..#...####.###...
###.T.##T#.#M#...
#TE...#.E###E####
###............B#
#################
```

'.' -> EMPTY,\
'#' -> WALL,\
'T' -> TREASURE,\
'E' -> ENEMY,\
'M' -> MACHINE,\
'B' -> BOSS

When done, replace the original map in ```game/assets/maps```.
