all:
	make main && make fast && make mrv && make old
main:
	gcc -Iinclude -lglut -lGLU -lEGL main.c src/gl.c -o main
fast:
	gcc -Iinclude -lglut -lGLU -lEGL -Ofast main.c src/gl.c -o fast
mrv:
	gcc -Iinclude -lglut -lGLU -lEGL mrv.c src/gl.c -o mrv
old:
	gcc -Iinclude -lglut -lGLU -lEGL old.c src/gl.c -o old
clean:
	rm main && rm fast && rm mrv && rm old && rm benchmark-report
