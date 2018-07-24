(*
 *	$Id: main.ml,v 1.2 2008/07/18 11:36:31 michiel Exp $
 *	Copyright (c) 2003, Hugues Cass� <hugues.casse@laposte.net>
 *
 *	Tool to compute loop bounds
 *)

    (*cSNPRT false pas d'analyse de domaine de pointeur dans util.ml de O_Range*)

open Printf

open Frontc
open Mergec
open Calipso
open Sortrec
open ExtractinfoPtr
open Cextraireboucle
open Cvarabs
open Cabs
open Coutput
open Orange

open Resumeforgraph
open Util

(* jz: xml parsing via xml-light-2.2 *)
open Xml

module TO = Orange.Maker(Orange.PartialAdapter(Cextraireboucle.TreeList))
module XO = Orange.Maker(Orange.PartialAdapter(Orange.MonList))
module EO = Orange.Maker(Flowfacts.Coarse)

(* === Utility functions === *)

(* Get outputs from a list of input/output *)
let rec listeOutputs listES =
if listES = [] then []
else
begin
	let (first, next) = (List.hd listES, List.tl listES) in
	(match first with
			ENTREE (_) ->	 listeOutputs next
			| SORTIE (nom) | ENTREESORTIE (nom) -> nom::listeOutputs next)
end



(* === Options management === *)
let banner =
	"orange V1.0\n" ^
	"Copyright (c) 2009, Marianne de Michiel <michiel@irit.fr>\n\n" ^
	"SYNTAX:\n" ^
	"Automated full analysis:\n" ^
	"\torange [options] --auto [--allow-pessimism] [--no-control-reduction] [--without-gobal-initial] files... entry-point [functions...|--funlist listfile] [-o flowfacts-file] [--outdir /tmp]\n" ^
	"\torange [options] --auto [--allow-pessimism] [--no-control-reduction] [--without-gobal-initial] -- entry-point [functions...|--funlist listfile] [-o flowfacts-file] [--outdir /tmp]\n" ^
	"Full analysis:\n" ^
	"\torange [options] files... entry-point [--no-control-reduction] [--without-gobal-initial] [functions...|--funlist listfile ] [-o flowfacts-file]\n" ^
	"\torange [options] -- entry-point [--no-control-reduction] [--without-gobal-initial] [functions...|--funlist listfile] [-o flowfacts-file]\n" ^
	"Partial analysis:\n" ^
	"\torange [options] -k files... [--no-control-reduction] [functions...|--funlist listfile] [--outdir dir]\n" ^
	"\torange [options] -k -- [--no-control-reduction] [functions...|--funlist listfile] [--outdir dir]\n" ^
	"Call graph generation:\n" ^
	"\torange [options] -g files... [functions...|--funlist listfile] [--outdir dir]\n" ^
	"\torange [options] -g -- [functions...|--funlist listfile] [--outdir dir]\n"^
	"Recurcivity detection with frontc produce an .rec_status file containing for each function its recursivity status:\n"^
	"\torange [options] --frec files... [functions...|--funlist listfile] [--outdir dir]\n" ^
	"\torange [options] --frec -- [functions...|--funlist listfile] [--outdir dir]\n"^
	"Call for debug info:\n" ^
	"\torange [options] --debug\n"^
	"\torange [options]  --resume listOfFunction  : count number of function call from the entry point. The first function of the list is the entry the other are resumed \n"^
    "\torange [options] --multitree listOfFunction make the tree of each function of a list"

(* ^
	Recurcivity application class : (0: no recursivity, 1: only single recursivity (that may be change into loop by calypso using --crec option), 2:others cases\n^
	\torange [options] --gfrec files... [functions...|--funlist listfile] \n ^
	\torange [options] --gfrec -- [functions...|--funlist listfile] \n
*)
let args: Frontc.parsing_arg list ref = ref []

let list_file_and_name: string list ref = ref []
let add_file_and_name filename =
	list_file_and_name := List.append !list_file_and_name [filename]
let from_stdin = ref false
let fun_list_file = ref ""
(* Return a list of function names from filename *)
let get_fun_list filename =
	match filename with
  | "" -> []
  | _ ->
    let content = ref "" in
    let chan = open_in filename in
    try
      while true; do
        content := (!content) ^ "\n" ^ (input_line chan)
      done; []
    with End_of_file ->
      close_in chan;
      Str.split (Str.regexp "[ \t\n]+") !content

(* preprocessing stuff *)
let run_calipso = ref false
let calipso_rrec = ref false
let frontc_frec = ref false

(* jz: there could be input ffx file -- its name*)
(* let in_ffx_file = ref "" *)
(* defined in utils as we need to access them in orange.ml *)
(*let ghost = ref false
let delta = ref false*)

(* output stuff *)
let out_file = ref ""
let out_dir = ref "."
let print_exp = ref false

(* auto stuff *)
let auto = ref false
let allow_pessimism = ref false

(* partial stuff *)
let partial = ref false

(* graph stuff *)
let onlyGraphe = ref false
let completeGraphe = ref false
let existsPartialResult _ = false
let withoutGlobalAndStaticInit = ref false
let multiTree = ref false
let resume = ref false

(* mettre une option pour mode trace trace:= true*)

let opts = [
	(*if !hasCondListFile_name  then condListFile_name *)

	(* Input options *)

  (* jz: there are two options: either, input ffx is treated as the effect of
     'global' assignments, or the input ffx is translated to statements
     that are inserted at the beginning of the main function 
  	 (globals: globales, alreadyAffectedGlobales) *)
	("--iffx", Arg.Set_string in_ffx_file, 
		"Input flow facts from the given file (effect of global assigns).");
	("--iffxg", Arg.Set ghost,
		"Input flow facts from the given file (instrumented 'ghost' instructions). Use with --iffx");
	("--delta", Arg.Unit (fun () -> delta := `Run (`OutDir ".") ),
		"Perform delta analysis. Don't use with --iffxg");
	("--odelta", Arg.String (fun dir -> delta := `Run (`OutDir dir)),
		"Perform delta analysis and put the output in the specified directory. Don't use with --iffxg");
	("--wcee", Arg.Set wcee,
		"Perform worst case execution estimation. Don't use with --iffxg");
	("--", Arg.Set from_stdin,
		"Takes input from standard input.");
	("--funlist", Arg.String (fun file -> fun_list_file := file),
		"File with the list of function names to be processed.");
	("--up", Arg.String (fun name -> Cextraireboucle.add_use_partial name; alreadyEvalFunctionAS := List.map (fun n ->  (n,Cextraireboucle.getAbsStoreFromComp n)  )!use_partial	;),
		"Use partial result (rpo file) for this function.");
	("--condIF",Arg.String (fun dir ->  hasCondListFile_name :=true;	 condListFile_name  := dir),
		"Use annotation for \"if\" from .rpo file (use -k on a function containing IF_x = 0|1).");
	(* Mode options *)
	("--auto", Arg.Set auto,
		"Automated full analysis");
	("--print_exp", Arg.Set print_exp, "print_expression of bounds or conditions");
	("--allow-pessimism", Arg.Set allow_pessimism,
		"Allow to automatically partialize even function that imply pessimism (faster)");
	("-k", Arg.Set partial,
		"Perform partial analysis on the given functions");
	(* Output options *)
	("-o", Arg.Set_string out_file,
		"Output flow facts to the given file.");
	("--outdir", Arg.String (fun dir -> out_dir := dir; Cextraireboucle.set_out_dir dir;),
		"Output directory for partial results (rpo files) or graphs (dot files).");
	("--without-global-initial",  Arg.Set withoutGlobalAndStaticInit  ,
		"Without initial global and static values");
    ("--debug",  Arg.Set Util.vDEBUG  ,
		"Print on stderr debug information");
    ("--resume",  Arg.Set resume  ,
		"count number of function call from the entry point. The first function of the list is the entry the other are resume");
    ("--multitree", Arg.Set multiTree  ,
		"Make the tree of each function of a list");
	(* graph   *)
	("-g", Arg.Unit (fun _ ->args := (Frontc.LINE_RECORD true)::!args;   calipso_rrec := true; run_calipso := true;args := USE_CPP :: !args;onlyGraphe := true),
		"Generate informations to draw call graph for given functions.");
	(* Frontc input options *)
	("--nogcc",  Arg.Unit (fun _ ->args := (Frontc.LINE_RECORD true)::!args;   calipso_rrec := true; run_calipso := true;args := USE_CPP :: !args ;args := (GCC_SUPPORT false) :: !args),
		"Do not use the GCC extensions.");
	(*"--pp", Arg.Unit (fun _ -> args := (Frontc.LINE_RECORD true)::!args; calipso_rrec := true; run_calipso := true;args := USE_CPP :: !args ),
		"Preprocess the input files.");
	("-l", Arg.Unit (fun _ ->args := (Frontc.LINE_RECORD true)::!args; calipso_rrec := true; run_calipso := true;),
		"Generate #line directive.");
	"--crec", Arg.Unit (fun _ -> args := (Frontc.LINE_RECORD true)::!args; calipso_rrec := false; run_calipso := true;args := USE_CPP :: !args ),
		"Remove simple recursions using Calipso.");*)
	("--frec", Arg.Unit (fun _ -> args := (Frontc.LINE_RECORD true)::!args; frontc_frec := true;run_calipso := false; args := USE_CPP :: !args ),
		"Control if it has some recursive function and if the function are all simple recurcivity function (see orange doc)");
	("--no-control-reduction", Arg.Unit (fun _ -> args := (Frontc.LINE_RECORD true)::!args;  run_calipso := false; args := USE_CPP :: !args),
		"No Process input files using Calipso.");
	("--proc", Arg.String (fun cpp -> args := (PREPROC cpp) :: !args),
		"Use the given preprocessor.");
	("-D", Arg.String (fun def -> args := (DEF def) :: !args),
		"Pass this definition to the preprocessor.");
	("-U", Arg.String (fun undef -> args := (UNDEF undef) :: !args),
		"Pass this undefinition to the preprocessor.");
	("-i", Arg.String (fun file -> args := (INCLUDE file) :: !args),
		"Include the given file.");
	("-I", Arg.String (fun dir -> args := (INCLUDE_DIR dir) :: !args),
		"Include retrieval directory.")
]



