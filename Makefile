play:
	connect

compile:
	cl Stack.c Hashmap.c AI.c data.c game.c menu.c /link /out:connect.exe
	
clean:
	del *.obj
	del *.o
	del *.exe