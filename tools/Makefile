

all: tables roots blzzd_tables blzzd_roots

tables:  tables.c
	$(CC) -Wall tables.c -lmpfr -lgmp -o tables

roots: roots.c 
	$(CC) -Wall roots.c  -lgmp  -o roots

blzzd_tables: blzzd_tables.c
	$(CC) -Wall blzzd_tables.c -o blzzd_tables

blzzd_roots: blzzd_roots.c
	$(CC) -Wall blzzd_roots.c -o blzzd_roots

clean:
	rm -f tables roots blzzd_tables blzzd_roots