(* === Partial analysis functions === *)

(* Return if the given component is in the list of to be partialised
	components *)
let isComponent comp =
	let rec aux = function
		| [] -> false
		| (comp_name) :: r ->
			if (comp = !comp_name)
				then (true)
				else (aux r)
		in aux !Cextraireboucle.names

(**
 * Build the partial results for functions which are marked as component.
 * @param fctList The list of functions of the program
 * @return The list of computed partial results (in the same order)
 *)
let rec getComps = function
	| [] ->   () 
	| (_, fn)::r ->
      getComps r;
     (* printf "Evalue le resultat partiel pour: %s\n" fn.nom;*)
     
      if (isComponent fn.nom) then begin
      
        printf "Evalue le resultat partiel pour:%s\n" fn.nom;
			(*alreadyEvalFunctionAS := List.map (fun n ->  (n,Cextraireboucle.getAbsStoreFromComp n)  )!use_partial	;
						List.iter(fun (n,_) ->Printf.printf "%s " n)!alreadyEvalFunctionAS;*)
            
  			TO.docEvalue := TO.new_documentEvalue [] []  ;
  			compEvalue := [];
  			listeAppels := [];
  			TO.varDeBoucleBoucle := "";
  			TO.listeDesMaxParIdBoucle := [];
  			TO.corpsEvalTMP := [] ;
  			TO.nouBoucleEval:= [];
  			TO.appelcourant := [] ;
  			TO.listBeforeFct := [];
  			TO.listeVB := [];
  			TO.listeVBDEP := [];
  			TO.curassocnamesetList := [];
  			TO.listeInstNonexe := [];
  			TO.aslAux := [];
  			TO.listCaseFonction := [];
  			let initListAssosPtrNameType = !listeAssosPtrNameType in
  			let initistAssocIdType = !listAssocIdType in
  			let initlistAssosIdTypeTypeDec = !listAssosIdTypeTypeDec in
  
  			(*TO.listeDesMaxParIdBoucle :=  [];
  			TO.corpsEvalTMP :=  [] ;
  			TO.nouBoucleEval:=  [];
  			TO.docEvalue :=  TO.new_documentEvalue  [] [];
  			TO.appelcourant :=   [] ;
  			TO.listBeforeFct :=  [];
  			TO.listeVB  := [];
  			TO.listeVBDEP := [];
  			TO.curassocnamesetList := [];
  			TO.listeInstNonexe := [];
  			TO.aslAux := [];
  			TO.listCaseFonction := []*)
        TO.isPartialisation := true;
        TO.isPrint_Expression := true;
  			listeASCourant := [];
  			Printf.printf "Longueur de l'arbre: avant %d.\n" (List.length !TO.docEvalue.TO.maListeEval);
  
        (* jz: input ffx treated as the effect of global assignments *)
  			let globales = !alreadyAffectedGlobales in
        globalesVar := !alreadyAffectedGlobales;
  
  			let typeE = TO.TFONCTION(fn.nom, !TO.numAppel, fn.lesAffectations, !listeDesEnum, [], [], [],  [], true, false, "", 0) in
        TO.dernierAppelFct := typeE;
  			TO.predDernierAppelFct := typeE;
  			let aslist = TO.completevaluerFonction (fn.nom) fn []  (EXP(NOTHING))   [typeE]  typeE true (*!listeASCourant*)
  				(evalStore (new_instBEGIN !listeDesEnum) [] [] []) (( CONSTANT(CONST_INT("1")))) (( CONSTANT(CONST_INT("0")))) [] [] in () ;
  
  			let compAS: abstractStore list =
  				filterwithoutInternal (*(evalStore (new_instBEGIN fn.lesAffectations) [] []) (listeOutputs fn.listeES) globales *)
  						aslist (listeOutputs fn.listeES) globales in
  				printf ".. l'abstractStore fait %u entrees, affichage: \n"(List.length(compAS));
  
  			(*afficherListeAS aslist;*)
  			printf "\n";
  			(* find if there is a loop inside abstract stores *)
  			let nb_loop = (List.fold_left
  				(fun nb_loop absstore ->
  					match absstore with
  						| ASSIGN_SIMPLE (s, e) ->
  							(match e with
  								| EXP(ex) ->
  									let startname = (
  										try (String.sub s 0 3)
  										with Invalid_argument(_) -> "")
  									in if (startname = "TWH")
  										then nb_loop + 1
  										else nb_loop
  								|_-> nb_loop
  							)
  						| _ -> nb_loop
  				)
  				0
  				compAS
  			) in let has_loop = (nb_loop > 0)
  			in Resumeforgraph.append_to_dot_size fn.nom (List.length(compAS)) has_loop;
  			(*printf "Nb loop : %d\n" nb_loop;               *)
  			printf ".. affichage des info. de boucles parametriques: %s de nom\n" fn.nom;
  			mainFonc := ref fn.nom;
  			(*(match !TO.docEvalue.TO.maListeEval with
  				| [] -> Printf.printf "Arbre vide\n"
  				| _ -> ()
  			);*)
  			Printf.printf "Longueur de l'arbre: %d.\n" (List.length !TO.docEvalue.TO.maListeEval);
  			let (result, _) = TO.afficherInfoFonctionDuDocUML !TO.docEvalue.TO.maListeEval  in
  			listeAssosPtrNameType := initListAssosPtrNameType ;
  			listAssocIdType := initistAssocIdType;
  			listAssosIdTypeTypeDec:= initlistAssosIdTypeTypeDec;
        let fName = (Filename.concat !out_dir ((fn.nom)^".rpo")) in
        printf "Stockage dans %s\n" fName;
        Printf.printf "Stockage dans %s\n" fName;
  			(* TO.afficherCompo	   result; *)
  			let chan = Unix.out_channel_of_descr (Unix.openfile fName [Unix.O_WRONLY;Unix.O_TRUNC;Unix.O_CREAT] 0o644) in
        let partialResult = {name=fn.nom; absStore=compAS; compES=(fn.listeES); expBornes=result} in
        Marshal.to_channel chan partialResult [];
  			close_out chan
  			(* to_channel{name=fn.nom; absStore=compAS; compES=(fn.listeES)}::reste *)
  			(* print_string ("\nDEBUT\n"^result^"\nFIN\n");  *)
  		end

