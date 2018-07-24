(* TCL -- Calipso command file.
**
** Project: 		Calipso
** File: 			calipso_bin.ml
** Version:			3.0
** Date:			7.20.99
** Author:			Hugues Cassé
**
*)

exception ParsingError


(* Useful Data *)
let version = "Calipso V3.0 7.20.99 Hugues Cassé"
let help = version ^ "\n" ^ "calipso [-hmPtsVv] [-r[bcfgkrs]] [-s[lrw]] [-p preprocessor] <file list> [-o <output file>]"
exception InternalError

(* Output management *)
let out = ref stdout
let close_me = ref false

let close_output _ =
	flush !out;
	if !close_me then close_out !out else ();
	close_me := false
let set_output filename =
	close_output ();
	(try out := open_out filename
	with (Sys_error msg) ->
		output_string stderr ("Error while opening output: " ^ msg); exit 1);
	close_me := true
	

(* File Management *)
let files = ref []
let verbose_mode = ref false
let add_file filename =
	files := List.append !files [filename]


(*
** Argument definition
*)
let calipso_options = ref []
let preproc = ref ""
let stat_display = ref false
let args : Frontc.parsing_arg list ref = ref []

let standard_remove _ =
	calipso_options := !calipso_options @ [
		Calipso.RemoveBreak;
		Calipso.RemoveContinue;
		Calipso.RemoveReturn;
		Calipso.RemoveSwitch(Reduce.REDUCE);
		Calipso.RemoveGoto
	]
let subtle_remove _ =
	calipso_options := !calipso_options @ [
		Calipso.RemoveBreak;
		Calipso.RemoveContinue;
		Calipso.RemoveReturn;
		Calipso.RemoveSwitch(Reduce.KEEP);
		Calipso.RemoveGoto
	]

let arg_def =
[
	"-V", Arg.Unit (fun _ -> print_endline help), "Informations";
	"-v", Arg.Unit (fun _ -> calipso_options := Calipso.Verbose :: !calipso_options ; verbose_mode := true),
					"Verbose mode";
	"-o", Arg.String (fun filename -> set_output filename), "Output file";
	"-m", Arg.Unit (fun _ -> calipso_options := Calipso.UseMask :: !calipso_options),
					"Use bitfield masks to handle labels.";
	"-p", Arg.String (fun id -> preproc := id), "Preprocessor command.";
	"-s", Arg.Set stat_display, "Display statistics on control flow structures.";
	"-P", Arg.Unit (fun _ -> preproc := "gcc -E %i -o %o"), "Use \"gcc -E %i -o %o\" as preprocessor.";
	"-rg", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveGoto :: !calipso_options),
					"Remove the goto statements.";
	"-rb", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveBreak :: !calipso_options),
					"Remove the break statements.";
	"-rc", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveContinue :: !calipso_options),
					"Remove the continue statements.";
	"-rr", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveReturn :: !calipso_options),
					"Remove the return statements.";
	"-rs", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveSwitch(Reduce.RAW) :: !calipso_options),
					"Reduce rawly the switch statements.";
	"-rk", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveSwitch(Reduce.KEEP) :: !calipso_options),
					"Reduce the switch statements but keep the regular ones.";
	"-rf", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveSwitch(Reduce.REDUCE) :: !calipso_options),
					"Reduce the switch statements but use a faster method for regular ones.";
	"-r", Arg.Unit standard_remove, "Set -rg, -rb, -rc, -rr, -rf options.";
	"-t", Arg.Unit subtle_remove, "Set -rg, -rb, -rc, -rr, -rk options.";
	"-sl", Arg.Unit (fun _ -> calipso_options := Calipso.Strategy(Algo.LEFT) :: !calipso_options),
					"Organize sequences to the left";
	"-sr", Arg.Unit (fun _ -> calipso_options := Calipso.Strategy(Algo.RIGHT) :: !calipso_options),
					"Organize sequences to the right";
	"-sw", Arg.Unit (fun _ -> calipso_options := Calipso.Strategy(Algo.WEIGHTED) :: !calipso_options),
					"Organize sequences using the weight system";
	"-l", Arg.Unit (fun _ -> args := (Frontc.LINE_RECORD true)::!args), "Preserve line numbers";
	"-rrec", Arg.Unit (fun _ -> calipso_options := Calipso.RemoveRecursive :: !calipso_options),
					"Try to transform recursive function into for loops."

]


