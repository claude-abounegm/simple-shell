project1os:	project1.h project1.c circular_array.c circular_array.h
	gcc -Wall project1.c circular_array.c -o project1os

clean:
	rm -f project1os