(* Start a partial analysis on file *)

(* Start a partial analysis on file *)           
let analysePartielle file =
	 

 TO.ptrInterval :=   [];
  TO.integerInterval :=   ["x"];



	printf "Lance analyse_defs ...\n";
	TO.numAppel := 0;
	idBoucle := 0;	idIf := 0;
	idAppel:=0;
	nbImbrications := 0;
	TO.enTETE :=  false;
	TO.estNulEng :=  false;  TO.isPartialisation := true; TO.isPrint_Expression := true; 
	TO.estDansBoucle :=  false;getOnlyBoolAssignment := true;
	     

	analyse_defs file;getOnlyBoolAssignment := false; phaseinit := false;
	   

	printf "analyse_defs OK, maintenant lance evaluation des composants.\n";
	Resumeforgraph.endForPartial "analyse_defs OK, maintenant lance evaluation des composants.\n" ;

	    
          		 

	getComps !doc.laListeDesFonctions;
 
		print_string "OK, fini.\n"
		
	
	
(*let analysePartielle file =

  if (!doc.laListeDesFonctions = []) then 
			Printf.printf "MAIN SECOND liste fonction vide 1\n"
		else  Printf.printf "MAIN SECOND liste fonction NON vide 1\n";



	printf "Lance analyse_defs ...\n";
	
  TO.ptrInterval :=   [];
  TO.integerInterval :=   ["x"];

  (*if need_analyse_defs
  	then  
  	begin Printf.printf "orange step1\n";

		analyse_defs defs2; (*step 1*)
	end;
*)	
	
	
	TO.numAppel := 0;
	idBoucle := 0;	idIf := 0;
	idAppel:=0;
	nbImbrications := 0;
	TO.enTETE :=  false;
	TO.estNulEng :=  false;  TO.isPartialisation := true; TO.isPrint_Expression := true;
	TO.estDansBoucle :=  false;getOnlyBoolAssignment := true;
	    
          			  if (!doc.laListeDesFonctions = []) then 
			Printf.printf "MAIN SECOND liste fonction vide 1\n"
		else  Printf.printf "MAIN SECOND liste fonction NON vide 1\n";


	(*analyse_defs file;*)
	
	getOnlyBoolAssignment := false; phaseinit := false;
	
	
	if (!doc.laListeDesFonctions = []) then 
	Printf.printf "printFile liste fonction vide 1\n"
else  Printf.printf "printFile liste fonction NON vide 1\n";

   let liste = !doc.laListeDesFonctions in
 
	printf "analyse_defs OK, maintenant lance evaluation des composants.\n";
	(*Resumeforgraph.endForPartial "analyse_defs OK, maintenant lance evaluation des composants.\n" ;*)
	getComps liste;
	print_string "OK, fini.\n"*)


