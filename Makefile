all:
	gcc -Iinclude -lglut -lGLU -lEGL main.c src/gl.c -o main

clean:
	rm main
