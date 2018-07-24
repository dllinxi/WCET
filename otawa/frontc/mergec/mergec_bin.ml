(* 
 *	$Id$
 *	Copyright (c) 2009, Florian Birée <florian@biree.name>
 *	Copyright (c) 2007, Hugues Cassé <hugues.casse@laposte.net>
 *
 *	Merge many C files into a single on.
 *	Based on printc_bin.ml v 1.2 by Hugues Cassé
 *)

open Frontc
open Cabs

(* Options *)
let banner =
	"mergec V0.2 (june 2009)\n" ^
	"Copyright (c) 2009, Florian Birée <florian@biree.name>" ^
	"Copyright (c) 2007, Hugues Cassé <hugues.casse@laposte.net>\n\n" ^
	"SYNTAX:\tmergec [options] files...\n" ^
	"\tmergec [options] --\n"
let args: parsing_arg list ref = ref []
let files: string list ref = ref []
let out_file = ref ""
let from_stdin = ref false
let prefix = ref "mergec_rename_"

(* Options scanning *)
let opts = [
	("-o", Arg.Set_string out_file,
		"Output to the given file.");
	("-pp", Arg.Unit (fun _ -> args := USE_CPP :: !args),
		"Preprocess the input files.");
	("-nogcc", Arg.Unit (fun _ -> args := (GCC_SUPPORT false) :: !args),
		"Do not use the GCC extensions.");
	("-proc", Arg.String (fun cpp -> args := (PREPROC cpp) :: !args),
		"Use the given preprocessor");
	("-i", Arg.String (fun file -> args := (INCLUDE file) :: !args),
		"Include the given file.");
	("-I", Arg.String (fun dir -> args := (INCLUDE_DIR dir) :: !args),
		"Include retrieval directory");
	("-D", Arg.String (fun def -> args := (DEF def) :: !args),
		"Pass this definition to the preprocessor.");
	("-U", Arg.String (fun undef -> args := (UNDEF undef) :: !args),
		"Pass this undefinition to the preprocessor.");
	("-l", Arg.Unit (fun _ -> args := (LINE_RECORD true)::!args),
		"Generate #line directive.");
	("--", Arg.Set from_stdin,
		"Takes input from standard input.");
	("-prefix", Arg.String (fun pref -> prefix := pref),
		"Renaming prefix for conflicting definitions.");
]

(* Main Program *)
let _ =
	
	(* Parse arguments *)
	Arg.parse opts (fun file -> files := file :: !files) banner;
	
	(* Get the output *)
	let (output, close) =
		if !out_file = "" then (stdout,false)
		else ((open_out !out_file), true) in
	
	
	(* Parse the input *)
	let parse opts =
		match Frontc.parse opts with
		  PARSING_ERROR ->  []
		| PARSING_OK file -> file in
	
	(* Merge multiple inputs *)
	let process = fun args files ->
		(Mergec.merge
			(Mergec.check
				!prefix 
				(List.rev 
					(List.map 
						(fun file -> parse ((FROM_FILE file)::args))
						files
					)
				)
			) 
		) in
	
	(* Process the inputs *)
	let _ =
		if !from_stdin || !files = []
		then Cprint.print output (parse !args)
		else Cprint.print output (process !args !files)  in
	
	(* Close the output if needed *)
	if close then close_out output
