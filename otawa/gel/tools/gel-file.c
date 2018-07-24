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
#include <gel/error.h>

/**
 * Display the help message and exit.
 */
void help(void) {
	printf(
		"SYNTAX: gel-file [-hn] <file name>\n"
		"  -h -- display this help message\n"
		"  -a -- display all information\n"
	);
	exit(1);
}


/**
 * Get a string for qualifying type of a file.
 * @param	type	File type.
 * @return			Matching string.
 */
const char *get_type(u16_t type) {
	static const char *names[] = {
		"ET_NONE",
		"ET_REL",
		"ET_EXEC",
		"ET_DYN",
		"ET_CORE"
	};
	if(type > sizeof(names) / sizeof(void *))
		return "UNKNOWN";
	else
		return names[type];
}


/**
 * Get the machine name from its code (if known).
 * @param	machine		Machine type.
 *  @return				Matching name.
 */
const char *get_machine(u16_t machine) {
	static struct {
		u16_t code;
		const char *name;
	} machines[] = {
		{ 0, "EM_NONE" },
		{ 1, "EM_M32" },
		{ 2, "EM_SPARC" },
		{ 3, "EM_386" },
		{ 4, "EM_68K" },
		{ 5, "EM_88K" },
		{ 6, "reserved" },
		{ 7, "EM_860" },
		{ 8, "EM_MIPS" },
		{ 9, "reserved" },
		{ 10, "EM_MIPS_RS4_BE" },
		{ 15, "EM_PARISC" },
		{ 17, "EM_VPP500" },
		{ 18, "EM_SPARC32PLUS" },
		{ 19, "EM_960" },
		{ 20, "EM_PPC" },
		{ 36, "EM_V800" },
		{ 37, "EM_FR20" },
		{ 38, "EM_RH32" },
		{ 39, "EM_RCE" },
		{ 40, "EM_ARM" },
		{ 41, "EM_ALPHA" },
		{ 42, "EM_SH" },
		{ 43, "EM_SPARCV9" },
		{ 44, "EM_TRICORE" },
		{ 45, "EM_ARC" },
		{ 46, "EM_H8_300" },
		{ 47, "EM_H8_300H" },
		{ 48, "EM_H8S" },
		{ 49, "EM_H8_500" },
		{ 50, "EM_IA_64" },
		{ 51, "EM_MIPS_X" },
		{ 52, "EM_COLDFIRE" },
		{ 53, "EM_68HC12" },
		{ 54, "EM_MMA" },
		{ 55, "EM_PCP" },
		{ 56, "EM_NCPU" },
		{ 57, "EM_NDR1" },
		{ 58, "EM_STARCORE" },
		{ 59, "EM_ME16" },
		{ 60, "EM_ST100" },
		{ 61, "EM_TINYJ" },
		{ 66, "EM_FX66" },
		{ 67, "EM_ST9PLUS" },
		{ 68, "EM_ST7" },
		{ 69, "EM_68HC16" },
		{ 70, "EM_68HC11" },
		{ 71, "EM_68HC08" },
		{ 72, "EM_68HC05" },
		{ 73, "EM_SVX" },
		{ 74, "EM_ST19" },
		{ 75, "EM_VAX" },
		{ 76, "EM_CRIS" },
		{ 77, "EM_JAVELIN" },
		{ 78, "EM_FIREPATH" },
		{ 79, "EM_ZSP" },
		{ 80, "EM_MMIX" },
		{ 81, "EM_HUANY" },
		{ 82, "EM_PRISM" },
		{ 0, 0 }
	};
	int i;

	for(i = 0; machines[i].name; i++)
		if(machines[i].code == machine)
			return machines[i].name;
	return "unknown";
}


/**
 * Display a block of bytes.
 * @param	bytes	Block to display.
 * @param	size	Size of the block.
 * @param	width	Number of bytes to display on a line.
 */
void display_block(const char *bytes, int size, int width) {
	int i, j;
	for(i = 0; i < size; i += width) {
		for(j = 0; j < width && i + j < size; j++)
			printf("%02x", bytes[i + j]);
		putchar('\t');
		for(j = 0; (j < width) && (i + j < size); j++) {
			if(isprint(bytes[i + j]))
				putchar(bytes[i + j]);
			else
				putchar('.');
		}
		putchar('\n');
	}
}


/**
 * Get a string matching the class.
 * @param class	Current class.
 * @return		Matching string.
 */
const char *get_class(char clazz) {
	switch(clazz) {
	case ELFCLASSNONE: return "ELFCLASSNONE";
	case ELFCLASS32: return "ELFCLASS32";
	case ELFCLASS64: return "EFLCLASS64";
	default: return "unknown";
	}
}


/**
 * Get a string matching the data.
 * @param data	Current data.
 * @return		Matching string.
 */
const char *get_data(char data) {
	switch(data) {
	case ELFDATANONE: return "ELFDATANONE";
	case ELFDATA2LSB: return "ELFDATA2LSB";
	case ELFDATA2MSB: return "ELFDATA2MSB";
	default: return "unknown";
	}
}


/**
 * Command entry point.
 */
int main(int argc, char **argv) {
	const char *path;
	gel_file_t *file;
	int opt;
	uint32_t flags = 0;
#	define	ALL		0xffffffff
#	define	NAMED	0x80000000
	gel_file_info_t infos;

	/* Check arguments */
	opterr = 1;
	while((opt = getopt(argc, argv, "ha")) != EOF)
		switch(opt) {
		case 'a':
			flags = ALL;
			break;
		case '?':
		case 'h':
			help();
			break;
		default:
			assert(0);
		}
	if(optind >= argc)
		help();
	path = argv[optind];

	/* open the file */
	file = gel_open(path, "", 0);
	if(file == NULL) {
    	printf("ERROR: %s\n", gel_strerror());
    	return 2;
  	}

	/* get GEL header */
	if(gel_file_infos(file, &infos) == -1) {
		fprintf(stderr, "ERROR: %s\n", gel_strerror());
		return 2;
	}

	/* Display information */
	printf("file name = %s\n", infos.filename);
	printf("type = %s (0x%04x)\n", get_type(infos.type), infos.type);
	printf("machine = %s (0x%04x)\n", get_machine(infos.machine), infos.machine);
	printf("version = %d\n", infos.version);
	printf("entry = 0x%08x\n", infos.entry);
	printf("identification\n");
	display_block(infos.ident, sizeof(infos.ident), 4);
	printf("ident[EI_CLASS] = %s (%d)\n", get_class(infos.ident[EI_CLASS]), infos.ident[EI_CLASS]);
	printf("ident[EI_DATA] = %s (%d)\n", get_data(infos.ident[EI_DATA]), infos.ident[EI_DATA]);
	printf("ident[EI_OSABI] = %d\n", infos.ident[EI_OSABI]);


	/* cleanup */
	gel_close(file);
	return 0;
}
