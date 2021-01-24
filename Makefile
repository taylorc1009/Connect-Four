play: # this should also work using shell, but just incase it doesn't there's a failsafe command below
	build/Connect

play-failsafe:
	./build/Connect

play-windows:
	Connect

play-shell:
	./Connect

compile:
	rm -rf build
	mkdir build
	cmake -B build
	cmake --build build

compile-windows:
	cl structs/Stack.c structs/Hashmap.c AI.c data.c game.c menu.c /link /out:Connect.exe

compile-gcc:
	gcc structs/Stack.c structs/Hashmap.c AI.c common/functions.c common/data.c game.c menu.c -lm -o Connect

clean:
	rm -rf build

clean-windows:
	del *.obj
	del *.o
	del *.exe

clean-shell:
	rm Connect
	rm *.o
	rm *.out
	rm *.run