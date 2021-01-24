#play: # this should also work using shell, but just incase it doesn't there's a failsafe command below
#	build/Connect # this doesn't work on Windows, hence why there's 2 'play' commands

#play-failsafe:
#	./build/Connect

play-cmake-windows: # Windows launcher for CMake compilation - ideally, we'd have the 2 (Windows and GCC compilers) merged into one, but Windows cannot execute the compilation by running 'build/Connect' like Linux can
	cd build/Debug
	Connect

play-cmake-shell: # Shell launcher for CMake compilation
	./build/Connect

play-windows: # Windows launcher for standard compilation
	Connect

play-shell: # Shell launcher for standard compilation
	./Connect

compile-cmake: # CMake compiler
	mkdir build
	cmake -B build
	cmake --build build

compile-windows: # standard Windows compiler
	cl structs/Stack.c structs/Hashmap.c structs/Matrix.c AI.c common/functions.c common/data.c game.c menu.c /link /out:Connect.exe

compile-gcc: # standard GCC compiler
	gcc structs/Stack.c structs/Hashmap.c AI.c common/functions.c common/data.c game.c menu.c -lm -o Connect

clean-cmake-windows: # clean the Windows CMake compiler remnants
	rmdir /s /q build
	rmdir /s /q out

clean-windows: # clean the Windows standard compiler remnants
	del *.obj
	del *.o
	del *.exe

clean-cmake-shell: # clean the Unix CMake compiler remnants
	rm -rf build

clean-shell: # clean the Unix standard compiler remnants
	rm Connect
	rm *.o
	rm *.out
	rm *.run