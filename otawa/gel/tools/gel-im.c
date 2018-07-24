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
#include <gel/image.h>
#include <unistd.h>
#include <assert.h>

/**
 * Display the help message and exit.
 */
void help(void) {
	printf(
		"SYNTAX: gel-im [-h] <file name>\n"
		"  -h - display this help message\n"
		"  -s - do not initialize the stack\n"
	);
	exit(1);
}


/**
 * Command entry point.
 */
int main(int argc, char **argv) {
	const char *path;
	gel_file_t *file;
	gel_image_t *im;
	int i;
	int opt;
	int opt_note = 0;
	gel_env_t *env;

	// prepare the environment
	env = gel_default_env();
	
	/* Check arguments */
	opterr = 1;
	while((opt = getopt(argc, argv, "hs")) != EOF)
		switch(opt) {
		case '?':
		case 'h':
			help();
			break;
		case 's':
			env->flags |= GEL_ENV_NO_STACK;
			break;
		default:
			assert(0);
		}
	if(optind >= argc)
		help();
	path = argv[optind];
	
	/* open the file */
	file = gel_open(path, "", 0);
	if(file == NULL)
    	return 2;

	/* build the image */
	im = gel_image_load(file, env, 0);
	if(!im) {
		fprintf(stderr, "ERROR: cannot make image: %s\n", gel_strerror ());
		free(env);
		return 3;
	}
	
	/* dump the image */
	else {
		int i;
		gel_image_info_t info;
		gel_image_infos(im, &info);
		for(i = 0; i < info.membersnum; i++) {
			int p = 15;
			gel_cursor_t c;
			gel_block2cursor(info.members[i], &c);
			printf("BLOCK %08lx (%08lx)",
				gel_cursor_vaddr(c),
				gel_cursor_avail(c));
			while(!gel_cursor_at_end(c)) {
				if(p == 15) {
					p = 0;
					printf("\n%08lx", gel_cursor_vaddr(c));
				}
				else
					p++;
				printf(" %02X", gel_read_u8(c));
			}
			printf("\n\n");
		}
		gel_image_close(im);
		free(env);
	}
 
	/* cleanup */ 
	gel_close(file);
	return 0;
}
