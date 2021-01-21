TYPE = 0
ifeq ($(OS),WINDOWS_NT)
	TYPE = 1
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		TYPE = 2
	endif
endif

play:
	ifeq ($(TYPE), 1)
		connect
	endif
	ifeq ($(TYPE), 2)
		./connect
	endif
	ifeq ($(TYPE), 0)
		echo "No compiler for your system is supported by this project."
	endif

compile:
	ifeq ($(TYPE), 1)
		cl structs/Stack.c structs/Hashmap.c AI.c data.c game.c menu.c /link /out:connect.exe
	endif
	ifeq ($(TYPE), 2)
		gcc structs/Stack.c structs/Hashmap.c AI.c common/functions.c common/data.c game.c menu.c -lm -o connect
	endif
	ifeq ($(TYPE), 0)
		echo "No compiler for your system is supported by this project."
	endif

clean:
	ifeq ($(TYPE), 1)
		del *.obj
		del *.o
		del *.exe
	endif
	ifeq ($(TYPE), 2)
		rm *.out
		rm *.o
		rm *.run
	endif