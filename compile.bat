mkdir bin
C:\gbdk\bin\lcc -c -o bin\main.o src\main.c
C:\gbdk\bin\lcc -c -o bin\LoseMap.o src\maps\LoseMap.c
C:\gbdk\bin\lcc -c -o bin\WinMap.o src\maps\WinMap.c
C:\gbdk\bin\lcc -c -o bin\GameOverMap.o src\maps\GameOverMap.c
C:\gbdk\bin\lcc -c -o bin\MineMap.o src\maps\MineMap.c
C:\gbdk\bin\lcc -c -o bin\MineTiles.o src\tiles\MineTiles.c

C:\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -o bin\mine.gb bin\main.o bin\LoseMap.o bin\WinMap.o bin\GameOverMap.o bin\MineMap.o bin\MineTiles.o 
