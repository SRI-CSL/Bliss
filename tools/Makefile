

all: tables roots blzzd_tables blzzd_roots bitrev_tables microsoft_tables \
	shoup_tables shoup_scaled_tables

tables:  tables.c
	$(CC) -Wall tables.c -lmpfr -lgmp -o tables

roots: roots.c 
	$(CC) -Wall roots.c  -lgmp  -o roots

blzzd_tables: blzzd_tables.c
	$(CC) -Wall blzzd_tables.c -o blzzd_tables

blzzd_roots: blzzd_roots.c
	$(CC) -Wall blzzd_roots.c -o blzzd_roots

bitrev_tables: bitrev_tables.c
	$(CC) -Wall bitrev_tables.c -o bitrev_tables

microsoft_tables: microsoft_tables.c
	$(CC) -Wall microsoft_tables.c -o microsoft_tables

shoup_tables: shoup_tables.c
	$(CC) -Wall shoup_tables.c -o shoup_tables

shoup_scaled_tables: shoup_scaled_tables.c
	$(CC) -Wall shoup_scaled_tables.c -o shoup_scaled_tables

clean:
	rm -f tables roots blzzd_tables blzzd_roots bitrev_tables \
	  microsoft_tables shoup_tables shoup_scaled_tables
