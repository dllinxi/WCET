/*
 * $Id$
 * Copyright (c) 2005-07, IRIT- UPS
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
#include <ctype.h>
#include <gel/prog.h>

/**
 * Display the help message and exit.
 */
void help(void) {
	printf(
		"SYNTAX: gel-prog [-hn] <file name>\n"
		"  -h - display this help message\n"
		"  -n - display the content of the PT_NOTE segments\n"
	);
	exit(1);
}


/**
 * Print the flags.
 * @param flags	Flags to display.
 */
void print_flags(Elf32_Word flags) {
	int one = 0;
	printf("%08x ", flags);
	if(flags & PF_X) {
		one = 1;
		printf("PF_X");
	}
	if(flags & PF_W) {
		if(one)
			printf(", ");
		else
			one = 1;
		printf("PF_W");
	}
	if(flags & PF_R) {
		if(one)
			printf(", ");
		else
			one = 1;
		printf("PF_R");
	}
	putchar('\n');
}


/**
 * Display the program header type.
 * @param type	Type to display.
 */
void print_type(Elf32_Word type) {
	static char *names[] = {
		"PT_NULL",
		"PT_LOAD",
		"PT_DYNAMIC",
		"PT_INTERP",
		"PT_NOTE",
		"PT_SHLIB",
		"PT_PHDR"
	};
	if(type <= PT_PHDR)
		printf("%-10s ", names[type]);
	else
		printf("%-10d ", type);
}


/**
 * Display a note program header.
 * @param phdr	Program header to process.
 */
void print_note(gel_prog_t *phdr) {
	gel_note_t cursor;
	
	/* Get the cursor */
	if(gel_note_init(phdr, &cursor) == -1)
		exit(1);
	
	/* Read it until end */
	while(gel_note_next(&cursor) != -1) {
		
		/* Display name */
		printf("NOTE \"%s\" : %d\n", gel_note_name(&cursor), gel_note_type(&cursor));
		
		/* Display description */
		{
			char buffer[gel_note_desc(&cursor).size];
			int i, j;
			gel_read_data(
				gel_note_desc(&cursor),
				buffer,
				gel_note_desc(&cursor).size);
			for(i = 0; i < gel_note_desc(&cursor).size; i += 8) {
				printf("%08x ", i);
				for(j = 0; j < 8; j++) {
					if(i + j < gel_note_desc(&cursor).size)
						printf("%02x", (int)buffer[i + j]);
					else
						printf("  ");
				}
				putchar(' ');
				for(j = 0; j < 8; j++) {
					if(i + j >= gel_note_desc(&cursor).size)
						putchar(' ');
					else if(isprint(buffer[i + j]))
						putchar(buffer[i + j]);
					else
						putchar('.');
				}
				putchar('\n');			
			}
		}
	}
	
	/* cleanup */
	gel_note_destroy(&cursor);
}


/**
 * Command entry point.
 */
int main(int argc, char **argv) {
	const char *path;
	gel_file_t *file;
	gel_file_info_t finfo;
	int i;
	int opt;
	int opt_note = 0;
	
	/* Check arguments */
	opterr = 1;
	while((opt = getopt(argc, argv, "nh")) != EOF)
		switch(opt) {
		case 'n':
			opt_note = 1;
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
	if(file == NULL || gel_file_infos(file, &finfo) == -1)
    	return 2;

	/* Display the headers */
	if(!opt_note)
		printf( "Index "
				"p_type     "
				"p_offset "
				"p_vaddr  "
				"p_paddr  "
				"p_filesz "
				"p_memsz  "
				"p_align  "
				"p_flags\n");
	
 	/* Display the program headers */
 	for(i = 0; i < finfo.prognum; i++) {
 		
 		// Get the entry
 		gel_prog_info_t pinfo;
 		gel_prog_t *prog = gel_getprogbyidx(file, i);
 		if(prog == NULL || gel_prog_infos(prog, &pinfo) == -1) {
    		printf("ERROR: %s\n", gel_strerror());
    		return 3;
 		}
 
 		// Display it
 		if(opt_note) {
 			if(pinfo.type == PT_NOTE)
 				print_note(prog);
 		}
 
 		// Display the information
 		else {
 			printf("%-6d", i);
 			print_type(pinfo.type);
 			printf("%08x ", pinfo.offset);
 			printf("%08x ", pinfo.vaddr);
 			printf("%08x ", pinfo.paddr);
 			printf("%08x ", pinfo.filesz);
 			printf("%08x ", pinfo.memsz);
 			printf("%-8x ", pinfo.align);
 			print_flags(pinfo.flags);
 		}
 	}
 
	/* cleanup */ 
	gel_close(file);
	return 0;
}