(*** preprocessing ***)
exception PreprocessingError
let preprocess inname outname =
	let rec replace str =
		try let idx = String.index str '%' in
			(if idx > 0 then String.sub str 0 idx else "")
			^ (match String.get str (idx + 1) with
				'i' -> inname
				| 'o' -> outname
				| '%' -> "%"
				| _ -> "")
			^ (if (idx + 2) >= (String.length str)
				then ""
				else replace
					(String.sub str (idx + 2) ((String.length str) - idx - 2))) 
		with Not_found -> str in
	let com = replace !preproc in
	let _ = if !verbose_mode
		then prerr_string ("Executing \"" ^ com ^ "\"\n")
		else () in
	if (Sys.command com) = 0 
		then ()
		else raise PreprocessingError


(* display_stats _ -> _
**	Display statistics about the number of statements.
*)
let display_stats _ =
	prerr_string ("  switch count = " ^ (string_of_int !Reduce.switch_count) ^ "\n");
	prerr_string ("    raw switch = " ^ (string_of_int !Reduce.raw_switch) ^ "\n");
	prerr_string ("   fast switch = " ^ (string_of_int !Reduce.fast_switch) ^ "\n");
	prerr_string ("   kept switch = " ^ (string_of_int !Reduce.kept_switch) ^ "\n");
	prerr_string ("   degenerated = " ^ (string_of_int !Algo.degenerated_switch) ^ "\n");
	prerr_string ("    case count = " ^ (string_of_int !Reduce.case_count) ^ "\n");
	prerr_string ("   break count = " ^ (string_of_int !Reduce.break_count) ^ "\n");
	prerr_string ("continue count = " ^ (string_of_int !Reduce.continue_count) ^ "\n");
	prerr_string ("  return count = " ^ (string_of_int !Reduce.return_count) ^ "\n");
	prerr_string ("    goto count = " ^ (string_of_int !Reduce.goto_count) ^ "\n");
	prerr_string ("   label count = " ^ (string_of_int !Reduce.label_count) ^ "\n");
	if !Algo.label_total = 0 then
		prerr_string "No label !\n"
	else begin
		prerr_string
			("   max labels per function = "
			^ (string_of_int !Algo.label_max) ^ "\n");
		prerr_string
			("               label total = "
			^ (string_of_int !Algo.label_total) ^ "\n");
		prerr_string
			("function containing labels = "
			^ (string_of_int !Algo.label_func) ^ "\n")
	end
	

(* Starter *)
let process filename =
	let parse file =
		let _ = if !verbose_mode then prerr_string "Parsing...\n" else () in
		(* Frontc.parse_file file stderr in *)
		Frontc.parse ((Frontc.FROM_FILE file)::!args) in
	match (
		if !preproc = ""
		then parse filename
		else
			let tmp = Filename.temp_file "rewrite" ".i" in
			try
				preprocess filename tmp;
				let res = parse tmp in
				Sys.remove tmp;
				res
			with PreprocessingError ->
				begin
					prerr_string
						("Error while preprocessing " ^ filename ^ "\n");
					Frontc.PARSING_ERROR
				end) with
	Frontc.PARSING_ERROR -> ()
	| Frontc.PARSING_OK defs ->
		begin
			Cprint.print
				!out
				(Calipso.process_remove defs (!calipso_options));
			if !stat_display then display_stats ()
		end
	
let rec process_files files =
	match files with
	[] -> ()
	| filename::fol -> (process filename; process_files fol)

let _ =
	Arg.parse arg_def add_file help;
	process_files !files;
	close_output ()
