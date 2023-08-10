run:
	make build && ./renderer

build:
	gcc -L/opt/homebrew/lib -lSDL2 -lm -I/opt/homebrew/include -D_THREAD_SAFE -Wall -Wfatal-errors -std=c99 ./src/*.c -o renderer

clean:
	rm renderer

wasm:
	emcc ./src/*.c -o web/renderer.js -g -lm --bind -s USE_SDL=2 --preload-file assets/ --use-preload-plugins -sALLOW_MEMORY_GROWTH -sENVIRONMENT=web -sMAX_WEBGL_VERSION=2 -sEXPORTED_FUNCTIONS=_main,_set_display_mode,_set_rotation_mode,_load_single_mesh,_load_all_meshes,_mesh_has_texture

wasm-prod:
	emcc ./src/*.c -o web/dist/renderer.js -g -lm --bind -s USE_SDL=2 --preload-file assets/ --use-preload-plugins -sALLOW_MEMORY_GROWTH -sENVIRONMENT=web -sMAX_WEBGL_VERSION=2 -sEXPORTED_FUNCTIONS=_main,_set_display_mode,_set_rotation_mode,_load_single_mesh,_load_all_meshes,_mesh_has_texture -O2