run:
	make build && ./renderer

build:
	gcc -L/opt/homebrew/lib -lSDL2 -lm -I/opt/homebrew/include -D_THREAD_SAFE -Wall -Wfatal-errors -std=c99 ./src/*.c -o renderer

clean:
	rm renderer

wasm:
	emcc ./src/*.c -o renderer.js -g -lm --bind -s USE_SDL=2 --preload-file assets/ --use-preload-plugins -sALLOW_MEMORY_GROWTH -sENVIRONMENT=web -sMAX_WEBGL_VERSION=2