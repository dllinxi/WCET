/*********************
Gliss CPU simulator validator
main.c : main functions
**********************/

#define MAIN_C
#define ISS_DISASM

#include "interface_code.h"
#include GLISS_REG_H
#include GLISS_API_H
#include "internal.h"
#include "all_inc.h"
#include "io_module.h"


/* pipes to communicate with GDB */
int to_gdb_pipe[2];
int from_gdb_pipe[2];

FILE * from_gdb;
uint32_t gdb_pc;
uint32_t gdb_npc;
uint32_t gliss_pc;
uint32_t gliss_npc;

int display_values;
int display_replies;
int display_full_dumps;
PROC(_state_t) * real_state;
PROC(_inst_t) *curinstr;
PROC(_platform_t) *platform;
PROC(_sim_t) *iss;


pid_t drive_gdb_pid;

void drive_gdb(); //la fonction qui exec() gdb
int init_gdb(char *, char *);
int init_gliss(char *);
void gdb_disasm_error_report();
char target_location[50];
int exit_on_errors = 1;
char gpname[200];
//int init_registers = 1; 
int instr_count;

void catch_sigusr1(int sig)
	{
	fprintf(stderr, "Program at 0x%08x, instruction number %d\n", gdb_pc, instr_count);
	}
	
void usage(char * pname)
{
	fprintf(stderr, "Usage: %s\t[-V|--version] [-h|--help] [--log|-l] [--values|-v] [--replies|-r] [--no-exit-error|-x] [--program|-p] [--dumps|-d] [target_host:port]\n", pname);
	fprintf(stderr, "\t--version\tdisplay version number\n"
			"\t--help\t\tdisplay this help screen\n"
			"\t--log\t\tlog all communication with gdb in `pwd`/log\n"
			"\t--values\tdisplay registers values on screen\n"
			"\t--replies\tdisplay GDB replies on screen\n"
			"\t--no-exit-error\tdo not exit on errors\n"
			"\t--program\tname of the test program to run\n"
			"\t--dumps\t\tdump the values of all registers on error\n");
}
	
