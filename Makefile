all:
	make main && make fast && make old
main:
	gcc -Iinclude -lglut -lGLU -lEGL main.c src/gl.c -o main
fast:
	gcc -Iinclude -lglut -lGLU -lEGL -Ofast main.c src/gl.c -o fast
old:
	gcc -Iinclude -lglut -lGLU -lEGL old.c src/gl.c -o old
clean:
	rm main && rm fast && rm old
