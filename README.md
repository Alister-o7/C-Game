# C-GAME

A simple cardgame build upon a custom game engine.\
Written in C using 'SDL2' and 'glad'.

## How to run

Use Visual Studio Developer Command Prompt,\
navigate to ```game``` folder and\
run ```clean & build & cgame```.

## Dungeon

Explore the dungeon, exchange coins for collectible cards\
by collecting tresure and winning enemy and boss encounters.

![cGame_dungeon_s](https://github.com/user-attachments/assets/39698287-9fbc-41da-8a6f-613a783d98e5)


## Cardgame

Deploy cards from your hand onto the board,\
up to three of player's cards can be deployed at once.

Each cards has its attack, defence and health value, aswell as an archetype\
that will determine it's weakness and advantage over other cards.

Game ends when either of the sides loses its total HP.

![cGame_cardgame_s](https://github.com/user-attachments/assets/89a0a6fa-fcb3-473b-af11-75fa9d1ab0c0)


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
