connect:
	cl Stack.c Hashmap.c AI.c connect.c /link /out:connect.exe
	del *.obj
	del *.o
	connect
	
clean:
	del *.obj
	del *.o
	del *.exe