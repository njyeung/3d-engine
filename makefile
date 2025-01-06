run: compile
	./app
compile:
	clang++ -std=c++17 -fdiagnostics-color=always -Wall -g -Wl,-rpath,@executable_path/lib -I/Users/nickjyeung/Desktop/3d-engine/include -L/Users/nickjyeung/Desktop/3d-engine/lib /Users/nickjyeung/Desktop/3d-engine/lib/libglfw.3.dylib /Users/nickjyeung/Desktop/3d-engine/*.cpp /Users/nickjyeung/Desktop/3d-engine/glad.c -o /Users/nickjyeung/Desktop/3d-engine/app -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation -Wno-deprecated