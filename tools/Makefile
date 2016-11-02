

all: tables roots



tables:  tables.c
	$(CC) -Wall tables.c -lmpfr -lgmp -o tables

roots: roots.c 
	$(CC) -Wall roots.c  -lgmp  -o roots


clean:
	rm -f tables roots
