# This was supposed to allow the Makefile to automatically compile for both Windows and Linux, but 
# TYPE = 0
# ifeq ($(OS),WINDOWS_NT)
# 	TYPE = 1
# else
# 	UNAME_S := $(shell uname -s)
# 	ifeq ($(UNAME_S),Linux)
# 		TYPE = 2
# 	endif
# endif

play: # this should also work on Linux, but just incase it doesn't there's a Linux command below
	build/Connect

play-linux:
	./build/Connect

# play:
	# ifeq ($(TYPE), 1)
	# 	connect
	# endif
	# ifeq ($(TYPE), 2)
	# 	./connect
	# endif
	# ifeq ($(TYPE), 0)
	# 	echo "No compiler for your system is supported by this project."
	# endif

compile:
	mkdir build
	cmake -B build
	cmake --build build
	# ifeq ($(TYPE), 1)
	# 	cl structs/Stack.c structs/Hashmap.c AI.c data.c game.c menu.c /link /out:connect.exe
	# endif
	# ifeq ($(TYPE), 2)
	# 	gcc structs/Stack.c structs/Hashmap.c AI.c common/functions.c common/data.c game.c menu.c -lm -o connect
	# endif
	# ifeq ($(TYPE), 0)
	# 	echo "No compiler for your system is supported by this project."
	# endif

clean:
	rm -rf build
	# ifeq ($(TYPE), 1)
	# 	del *.obj
	# 	del *.o
	# 	del *.exe
	# endif
	# ifeq ($(TYPE), 2)
	# 	rm *.out
	# 	rm *.o
	# 	rm *.run
	# endif