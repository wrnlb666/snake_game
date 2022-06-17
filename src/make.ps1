#make file for everything. For convenience, every library will be compile to a dynamic linked library(*.dll file).
#if script language doesn't work, simply change the suffix of the file to .bat

#libraries
gcc falloc.c -shared -fpic -o falloc.dll
gcc frame.c -shared -fpic -L. -lfalloc -o frame.dll
gcc snake.c -shared -fpic -L. -lfalloc -lframe -o snake.dll
gcc multiplayer.c -shared -fpic -L. -lfalloc -lframe -lsnake -o multiplayer.dll
gcc multisnake.c -shared -fpic -L. -lfalloc -lframe -lsnake -lmultiplayer -lws2_32 -o multisnake.dll


#main(two applications)
gcc game.c -L. -lfalloc -lframe -lsnake -o game
gcc snake_game.c -L. -lfalloc -lframe -lsnake -lmultisnake -lws2_32  -o snake_game