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
#include <gel/gel.h>
#include <gel/gel_elf.h>
#include <unistd.h>
#include <assert.h>
#include <gel/file.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Display the help message and exit.
 */
void help(void) {
	printf(
		"SYNTAX: gel-sect [-h] [-f ADDRESS] <file name>\n"
		"	-f ADDRESS -- find the section containing this address"
		"FLAGS :\n"
		"	W -- SHF_WRITE\n"
		"	A -- SHF_ALLOC\n"
		"	X -- SHF_EXECINSTR\n"
	);
	exit(1);
}


const char *get_type(u32_t type) {
	static const char *label[] = {
		"SHT_NULL",
		"SHT_PROGBITS",
		"SHT_SYMTAB",
		"SHT_STRTAB",
		"SHT_RELA",
		"SHT_HASH",
		"SHT_DYNAMIC",
		"SHT_NOTE",
		"SHT_NOBITS",
		"SHT_REL",
		"SHT_SHLIB",
		"SHT_DYNSYM"
	};
	static char buf[10];
	if(type >= sizeof(label) / sizeof(const char *)) {
		sprintf(buf, "%08x", type);
		return buf;
	}
	else
		return label[type];
}

void display_section(int i, gel_sect_info_t *info) {
	printf("%5u %-12s %c%c%c   %08x %08x %08x %s\n", i, get_type(info->type),
		(info->flags & SHF_WRITE ? 'W' : '-'),
		(info->flags & SHF_ALLOC ? 'A' : '-'),
		(info->flags & SHF_EXECINSTR ? 'X' : '-'),
		info->vaddr, info->size, info->offset, info->name);
}


/**
 * Command entry point.
 */
int main(int argc, char **argv) {
	const char *path;
	gel_file_t *file;
	int opt, i = 0;
	const char *find = 0;
	
	/* Check arguments */
	opterr = 1;
	while((opt = getopt(argc, argv, "hf:")) != EOF)
		switch(opt) {
		case 'h':
			help();
		case 'f':
			find = optarg;
			break;
		default:
			assert(0);
		}
	if(optind >= argc) {
		fprintf(stderr, "ERROR: a binary file is required !\n");
		help();
	}
	path = argv[optind];
	
	/* open the file */
	file = gel_open(path, "", 0);
	if(file == NULL) {
    	printf("ERROR: %s\n", gel_strerror());
    	return 2;
  	}

	/* find the section if required */
	if(find) {

		/* convert address */
		uint32_t addr;
		addr = strtoul(find, 0, 16);

		/* perform the search */
		for(i = 0; i < file->sectnum; i++) {

			/* get the section information */
			gel_sect_info_t info;
			gel_sect_t *sect = gel_getsectbyidx(file, i);
			assert(sect);
			if(gel_sect_infos(sect, &info) < 0) {
				fprintf(stderr, "ERROR: %s\n", gel_strerror());
				return 3;
			}

			/* found ? */
			if(addr >= info.vaddr && addr < info.vaddr + info.size) {
				printf("address %08x found in section \"%s\"\n", addr, info.name);
				display_section(i, &info);
				return 0;
			}

		}
		fprintf(stderr, "ERROR: no section containing address %08x\n", addr);
		return 1;
	}

	/* list sections */
	printf("INDEX TYPE         FLAGS VADDR    SIZE     OFFSET   NAME\n");
	for(i = 0; i < file->sectnum; i++) {
		
		/* get information */
		gel_sect_info_t info;
		gel_sect_t *sect = gel_getsectbyidx(file, i);
		assert(sect);
		if(gel_sect_infos(sect, &info) < 0) {
			fprintf(stderr, "ERROR: %s\n", gel_strerror());
			return 3;
		}
		
		/* display information */
		display_section(i, &info);
	}

	/* cleanup */ 
	gel_close(file);
	return 0;
}