(* GetSortRecStatus as an exemple  *)
let getSortRecStatus fp =
			let out = open_out ".rec_status" in
			Sortrec.test out fp fp;
			let outputstatut = (Sortrec.applicationRecursivityClass fp fp) in

			let strToP =
				if outputstatut = 0 then 
          			Printf.sprintf "Application is : not recursive\n"
				else if outputstatut = 1 then 
          			Printf.sprintf "Application containts only simple recursivity\n"
		    		else 
		      			Printf.sprintf "Application containts complex recursivity\n" in
						output_string  out strToP;
					close_out out;

			if outputstatut = 2 then 
			begin
				Printf.eprintf "Application containts complex recursivity : oRange cannot be applied\n" ;
                exit 1;
			end

 	

(* === Main program === *)
let _ =
	(* Set needed output variables *)
	let (output, close) =
		if !out_file = "" then (stdout,false)
		else ((open_out !out_file), true)
  in

	Cextraireboucle.set_out_dir (!out_dir);
  (*	trace:= true;*)

	(* Parse arguments *)
  args := (Frontc.LINE_RECORD true)::!args;
  calipso_rrec := true;
  run_calipso := true;
  args := USE_CPP :: !args ;
	out_dir := "/tmp"; Cextraireboucle.set_out_dir "/tmp";
	Arg.parse opts add_file_and_name banner;
	(* Get input file names and function names *)
	list_file_and_name := !list_file_and_name @ (get_fun_list !fun_list_file);
	Cextraireboucle.sort_list_file_and_name !list_file_and_name;

  (* jz: read input ffx for scenario -- c.f. util *)
  (* TODO / FIXME: catch xml-exception! *)
  if ((!in_ffx_file) <> "") then begin
    let ffx_flowfacts = Xml.parse_file !in_ffx_file in
    print_endline (Xml.tag ffx_flowfacts);
    parseFFXScenario ffx_flowfacts ;
      List.iter (
        fun x ->
          let (xvar, xlval, xuval) = x in (* append to abstract store list*)
            if (xlval = xuval) then (* lower, upper equal -> no range *)
            begin (* scenario: ghost instructions or effect of global asgns *)
              if (!ghost) then begin
                scenarioAsDocInsts := 
                  [BINARY(ASSIGN, VARIABLE(xvar), CONSTANT(CONST_INT(xlval)))] @ !scenarioAsDocInsts;
              end else begin
                scenarioAsGlobals :=
                  [ASSIGN_SIMPLE(xvar, EXP (CONSTANT (CONST_INT(xlval))))] @ !scenarioAsGlobals;
                (* 2nd part of hack to get in scenario: this way we add it to the list of global 
                initializations. TODO :: similarly, we need some way to merge/decide which value 
                to pick in case of multiple initializations (e.g. when a var is initialized globally
                and by scenario *)
                scenarioAsGlobalInsts :=
                  [VAR(xvar, EXP (CONSTANT (CONST_INT(xlval))), [], [])] @ !scenarioAsGlobalInsts 
              end
            end (*else (* not used anymore *) scenarioAsGlobals :=
                [ASSIGN_SIMPLE(xvar, EXP (CALL (VARIABLE "SET", [CONSTANT (CONST_INT(xlval));     
                  (* <---- SET? TODO *) CONSTANT (CONST_INT(xuval))])))] @ !scenarioAsGlobals *)
      ) !rscAS
  end;

	(* Check the number of anonymous parameters *)
	if !hasCondListFile_name then begin
		condAnnotated := getAbsStoreFromComp !condListFile_name ;
		(*afficherListeAS   !condAnnotated; *)
	end;
	if ((not !partial) && (not !onlyGraphe)) then (
    (* full analysis mode *)
		if (List.length !Cextraireboucle.names < 1) then begin
      Arg.usage opts banner;
      prerr_string "Error: give at least one task entry function.\n";
      exit 1
    end)
	else if (!partial) then (	(* partial analysis mode *)
		if (List.length !Cextraireboucle.names < 1) then begin
				Arg.usage opts banner;
				prerr_string "Error: give at least one function to partialize.\n";
				exit 1
			end)
  else if (!onlyGraphe) then ((* call graph mode *)
		if (List.length !Cextraireboucle.names < 1) then begin
				list_file_and_name := "main" :: (!list_file_and_name);
				Cextraireboucle.files := [];
				Cextraireboucle.sort_list_file_and_name !list_file_and_name;
				(*completeGraphe := true*)
  end);

	(*prerr_string "names&files\n";
	List.iter (fun r -> prerr_string (r ^ "\n")) !list_file_and_name;
	prerr_string "names\n";
	List.iter (fun r -> prerr_string (!r ^ "\n")) !Cextraireboucle.names;
	prerr_string "files\n";
	List.iter (fun r -> prerr_string (r ^ "\n")) !Cextraireboucle.files;*)

	(* For full analysis, get the entry point *)
	if ((not !partial) && (not !onlyGraphe))
		then (
			let hd = (!(List.hd (!Cextraireboucle.names)))
			and tl = (List.tl (!Cextraireboucle.names))
			in Cextraireboucle.maj hd tl
		);

	(* Process input using FrontC *)
	let a1 = !args in
	let a2 = (List.filter
		(fun e ->
      match e with 
        LINE_RECORD _-> false 
      | _ -> true)
		a1
	) in
if (!vDEBUG	) then Printf.printf "Begin Merge\n" ;
	(* Merge given files into one with MergeC *)
	let getMergedFile args =
		let cfiles = (List.map
			(fun filename ->
				match (Frontc.parse (FROM_FILE(filename) :: args)) with
					| PARSING_ERROR -> failwith ("Frontc Failed to load " ^ filename);
					| PARSING_OK(defs) -> defs
			) !Cextraireboucle.files
		) in
if (!vDEBUG	) then Printf.printf "End Merge => Calipso\n" ;
		(* Calipso processing *)
		let cfiles = if (!run_calipso) then 
      let calipso_opts =
				[
					Calipso.RemoveGoto;
					Calipso.RemoveBreak;
					Calipso.RemoveContinue;
					Calipso.RemoveReturn;
					Calipso.RemoveSwitch(Reduce.RAW);
				] @ (if (!calipso_rrec && !frontc_frec = false)
					then
						begin 
							
							 [Calipso.RemoveRecursive]
							 
						end
					else []
				) in (List.map
					(fun defs ->  getSortRecStatus defs; (Calipso.process_remove defs calipso_opts))
					cfiles
				)
			else
        cfiles 
      in
      let removedextern = (Mergec.removeDuplicatedExtern cfiles) in
      let chk_cfiles = (Mergec.check "mergec_rename__" removedextern) in
      let merge_file = Mergec.merge chk_cfiles in
        if Sortrec.hasRecursivity merge_file merge_file then 
          failwith "Recursivity within the application. Orange cannot be applied.";
          merge_file 
  in
if (!vDEBUG	) then Printf.printf "End Calipso\n" ;
	(* First parse *)
	let firstParse =
		let merge_file = (getMergedFile a1) in
		Rename.go (Frontc.trans_old_fun_defs merge_file) in
if (!vDEBUG	) then Printf.printf "cons merge file => .merge.cm\n" ;
		(* cons merge file *)
		(*let out = open_out ".merge.cm" in
		Cprint.print out firstParse;
		close_out out;*)
if (!vDEBUG	) then Printf.printf "get rec\n" ;
		(* get recursivity*)
		if (!frontc_frec) then begin
			let out = open_out ".rec_status" in
			Sortrec.test out firstParse firstParse;
			let outputstatut = (Sortrec.applicationRecursivityClass firstParse firstParse) in

			let strToP =
				if outputstatut = 0 then 
          Printf.sprintf "Application is : not recursive\n"
				else if outputstatut = 1 then 
          Printf.sprintf "Application containts only simple recursivity\n"
        else 
          Printf.sprintf "Application containts complex recursivity\n" in
			output_string  out strToP;
			close_out out;
		end else begin
		(* *)
 if (!vDEBUG	) then Printf.printf " rec OK\n" ;		
 if (!vDEBUG	) then  Printf.printf "first path end => go to orange\n" ;
      if ((!partial) || (!auto)) then
         TO.initref stdout firstParse
      else 
         XO.initref stdout firstParse;

	(* Second parse *)
	let secondParse =
		let merge_file = (getMergedFile a2) in
    Rename.go (Frontc.trans_old_fun_defs merge_file) in
if (!vDEBUG	) then Printf.printf "  second path begin\n" ; 


	if !onlyGraphe then	(* Call graph mode *)
		if (!completeGraphe) then 
			Resumeforgraph.resume secondParse true
		else 
		Resumeforgraph.resume secondParse false
	else 
	begin
			(* Analysis mode *)		
			 
				
			if (!auto) (* automated full analysis *) then begin
			  (* apply a partialization strategy *)
					let rec auto_part strategy =
						match strategy with
				  | [(level, _)] ->
					begin
					  (* never partialize the last (main) level... *)
					  printf "Last level (%d) reached. Stop partializing.\n" level;
					end
				  | (level, []) :: t ->
					begin
					  printf "Nothing to partialize at level %d.\n" level;
					  auto_part t;
					end
				  | (level, fun_list) :: t ->
					let names = (List.map (fun (n, size) -> n) fun_list) in
					begin
					  printf "Partializing level %d:\n" level;
					  printf "\t%s\n" (List.fold_left (fun p n -> p ^ " " ^ n) "" names);
					  Cextraireboucle.names := [];
					  List.iter (fun n ->
						(* init the environment *)
						Cextraireboucle.sort_list_file_and_name [n];
					  ) names;
					  TO.numAppel := 0;
								idBoucle := 0;
					  idIf := 0;
								idAppel:= 0;
					  nbImbrications := 0;
								TO.enTETE := false;
								TO.estNulEng := false;
								TO.estDansBoucle := false;
								(* start the partialization *)
			
						 

								getComps !doc.laListeDesFonctions;
								(* update environment for the next level *)
					  List.iter (fun n -> Cextraireboucle.add_use_partial n) names;
					  alreadyEvalFunctionAS :=
						List.map (
						  fun n -> (n, Cextraireboucle.getAbsStoreFromComp n)
						) !use_partial;
		  
					  auto_part t;
					end
				  | [] ->
					printf "Partializing level fin:\n"; ()
			  in

			  (* get the strategy *)
				let _ = Cextraireboucle.names := [] in
				let _ = Cextraireboucle.sort_list_file_and_name [!(!Cextraireboucle.mainFonc)] in
				let strategy = 
				List.hd (
				  if !allow_pessimism then
					(Resumeforgraph.get_all_big_strategy secondParse)
				  else
					(Resumeforgraph.get_only_without_pessimism_strategy secondParse)
				)
			  in
			  (*let strategy = [(0, [("WriteMemory", 42); ("DelayAwhile", 42)]); (1, ["", 42])] in*)
			  (* partialize each level *)
			  (*TO.initref stdout firstParse;*)
			  (*analyse_defs secondParse;*)
				auto_part strategy;
		  
				(* do a full analysis using all partialized functions *)
				printf "Compute the final analysis\n";
				Cextraireboucle.names := [];
				list_file_and_name := !list_file_and_name @ (get_fun_list !fun_list_file);
				Cextraireboucle.sort_list_file_and_name !list_file_and_name;
				let hd = (!(List.hd (!Cextraireboucle.names)))
				and tl = (List.tl (!Cextraireboucle.names))
				in Cextraireboucle.maj hd tl;
			  (*XO.initref stdout firstParse;*)
				if !print_exp then
					XO.isPrint_Expression := true
				else
					XO.isPrint_Expression := false;
				
							  if (!doc.laListeDesFonctions = []) then 
					Printf.printf "MAIN liste fonction vide 3\n"
				else  Printf.printf "MAIN liste fonction NON vide 3\n";
				
				
				XO.notwithGlobalAndStaticInit := !withoutGlobalAndStaticInit;
			 
				XO.docEvalue :=  XO.new_documentEvalue [] []  ;
				 
			  compEvalue := [];
				listeAppels :=  [];
				
				
			  let myMode = if !resume then "resume" else if !multiTree then "multitree" else "nothing" in
			  let result = XO.printFile stdout secondParse false myMode in
				(if !out_file = "" then 
				List.iter (fun s-> print_string (s ^"\n") ) result
				else
					let out = open_out !out_file in
					List.iter (fun s-> output_string out (s ^"\n")) result;
					close_out out;
				);
    end 
    else if (!partial) (* partial analysis *) then begin 
         

      analysePartielle secondParse
    end else if !wcee
      then (* worst case event count analysis *) begin
	Printf.eprintf "PERFORMING the worst case event count analysis\n";
	let result = match EO.printFile stdout secondParse true "nothing" with
	  | [x] -> x
	  | _ -> failwith "unexpected number of analysis results" in
	Flowfacts.Coarse.dump stderr result;
	let ff = Flowfacts.LoopInfo.(to_ff_input (make secondParse result)) in
	let entry = match !names with
	  | [x] -> !x
	  | l -> failwith (Printf.sprintf "unexpected number of entry points (%d)"
			     (List.length l)) in
	Printf.eprintf "Running the analysis with entry point \"%s\".\n" entry;
	let wcee = Wcee.analysis ff secondParse entry in
	Printf.eprintf "Worst case event count analysis DONE\n";
	()
      end
      else (* full analysis *) begin
      if !print_exp then
        XO.isPrint_Expression := true
      else
        XO.isPrint_Expression := false;
      XO.notwithGlobalAndStaticInit := !withoutGlobalAndStaticInit;
      (*Resumeforgraph.get_intervals secondParse;*)
      (*let result = XO.printFile stdout secondParse (*true si pas Resumeforgraph.get_intervals secondParse;*) (*false*) true  in*)
      
      			   
      
        let myMode = if !resume then "resume" else if !multiTree then "multitree" else "nothing" in
  		let result =
  		if !cSNPRT then
  		  XO.printFile stdout secondParse 
        (*true si pas Resumeforgraph.get_intervals secondParse;*) true myMode
      else
        (Resumeforgraph.get_intervals secondParse;
  		  XO.printFile stdout secondParse (*true si pas Resumeforgraph.get_intervals secondParse;*) false myMode)
      in
      if !out_file = "" then
         List.iter (fun s-> print_string (s ^"\n")) result
      else
        let out = open_out !out_file in
        List.iter (fun s-> output_string out (s ^"\n")) result;
  			close_out out
  		end
    end
  end;
  (* Close the output if needed *)
  if close then close_out output
  
