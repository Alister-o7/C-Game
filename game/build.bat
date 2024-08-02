set render=src\engine\render\render.c src\engine\render\render_init.c src\engine\render\render_util.c
set io=src\engine\io\io.c
set config=src\engine\config\config.c
set input=src\engine\input\input.c
set mouse=src\engine\mouse\mouse.c
set time=src\engine\time\time.c
set physics=src\engine\physics\physics.c
set array_list=src\engine\array_list\array_list.c
set entity=src\engine\entity\entity.c
set animation=src\engine\animation\animation.c
set audio=src\engine\audio\audio.c
set game=src\engine\game\game.c
set files=src\glad.c src\main.c src\engine\global.c %render% %io% %config% %input% %mouse% %time% %physics% %array_list% %entity% %animation% %audio% %game%
set libs=C:\CardGame\lib\SDL2main.lib C:\CardGame\lib\SDL2.lib C:\CardGame\lib\SDL2_mixer.lib C:\CardGame\lib\freetype.lib C:\CardGame\lib\libsodium.lib

CL /Zi /I C:\CardGame\include %files% /link %libs% /OUT:cgame.exe
