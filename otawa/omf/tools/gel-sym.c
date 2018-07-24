/*
 * $Id$
 * Copyright (c) 2008, IRIT- UPS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gel/gel.h>
#include <gel/gel_elf.h>
#include "../src/gel_types_private.h"


/**
 * Display the help message and exit.
 */
void help(void) {
	printf(
		"SYNTAX: gel-sym [-h] <file name>\n"
	);
	exit(1);
}
#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) & 0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))


/**
 * Get the binding of a symbol.
 * @param infos		Information about the symbol.
 * @return			Binding as a string.
 */
const char *get_binding(const gel_sym_info_t *infos) {
	static char buf[32];
	switch(ELF32_ST_BIND(infos->info)) {
	case STB_LOCAL: return "local";
	case STB_GLOBAL: return "global";
	case STB_WEAK: return "weak";
	default:
		sprintf(buf, "%d", ELF32_ST_BIND(infos->info));
		return buf;
	}
}


/**
 * Get the type of a symbol.
 * @param infos		Information about the symbol.
 * @return			Type as a string.
 */
const char *get_type(const gel_sym_info_t *infos) {
	static char buf[32];
	switch(ELF32_ST_TYPE(infos->info)) {
	case STT_NOTYPE: return "notype";
	case STT_OBJECT: return "object";
	case STT_FUNC: return "func";
	case STT_SECTION: return "section";
	case STT_FILE: return "file";
	default:
		sprintf(buf, "%d", ELF32_ST_TYPE(infos->info));
		return buf;
	}
}


/**
 * Get a section as a text.
 * @param file		Current file.
 * @param infos		Information about the symbol.
 * @return			Section as a string.
 */
const char *get_section_index(gel_file_t *file, const gel_sym_info_t *infos) {
	static char buf[32];
	switch(infos->sect) {
	case SHN_UNDEF: return "undef";
	case SHN_ABS: return "abs";
	case SHN_COMMON: return "common";
	default: {
			gel_sect_info_t si;
			gel_sect_t *sect = gel_getsectbyidx(file, infos->sect);
			if(sect == NULL) {
				sprintf(buf, "%d", infos->sect);
				return buf;
			}
			gel_sect_infos(sect, &si);
			return si.name;
		}
	}
}


int main(int argc, char **argv) {
	const char *path;
	gel_file_t *file;
	int opt;
	gel_sym_iter_t iter;
	gel_sym_t *sym;

	/* Check arguments */
	opterr = 1;
	while((opt = getopt(argc, argv, "h")) != EOF)
		switch(opt) {
		case 'h':
			help();
		default:
			assert(0);
		}
	if(optind >= argc)
		help();
	path = argv[optind];

	/* open the file */
	file = gel_open(path, "", 0);
	if(file == NULL) {
    	fprintf(stderr, "ERROR: %s\n", gel_strerror());
    	return 2;
  	}

	/* display header */
	printf("st_value st_size  binding type    st_shndx         name\n");

	/* iterate on symboles */
	for(sym = gel_sym_first(&iter, file); sym; sym = gel_sym_next(&iter)) {

		/* get information */
		gel_sym_info_t infos;
		gel_sym_infos(sym, &infos);
		if(strcmp(infos.name, "") == 0)
			continue;

		/* display information */
		printf("%08x %8x %-7s %-7s %-16s %s\n",
			infos.vaddr,
			infos.size,
			get_binding(&infos),
			get_type(&infos),
			get_section_index(file, &infos),
			infos.name);
	}

	/* cleanup */
	gel_close(file);
	return 0;
}
