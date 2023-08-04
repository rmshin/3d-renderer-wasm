run:
	make build && ./renderer

build:
	gcc -L/opt/homebrew/lib -lSDL2 -lm -I/opt/homebrew/include -D_THREAD_SAFE -Wall -Wfatal-errors -std=c99 ./src/*.c -o renderer

clean:
	rm renderer