#include <stdio.h>
#include <assert.h>
#include "gel/gel.h"
#include "gel/dwarf_line.h"

int main(int argc, char **argv) {
	assert(argc > 1);
	dwarf_line_iter_t line;
	dwarf_addr_iter_t addr;
	dwarf_location_t loc;
	
	/* Open the file */
	gel_file_t *file = gel_open(argv[1], "", 0);
	if(!file) {
		printf("ERROR: open(%d: %s)\n", gel_errno, gel_strerror());
		return 1;
	}
	
	/* Get dwarf information */
	dwarf_line_map_t *map = dwarf_new_line_map(file, 0);
	if(!map) {
		printf("ERROR: %d\n", gel_errno);
		return 1;
	}
	
	/* Display lines */
	printf("LINES\n");
	loc = dwarf_first_line(&line, map);
	while(loc.file) {
		printf("%s:%d: %08lx-%08lx\n", loc.file, loc.line,
			(unsigned long)loc.low_addr, (unsigned long)loc.high_addr);
		loc = dwarf_next_line(&line);
	}
	
	/* Display addresses */
	printf("ADDRESSES\n");
	loc = dwarf_first_addr(&addr, map);
	while(loc.file) {
			printf("%08lx -> %s:%d\n", (unsigned long)loc.low_addr,
				loc.file, loc.line);
			loc = dwarf_next_addr(&addr);
	}
	
	/* success */
	dwarf_delete_line_map(map);
	printf("SUCCESS\n");
	return 0;
}
