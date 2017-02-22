CC=clang

all: tables repetition ell roots blzzd_tables blzzd_roots bitrev_tables microsoft_tables \
	shoup_table shoup_scaled_table rev_shoup_table rev_shoup_scaled_table \
	psi_power_tables shoup_red_table shoup_red_scaled_table \
	rev_shoup_red_table rev_shoup_red_scaled_table


tables:  tables.c
	$(CC) -Wall tables.c -lmpfr -lgmp -o tables

repetition:  repetition.c
	$(CC) -Wall repetition.c -lmpfr -lgmp -o repetition

ell:  ell.c
	$(CC) -Wall ell.c -lmpfr -lgmp -o ell

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

shoup_table: shoup_table.c
	$(CC) -Wall shoup_table.c -o shoup_table

shoup_scaled_table: shoup_scaled_table.c
	$(CC) -Wall shoup_scaled_table.c -o shoup_scaled_table

rev_shoup_table: rev_shoup_table.c
	$(CC) -Wall rev_shoup_table.c -o rev_shoup_table

rev_shoup_scaled_table: rev_shoup_scaled_table.c
	$(CC) -Wall rev_shoup_scaled_table.c -o rev_shoup_scaled_table

shoup_red_table: shoup_red_table.c
	$(CC) -Wall shoup_red_table.c -o shoup_red_table

shoup_red_scaled_table: shoup_red_scaled_table.c
	$(CC) -Wall shoup_red_scaled_table.c -o shoup_red_scaled_table

rev_shoup_red_table: rev_shoup_red_table.c
	$(CC) -Wall rev_shoup_red_table.c -o rev_shoup_red_table

rev_shoup_red_scaled_table: rev_shoup_red_scaled_table.c
	$(CC) -Wall rev_shoup_red_scaled_table.c -o rev_shoup_red_scaled_table

psi_power_tables: psi_power_tables.c
	$(CC) -Wall psi_power_tables.c -o psi_power_tables

clean:
	rm -f tables  repetition roots blzzd_tables blzzd_roots bitrev_tables \
	  microsoft_tables shoup_table shoup_scaled_table \
	  rev_shoup_table rev_shoup_scaled_table psi_power_tables \
	  shoup_red_table shoup_red_scaled_table \
	  rev_shoup_red_table rev_shoup_red_scaled_table
	rm -f *~
	rm -rf *.dSYM

.phony: all clean