void parse_commandline(int argc, char ** argv)
	{
		
	int longindex;
	char option;
	extern int setenv (const char *name, const char *value, int overwrite);
	struct option longopts[] = {

		/*name                  arg             flag                    val */
		{"version",     0,      NULL,   'V'},
		{"help",        0,      NULL,   'h'},
		{"log", 0, NULL,  'l'},
		{"values", 0, NULL, 'v'},
		{"replies", 0, NULL, 'r'},
		{"no-exit-error", 0, NULL, 'x'},
		{"program", 1, NULL, 'p'},
		{"dumps", 0, NULL, 'd'},
		//{"no-reg-init", 0, NULL, 'n'},
		{NULL, 0, NULL, 0},
		};

	char *optstring = "Vhrlvxp:dn";
	while ((option = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1)
	{

		switch (option)
		{
			case 'V': printf("\n"); exit (1); break;
			case 'h': usage(argv[0]); exit(1); break;
			case 'l' : do_logging = 1; break;
			case 'v' : display_values = 1; break;
			case 'r' : display_replies = 1; break;
			case 'x': exit_on_errors = 0; break;
			case 'p': sprintf(gpname, "%s", optarg); break;
			case 'd' : display_full_dumps = 1; break;
			//case 'n' : /*init_registers = 0;*/ break;
			default:
				fprintf(stderr, "Unknown option %c\n", optopt);
				usage(argv[0]);
				exit(5);
		}
	}
		
	if ( optind < argc )
	{
		printf("Using GDB debugger \"%s\"\n", argv[optind]);
		sprintf(target_location, "%s", argv[optind]);
	}
	else 
	{
		fprintf(stderr, "ERROR: No commandline parameter given for GDB debugger location\n");
		usage(argv[0]);
		exit(1);
	}
}
	
void disasm_error_report(char * drive_gdb_reply_buffer, PROC(_state_t) * state, PROC(_inst_t) * instr, int cpt, int do_exit)
{
	printf("PC(nPC) : GDB=%08X(%08X), GLISS=%08X(%08X)\n", gdb_pc, gdb_npc, gliss_pc, gliss_npc);
	
	char * reptr;

	/* example of reply: */
	/* ^done,asm_insns=[{address="0x40000000",func-name="start",offset="0",inst="mov  %g0, %g4"}]\n */
	/* it's an array [.,.,...], each element is like {address=..,func-name=..,offset=..,inst="disasm"} */
	/* we replace "},{" by " \n ", "}]" by "\n\0" and we print the whole string */
	/* beginning after the start of the list "[{" */
	while ( (reptr = strstr(drive_gdb_reply_buffer, "},{")) )
	{
		*reptr = ' ';
		*(reptr+1)= '\n';
		*(reptr+2)= ' ';
	}
	while ( (reptr = strstr(drive_gdb_reply_buffer, "}]")) )
	{
		*reptr = '\n';
		*(reptr+1)= '\0';
	}
	
	reptr = strstr(drive_gdb_reply_buffer, "[{");
	reptr += 2;
	printf("====GDB disasm: ");
	/* cannot use printf as output contains things like %o6, %psr, etc */
	while (*reptr)
		putchar(*reptr++);

	/* now gliss disasm */
	char dis[200];
	if ( instr ) 
	{
		/* only 1 instr */
		uint32_t cod = PROC(_mem_read32)(PROC(_get_memory)(platform, LEON_MAIN_MEMORY), gliss_pc);
		PROC(_disasm)(dis, instr);
		printf("====GLISS disasm: %08X (%08X)  \"%s\"\n", gliss_pc, cod, dis);
	}
	else
	{
		printf("====GLISS disasm: ");
		int n = 0;
		while (n < cpt)
		{
			PROC(_inst_t) *inst = PROC(_decode)(iss->decoder, gliss_pc + (n<<2) /* for RISC 32 bit ISA */);
			PROC(_disasm)(dis, inst);
			printf("%08X  \"%s\"\n", gliss_pc + (n<<2), dis);
			PROC(_free_inst)(inst);
			n++;
		}
	}
	/*dump_float_registers(real_state);*/
	if ( exit_on_errors && do_exit ) exit(1);		
}
	
int init_gliss(char * drive_gdb_reply_buffer)
{
	/* make the platform */
	platform = PROC(_new_platform)();
	if (platform == NULL) {
		fprintf(stderr, "ERROR: no more resources\n");
		exit(2);
	}

	/* load the image in the platform */
	if (PROC(_load_platform)(platform, gpname) == -1) {
		fprintf(stderr, "ERROR: cannot load the given executable : %s.\n", gpname);
		exit(2);
	}

	/* make the state depending on the platform */
	real_state = PROC(_new_state)(platform);
	if (real_state == NULL)  {
		fprintf(stderr, "ERROR: no more resources\n");
		exit(2);
	}

	/* make the simulator */
	iss = PROC(_new_sim)(real_state, 0, 0);
	if (iss == NULL) {
		fprintf(stderr, "ERROR: no more resources\n");
		exit(2);
	}

	/* set callback function for IO memory mapped register zones */
	leon_set_range_callback(leon_get_memory(platform, LEON_MAIN_MEMORY), 0X80000000, 0xFFFFFFFF, &gdb_callback);
	// !!DEBUG!! trouble with mem accesses (double accesses like ldd, std, lddf, stdf), it seems
	//leon_set_range_callback(leon_get_memory(platform, LEON_MAIN_MEMORY), 0X40300000, 0x40400000, &debug_callback);

	send_gdb_cmd("-data-evaluate-expression $sp\n", drive_gdb_reply_buffer, display_replies);
	printf("-data-eval-expr sp :%s\n", drive_gdb_reply_buffer);
	uint32_t sp;
	read_gdb_output_pc(drive_gdb_reply_buffer, &sp);
	printf( " => gdb sp=%08X\n", sp);

	send_gdb_cmd("-data-evaluate-expression $fp\n", drive_gdb_reply_buffer, display_replies);
	printf("-data-eval-expr fp :%s\n", drive_gdb_reply_buffer);
	uint32_t fp;
	read_gdb_output_pc(drive_gdb_reply_buffer, &fp);
	printf( " => gdb fp=%08X\n", fp);

	/* gliss is stopped at PC=0x40000000 (trap table), gdb also
	 * let's read PC and nPC */
	send_gdb_cmd("-data-evaluate-expression $pc\n", drive_gdb_reply_buffer, display_replies);
	read_gdb_output_pc(drive_gdb_reply_buffer, &gdb_pc);
	send_gdb_cmd("-data-evaluate-expression $npc\n", drive_gdb_reply_buffer, display_replies);
	read_gdb_output_pc(drive_gdb_reply_buffer, &gdb_npc);
	gliss_pc = real_state->PC;
	gliss_npc = real_state->nPC;

	/* processor specific initialization code */
	PROC_INIT_CODE

	return 0;
}

int init_gdb(char * drive_gdb_reply_buffer, char * target)
{
	/* Initialisation de GDB */
	close(to_gdb_pipe[0]);
	close(from_gdb_pipe[1]);
	from_gdb = fdopen(from_gdb_pipe[0], "r");
	
	/* wait for the command invite, discard the rest */
	while ( ! strstr(drive_gdb_reply_buffer, "(gdb)") )
	{
		memset(drive_gdb_reply_buffer, 0, 3999);
		fgets(drive_gdb_reply_buffer, 3999, from_gdb);
		printf(drive_gdb_reply_buffer);
	}

	char finalcmd[200];
	snprintf(finalcmd, 199, "-target-select remote tcp:%s\n", target);
	send_gdb_cmd(finalcmd, drive_gdb_reply_buffer, display_replies);
	match_gdb_output(drive_gdb_reply_buffer, "^connected", IS_ERROR, "When connecting to target, ");
	send_gdb_cmd("-target-download\n", drive_gdb_reply_buffer, display_replies);
	match_gdb_output(drive_gdb_reply_buffer, "^done", IS_ERROR, "When connecting to simulator, ");

	return 0;	
}


int main(int argc, char ** argv)
	{
	char drive_gdb_cmd_buffer[150];
	memset(drive_gdb_cmd_buffer, 0, 150);
	char drive_gdb_reply_buffer[4000];
	memset(drive_gdb_reply_buffer, 0, 4000);
	setvbuf(stdout, NULL, _IONBF, 0);
	open_log_file("log");
	parse_commandline(argc, argv);

	if ( ! strlen(gpname) )
	{
		fprintf(stderr, "ERROR: No test program given\n");
		usage(argv[0]);
		exit (1);
	}
		
	/* creating pipes to redirect GDB I/O */
	if ( pipe(to_gdb_pipe) )
	{
		fprintf(stderr, "ERROR: Couldn't create pipe to communicate with gdb driving process\n");
		exit(-1);
	}
	if ( pipe(from_gdb_pipe) )
	{
		fprintf(stderr, "ERROR: Couldn't create pipe to communicate with gdb driving process\n");
		exit(-1);
	}

	struct stat check_file;
	if ( stat(gpname, &check_file))
	{
		fprintf(stderr, "ERROR: Couldn't find test program %s\n", gpname);
		usage(argv[0]);
		exit(1);
	}
			
	signal(SIGUSR1, catch_sigusr1);
		
		
	
	/* launching GDB */
	if ( ! ( drive_gdb_pid = fork () ) )
		drive_gdb();


	printf("Initializing GDB\n");
	init_gdb(drive_gdb_reply_buffer, target_location);
		
	printf("Initializing Gliss\n");
	init_gliss(drive_gdb_reply_buffer);
	
	/* after gliss and gdb are set, initialize the structure containing the infos about registers */
	init_gdb_regs(drive_gdb_reply_buffer);

	instr_count = 0;
	curinstr = PROC(_decode)(iss->decoder, real_state->PC);
	read_vars_this_instruction(drive_gdb_reply_buffer);
	compare_regs_this_instruction(drive_gdb_reply_buffer, real_state, curinstr, instr_count);
	PROC(_free_inst)(curinstr);
	
	/* used to pause gdb while waiting for gliss2 to catch up */
	int stall_gdb = 0;

	while ( 1 ) 
	{
		instr_count++;
		
		/* !!WARNING!! LEON specific */
		stall_gdb = real_state->ANNUL_BIT;
		
		/* update PCs and nPCs */
		send_gdb_cmd("-data-evaluate-expression $pc\n", drive_gdb_reply_buffer, display_replies);
		read_gdb_output_pc(drive_gdb_reply_buffer, &gdb_pc);
		send_gdb_cmd("-data-evaluate-expression $npc\n", drive_gdb_reply_buffer, display_replies);
		read_gdb_output_pc(drive_gdb_reply_buffer, &gdb_npc);
		gliss_pc = real_state->PC;
		gliss_npc = real_state->nPC;
		/*printf("\nAbout to execute inst %d, GDB: PC=%08X(%08X), GLISS: PC=%08X(%08X)\n", instr_count, gdb_pc, gdb_npc, gliss_pc, gliss_npc);*/

		if (! stall_gdb)
		{
			sprintf(drive_gdb_cmd_buffer, "-data-disassemble -s 0x%08X -e 0x%08X -- 0\n", gdb_pc, gdb_pc+4);
			send_gdb_cmd(drive_gdb_cmd_buffer, drive_gdb_reply_buffer, 0);
			/*if ((instr_count % 50000) == 0)
			{
				printf("\nAbout to execute inst %d, GDB: PC=%08X(%08X), GLISS: PC=%08X(%08X)\n", instr_count, gdb_pc, gdb_npc, gliss_pc, gliss_npc);
				disasm_error_report(drive_gdb_reply_buffer, NULL, NULL, 1, 0);
			}*/
		}
		
		/* LEON specific */
		/* GDB steps automaticaly over annuled instr, gliss2 doesn't do that */
		/* so we have to make sure GDB waits for GLISS2 in those cases */
		
		/* GDB step */
		if (! stall_gdb)
		{
			send_gdb_cmd("-exec-step-instruction\n", drive_gdb_reply_buffer, display_replies);
			match_gdb_output(drive_gdb_reply_buffer, "^running", IS_ERROR, "When trying to advance of one step, ");

			wait_for_gdb_output(drive_gdb_reply_buffer, 0);
			if ( ! match_gdb_output(drive_gdb_reply_buffer, "*stopped,reason=\"exited", IS_HANDLED_ELSEWHERE, NULL))
			{
				printf("Program %s\n", drive_gdb_reply_buffer + 1 );
				break;
			}
		
			match_gdb_output(drive_gdb_reply_buffer, "*stopped,reason=\"end-stepping-range\"", IS_ERROR, "When trying to advance of one step, ");
		}
		/*else
			printf("====Annulled instruction, GDB inhibited to wait for GLISS2\n");*/
				
		/* GLISS step */
		if (display_values) printf("Before instr %10d, PC(nPC) gdb %08X(%08X) gliss %08X(%08X)\n", instr_count, gdb_pc, gdb_npc, gliss_pc, gliss_npc);
		if ( !stall_gdb && (gliss_pc != gdb_pc))
		{
			fprintf(stderr, "Gliss and GDB PCs differ : gdb 0x%08x gliss 0x%08x\n", gdb_pc, gliss_pc);
			fprintf(stdout, "Assembly follows\n");
			sprintf(drive_gdb_cmd_buffer, "-data-disassemble -s 0x%08X -e 0x%08X -- 0\n", gdb_pc, gdb_pc+4);
			send_gdb_cmd(drive_gdb_cmd_buffer, drive_gdb_reply_buffer, 0);
			disasm_error_report(drive_gdb_reply_buffer, NULL, NULL, 1, 1);
			exit(1);
		}

		curinstr = PROC(_decode)(iss->decoder, real_state->PC);
		PROC(_step)(iss);
		
		read_vars_this_instruction(drive_gdb_reply_buffer);
		if (! stall_gdb)
			compare_regs_this_instruction(drive_gdb_reply_buffer, real_state, curinstr, instr_count);
			
                PROC(_free_inst)(curinstr);
	}
		
		
		
	/*struct rusage statistiques;
	getrusage(RUSAGE_SELF, & statistiques);
	printf("User time : %ld s %ld us\n", statistiques.ru_utime.tv_sec, statistiques.ru_utime.tv_usec);
	printf("Sys time : %ld s %ld us\n", statistiques.ru_stime.tv_sec, statistiques.ru_stime.tv_usec);*/
	send_gdb_cmd("-gdb-exit\n", drive_gdb_reply_buffer, display_replies);
	fd_set tmp_set;
	FD_ZERO(&tmp_set);
	FD_SET(from_gdb_pipe[0], &tmp_set);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	while ( select(from_gdb_pipe[0] + 3, &tmp_set, NULL, NULL, &tv) )
		{
		FD_SET(from_gdb_pipe[0], &tmp_set);
		memset(drive_gdb_reply_buffer, 0, 1499);
		if( fgets(drive_gdb_reply_buffer, 10, from_gdb) == NULL ) break;
		printf("%s", drive_gdb_reply_buffer);
		}

	close_log_file();
	fclose(from_gdb);
	close(to_gdb_pipe[1]);
	printf("%d instructions\n", instr_count);
	return 0;
	}

void drive_gdb()
	{
	char *gdb_argv[] = {GNU_TARGET"gdb", "--interpreter", "mi",  "--quiet", gpname, NULL};
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
	close(to_gdb_pipe[1]);
	
	/* no logfile needed */
	close_log_file();
	
	/* redirecting stdin */
	if ( dup2(to_gdb_pipe[0], STDIN_FILENO) == -1 )
		{
		fprintf(stderr, "ERROR: Couldn't dup2() GDB pipe to stdin\n");
		exit(-1);
		}
	close(to_gdb_pipe[0]);

	
	close(from_gdb_pipe[0]);
	/* redirecting stdout */
	if ( dup2(from_gdb_pipe[1], STDOUT_FILENO) == -1 )
		{
		fprintf(stderr, "ERROR: Couldn't dup2() GDB pipe to stdout\n");
		exit(-1);
		}
	close(from_gdb_pipe[1]);


	/* now we launch GDB with M/I interpreter */
	printf("execvp(%s)", GDB_NAME);
	if ( execvp(GDB_NAME, gdb_argv) )
		{
		perror("GDB execvp()");
		exit(-1);
		}
	}


#undef MAIN_C
	
