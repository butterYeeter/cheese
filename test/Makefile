main:	test.o
	gcc -o main test.o glad.o stb_image.o  fast_obj.o -I../include -L../lib/windows -lglfw3dll -g -O0

test.o: test.c
	gcc -c test.c -I../include -L../lib/windows -lglfw3dll -g -O0