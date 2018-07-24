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
#include <gel/file.h>
#include <gel/dwarf_line.h>
#include <gel/error.h>
#include <stdlib.h>

/**
 * Display the help message and exit.
 */
void help(void) {
	printf(
		"SYNTAX: gel-prog [-hn] <file name>\n"
		"  -h -- display this help message\n"
		"  -l -- display file/line to code [default]\n"
		"  -c -- display code to file/line\n"
		"  -C line -- display code address for the given line\n"
		"  -L address -- get file and line for the given address\n"
	);
	exit(1);
}


/**
 * Command entry point.
 */
int main(int argc, char **argv) {
	const char *path;
	gel_file_t *file;
	int opt;
	enum {
		LINE_TO_CODE = 0,
		CODE_TO_LINE = 1,
		GET_LINE = 2,
		GET_ADDRESS = 3
	} opt_mode = LINE_TO_CODE;
	uint32_t opt_addr = 0;
	int opt_line = 0;
	const char *opt_file = 0;
	dwarf_line_map_t *map;

	/* Check arguments */
	opterr = 1;
	while((opt = getopt(argc, argv, "nlcL:C:")) != EOF)
		switch(opt) {
		case 'l':
			opt_mode = LINE_TO_CODE;
			break;
		case 'c':
			opt_mode = CODE_TO_LINE;
			break;
		case 'L':
			opt_mode = GET_LINE;
			opt_addr = strtoul(optarg, 0, 16);
			break;
		case 'C': {
				char *pos = strrchr(optarg, ':');
				if(!pos) {
					fprintf(stderr, "ERROR: -C arguments requires file:line format");
					return 3;
				}
				*pos = '\0';
				opt_file = optarg;
				opt_line = strtoul(pos + 1, 0, 10);
				opt_mode = GET_ADDRESS;
			}
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

	/* Get the line map */
	map = gel_new_line_map(file);
	if(!map) {
		switch(gel_errno) {
		case GEL_ENOTFOUND:
			fprintf(stderr, "ERROR: no debugging line section in %s\n", path);
			return 1;
		case GEL_EFORMAT:
			fprintf(stderr, "ERROR: bad formatted debugging line\n");
			return 1;
		case GEL_ERESOURCE:
			fprintf(stderr, "ERROR: not enough memory \n");
			return 1;
		case GEL_EDEPEND:
			fprintf(stderr, "ERROR: no parser for available debugging information\n");
			return 2;			
		default:
			assert(0);
		}
		gel_close(file);
		return 1;
	}

	switch(opt_mode) {

	/* LINE_TO_CODE */
	case LINE_TO_CODE: {
			dwarf_line_iter_t iter;
			dwarf_location_t loc;
			printf("LOW      HIGH     LINE     FILE\n");
			for(loc = dwarf_first_line(&iter, map);
			loc.file;
			loc = dwarf_next_line(&iter))
				printf("%08x %08x %8d %s\n", loc.low_addr, loc.high_addr,
					loc.line, loc.file);
		}
		break;

	/* CODE_TO_LINE */
	case CODE_TO_LINE: {
			dwarf_addr_iter_t iter;
			dwarf_location_t loc;
			printf("LOW      HIGH     LINE     FILE\n");
			for(loc = dwarf_first_addr(&iter, map);
			loc.file;
			loc = dwarf_next_addr(&iter))
				printf("%08x %08x %8d %s\n", loc.low_addr, loc.high_addr,
					loc.line, loc.file);
		}
		break;

	/* GET_LINE */
	case GET_LINE: {
			const char *file;
			int line;
			if(dwarf_line_from_address(map, opt_addr, &file, &line) == -1) {
				fprintf(stderr, "ERROR: %08x does not match\n", opt_addr);
				return 1;
			}
			else
				printf("%s:%d\n", file, line);
		}
		break;

	/* GET_ADDRESS */
	case GET_ADDRESS: {
			vaddr_t addr = dwarf_address_from_line(map, opt_file, opt_line);
			if(!addr) {
				fprintf(stderr, "unknown line %s:%d\n", opt_file, opt_line);
				return 1;
			}
			else
				printf("0x%08x\n", addr);
		}
		break;

	default:
		assert(0);
	}

	/* cleanup */
	gel_delete_line_map(map);
	gel_close(file);
	return 0;
}
