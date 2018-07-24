(* orange -- eval loop bound and eval flowfact. Corresponds to step 2.
Functions are duplicated because of the treatment of partialisation.

**
** Project: O_Range
** File:  orange.ml
** Version: 1.1
** Date:  11.7.2008
** Author:  Marianne de Michiel
*)

open Cabs
open Cxml
open Cprint
open Cexptostr
open Coutput
open Cvarabs
open Cvariables
open Rename
open Printf
open ExtractinfoPtr
open Util

(*open Cevalexpression*)
open Cextraireboucle
open TreeList
open Constante


let exp_VERBOSE = ref false
let print_EnTETE = ref true
let print_FIN = ref true
module type LISTENER =
  sig
    type t
    val null: t
    val onBegin: t -> t
    val onEnd: t -> t
    (**
     * @param result Original result
     * @param isInloop Is the function in a loop?
     * @param isExe Is the function executed?
     * @param isExtern Is the function extern?
     * @return Updated result
     *)
    val onFunction: t -> string -> bool -> bool -> bool -> t
    (**
     * @param result Original result
     * @param fname The function name
     * @param numCall the call number ID
     * @param line The line number of the call
     * @param source The source filename of the call
     * @param isInloop Is the function in a loop?
     * @param isExe Is the function executed?
     * @param isExtern Is the function extern?
     * @return Updated result
     *)
    val onCall: t -> string -> int -> int -> string -> bool -> bool -> bool -> Cabs.expression -> Cabs.expression-> t
    val onReturn: t -> t
    (**
     * @param result Original result
     * @param loopid The Loop-ID
     * @param line Line number of the loop
     * @param source The source filename
     * @param exact Is exact?
     * @param maxcount Max count
     * @param totalcount Total count
     * @param maxexp Max symbolic expression
     * @param totalexp Total symbolic expression
     *)
    val onLoop: t -> int -> int -> string -> bool ->  expressionEvaluee-> expressionEvaluee -> Cabs.expression -> Cabs.expression -> Cabs.expression -> sens -> Cabs.expression -> Cabs.expression-> t

    val onLoopEnd: t -> t

    val onIfT: t -> bool-> oAdress -> t
    val onIfF: t  -> bool-> oAdress->t
    val onIf: t -> string  -> int -> string -> bool -> int -> Cabs.expression -> Cabs.expression -> Cabs.expression -> Cabs.expression -> Cvarabs.typeListeAS -> t

    val onFunctionEnd: t -> t
    val onIfEnd: t -> t
    val onIfTEnd: t -> t
    val onIfFEnd: t -> t

    val concat: t -> t -> t
  end;;


let ifCaseDecode  executed = 
match executed with
      1 -> (true, false)
    | 2 -> (false, true)
    | 3 -> (true, true)
    | _ -> (false, false)


module MonList = struct
  type t = string
  let tab = ref 0
  let null = ""
  let tabsize = 4
  let tabstr = " "
  let nbLigne = ref 0
  let predListener = ref ""

  let concat (x:t) (y:t) = x^y
  let left () = tab := !tab - tabsize
  let right () = tab := !tab + tabsize
  let indent (res:t) :t =
      let rec indent_aux (r:t) (n:int) :t = if (n > 0) then ((indent_aux (r^tabstr) (n-1))) else r in
      indent_aux res (!tab)

  let onBegin res =
    predListener:="";
    let text = if !print_EnTETE then "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<flowfacts>\n" else "" in
    nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in

  let newRes = (indent resaux)^text in
  right ();
      newRes

  let onEnd res =
    let text = if  !print_FIN then "</flowfacts>\n" else "\n" in
  left ();
    let newRes =(indent res)^text in
  concat !predListener newRes

  let onFunction res name inloop executed extern =
    let text = sprintf "<function name=\"%s\" executed=\"%b\" extern=\"%b\">\n" name executed extern in
    nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in
    let newRes =(indent resaux)^text in
  right ();
  newRes

  let onFunctionEnd res =
    let text = "</function>\n" in
  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in
  left ();
    let newRes = (indent resaux)^text in
  newRes


let infoCondStr = function
  | None ->""
  | Some (line, file) ->   sprintf "line=\"%u\" source=\"%s\"" line file



let onIf res name line source inloop executed condVa cond lt lf asL =
  let condexpStr = (*string_from_expr*) pcdata_from_expr (condVa) in
  let condexpStrInit = (*string_from_expr*) pcdata_from_expr  (cond) in


(* Printf.printf "executed= %d  %s>\n" executed name ;*)
  let (isexet, isexee) = ifCaseDecode  executed   in
(*Printf.printf "executed then = %b  else =%b\n" isexet isexee ;*)

  (* balance: add the if statement *)
  let newHead = (name, [isexet; isexee]) in  
  let newHeadRef = ref newHead in
  ifInfo := !ifInfo @ [newHeadRef];

  (* store parent - child relations between if statements:
     if the ifstack is empty, we're at the top lvl, otherwise
     we add top-cur relation to parent data structure *)
  if Stack.is_empty ifstack then begin
    (* top level if *)
    Hashtbl.add parents name []
  end
  else begin
    let (top_of_stack, tos_lsit) = !(Stack.top ifstack) in
    let children = try Hashtbl.find parents top_of_stack
      with Not_found -> Hashtbl.add parents name []; []
    in
    Hashtbl.remove parents top_of_stack;
    let new_children = [ name ] @ children in
    Hashtbl.add parents top_of_stack new_children
  end
  ;
    
  (* we keep the implementation in onIf and onIfEnd and just push/pop 
     BUT: when we insert the execution info for the branches, we add them
     via the top ref on the stack. this way we should insert @ the right 
     places *)
  Stack.push newHeadRef ifstack;

  (*let t1 = sprintf "<conditional>\n" in
  right ();
  nbLigne := !nbLigne +1;
  let resaux1 =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in
  let t2 =(indent resaux1)^t1 in*)

  (* jz: add asL information about variable values to FFX elements *)
  let variableRanges = ref "" in
  List.iter (
    fun astore ->
      (* let applied = applyStoreVA (EXP(condVa)) asL in *)
      (match astore with
        ASSIGN_SIMPLE(varname, EXP(CONSTANT(CONST_INT(varval)))) ->
          variableRanges := !variableRanges ^ "\n<data name=\"" ^ varname ^
                            "\"><const value=\"" ^ varval ^ "\" /></data>";
      | ASSIGN_SIMPLE(varname, EXP(mexpr)) -> (* EVALUATE THE THINGS (sdeng/uding) *)
          (* jz: an initial treatment of non-const assignments, depending very
                 much on simpleCalculerConstVal, which is basically a copy of
                 Cvarabs.calculer. We might be able to extend this to intervals.
                 simpleCalculerConstVal removed -- using calculer *)
          (*let c1 = simpleCalculerConstVal (EXP(mexpr)) (!infoaffichNull) [] 1 in*)
          let c1 =calculer (EXP(mexpr)) (!infoaffichNull) [] 1 in
          (match c1 with
            ConstInt (infintval) -> variableRanges := !variableRanges ^ "\n<data name=\"" ^ varname ^
                            "\"><const value=\"" ^ infintval ^ "\" /></data>"
          | _ -> variableRanges := !variableRanges )
      | _ -> ())
  ) asL;

  (* jz: add asL backward-cond-slice characterization -- this doesn't work too well *)
  let reaching = ref "" in
  List.iter (
    fun reachconds ->
      (match reachconds with
        ASSIGN_SIMPLE(varname, exp) ->
          if (String.length varname >= 3) &&
              (String.compare name varname) != 0 &&
              (String.compare (String.sub varname 0 3) "IF-") == 0 then
  (* TODO :: there's no guarantee that a var isnt' named IF-SOMETHING !!
             if there is, then we will add it to the list of reaching conds,
             this might be not so nice ... so check if we have digits only after '-' *)
                reaching := varname ^ " " ^ !reaching
       | _ -> ())
  ) asL;

  (* jz: added variable ranges and backward-reachable-conds *)
  
  let text =
		(* jz: added id=%s(==name) as attribute in conditional + name param in both cases *)
		if (!exp_VERBOSE = true) then
		  sprintf "<conditional id=\"%s\"> <condition varcond=\"%s\" line=\"%u\" source=\"%s\" isexecuted=\"%b\" expcond=\"%s\" expcondinit=\"%s\"> %s</condition>\n"
				  name name line source (executed != 4) condexpStr condexpStrInit ""
				  (* jz: DISABLED !variableRanges *)
		  (*sprintf "<conditional id=\"%s\"> <condition varcond=\"%s\" line=\"%u\" source=\"%s\" isexecuted=\"%b\" expcond=\"%s\" expcondinit=\"%s\"> %s<reaching-conds>%s</reaching-conds></condition>\n"
			  name name line source (executed != 4) condexpStr condexpStrInit !variableRanges !reaching
		  *)
		else
		  sprintf "<conditional id=\"%s\"> <condition line=\"%u\" source=\"%s\" executed=\"%b\"> %s</condition>\n"
				  name line source (executed != 4) "" (* DISABLED !variableRanges *)
 
  in

   nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res; "")
    else
      res
  in
  let newRes = (indent resaux) ^ text in
  right ();
  newRes


let onIfEnd res =
  (* gathering info for balance *)
  ignore (Stack.pop ifstack);

  let text =   "</conditional>\n"  in
   nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res;"")
    else
      res
  in
  left ();
  let newRes = (indent resaux)^text in
  newRes

let onIfF res isexe infoFalse =
  let text = sprintf "<case cond=\"0\" executed=\"%b\" %s>\n" isexe (infoCondStr infoFalse) in

  (* jz: balance 
     additionally, we store parent-child relationships between ifs, i.e.
     for each if stmt we store it's parent if statement. this is necessary to
     compute ''normalized'' deltas, where we remove delta contributions of 
     if children. *)
  (* ASSERT ifInfoHead != [] ... or something like that *)
  let curIfRef = Stack.top ifstack in
  (match !curIfRef with
    (id, [texec; fexec]) -> 
      curIfRef := (id, [texec; isexe]);  
  | _ -> ());

  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res;"")
    else
      res
  in
  let newRes = (indent resaux)^text in
  right ();
  newRes

let onIfFEnd res =
  let text = "</case>\n"  in
  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res;"")
    else
      res
  in
  left ();
  let newRes = (indent resaux)^text in
  newRes

let onIfT res isexe infoThen =

  let text = sprintf "<case cond=\"1\" executed=\"%b\" %s>\n" isexe (infoCondStr infoThen) in
  (* balance *)
  (* ASSERT ifInfoHead != [] ... or something *)
  let curIfRef = Stack.top ifstack in
  (match !curIfRef with 
    (id, [texec; fexec]) -> 
      curIfRef := (id, [isexe; fexec]); 
  | _ -> ());

  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res; "")
    else
      res
    in
    let newRes =(indent resaux)^text in
    right ();
    newRes

let onIfTEnd res =
  let text = "</case>\n" in
  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res; "")
    else
      res
  in
  left ();
  let newRes = (indent resaux)^text  in
  newRes

let onCall res name numCall line source inloop executed extern lt lf =
  (* jz :: find in hashtable the stuff and add it *)
  let cll = try Hashtbl.find varsPointingToFcts (source, line) 
    with Not_found -> ("", "") in  
  let (callvia, called) = cll in
  (* Printf.printf "call via ptr: %s %s" callvia called *)
  let fctname = if (name = "") then called else name in 

  let text = sprintf "<call name=\"%s\" numcall=\"%u\" line=\"%u\" source=\"%s\" executed=\"%b\" extern=\"%b\">\n" fctname numCall line source executed extern  in
  let text2 = sprintf "<function name=\"%s\">\n" fctname in
  nbLigne := !nbLigne +1;
  let resaux1 =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener res;"")
    else
      res
  in
  let newRes1 =(indent resaux1)^text in
  right ();
  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne >= 50 then
      (nbLigne := 0; predListener := concat !predListener newRes1; "")
    else
      newRes1
  in
  let newRes =(indent resaux)^text2 in
  right ();
  newRes

let onReturn res =
  let text2 = sprintf "</function>\n"  in
  nbLigne := !nbLigne +1;
  let resaux1 =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in
    left ();
    let newRes1 =(indent resaux1)^text2 in


    let text = "</call>\n" in
  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener newRes1 ;"") else newRes1  in
  left ();
    let newRes =(indent resaux)^text in
  newRes


let onLoop res loopID line source exact maxcount totalcount maxexp totalexp expinit sens lt lf =
    let extractExp = function
    (ConstInt(valeur)) ->  valeur
    |(ConstFloat(valeur)) -> valeur
    |(RConstFloat(valeur)) -> Printf.sprintf "%g" valeur
    | _ -> "NOCOMP"
    in

  (* jz: balance.ml -- store loop id and bounds *)
  loopInfo := !loopInfo @ [(loopID, string_to_int (extractExp maxcount), string_to_int (extractExp totalcount))];

  let maxexpStr = (*string_from_expr*)pcdata_from_expr  (remplacerNOTHINGPar maxexp) in
  let totalexpStr = (*string_from_expr*)pcdata_from_expr  (remplacerNOTHINGPar totalexp) in
  let initexpStr = (*string_from_expr*)pcdata_from_expr  (remplacerNOTHINGPar expinit) in

  let max = (extractExp maxcount)  in
  let esc s = Cxml.escape_attr s '"' in
  let text =

      if ( !exp_VERBOSE =  true) then
      begin
        if max = "NOCOMP" && expinit != NOTHING then
            sprintf "<loop loopId=\"%u\" line=\"%u\" source=\"%s\" exact=\"%b\" maxcount=\"%s\" totalcount=\"%s\" maxexpr=\"%s\" totalexpr=\"%s\" expinit=\"%s\">\n"
                loopID line source exact (extractExp maxcount) (extractExp totalcount) (esc maxexpStr) (esc totalexpStr) (esc initexpStr)
        else
          sprintf "<loop loopId=\"%u\" line=\"%u\" source=\"%s\" exact=\"%b\" maxcount=\"%s\" totalcount=\"%s\" maxexpr=\"%s\" totalexpr=\"%s\">\n"
                loopID line source exact (extractExp maxcount) (extractExp totalcount) (esc maxexpStr) (esc totalexpStr)
      end
      else
      begin
        if max = "NOCOMP" && expinit != NOTHING then
          sprintf
          "<loop loopId=\"%u\" line=\"%u\" source=\"%s\" exact=\"%b\" maxcount=\"%s\" totalcount=\"%s\">\n"
          loopID line source exact (extractExp maxcount) (extractExp totalcount)
        else
          sprintf "<loop loopId=\"%u\" line=\"%u\" source=\"%s\" exact=\"%b\" maxcount=\"%s\" totalcount=\"%s\">\n"
                  loopID line source exact (extractExp maxcount)  (extractExp totalcount)

      end
      in
  nbLigne := !nbLigne +1;
  let resaux =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in
    let newRes =(indent resaux)^text in
  right ();
  newRes

  let onLoopEnd res =
    let text = "</loop>\n" in
  let resaux =
    if !nbLigne>=50 then (nbLigne := 0; predListener := concat !predListener res;"") else res in
  nbLigne := !nbLigne +1;
    left ();
    let newRes =(indent resaux)^text in
  newRes
end ;;

type typeCompEvalue =  int * string * TreeList.tree
let compEvalue = ref ([]: typeCompEvalue list)

let listeAppels = ref []
let listNotEQ = ref []

let existeCompParCall num nom=
  let rec aux = function
     [] -> false
    | (t,n,_)::r -> if (t == num) && n = nom then (true) else (aux r)
    in
  aux !compEvalue

let rec getfirtFreeCompParCall num nom= num
(*if existeCompParCall num nom then getfirtFreeCompParCall (num + 1) nom
else num*)


let rechercheCompParCall num  nom=
  let rec aux = function
    [] -> failwith "not found component"
    | (t,n,compInfo)::r -> if (t == num) && n = nom then (compInfo) else (aux r)
    in
  aux !compEvalue

let firstItem = ref true

let compofilter num nom=
firstItem := true;
compEvalue := List.filter (fun (t,n,_) -> if t == num && n = nom && !firstItem then (firstItem:=false; false) else true) !compEvalue


module PartialAdapter =
  functor (Listener : LISTENER) ->
  struct
    type t = Listener.t

    let concat = Listener.concat
    let null = Listener.null
    let onBegin = Listener.onBegin
    let onEnd = Listener.onEnd
    let onFunction = Listener.onFunction
    let onFunctionEnd = Listener.onFunctionEnd
    let onIfF = Listener.onIfF
    let onIfT = Listener.onIfT
    let onIf = Listener.onIf

    let onComponent res comp_tree name numCall line source inloop executed =

      let rec aux res = function
        Doc subtree -> List.fold_left aux res subtree
    (*| Function (x, subtree) ->  List.fold_left aux res subtree*)
  | Function ((name, inloop, executed, extern ), subtree) ->
(*Printf.printf "Try to eval call: %s\n" name;*)
    (*let res = Listener.onFunction res name inloop executed extern  in*)
    let res = List.fold_left aux res  (List.rev subtree) in
    res
    (*Listener.onReturn res*)
    (*| Call (x, subtree) -> List.fold_left aux res subtree*)
  | Call ((name, numCall, line, source, inloop, executed, extern ,lt, lf ) , subtree) ->
(*Printf.printf "Try to eval call: %s %s %d\n" name source line;*)
    let res = Listener.onCall res name numCall line source inloop executed extern lt lf   in
    let res = List.fold_left aux res (List.rev subtree) in
    Listener.onReturn res
  | If ((name, line, source, inloop, executed, condVa, cond, lt, lf, asL), subtree) ->
(*Printf.printf "Try to eval IF: %s %s %d\n" name source line;*)
    let res = Listener.onIf res name line source inloop executed condVa cond lt lf asL in
    let res = List.fold_left aux res (List.rev subtree) in
    Listener.onIfEnd res
  | IfF ((isexe, infoThen), subtree) ->
    let res = Listener.onIfF res isexe infoThen in
    let res = List.fold_left aux res (List.rev subtree) in
    Listener.onIfFEnd res
  | IfT ((isexe, infoElse) , subtree) ->
    let res = Listener.onIfT res isexe infoElse in
    let res = List.fold_left aux res (List.rev subtree) in
    Listener.onIfTEnd res
  | Loop ((id, line, source, exact, max, total, expMax, expTotal, expInit, sens,lt, lf ), subtree) ->
(*Printf.printf "Try to eval loop call: %s %d\n" source line;*)

    let max_final = if (estDefExp max) then max else
    begin
      let valmax = (calculer_avec_sens (EXP expMax) sens) in
      if estDefExp valmax && getDefValue valmax <= 0.0 then ConstInt("0") else valmax
    end in
    let total_final =  if (estDefExp total) then total else
    begin
      let valmax =   (calculer_avec_sens (EXP expTotal) sens) in
      if estDefExp valmax && getDefValue valmax <= 0.0 then ConstInt("0") else valmax
    end in
   (* print_string (string_from_expr expTotal);*)

    let res = Listener.onLoop res id line source exact max_final total_final expMax expTotal expInit sens lt lf  in
    let res = List.fold_left aux res (List.rev subtree) in
    let res = Listener.onLoopEnd res in
    res
      in aux res comp_tree


    let onCall res name numCall line source inloop executed extern lt lf =

    (*  Printf.printf "Try to eval call: %s\n" name;  *)

  let (res:t) = Listener.onCall res name numCall line source inloop executed extern lt lf  in
  if (extern && (existeCompParCall numCall name)) then
  begin
      let result = onComponent res (rechercheCompParCall numCall name) name numCall line source inloop executed in
    compofilter numCall name;
    result
  end else res

    let onReturn = Listener.onReturn
    let onLoop = Listener.onLoop
    let onLoopEnd = Listener.onLoopEnd
  let onIfEnd = Listener.onIfEnd
  let onIfFEnd = Listener.onIfFEnd
  let onIfTEnd = Listener.onIfTEnd

  end;;

module Maker =
  functor (Listener: LISTENER) ->
  struct

let cptFunctiontestIntoLoop = ref 0
let version = "orange Marianne de Michiel"
let enTETE = ref false
let numAppel = ref 0
let estNulEng = ref false
let notwithGlobalAndStaticInit =ref true
let estDansBoucle = ref false
let varDeBoucleBoucle = ref""
let isPartialisation = ref false
let isPrint_Expression = ref false

(*type boucleEval idem fonction eval*)
type evaluationType =
  TBOUCLE   of     int* int * listeDesInst  *  typeListeAS (*num loop, num call+body+contexte *)* bool* string list * string list *string*int(*  fic*line *)
| TFONCTION of  string * int * listeDesInst * listeDesInst *  typeListeAS * listeDesInst(*nom +id call+body+affectappel*) * string list * string list *bool (*is exe*)*bool (*idintoloop*) *string*int(*  fic*line *)

type resMaxType =
  EVALEXP   of    expressionEvaluee
| EXPMAX of   expression list


(* string list * string list is the true conditional list and the false one on with the call or loop 
   execution depend on *)

(*type resEval of int *int **)

let afficheEvaluationType liste =
List.iter (fun e ->
      match e with
        TBOUCLE(num, numa, _, _, _, _, _, _, _) -> Printf.printf "Loop %d into call  %d\n" num numa
      | TFONCTION(num, numa, _, _, _, _, _, _, _, _, _, _) -> Printf.printf "Function %s into call  %d\n" num numa
)liste

let listeDesMaxParIdBoucle = ref []
let existeAssosBoucleIdMax id = List.mem_assoc id  !listeDesMaxParIdBoucle
let getAssosBoucleIdMax id = List.assoc id !listeDesMaxParIdBoucle
let resetAssosIdMax = listeDesMaxParIdBoucle := []

let setAssosBoucleIdMaxIfSupOldMax id newmax  =
  if existeAssosBoucleIdMax id then
  begin
    let om = getAssosBoucleIdMax id in
    let maximum =
      (match newmax with
        EVALEXP(new_max)->
          (match om with
            EVALEXP(oldMax)->
                if oldMax != new_max then  EVALEXP(maxi oldMax new_max)
                else EVALEXP(new_max)
            |EXPMAX(lold) ->
             if List.mem (expressionEvalueeToExpression new_max) lold then om
             else  EXPMAX(List.append [expressionEvalueeToExpression new_max] lold) )

        |EXPMAX(ml) ->
          let new_exp = List.hd ml in
          (match om with
            EVALEXP(oldMax)->
              EXPMAX (List.append [expressionEvalueeToExpression oldMax] ml )
           |EXPMAX(lold) ->
                    if List.mem  new_exp lold then om
                      else   EXPMAX(List.append ml lold)
                   )

      ) in
    listeDesMaxParIdBoucle := List.remove_assoc id !listeDesMaxParIdBoucle;
    listeDesMaxParIdBoucle := List.append [(id, maximum)] !listeDesMaxParIdBoucle
  end
  else
  begin   listeDesMaxParIdBoucle := List.append [(id, newmax)] !listeDesMaxParIdBoucle end


let typeNidTeteCourant = ref (TBOUCLE(0,0,[], [], true,[], [],"",0))
let dernierAppelFct = ref  (TFONCTION(!(!mainFonc),0,[], [], [], [],[], [], true,false,"",0))
let predDernierAppelFct  = ref  (TFONCTION(!(!mainFonc),0,[], [], [], [],[],[], true, false,"",0))
type nidEval =
{
  sensVariation : sens;
  idBoucleN : evaluationType;
  expressionBorneToutesIt : expVA;
  varDeBoucleNidEval : string;
  maxUneIt : expVA;
  isExecuted : bool;
  isIntoIf : bool;
  num : int ;
}

type elementEval =
    BOUCLEEVAL of nidEval * evaluationType *elementEval list *expression * expression
  | APPELEVAL of evaluationType * expVA *elementEval list *expression * expression
  | IFEVAL of expression * elementEval list (*then list*) * elementEval list (*false list*) *
              expression * string (*file name *) * int (*num line*) * bool (*is into loop*) *
              int (*is executed false = not executed = true executed or undifined*) *
              string (*ifvarname*) * expression * expression (*list true, list false*) * oAdress * oAdress
              (* jz: add information about variables when evaluating the conditionals *)
              * typeListeAS

let new_elementEvalb nid n l et ef = BOUCLEEVAL(nid, n, l,et, ef)
let new_elementEvala n e l et ef= APPELEVAL (n,e,l,et, ef)
let new_elementEvali e subtT subtF e2 file line isinloop exectutioncase var e3 e4 infoThen infoElse asL = IFEVAL(e,  subtT, subtF, e2, file, line, isinloop, exectutioncase, var, e3, e4, infoThen, infoElse, asL)

let corpsEvalTMP = ref []
let nouBoucleEval = ref []

let new_nidEval  t  e2 var sensV  expMax b isi n=
{

  sensVariation = sensV;
  idBoucleN = t;
  expressionBorneToutesIt = e2;
  varDeBoucleNidEval = var;
  maxUneIt = expMax;
  isExecuted = b;
  isIntoIf = isi;
  num = n
}

type  documentEvalue =
{
    maListeNidEval : nidEval list;(* transitoire*)
    maListeEval : elementEval list;
}

let new_documentEvalue  listeN listeF =
{
    maListeNidEval = listeN;
    maListeEval = listeF;
}
(*let docEvalueCour  = ref (new_documentEvalue  [] [])*)
let docEvalue = ref (new_documentEvalue  [] [])
let appelcourant = ref  []

let estTROUVEID = ref false
let estTROUVEIDO = ref false


(* jz: balanced.ml *)
let (docToAnalyze : document ref) = ref (new_document [] [] [] [])
let (docAnalyzed : documentEvalue ref) = ref (new_documentEvalue [] [])





let rec jusquaB listeInst saufId  =
  if listeInst <> [] then begin
    let (first, trouve) = jusquaPourBoucle (List.hd listeInst) saufId in
    if trouve != true then begin
      let (second, trouve2) = jusquaB (List.tl listeInst) saufId in
      (List.append  first   second, trouve2)
    end else
      (first, trouve)
  end else
    ([],false)


and jusquaPourBoucle premiere sId  =
  match premiere with
    FORV(num, _, _, _, _, _, i,_) ->
      if num != sId then
        ([premiere], false)
      else begin
        ([], true)
      end
  | VAR (_, _, _, _)
  | TAB (_, _, _, _, _)
  | APPEL (_, _, _, _, _, _, _, _)
  | MEMASSIGN (_, _, _, _, _)->
      ([premiere],false)
  | IFVF (_, i1, _)
  | IFV ( _, i1) ->
      ([premiere],false)
  | BEGIN (liste) ->
      jusquaB liste sId


let rec jusquaI listeInst saufId  =
  if listeInst <> [] then begin
    let (first, trouve) =  jusquaPourI (List.hd listeInst) saufId in
    if trouve != true then begin
      let (second, trouve2) = jusquaI (List.tl listeInst) saufId in
      (List.append  first   second, trouve2)
    end else
      (first, trouve)
  end else begin (*Printf.printf "jusquaI : empty list\n";*)
    ([], false)
  end


and jusquaPourI premiere saufId =
  match premiere with
    FORV (_, _, _, _, _, _, _, _)
  | VAR (_, _,_,_)
  | TAB ( _, _, _, _, _)
  | MEMASSIGN(_, _, _, _, _)
  | APPEL (_, _, _, _, _, _, _, _) ->
      ([premiere], false)
  | IFVF ( cond , i1, i2) ->
      if cond = EXP(VARIABLE(saufId)) then
        ([], true)
      else
        ([premiere],false)
  | IFV (cond, i1) ->
      if cond = EXP(VARIABLE(saufId)) then
        ([], true)
      else
        ([premiere], false)
  | BEGIN (liste) ->
      jusquaI liste saufId


let rec jusquaF listeInst saufId  =
  if listeInst <> [] then begin
    let (first, trouve) =  jusquaPourF (List.hd listeInst) saufId in
    if trouve != true then begin
      let (second, trouve2) = jusquaF (List.tl listeInst) saufId in
      (List.append  first   second, trouve2)
    end else
      (first, trouve)
  end else
    ([],false)

and jusquaPourF premiere saufId =
  match premiere with
    FORV (n, a, b, c, d, e, i, _) ->
      let (_, trouve) = jusquaPourF i saufId in
      ([premiere],  trouve)
  | VAR ( _, _, _, _)
  | TAB ( _, _, _, _,_)
  | MEMASSIGN (_, _, _, _, _) ->
      ([premiere] ,false)
  | APPEL (num,_, _, _,_,_,_,_) ->
      if num != saufId then
        ([premiere],  false)
      else begin
        appelcourant := [premiere];
        ([] ,true)
      end
  | IFVF (_, i1, _)
  | IFV ( _, i1)  ->
      ([premiere], false)
  | BEGIN (liste) ->
      jusquaF liste saufId


let rec nextInstructionsB id inst=
  (*Printf.printf "nextInstructionsB\n";*)
  if inst = [] then
    ([], false)
  else begin
    let (premiere, suite) = (List.hd inst, List.tl inst) in
    match premiere with
      FORV (num, _, _, _, _, _, _,_) ->
        if num = id then
          (suite, true)
        else
          nextInstructionsB id suite
    | VAR ( _, _, _, _)
    | TAB ( _, _, _, _, _)
    | MEMASSIGN (_, _, _, _, _)
    | APPEL (_, _, _, _, _, _, _, _) ->
        nextInstructionsB id suite
    | IFVF ( c, i1, i2) ->
      let (c1, t1) = nextInstructionsB id [i1] in
      let (c2, t2) = nextInstructionsB id [i2] in
        if (t1 = false && t2 = false) || (c1 = [] && c2 = []) then
          nextInstructionsB id suite
        else begin
          if (t1 = true) then
            (List.append [IFVF (c, BEGIN(c1), i2)] suite, true)
          else
            (List.append [IFVF (c, BEGIN(c1), BEGIN(c2))] suite, true)
       end
    | IFV (c, i1) ->
        let (c1, t1) = nextInstructionsB id [i1] in
          if t1 = false || c1 = [] then
            nextInstructionsB id suite
          else
            (List.append [IFV (c,  BEGIN(c1))] suite, true)
    | BEGIN (liste) ->
        let (top, trouve) =  (nextInstructionsB id liste) in
        if top = [] || trouve = false then
          (nextInstructionsB id suite)
        else
          (List.append [BEGIN (top)] suite, true)
  end


let rec nextInstructionsF id inst=
  (*Printf.printf "nextInstructionsF\n";*)
  if inst = [] then
    ([], false)
  else begin
    let (premiere, suite) = (List.hd inst, List.tl inst) in
    match premiere with
      FORV (_, _, _, _, _, _, _, _)
    | VAR (_, _, _, _)
    | TAB (_, _, _, _, _)
    | MEMASSIGN ( _, _, _, _, _) ->
        nextInstructionsF id suite
    | APPEL (num,_, _, _, _, _, _, _) ->
        if num = id then begin
          (suite, true)
        end else
          nextInstructionsF id suite
    | IFVF (c, i1, i2) ->
        let (c1,t1) = nextInstructionsF id [i1] in
        let (c2, t2) = nextInstructionsF id [i2] in
          if (t1= false && t2 = false) || (c1 = [] && c2 = []) then
            nextInstructionsF id suite
          else begin
            if (t1 = true) then
              (List.append [IFVF (c, BEGIN(c1), i2)] suite, true)
            else
              (List.append [IFVF (c, BEGIN([]), BEGIN(c2))] suite, true)
          end
    | IFV ( c, i1) ->
        let (c1, t1) = nextInstructionsF id [i1]  in
        if t1 = false || c1 = [] then
          nextInstructionsF id suite
        else
          (List.append [IFV (c, BEGIN(c1))] suite, true)
    | BEGIN (liste) ->
        let (top, trouve) =  (nextInstructionsF id liste) in
        if top = [] || trouve = false then
          (nextInstructionsF id suite)
        else
          (List.append [BEGIN (top)] suite, true)
  end

let rec nextInstructionsI id inst=
  (*Printf.printf "nextInstructionsF\n";*)
  if inst = [] then
    ([], false)
  else begin
    let (premiere, suite) = (List.hd inst, List.tl inst) in
    match premiere with
      FORV (_, _, _, _, _, _, _, _)
    | VAR (_, _, _, _)
    | TAB (_, _, _, _, _)
    | MEMASSIGN(_, _, _, _, _)
    | APPEL (_, _, _, _, _, _, _, _) ->
        nextInstructionsI id suite
    | IFVF ( c, i1, i2) ->
        (match c with
          EXP(VARIABLE(var)) ->
            if var = id then
              (suite, true)
            else begin
              let (c1, t1) = nextInstructionsI id [i1] in
              let (c2, t2) = nextInstructionsI id [i2] in
              if (t1 = false && t2 = false) || (c1 = [] && c2 = []) then
                nextInstructionsI id suite
              else begin
                if (t1 = true) then
                  (List.append [IFVF (c, BEGIN(c1), i2)] suite, true)
                else
                  (List.append [IFVF (c, BEGIN(c1), BEGIN(c2))] suite, true)
              end
            end
    |_->
        let (c1, t1) = nextInstructionsI id [i1] in
        let (c2, t2) = nextInstructionsI id [i2] in
          if (t1= false && t2 = false) || (c1 = [] && c2 = []) then
            nextInstructionsI id suite
          else begin
            if (t1 = true) then
              (List.append [IFVF (c, BEGIN(c1), i2)] suite, true)
            else (List.append [IFVF (c, BEGIN(c1), BEGIN(c2))] suite, true)
          end)
    | IFV (c, i1) ->
        if c = EXP(VARIABLE(id)) then
          (suite, true)
        else begin
          let (c1, t1) = nextInstructionsI id [i1]  in
          if t1 = false || c1 = [] then
            nextInstructionsI id suite
          else
            (List.append [IFV (c, BEGIN(c1))] suite, true)
        end
    | BEGIN (liste)   ->
        let (top, trouve) =  (nextInstructionsI id liste) in
        if top = [] || trouve = false then
          (nextInstructionsI id suite)
        else
          (List.append [BEGIN (top)] suite, true)
  end

and endOfcontexte affec last new_contexte globales =
  if last = [] then begin (*Printf.printf"lastvide \n";*)
    myCurrentPtrContextAux := (localPtrAnalyse [new_instBEGIN (affec)]
                                  !myCurrentPtrContext !estDansBoucle false);
    evalStore (BEGIN(affec)) new_contexte globales !myCurrentPtrContext
  end else begin
    let (fin,_) =
      (match List.hd last with
        IDBOUCLE (num, _, _, _, _) ->
          (*Printf.printf"last loop %d\n" num;*)
          nextInstructionsB num affec
      | IDAPPEL (numf, _, _, _, _, _, _, _) ->
          (* Printf.printf"last function  %d\n" numf;*)
          nextInstructionsF numf affec
      | IDIF (var, _, _, _, _, _, _, _, _, _, _, _)  ->
          (*Printf.printf"last if  %s\n" var;*)
          nextInstructionsI var affec)
      in
        if fin = [] then
          new_contexte
        else begin
          myCurrentPtrContextAux := (localPtrAnalyse [new_instBEGIN (fin)]
                                      !myCurrentPtrContext !estDansBoucle false);
          evalStore (BEGIN(fin)) new_contexte globales !myCurrentPtrContext
        end
  end

let jusquaFaux listeInst saufId contexte lastLoopOrCall globales =
      (* Printf.printf "looking for function into %d\n" saufId;
      afficherLesAffectations ( listeInst) ;new_line () ;*)
      (*Printf.printf "jusqu'aFaux %d\n" saufId;*)
  let (res, trouve) = jusquaF listeInst saufId in
      (*if trouve = false then Printf.printf "not found function %d\n" saufId;
      afficherLesAffectations ( res) ;new_line () ;
      Printf.printf "jusquaFaux %d\n" saufId;*)
  let newres = endOfcontexte res lastLoopOrCall contexte globales in
      (*afficherListeAS ( newres) ;new_line () ;
      Printf.printf "jusquaFaux %d\n" saufId;*)
  newres


let jusquaIaux listeInst saufId contexte lastLoopOrCall globales =
      (*Printf.printf "jusqu'jusquaIaux %s\n" saufId;*)
  let (res, _)= jusquaI listeInst saufId in
      (* if trouve = false then Printf.printf "If  not found %s\n" saufId;*)
      (*Printf.printf "if looking for into  %s\n" saufId;*)
      (* afficherLesAffectations ( res) ;new_line () ;*)
  let newres = endOfcontexte res lastLoopOrCall contexte globales in
      (*afficherListeAS ( newres) ;new_line () ;
      Printf.printf "jusquaIaux %s\n" saufId;*)
  newres


let rec listeSAUFIDB listeInst sId l =
  if listeInst <> [] then
    List.append [traiterUneIntructionPourBoucle (List.hd listeInst) sId l]
                (listeSAUFIDB (List.tl listeInst) sId l)
  else
    []

and traiterUneIntructionPourBoucle premiere sId l =
  match premiere with
    FORV (n, a, b, c, d, e, i,cp) ->
      if (n = sId) then
        BEGIN(l)
      else begin
        let ni =traiterUneIntructionPourBoucle i sId l in memAssign [ni];
        FORV (n,a,b,c,d,e, ni,!myChangePtr)
      end
  | IFVF (c, i1, i2) ->
      IFVF (c, traiterUneIntructionPourBoucle i1 sId l,
                  traiterUneIntructionPourBoucle i2 sId l)
  | IFV (c, i1) ->
      IFV ( c, traiterUneIntructionPourBoucle i1 sId l)
  | BEGIN (liste) ->
      BEGIN (listeSAUFIDB liste sId l)
  | APPEL (num, avant, nom, apres, CORPS corps, x, r, u) ->
      (*let a = List.assoc nom !alreadyEvalFunctionAS  in*)
      if List.mem_assoc nom !alreadyEvalFunctionAS then
        APPEL (num, avant, nom, apres, ABSSTORE
                  (List.assoc nom !alreadyEvalFunctionAS),x,r,u)
      else
        APPEL (num, avant, nom, apres,
                  CORPS (traiterUneIntructionPourBoucle corps sId l),x,r,u)
  |_-> premiere

let rec estDansListeInstBoucle l id =
  if l = [] then
    false
  else
    estDansCorpsBoucle (List.hd l) id || estDansListeInstBoucle (List.tl l) id

and estDansCorpsBoucle corps id =
  match corps with
    FORV (num, a, b, c, d, e, i,_) ->
      if (num = id) then
        true
      else
       estDansCorpsBoucle i id
  | IFVF (c, i1, i2) ->
      estDansCorpsBoucle i1 id || estDansCorpsBoucle i2 id
  | IFV (c, i1) ->
      estDansCorpsBoucle i1 id
  | BEGIN (liste) ->
      estDansListeInstBoucle liste id
  | _ -> false

let jusquaBaux listeInst saufId contexte lastLoopOrCall globales =
        (*Printf.printf "jusquaBaux %d\n" saufId;
        afficherLesAffectations ( listeInst) ;new_line () ;*)
  let (res,trouve) = jusquaB listeInst saufId in
  if trouve = false then
    Printf.printf "loop not found %d\n" saufId;
        (*afficherLesAffectations ( res) ;new_line () ;
        Printf.printf "jusquaBaux %d\n" saufId;*)
  let newres = endOfcontexte res lastLoopOrCall contexte globales in
        (*afficherListeAS ( newres) ;new_line () ;
        Printf.printf "jusquaBaux %d\n" saufId;*)
  newres


let nomFonctionDansDeclaration dec =
  let (_, _, name) = (dec) in
  let (s,_,_,_) = name in
  s

let existeFonctionParNom nom doc =

  (not (Cextraireboucle.is_in_use_partial nom))
  && (List.exists (
        fun (_, f) ->
          let (_, _, name) = (f.declaration) in
          let (s, _, _, _) = name in
          s = nom
     ) !doc.laListeDesFonctions)

let rechercherFonctionParNom nom docu =
  (List.find (
    fun (_, f) ->
      let (_, _, name) = (f.declaration) in
      let (s, _, _, _) = name in
      s = nom
  ) !docu.laListeDesFonctions)

let existeFonctionParNom nom docu =
  (not (Cextraireboucle.is_in_use_partial nom))
  && (List.exists (
        fun (_, f) ->
          let (_, _, name) = (f.declaration) in
          let (s, _, _, _) = name in
          s = nom
     ) !docu.laListeDesFonctions)


let rec getIntoAffect nom idappel pred listeinst =
if listeinst = [] then pred
else
begin
  let ini = List.hd listeinst in
  let suite = List.tl listeinst in
  match ini with
    VAR (_, _,_,_) | TAB (_, _, _, _,_)|  MEMASSIGN ( _, _, _, _,_) -> (getIntoAffect nom idappel  pred suite)
    | BEGIN liste ->
         let r1 = (getIntoAffect nom idappel  pred liste) in
         if r1 = pred then    (getIntoAffect nom idappel pred suite) else r1
    | IFVF (_, i1, i2) ->
      let liste1 = match i1 with BEGIN(e)-> e |_->[] in
      let res1 =   getIntoAffect nom idappel  pred liste1 in
      if res1 = pred then
      begin
        let liste2 = match i2 with BEGIN(e)-> e |_->[]  in
        let res2 =  getIntoAffect nom idappel  pred liste2 in
        if res2 =pred then (getIntoAffect nom idappel  pred suite) else res2
      end
      else res1
    | IFV ( _, i1)  | FORV (_,_, _, _, _, _, i1,_)  ->
      let liste1 = match i1 with BEGIN(e)-> e |_->[] in
      let res1 =   getIntoAffect nom idappel  pred liste1 in
      if res1 = pred then (getIntoAffect nom idappel  pred suite)
      else res1

    | APPEL (i,a,nomFonc,apres,CORPS c,varB,r,u)->
        if List.mem_assoc  nomFonc !alreadyEvalFunctionAS then
        begin
          if i = idappel && nomFonc = nom then APPEL (i, a, nomFonc, apres,ABSSTORE (List.assoc nomFonc !alreadyEvalFunctionAS ),varB,r,u)
          else (getIntoAffect nom idappel  pred suite)

        end
        else
        begin
            if i = idappel && nomFonc = nom then ini
            else
            begin
              let liste1 = match c with BEGIN(e)-> e |e->[e] in
              let corps =
                if liste1 = [] then
                begin
                    if ((existeFonctionParNom nomFonc doc) && (not (Cextraireboucle.is_in_use_partial nomFonc))) then
                      begin
                      let (_, func) = (rechercherFonctionParNom nomFonc doc) in
                      func.lesAffectations
                    end
                    else liste1
                end
                else liste1 in
              let res =  getIntoAffect nom idappel  pred  corps in
              if res = pred then (getIntoAffect nom idappel  pred suite)
              else res
            end
        end

    | APPEL (i,_,nomFonc,_, ABSSTORE c,_,_,_)->
      if i = idappel && nomFonc = nom then ini
      else (getIntoAffect nom idappel  pred suite)
end

let  getIntoAffectB nom idappel  pred listeinst =  getIntoAffect nom idappel  pred listeinst

let rec reecrireCallsInLoop var listeinst =
(*Printf.printf "reecrireCallsInLoop for var %s\n" var;*)
if listeinst = [] then listeinst
else
begin
  let i = List.hd listeinst in
  let suite = List.tl listeinst in
  match i with
    VAR (id, exp,_,_) -> List.append [i] (reecrireCallsInLoop var suite)
    | TAB (id, exp1, exp2,_,_) -> List.append [i] (reecrireCallsInLoop var suite)
    |  MEMASSIGN ( id, expVA1, expVA2,_,_)  ->   List.append [i] (reecrireCallsInLoop var suite)
    | BEGIN liste ->    List.append [BEGIN(reecrireCallsInLoop var liste)]   (reecrireCallsInLoop var suite)
    | IFVF (t, i1, i2) ->
      let liste1 = match i1 with BEGIN(e)-> e |_->[] in
      let res1 = reecrireCallsInLoop var liste1 in
      let liste2 = match i2 with BEGIN(e)-> e |_->[]  in
      let res2 = reecrireCallsInLoop var liste2 in
      List.append  [IFVF(t, BEGIN(res1), BEGIN(res2))] (reecrireCallsInLoop var suite)
    | IFV ( t, i1)    ->
      let liste1 = match i1 with BEGIN(e)-> e |_->[] in
      let res1 = reecrireCallsInLoop var liste1 in
      List.append [IFV ( t, BEGIN(res1))] (reecrireCallsInLoop var suite)
    | FORV (num,id, e1, e2, e3, nbIt, inst,cp)  ->
      let liste1 = match inst with BEGIN(e)-> e |_->[] in
      let res1 = reecrireCallsInLoop id liste1 in memAssign res1;

      List.append [FORV (num,id, e1, e2, e3, nbIt,  BEGIN(res1),!myChangePtr)] (reecrireCallsInLoop var suite)
    | APPEL (i,e,nomFonc,s,CORPS c,_,r,u)->
      (*Printf.printf "reecrireCallsInLoop call %s var %s \n" nomFonc var;*)

      if List.mem_assoc  nomFonc !alreadyEvalFunctionAS then
        List.append [APPEL (i, e ,nomFonc,s,ABSSTORE (List.assoc nomFonc !alreadyEvalFunctionAS ),var,r,u)] (reecrireCallsInLoop var suite)
      else
      begin
        let liste1 = match c with BEGIN(e)-> e |e->[e] in
        let corps =
          if liste1 = [] then
          begin
              if existeFonctionParNom nomFonc doc && (not (Cextraireboucle.is_in_use_partial nomFonc)) then
                begin
                let (_, func) = (rechercherFonctionParNom nomFonc doc) in
                func.lesAffectations
              end
              else liste1
          end
          else liste1 in
        let res1 = reecrireCallsInLoop var corps in (* SORTIR DE COMMENTAIRE mais n'appeler que dans traiter boucle internes*)
        List.append [APPEL (i, e ,nomFonc,s,CORPS (BEGIN(res1)),var,r,u)] (reecrireCallsInLoop var suite)
      end
    | APPEL (i,e,nomFonc,s, ABSSTORE a,_,r,u)->
      List.append [APPEL (i, e ,nomFonc,s,ABSSTORE a,var,r,u)] (reecrireCallsInLoop var suite)
end


let lesVardeiSansj nt nEC id l=
  let saufId = id in
 (* Printf.printf"lesVardeiSansj de i = %d et j = %d \n" (getBoucleInfoB (nEC.infoNid.laBoucle)).identifiant saufId;*)
  let listeInter =  listeSAUFIDB (reecrireCallsInLoop nEC.varDeBoucleNid  nEC.lesAffectationsBNid)   saufId  l in
 (* let info = (getBoucleInfoB (nEC.infoNid.laBoucle)) in
  let nomE =  (getBoucleIdB nt.infoNid.laBoucle) in
  let num = info.identifiant  in*)

    let loopTYpe = !myCurrentPtrContext
 in

  evalStore  (new_instBEGIN(listeInter)) [] [] loopTYpe


let rec recherche numappel liste =
if liste = [] then TFONCTION(!(!mainFonc),0,[], [], [], [],[],[], true,false,"",0)
else
begin
  let pred = (List.hd liste) in
  match pred with
  TBOUCLE(_, _,_,_,_,_,_,_,_) -> recherche numappel (List.tl liste)
  | TFONCTION (_, numF,_,_, _,_,_,_,_,_,_,_)  -> if numF = numappel then pred else recherche numappel (List.tl liste)
end

let rec majlappel  liste le =
if liste = [] then  []
else
begin
  let premiere = List.hd liste in
  match premiere with
  FORV ( n, a, b, c, d, e, i1,cp) -> List.append [FORV (n,a,b,c,d,e,  (BEGIN (majlappel  [i1] le)) ,cp)] (majlappel (List.tl liste) le)
  | IFVF ( c, i1, i2)   -> List.append  [IFVF ( c, (BEGIN (majlappel [i1]  le)), (BEGIN (majlappel [i2]  le))) ] (majlappel (List.tl liste) le)
  | IFV (c,i1)-> List.append  [IFV(c,(BEGIN (majlappel [i1]  le)))](majlappel (List.tl liste) le)
  | BEGIN (l) ->  List.append  [BEGIN (majlappel l le)] (majlappel (List.tl liste) le)
  | APPEL (num, avant, nom, apres,CORPS c,varB,r,u) ->
  if List.mem_assoc  nom !alreadyEvalFunctionAS then
        List.append [APPEL (num, avant ,nom,apres,ABSSTORE (List.assoc nom !alreadyEvalFunctionAS ),varB,r,u)]  (majlappel (List.tl liste) le)
  else
  begin
      let liste1 = match c with BEGIN(e)-> e |e->[e] in
      let corps =
        if liste1 = [] then
        begin
            if existeFonctionParNom nom doc && (not (Cextraireboucle.is_in_use_partial nom)) then
              begin
              let (_, func) = (rechercherFonctionParNom nom doc) in
              func.lesAffectations
            end
            else liste1
        end
        else liste1 in

      List.append [APPEL (num, avant, nom, apres, CORPS (BEGIN (majlappel  corps  le))  , varB,r,u) ] (majlappel (List.tl liste) le)
  end
  |_-> List.append [premiere] (majlappel (List.tl liste) le)
end



let rec rechercheTFonction liste nom numee=
if liste <> [] then
begin
  let pred = (List.hd liste) in
  match pred with
  TBOUCLE(_, _,_,_,_,_,_,_,_) -> rechercheTFonction (List.tl liste) nom numee
  | TFONCTION(n,num,c,_,_,_,_,_,_,_,_,_) ->  if n = nom && numee = num then pred else rechercheTFonction (List.tl liste) nom numee
end
else (TFONCTION(!(!mainFonc),0,[], [], [], [],[], [], true, false,"",0))


let rec consassigntrueOrfalse l op=
if List.tl l = [] then   VARIABLE(List.hd l)
else BINARY (op, VARIABLE(List.hd l) ,consassigntrueOrfalse (List.tl l) op)

let rec creerLesAffectEXECUTEDFct listeFct  =
if listeFct = [] then []
else
begin
  let (head,next)  = (List.hd listeFct,List.tl listeFct) in
  let (name, num, lt,lf) = head in
  (*Printf.printf "creerLesAffectEXECUTEDFct %s %d \n"name num ;
  ( Printf.printf "isExecuted : list of true variables of condition \n"; List.iter (fun e-> Printf.printf "%s "e) lt);
  (  Printf.printf "isExecuted : list of false variables of condition\n"; List.iter (fun e-> Printf.printf "%s "e) lf);*)
  let (_,_,l)= creerLesAffectEXECUTED lt lf name num 0 !cptFunctiontestIntoLoop in
  List.append l (creerLesAffectEXECUTEDFct next)
end

and creerLesAffectEXECUTED lt lf f nappel id conte=
  let varExeT =  Printf.sprintf "ET-%s-%d-%d-%d" f  nappel id conte in
  let varExeF =  Printf.sprintf "EF-%s-%d-%d-%d" f  nappel id conte in

  if lt = [] && lf = [] then ([varExeT], [varExeF], []  )
  else
  begin
  cptFunctiontestIntoLoop := !cptFunctiontestIntoLoop +1;
  if lt != [] && lf != [] then
    ([varExeT], [varExeF],
      List.append  [new_instVar varExeT (EXP(consassigntrueOrfalse lt AND))]  [new_instVar varExeF (EXP(consassigntrueOrfalse lf OR))]  )
  else if lt = [] then  ([varExeT], [varExeF],    [new_instVar varExeF (EXP(consassigntrueOrfalse lf OR))]   )
       else ([varExeT], [varExeF],   [new_instVar varExeT (EXP(consassigntrueOrfalse lt AND))]  )
  end

let creerVarTF lt lf contexte globales=
  if lt = [] && lf = [] then ( CONSTANT(CONST_INT("1")), CONSTANT(CONST_INT("0")) )
  else begin
    if lt != [] && lf != [] then  (
                  applyStore (applyStore (consassigntrueOrfalse lt AND) contexte)globales,
                  applyStore (applyStore (consassigntrueOrfalse lf OR) contexte)globales
                 )
    else if lt = [] then    (
                  CONSTANT(CONST_INT("1")),
                  applyStore (applyStore (consassigntrueOrfalse lf OR) contexte)globales
                )
          else
                (
                  applyStore (applyStore (consassigntrueOrfalse lt AND) contexte)globales,
                  CONSTANT(CONST_INT("0"))
                )
  end

let creerVarTFE   ltv ntf   =
      let first =   calculer   (EXP(ltv))  !infoaffichNull  [] 1  in
      let second =   calculer  (EXP( ntf))  !infoaffichNull  [] 1  in

            (match first with
              Boolean(false) | ConstInt("0") ->   false
            |_ ->  (match second with  Boolean(true)  | ConstInt("1") ->false  |_-> true))


(*let listTest =ref []*)
let listBeforeFct =ref []

let rec numberOfCall listBefore nomf numF =
if listBefore = [] then 0
else
begin
  let (n, c,_,_) = List.hd listBefore in
  if n= nomf  && numF = c then 1+numberOfCall (List.tl listBefore) nomf numF else numberOfCall (List.tl listBefore) nomf numF
end

let rec getFirts nom num liste =
if liste = [] then []
else
begin
  let (firtC, nextC) =  (List.hd liste,List.tl liste)  in
  let (nameB,numB,ltB,lfB) = firtC in
  if nameB = nom && nextC = num then  nextC else getFirts nom num nextC
end

let rec listeSAUFIDA listeInst saufId ainserer  input le la =
  if listeInst <> [] then
  begin
    let premiere = List.hd listeInst in
    let suite = List.tl listeInst in

    let na = traiterUneIntructionPourAppel premiere saufId ainserer  input le la  in
    if !estTROUVEID then List.append  [na] (majlappel suite le)
    else  List.append (majlappel [premiere] le) (listeSAUFIDA suite saufId ainserer  input le la )
  end
  else  []


and traiterUneIntructionPourAppel premiere sId ainserer  input le la =
  match premiere with
  FORV (n,a,b,c,d,e,i,_) ->
    let ni= traiterUneIntructionPourAppel i sId ainserer  input le la in memAssign [ni];

    FORV (n,a,b,c,d,e,  ni,!myChangePtr)
  | IFVF ( c, i1, i2)     ->
    IFVF ( c, traiterUneIntructionPourAppel i1 sId ainserer  input le la ,
       traiterUneIntructionPourAppel i2 sId ainserer  input le la )
  | IFV (c,i1)-> IFV(c,traiterUneIntructionPourAppel i1 sId ainserer  input le la )
  | BEGIN (liste)   ->    BEGIN (listeSAUFIDA liste sId ainserer  input le la )
  | APPEL (num, avant, nom, apres,CORPS corps,varB,r,u) ->
    if List.mem_assoc  nom !alreadyEvalFunctionAS then
    traiterUneIntructionPourAppel (APPEL (num, avant, nom, apres,ABSSTORE (List.assoc nom !alreadyEvalFunctionAS ),varB,r,u)) sId ainserer  input le la
      else
    begin
        let liste1 = match corps with BEGIN(c)-> c |c->[c] in
        let corpsF =
            if liste1 = [] then
            begin
                if existeFonctionParNom nom doc  then
                  begin
                  let (_, func) = (rechercherFonctionParNom nom doc) in
                 BEGIN ( func.lesAffectations   )
                end
                else BEGIN liste1
            end
            else BEGIN liste1
         in

          if !listBeforeFct = [] then
        begin
            if  num != sId (* || ( num = sId && la != 0 )*) then
            begin
              let suite = traiterUneIntructionPourAppel ((*BEGIN*)(corpsF)) sId ainserer  input le (*listbd*)la  in
              APPEL (num, avant, nom, apres,CORPS suite,varB ,r,u)
            end
            else
            begin
                appelcourant := [premiere];
                estTROUVEID := true;
              let new_appel = APPEL (num, avant, nom, apres,   CORPS ( BEGIN (  ainserer)), varB,r,u) in
                BEGIN (List.append ( input)   [new_appel]);
            end
        end
        else
        begin
          let (firtC, nextC) =  (List.hd !listBeforeFct,List.tl !listBeforeFct)  in

          let (nameB,numB,ltB,lfB) = firtC in
          listBeforeFct := if nameB = nom && numB = num then nextC else !listBeforeFct;
          let nb = numberOfCall !listBeforeFct nameB numB in

          if List.mem_assoc  nom !alreadyEvalFunctionAS then
            BEGIN (List.append (  creerLesAffectEXECUTEDFct ([(nameB,numB,ltB,lfB)] ))  [premiere])
          else
          begin
            let suite = traiterUneIntructionPourAppel ((*BEGIN*)(corpsF)) sId ainserer  input le la  in
            if nb != 0  then APPEL (num, avant, nom, apres,CORPS suite,varB ,r,u)
            else
            begin
              (*Printf.printf "before fct %s %d\n"nameB numB;*)
              let listtestfct =   creerLesAffectEXECUTEDFct ([(nameB,numB,ltB,lfB)] ) in
              let new_appel = APPEL (num, avant, nom, apres,CORPS suite,varB ,r,u) in
              BEGIN (List.append (  listtestfct)   [new_appel]);
            end
          end
        end
    end
  | APPEL (num, avant, nom, apres,ABSSTORE a,varB,r,u) ->
    if !listBeforeFct = [] then
    begin

        if num != sId (* || ( num = sId && la != 0 ) *)then
        begin
          let new_appel = APPEL (num, avant, nom, apres, ABSSTORE a,varB,r ,u) in
          new_appel
        end
        else
        begin
          appelcourant := [premiere];
          estTROUVEID := true;
          let nas = evalStore   ( BEGIN (   ainserer)) [] [] !myCurrentPtrContext in    (*OK *)
          let new_appel = APPEL (num, avant, nom, apres,   ABSSTORE nas, varB,r,u)  in
        (* afficherUneAffect (new_instBEGIN input); Printf.printf "evalSIDA fin\n";
      afficherUneAffect (avant); Printf.printf "evalSIDA fin\n";*)
          BEGIN (List.append (  input)   [new_appel]);
        end
    end
    else
    begin
      let (firtC, nextC) =  (List.hd !listBeforeFct,List.tl !listBeforeFct)  in

      let (nameB,numB,ltB,lfB) = firtC in
      listBeforeFct := if nameB = nom && numB = num then nextC else !listBeforeFct;
      let nb = numberOfCall !listBeforeFct nameB numB in

      if nb != 0  then APPEL (num, avant, nom, apres,ABSSTORE a,varB ,r,u)
      else
      begin
        let listtestfct =   creerLesAffectEXECUTEDFct ([(nameB,numB,ltB,lfB)] ) in
        let new_appel = APPEL (num, avant, nom, apres,ABSSTORE a,varB ,r,u) in
        BEGIN (List.append (  listtestfct)   [new_appel]);
      end
    end
    (*end*)
  |_-> premiere


let existeappel l  saufId=  List.exists (fun i -> match i with APPEL (num,_, _, _,_,_,_,_) ->  num = saufId  |_-> false  )l

let rechercheAppelCourant l saufId= List.find(fun i -> match i with  APPEL (num,_, _, _,_,_,_,_) ->  num = saufId |_-> false  )l

let  evalSIDA nT nEC listeInst saufId   ainserer  input le la listBefore=
  estTROUVEID := false;
  let nc = new_instBEGIN(listeSAUFIDA listeInst saufId ainserer  input le la ) in
 (*Printf.printf "evalSIDA nc %d\n" saufId; afficherUneAffect nc;Printf.printf "evalSIDA end\n"; *)
  let info = (getBoucleInfoB (nEC.infoNid.laBoucle)) in
  let nomE =  (getBoucleIdB nT.infoNid.laBoucle) in
  let num = info.identifiant  in

    let loopTYpe =
      if nomE = num then !myCurrentPtrContext (* most upper loop *)
      else
      begin
        let (mycontextInner, iscurrentUpper) =
          if List.mem_assoc num !callsListEvalLoop then
            ( (* Printf.printf "\nEvalStore boucle   %d current first\n" num ;*)(List.assoc num !callsListEvalLoop, true))
          else if List.mem_assoc num !callsListEvalLoopInner then
             begin
              let localUpper = List.assoc num !callsListEvalLoopInner in
              if List.mem_assoc localUpper !callsListEvalLoop then
                (  (*Printf.printf "\nlocalUpper Inner   %d current first %d\n" localUpper id;*)
                  (List.assoc localUpper !callsListEvalLoop, false))
              else
                ( (*if nomE != localUpper then
                    Printf.printf "\nlocalUpper boucle last  %d current first la %d %d\n" localUpper nomE id;*)
                    (!myCurrentPtrContext,false) )
            end
            else (
                (*if nomE != num then  Printf.printf "\nlocalUpper boucle last  2 %d current first %d \n" num id;*)
                  (!myCurrentPtrContext,false) )
        in

         if iscurrentUpper then mycontextInner (* its an upper loop for the function call but its call is into an other loop *)
         else  (* an always inner loop *)mycontextInner

      end in



  let res= evalStore  nc [] []  loopTYpe in
 (* print_string " evalStore Result:\n";
  afficherListeAS res;
  print_string "End evalStore Result.\n";*)
  res


let rechercherEvalParNomAppel nomF idB appel listeF=
List.find
(fun e ->
  match e with
    BOUCLEEVAL  (_,ty,_,_,_)->(match ty with TBOUCLE(ide, appele,_,_,_,_,_,_,_)-> idB = ide && appele = appel |_-> false)
  | APPELEVAL (ty,_,_,_,_)->(match ty with TFONCTION(nom, appele,_,_,_, _,_,_,_,_,_,_)-> nom = nomF && appele = appel|_->false)
  |_->false
)listeF

let rec isExecutedNidEval id appel listeEval =
  if listeEval = [] then false
  else
  begin
    match (List.hd listeEval).idBoucleN  with
    TBOUCLE(ide, appele,_,_,_,_,_,_,_)  ->
      if (id = ide) && (appele = appel) then (List.hd listeEval).isExecuted
      else  isExecutedNidEval id appel (List.tl listeEval)
    | _ -> isExecutedNidEval id appel (List.tl listeEval)
  end

let rec allerJusquaFonction liste =
if liste = [] then []
else
begin
  match (List.hd liste) with TBOUCLE(_, _,_,_,_,_,_,_,_) -> allerJusquaFonction (List.tl liste)  | TFONCTION(_,_,_,_,_,_,_,_,_,_,_,_) -> liste
end

let rec rechercheDerniereBoucle liste =
  let pred = (List.hd liste) in
  match pred with
  TBOUCLE(a, b,c,d,isExeE,lt,lf,_,_) -> (a, b, c,d,isExeE,lt,lf, List.tl liste)
  | TFONCTION(n,b,_,_,_,_,lt,lf,_,_,_,_) -> listeAppels := List.append  !listeAppels [(n,b,lt,lf)]; dernierAppelFct := pred; rechercheDerniereBoucle  (List.tl liste)

let rec rechercheDernierAppel liste =
  let pred = (List.hd liste) in
  match pred with
  TBOUCLE(a, b,c,d,isExeE,lt,lf,_,_) -> rechercheDernierAppel (List.tl liste)
  | TFONCTION(_,_,_,_,_,_,_,_,_,_,_,_) ->  dernierAppelFct := pred

let rec rechercheDerniereBoucleApresId id liste =

let pred = (List.hd liste) in
  match pred with
  TBOUCLE(idb, _,_,_,_,_,_,_,_) ->
  (*  Printf.printf "rechercheDerniereBoucleApresId loop cour %d loop cherchee %d" idb id;*)
    if idb = id then
    begin
      listeAppels := [];
      let (a,b,c,d,isExeE,lt,lf,_) = rechercheDerniereBoucle (List.tl liste) in (a,b,c,d,isExeE,lt,lf)
    end
    else rechercheDerniereBoucleApresId  id (List.tl liste)
  | TFONCTION(n,b,_,_,_,_,lt,lf,_,_,_,_) ->
    (*Printf.printf "rechercheDerniereBoucle fonction courante %s loop cherchee %d " n id ;*)
   listeAppels := List.append !listeAppels [(n,b,lt,lf)] ;  dernierAppelFct := pred; rechercheDerniereBoucleApresId  id (List.tl liste)

let rec existeDerniereBoucle liste =
  if liste = [] then false
  else
  begin
    match List.hd liste with
      TBOUCLE(_, _,_,_,_,_,_,_,_) -> true | TFONCTION(_,_,_,_,_,_,_,_,_,_,_,_) -> existeDerniereBoucle ( List.tl liste)
  end

let rec rechercheNbTotalIti id appel listeEval =
  if listeEval = [] then EXP(NOTHING)
  else
  begin
    match (List.hd listeEval).idBoucleN  with
    TBOUCLE(ide, appele,_,_,_,_,_,_,_)  ->
      if (id = ide) && (appele = appel) then (List.hd listeEval).expressionBorneToutesIt
      else  rechercheNbTotalIti id appel (List.tl listeEval)
    | _ -> rechercheNbTotalIti id appel (List.tl listeEval)
  end

let  rec existeNidContenant liste  id =
  if liste = [] then false
  else
  begin
    let (listeInt,_,_ ) = (List.hd liste) in
    if List.mem id listeInt then true else existeNidContenant (List.tl  liste) id
  end

let rec rechercheNidContenant liste  id =
  if liste = [] then []
  else
  begin
    let (listeInt,_,_ ) = (List.hd liste) in
    if List.mem id listeInt then [(List.hd liste)]
    else rechercheNidContenant (List.tl  liste) id
  end

let resAuxTN = ref MULTIPLE
let maxAuxTN = ref MULTIPLE
let isIntoIfLoop = ref false
let isEnd = ref false
let isEndNONZERO = ref false

let creerLesAffect tN max tni num nappel=
  let varBoucleTN =  Printf.sprintf "%s-%d_%d" "tN" num nappel in
  let varBouclemax =  Printf.sprintf "%s-%d_%d" "max" num nappel in
  let varBoucleTNI =  Printf.sprintf "%s-%d_%d" "tni" num nappel in
  let output =  List.append  [new_instVar varBoucleTN (EXP(VARIABLE(varBoucleTN)))]
    (List.append [new_instVar varBouclemax (EXP(VARIABLE(varBouclemax)))]  [new_instVar varBoucleTNI (EXP(VARIABLE(varBoucleTNI)))]) in
  (varBoucleTN,varBouclemax,varBoucleTNI,
  List.append  [new_instVar varBoucleTN tN] (List.append [new_instVar varBouclemax max] [new_instVar varBoucleTNI tni]), output)

let rec listejusquafonction listeEng id pred  =
  if listeEng = [] then  begin (*Printf.printf "non trouvee %d \n" id ;*)(pred , false)end
  else
  begin
    let premier =  List.hd listeEng in
    let suite =  List.tl listeEng in
    match premier with
      TBOUCLE(n, _,_,_,_,_,_,_,_) ->
        if id = n then begin (*Printf.printf "boucle trouvee %d \n" id ;*)(pred, true)end
        else
        begin (*Printf.printf "boucle  continue  %d \n" id ;*)
          listejusquafonction suite id pred
        end
      | TFONCTION(nom, _,_,_,_,_,_,_, _,_,_,_) -> (*Printf.printf "listejusquafonction ajout fonction  %d nom %s\n"  id nom;*)
        listejusquafonction suite id premier
  end



let valeurEng = ref NOCOMP
let borneAux = ref NOCOMP
let borneMaxAux = ref NOCOMP

let listeVB = ref []
let listeVBDEP = ref []
(*let listeVBPredNonNidNonNul = ref [] *)
let isProd = ref false
let isExactEng = ref true
let curassocnamesetList = ref []

let rec isSetExpression exp  =
match exp with
    |CALL(VARIABLE "SET", args)->  (true,List.hd args,List.hd (List.tl args))
    | _-> (false,NOTHING, NOTHING)


let rec compNotInnerDependentLoop nnE iscompo=
match nnE.idBoucleN with
TBOUCLE(num, appel, _,_,_,_,_,_,_) ->
   if iscompo = false then
   begin
    let nia =   nnE.sensVariation   in
      let nm =  nnE.maxUneIt  in
    isProd := false;
    hasSETCALL := false;

    let c1 = calculer  nm   nia [] 1 in

    let hasinit = !hasSETCALL in
    curassocnamesetList := [];
    if estDefExp c1 = false then
    begin
      let varOfExp = listeDesVarsDeExpSeules (expVaToExp nm) in
       List.iter
      (fun n ->
        if getNbIt n (expVaToExp nm) > 1 then
        begin
          if existeAffectationVarListe n !listeVBDEP then
          begin
            let assign =  ro n  !listeVBDEP in
            match assign with ASSIGN_SIMPLE (_, exp) ->
              let (boolean , e1, e2) = isSetExpression (expVaToExp exp) in
              if boolean then
                curassocnamesetList := List.append [n, ASSIGN_SIMPLE (n, EXP(e1)),ASSIGN_SIMPLE (n, EXP(e2))] !curassocnamesetList;
                |_-> ()
          end

        end

      )varOfExp
      end;
    hasSETCALL := false;
      let new_expmax =
      if !curassocnamesetList = []  then
        applyif nm !listeVBDEP
      else
      begin
        let (_, a1, a2) =List.hd !curassocnamesetList in
        let (eva1, eva2) =(applyif ( applyif nm [a1])  !listeVBDEP , applyif ( applyif nm [a2])  !listeVBDEP ) in
          EXP(CALL (VARIABLE("MAXIMUM") , (List.append [expVaToExp eva1] [expVaToExp eva2] )) )

      end
      in

    let (expmax1, reseval) =
      ( if estDefExp c1 = false then begin  (calculer new_expmax  nia [] 1, false) end
          else (c1,true) ) in (* valeur max apres propagation*)
    let hass = !hasSETCALL in
    let myMaxIt = expmax1  in

    let varBoucleIfN =  Printf.sprintf "%s-%d" "bIt" num in
    listeVB := listeSansAffectVar !listeVB varBoucleIfN;
      listeVBDEP := listeSansAffectVar !listeVBDEP varBoucleIfN;
    if estDefExp myMaxIt && (getDefValue myMaxIt <=0.0)  then
    begin
        borneMaxAux:= (ConstInt("0"));
        setAssosBoucleIdMaxIfSupOldMax num (EVALEXP(ConstInt("0")))
    end
    else
    begin
        if estDefExp myMaxIt then
        begin
          borneMaxAux:= myMaxIt ;
          setAssosBoucleIdMaxIfSupOldMax num (EVALEXP(myMaxIt));
        end
        else
        begin
          let maxp = if existeNid num then (rechercheNid num).infoNid.expressionBorne else NOTHING in
          setAssosBoucleIdMaxIfSupOldMax num (EXPMAX [maxp]);
          borneMaxAux :=  NOCOMP
        end
    end;

    let (iAmExact, myVar)=
        if existeNid num then  ( hasinit= false&&hass=false &&(rechercheNid num).infoNid.isExactExp && (nnE.isIntoIf = false),  varBoucleIfN)
        else (false ,  varBoucleIfN)
          in


    if (*!isPartialisation = false &&*) hasinit= false&&hass=false&& (rechercheNid num).infoNid.isExactExp &&  !isExecutedOneTimeOrMore && estDefExp myMaxIt && (estNul myMaxIt) = false then
    begin


      isExecutedOneTimeOrMoreList := List.append  !isExecutedOneTimeOrMoreList [(num,true)]
    end
    else isExecutedOneTimeOrMoreList := List.append !isExecutedOneTimeOrMoreList  [(num,false)];


     let mymax = !borneMaxAux in
      let nb = expressionEvalueeToExpression (evalexpression  (Diff (mymax, ConstInt ("1"))))  in
      let exp_nb = (BINARY(SUB, (expVaToExp new_expmax), CONSTANT (CONST_INT "1"))) in
    let assignVB =
      if iAmExact   then   ASSIGN_SIMPLE (myVar, EXP(nb))
      else
      begin
        if  estDefExp myMaxIt &&  (*estNul myMaxIt= false*) getDefValue myMaxIt > 0.0    then
        begin
          if (not (estNothing (EXP nb))) then
            ASSIGN_SIMPLE (myVar,  EXP(CALL (VARIABLE("SET") ,  List.append [CONSTANT (CONST_INT "-1")] [nb] )) )
          else
           ASSIGN_SIMPLE (myVar,  EXP(CALL (VARIABLE("SET") ,  List.append [CONSTANT (CONST_INT "-1")] [exp_nb] )) )
        end
        else  ASSIGN_SIMPLE (varBoucleIfN, EXP(CONSTANT  (CONST_INT "-1"))) ;
        end in
      listeVBDEP := rond !listeVBDEP  [assignVB];
  end;
   ()
  | _->()


let rec afficherNidUML nnE  liste tab  fichier ligne lt lf(result:Listener.t) : Listener.t =
  match nnE.idBoucleN with
  TBOUCLE(num, appel, _,_,_,_,_,ficaux,ligaux) ->
      (*Printf.printf "\n\n\nAFFICHE boucle %d\n"num;*)
    let estNulEngPred = !estNulEng in
    let exactEng = !isExactEng in
    let borneEng = !valeurEng in
      (* let (fic,lig)=getAssosIdLoopRef num in*)
    let (fic,lig)=(fichier , ligne) in
    let nia =   nnE.sensVariation   in
    let nm =  nnE.maxUneIt  in
    isProd := false;
    hasSETCALL:=false;
    let c1 = calculer  nm   nia [] 1 in
    let hasinit = !hasSETCALL in
    curassocnamesetList := [];
    if estDefExp c1 = false then begin
      let varOfExp = listeDesVarsDeExpSeules (expVaToExp nm) in
      List.iter(fun n ->
          if getNbIt n (expVaToExp nm) > 1 then
          begin
            if existeAffectationVarListe n !listeVBDEP then
            begin
              let assign =  ro n  !listeVBDEP in
              match assign with ASSIGN_SIMPLE (_, exp) ->
                let (boolean , e1, e2) = isSetExpression (expVaToExp exp) in
                if boolean then curassocnamesetList :=
                   List.append [n, ASSIGN_SIMPLE (n, EXP(e1)),ASSIGN_SIMPLE (n, EXP(e2))] !curassocnamesetList;
              |_-> ()
            end

          end

        )varOfExp
    end;
    let new_expmax =
      if !curassocnamesetList = [] then
        (applyif nm !listeVBDEP)
      else begin
            let (_, a1, a2) =List.hd !curassocnamesetList in
            let (eva1, eva2) =(applyif ( applyif nm [a1])  !listeVBDEP , applyif ( applyif nm [a2])  !listeVBDEP ) in
            EXP(CALL (VARIABLE("MAXIMUM") , (List.append [expVaToExp eva1] [expVaToExp eva2] )) )
      end
    in
    hasSETCALL:=false;
    let (expmax1, reseval) = (
    if estDefExp c1 = false then begin  (calculer new_expmax  nia [] 1, false) end
    else (c1,true) ) in (* valeur max apres propagation*)
  let hass = !hasSETCALL in
  let myMaxIt = if estNulEngPred =false then  expmax1 else  ConstInt("0") in

  let ne =  nnE.expressionBorneToutesIt  in

  let new_exptt =(* (applyStoreVA ne !listeVB) in (* expression total apres propagation*)*) applyif ne !listeVBDEP in


  let c2 = calculer  nnE.expressionBorneToutesIt   nia [] 1 in
  let exptt1 =
  ( if estDefExp c2 =false|| reseval = false then calculer new_exptt  nia [] 1 else c2 ) in
    (* valeur total apres propagation*)

  let borne =
       if  (estNulEngPred =true) || (estDefExp myMaxIt && getDefValue myMaxIt <= 0.0  )   then ConstInt("0")
        else   exptt1 in

    if estDefExp borne =false  then
    begin
      isProd := true;
      if (estDefExp borneEng ) && (estDefExp myMaxIt ) then
      begin
        borneAux := evalexpression (Prod (borneEng, myMaxIt)) ;isProd := true;
      end
      else
        borneAux := NOCOMP
    end
    else
    begin
      borneAux := (if estDefExp myMaxIt &&  (*estNul myMaxIt= false*) getDefValue myMaxIt > 0.0 then
            begin

              let prod = evalexpression (Prod (borneEng, myMaxIt)) in
              if estDefExp prod then
                if estPositif (evalexpression (Diff( prod, borne))) then borne else begin isProd := true; prod end
              else borne
            end
            else ConstInt("0"))
    end;

    let varBoucleIfN =  Printf.sprintf "%s-%d" "bIt" num in
    listeVB := listeSansAffectVar !listeVB varBoucleIfN;
    listeVBDEP := listeSansAffectVar !listeVBDEP varBoucleIfN;
      estNulEng := false;

    if estDefExp !borneAux && estNul !borneAux then
    begin
      borneMaxAux:= (ConstInt("0"));
      setAssosBoucleIdMaxIfSupOldMax num (EVALEXP(ConstInt("0"))); (*attention on peut avoir plusieurs fois la meme variable de boucle donc ici on ajoute dans false on peut supprimer*)
      estNulEng := true
    end
    else
    begin
      if estDefExp myMaxIt then
      begin
        borneMaxAux:= myMaxIt ;
        setAssosBoucleIdMaxIfSupOldMax num (EVALEXP(myMaxIt));
      end
      else
      begin
        let maxp = if existeNid num then (rechercheNid num).infoNid.expressionBorne else NOTHING in
       (* let resaux = calculer (EXP(maxp))  nia [] 1 in*)
        borneMaxAux :=(* if estDefExp resaux then
              begin
                setAssosBoucleIdMaxIfSupOldMax num (EVALEXP (resaux));
                resaux
              end
              else *)
              begin
                setAssosBoucleIdMaxIfSupOldMax num (EXPMAX [maxp]);
                NOCOMP
              end
      end
    end;

    valeurEng := !borneAux;
(* ajouter SET*)
    let (iAmExact, myVar,myBorne)=
     if existeNid num then
      (hasinit= false&&hass=false &&(rechercheNid num).infoNid.isExactExp && (!isProd = false) &&
    (hasSygmaExpVA ne = false) && !isExactEng && (nnE.isIntoIf = false), varBoucleIfN, !borneAux)
     else (false,   varBoucleIfN, !borneAux)
     in

  let ett = if nnE.isIntoIf then if !borneAux = NOCOMP then NOTHING else (expVaToExp new_exptt) else (expVaToExp new_exptt) in
  let em = if nnE.isIntoIf then if !borneAux = NOCOMP then NOTHING else (expVaToExp new_expmax) else (expVaToExp new_expmax) in

    let iAmNotNul = (!estNulEng = false) in
    if iAmExact = false then isExactEng := false else isExactEng := true;

    let mymax = !borneMaxAux in
    let einit = if existeNid num then  ((rechercheNid num).infoNid.expressionBorne) else NOTHING in
    let result = Listener.onLoop result num lig fic iAmExact  !borneMaxAux !borneAux  em ett einit nia lt lf in

    let nb = expressionEvalueeToExpression (evalexpression  (Diff (mymax, ConstInt ("1"))))  in
    let exp_nb = (BINARY(SUB, (expVaToExp new_expmax), CONSTANT (CONST_INT "1"))) in

  let assignVB =
    if iAmExact   then   ASSIGN_SIMPLE (myVar, EXP(nb))
    else
    begin
      if  iAmNotNul  then
      begin
        if (not (estNothing (EXP nb))) then
          ASSIGN_SIMPLE (myVar,  EXP(CALL (VARIABLE("SET") ,  List.append [CONSTANT (CONST_INT "-1")] [nb] )) )
        else
         ASSIGN_SIMPLE (myVar,  EXP(CALL (VARIABLE("SET") ,  List.append [CONSTANT (CONST_INT "-1")]
            [exp_nb] )) )


      end
      else  ASSIGN_SIMPLE (varBoucleIfN, EXP(CONSTANT  (CONST_INT "-1"))) ;
      end in

     listeVBDEP := rond !listeVBDEP  [assignVB];
     let result = afficherCorpsUML liste  (tab+5) true result in
     let result = Listener.onLoopEnd (result:Listener.t) in

    isExactEng := exactEng;
    estNulEng := estNulEngPred;
    valeurEng := borneEng;
(*Printf.printf "\n\n\nFIN AFFICHE boucle %d \n"num;*)
    result
  | _-> Listener.null



and afficherUnAppelUML exp l tab numCall isExe isInLoop fichier ligne lt lf (result:Listener.t) : Listener.t =
  (*let _ = Printf.printf "Go in afficherUnAppelUML, len(l)=%d.\n" (List.length l) in*)
  match exp with
    EXP(appel)->
      let nomFonction =
        (match appel with
          CALL (exp,_) ->
            (match exp with
              VARIABLE (nomFct) -> nomFct
            | _ -> "")
        | _ -> "")
      in
      afficherInfoFonctionUML nomFonction l (tab) numCall isExe isInLoop fichier ligne lt lf result
  | _-> Printf.printf "MULTIPLE\n";
  Listener.null



(* if printer*)
and afficherUnCallIfUML variable expr listeT listeF e1 tab isExe isInloop fichier ligne lt lf infothen infoElse asL (result:Listener.t) : Listener.t =
  let (isexet, isexee) =   ifCaseDecode  isExe in
   
  (* only 1 the then branch is executed, only the else branch is executed,
  3 we do not know the executed branch , neither the then or the else is executed*)
  let result = Listener.onIf result variable ligne fichier isInloop isExe expr e1 lt lf asL in
(*print THEN*)

 
  let result = Listener.onIfT result isexet infothen  in
  let result = (afficherCorpsUML listeT (tab+5) isexet  (result:Listener.t))  in
  let result = Listener.onIfTEnd (result:Listener.t) in
(*print ELSE*)
   let result = Listener.onIfF (result:Listener.t) isexee infoElse in
  let result = (afficherCorpsUML listeF (tab+5) isexee (result:Listener.t))  in
  let result = Listener.onIfFEnd (result:Listener.t) in
  Listener.onIfEnd result

and afficherInfoFonctionUML nom corps tab numCall isExe isInLoop fichier ligne lt lf (result:Listener.t) : Listener.t  =
  let isExtern = (not (existeFonctionParNom nom doc)) in
 (* let (fichier , ligne ) = getAssosIdCallFunctionRef numCall in*)
  let result = Listener.onCall result nom numCall ligne fichier isInLoop isExe isExtern  lt lf in
  (*let _ = Printf.printf "Go in afficherInfoFonctionUML, len(corps)=%d.\n" (List.length corps) in*)
  let result = if (not isExtern) then (afficherCorpsUML corps (tab+5) true result) else result in
(*Printf.printf "BLABLA line=\"%d\" source=\"%s\" extern=\"true\">\n" ligne fichier ;*)
  Listener.onReturn result



and afficherCorpsUML lboua tab isexecuted (result:Listener.t) : Listener.t =
          (*let _ = Printf.printf "Go in afficherCorpsUML, len(lboua)=%d.\n" (List.length lboua) in*)
  List.fold_left(
    fun result unboua ->
      match unboua with
        BOUCLEEVAL (nid, ty, cont, lt, lf) ->
          (match ty with
            TBOUCLE (_, _,_,_,_,_,_,fichier,ligne) ->
              afficherNidUML nid cont tab fichier ligne lt lf result
          | _-> afficherNidUML nid  cont tab  "" 0  lt lf result)

      | APPELEVAL (ty, expr,liste, lt, lf) ->
          let (numCall, isExe, isInLoop,fichier,ligne) =
            (match ty with
              TFONCTION(nom, appele,_,_,_, _,_,_,e,b,fichier,ligne) ->
                (appele, e, b,fichier,ligne)
              | _ -> (0, true, false,"",0)) in
                if isInLoop = false then begin
                  valeurEng := NOCOMP ;
                  borneAux := NOCOMP ;
                  estNulEng := false;
                  isExactEng := true;
                end;
                afficherUnAppelUML expr liste tab numCall isExe isInLoop fichier ligne lt lf result
      | IFEVAL (expr, listeT, listeF, e1, fichier, ligne, intoloop, isExe, variable, lt, lf, infothen, infoElse, asL) ->
		let resexe = if isexecuted =false then 4 else isExe in
 
        (*let c1 = calculer  expr    !infoaffichNull [] 1 in*) 
        (*isin loop i do not know if i have to add this info*)
        (* jz: let c1 = simpleCalculerConstVal (EXP(expr)) !infoaffichNull [] 1 in*)
      afficherUnCallIfUML variable expr listeT listeF e1 tab resexe intoloop fichier ligne  lt lf  infothen infoElse asL result
  ) result lboua


let rec isExecutedTrue ltrue contexte  affiche globales=
  if ltrue = [] then begin
    (*Printf.printf " liste isexecuted vide pour true \n" ;*)
    true
  end else begin
    (*Printf.printf "isExecutedTrue liste des variables true :\n"; List.iter (fun e-> Printf.printf "%s "e) ltrue;*)
    if !hasCondListFile_name && existeAffectationVarListe (List.hd ltrue) !condAnnotated then begin
      let affect = rechercheAffectVDsListeAS  (List.hd ltrue)   !condAnnotated in
      let cond = calculer  affect !infoaffichNull  [] 1 in
      if affiche && !isPartialisation = false && !trace then begin
        Printf.printf "\n%s(expression) : \n" (List.hd ltrue) ;print_expVA affect;flush(); space(); new_line(); space(); new_line() ;flush();
        Printf.printf "\n%s(value) :\n" (List.hd ltrue) ; print_expTerm  cond;  space(); new_line() ;flush(); space(); new_line() ;flush()
      end;
      match cond with
        Boolean(false)
      | ConstInt("0") ->
          false
      | Boolean(true)
      | ConstInt("1")
      | _ ->
          isExecutedTrue (List.tl ltrue) contexte affiche globales
      end else begin
        if existeAffectationVarListe (List.hd ltrue) contexte then begin
          let affect = (applyStoreVA (rechercheAffectVDsListeAS  (List.hd ltrue) contexte) globales) in
          (*Printf.printf "CALCUL affect true %s\n" (List.hd ltrue) ; print_expVA affect;flush(); space(); new_line();*)
          let cond = calculer  affect !infoaffichNull  [] 1 in
          if affiche && !isPartialisation = false && !trace then begin
            (*print_expTerm  cond; flush(); space();new_line(); *)
            Printf.printf "\n%s(expression) : \n" (List.hd ltrue) ;
            print_expVA affect;flush(); space(); new_line(); space(); new_line() ;flush();
            Printf.printf "\n%s(value) :\n" (List.hd ltrue) ; print_expTerm  cond;  space(); new_line() ;flush(); space(); new_line() ;flush();
          end;
          (* print_expTerm  cond;flush(); space(); new_line();  *)
          match cond with
            Boolean(false)
          | ConstInt("0")-> (*Printf.printf " non execute %s" (List.hd ltrue);*)
              false
          | Boolean(true)
          | ConstInt("1")
          | _ -> isExecutedTrue (List.tl ltrue) contexte affiche globales
        end else
          true
      end
  end

let rec isExecutedFalse lfalse contexte affiche globales=
  if lfalse = [] then begin (*Printf.printf " liste isexecuted vide pour false \n" ;*)true end
  else
  begin
(*Printf.printf "isExecutedFalse liste des variables false :\n"; List.iter (fun e-> Printf.printf "%s "e) lfalse;*)
      if !hasCondListFile_name && existeAffectationVarListe (List.hd lfalse) !condAnnotated then
      begin

        let affect = rechercheAffectVDsListeAS  (List.hd lfalse)   !condAnnotated in
        let cond = calculer  affect !infoaffichNull  [] 1 in
        if affiche && !isPartialisation = false && !trace then
            begin
                (*print_expTerm  cond; flush(); space();new_line(); *)
                Printf.printf "\n%s(expression) : \n" (List.hd lfalse);
                print_expVA affect;flush(); space(); new_line(); space(); new_line() ;flush();
                Printf.printf "\n%s(value) :\n" (List.hd lfalse) ; print_expTerm  cond;  space(); new_line() ;flush(); space(); new_line() ;flush();
            end;

        match cond with
            Boolean(true) | ConstInt("1")->  false
            | Boolean(false)  | ConstInt("0") |_-> isExecutedFalse (List.tl lfalse) contexte affiche globales
      end
      else
      begin

          if existeAffectationVarListe (List.hd lfalse) contexte then
          begin
        (*Printf.printf "existe \n";*)

          let affect = (applyStoreVA (rechercheAffectVDsListeAS  (List.hd lfalse) contexte) globales) in
          (*  Printf.printf "CALCUL affect false %s\n" (List.hd lfalse) ; print_expVA affect; flush(); space();new_line();print_expVA affect;flush(); space(); new_line();*)
            let cond = calculer   affect  !infoaffichNull  [] 1 in
            if affiche && !isPartialisation = false && !trace  then
            begin
                (*print_expTerm  cond; flush(); space();new_line(); space(); new_line() ;flush(); *)
                Printf.printf "%s(expression) : "  (List.hd lfalse) ; print_expVA affect; flush(); space();new_line();flush(); space(); new_line();flush(); space(); new_line();
                Printf.printf "\n%s(value) : \n" (List.hd lfalse) ; print_expTerm  cond;  space(); new_line() ;flush();flush(); space(); new_line();
            end;
            match cond with
            Boolean(true) | ConstInt("1")-> (*Printf.printf "isExecutedFalse non execute %s" (List.hd lfalse) ;*) false
            | Boolean(false)  | ConstInt("0") |_-> isExecutedFalse (List.tl lfalse) contexte affiche globales
          end
          else begin (*Printf.printf " liste affect non trouve sur n autre chemin\n" ;*) true end
      end

end

let isExecuted ltrue lfalse contexte appel globales affiche=

 (* Printf.printf "isExecuted : traiterboucleinterne contexteAux : \n"; afficherListeAS contexte; Printf.printf "FIN CONTEXTE \n";
  Printf.printf "isExecuted : traiterboucleinterne appel : \n"; afficherListeAS appel; Printf.printf "FIN appel \n";
  Printf.printf "isExecuted : traiterboucleinterne globales : \n"; afficherListeAS globales; Printf.printf "FIN CONTEXTE \n";*)
  let listeP = !listeASCourant in
  let res = (rond appel contexte) in
 (* if affiche && !isPartialisation = false && !trace then
  begin
   if ltrue <> [] then ( Printf.printf "isExecuted : list of true conditions variables\n"; List.iter (fun e-> Printf.printf "%s "e) ltrue);
   if lfalse <> [] then (  Printf.printf "isExecuted : list of false conditions variables\n"; List.iter (fun e-> Printf.printf "%s "e) lfalse)
  end;*)
 (*afficherListeAS( res);new_line () ;*)
  let valeur = if ltrue = [] && lfalse = [] then true else (isExecutedTrue ltrue res affiche globales) && (isExecutedFalse lfalse res affiche globales)   in
  listeASCourant := listeP;
  valeur

let isExeBoucle =ref true
let listeInstNonexe = ref []

(*let funcContext = ref []*)

let onlyNotLoopVar liste =
List.filter (
fun name -> if (String.length name > 4) then
    if (String.sub name  0 4) = "bIt_" then false else true else true)liste



let rec listBeforeCall listb name id =
if listb = [] then []
else
begin
  let (n,c,_,_) = List.hd listb in
  if n = name && id = c then List.tl listb
  else  listBeforeCall (List.tl listb) name id
end


let rec isExecutedFunction   exeassign =

if exeassign = [] then true
else
begin
  match (List.hd exeassign) with
    ASSIGN_SIMPLE (n, exp) ->

      let (var,istrue) = if (String.length n > 3) then

          if  (String.sub n  0 3) = "ET-" then (n,true) else  (n,false)
          else (n, true)
        in
      let first =   calculer   exp  !infoaffichNull  [] 1  in
      let firstValue =
            (match first with
              Boolean(false)  | ConstInt("0")-> (*Printf.printf " non execute %s" (List.hd ltrue);*)if istrue then false else true
            | Boolean(true)   | ConstInt("1")-> if istrue then true else false
            |_-> true)

        in
          if List.tl exeassign = [] then firstValue else  firstValue &&  isExecutedFunction   (List.tl exeassign)

     | _-> true
end



let getMaxTotalArraySizeDep finstanciedMax finstanciedTotal contexte globales =
let (rep, var,isvar,expression) = hasPtrArrayBoundCondition (EXP(finstanciedMax)) in
        let (nmax,ntotal) =
                if rep = true then
                begin
                  if isvar then
                  begin
                    (*Printf.printf "ON A COMPOSE la boucle ID %u , array \n" id ;*)
                    let av = if (existeAffectationVarListe var contexte) then
                          applyStoreVA(rechercheAffectVDsListeAS  var contexte)globales
                        else rechercheAffectVDsListeAS  var globales in


                    let newe = expVaToExp( av )  in
                    let (tab1,lidx1, e1) =getArrayNameOfexp newe in
                    if tab1 != "" then
                    begin
                      let nsup = changeExpInto0 e1 (  finstanciedMax ) in
                      let ntot = changeExpInto0 e1 (  finstanciedTotal ) in
                      let size = getAssosArrayIDsize tab1 in
                      let varName =  Printf.sprintf "%s_%s" "getvarTailleTabMax" var in
                      (match size with
                        NOSIZE -> (finstanciedMax,finstanciedTotal)
                        | SARRAY (v) ->
                          let arraySize = (CONSTANT (CONST_INT (Printf.sprintf "%d" v) )) in
                          (remplacerValPar  varName arraySize nsup,remplacerValPar  varName arraySize ntot)
                        | MSARRAY (lsize) ->
                          let tsize = expressionEvalueeToExpression (prodListSize lsize) in
                           (remplacerValPar  varName tsize  nsup,remplacerValPar  varName tsize ntot))
                    end
                    else (finstanciedMax,finstanciedTotal)
                  end
                  else
                  begin
                    let newe = expVaToExp( applyStoreVA(applyStoreVA  (EXP(expression)) contexte)globales)  in
                    (* Printf.printf "ON A COMPOSE la boucle ID %u , array indirect\n" id ;*)
                    let (tab1,lidx1, e1) =getArrayNameOfexp newe in
                    if tab1 != "" then
                    begin
                      let nune = changeExpInto0 e1 (  finstanciedMax) in
                      let ntot = changeExpInto0 e1 (  finstanciedTotal ) in
                      let size = getAssosArrayIDsize tab1 in

                      (match size with
                        NOSIZE ->  (finstanciedMax,finstanciedTotal)
                        | SARRAY (v) ->
                          let arraySize = (CONSTANT (CONST_INT (Printf.sprintf "%d" v) )) in
                          (remplacergetvarTailleTabMaxFctPar  nune arraySize ,remplacergetvarTailleTabMaxFctPar  ntot arraySize )
                        | MSARRAY (lsize) ->
                          let tsize = expressionEvalueeToExpression (prodListSize lsize) in
                          (remplacergetvarTailleTabMaxFctPar  nune tsize,remplacergetvarTailleTabMaxFctPar  ntot tsize   ))
                    end

                    else  (finstanciedMax,finstanciedTotal)


                  end
                end
                else  (finstanciedMax,finstanciedTotal)  in

(nmax,ntotal)


let aslAux = ref []






(*--------------------------------traiterBouclesInternes*)

let rec traiterBouclesInternes  nT (*tete nid contenant bi*)  nEC (*noeud englobantcourant *)
                idEng (*id noeud englobant  ou stopper *)
                id (*courant a evaluer bi*)  tN
                appel (*contexte appel pour le moment fonction puis doc *)
                listeEng typeE numAp max isExeE lt lf borne   sansP globales maxinit varLoop direction idpred lcond iscompo =
  (* il faut evaluer le nombre total d'iteration  de la boucle courante n*)
  (*  Pour toutes les boucles bi englobantes de Bj a partir de la boucle immediatement englobante de Bj
  jusqu'a la mere du nid faire*) (*donc en remonte de recursivite*)

  let info = (getBoucleInfoB (nEC.infoNid.laBoucle)) in
  let nomE = info.identifiant  in
  let saBENG = (if aBoucleEnglobante info then info.nomEnglobante else 0) in
  estDansBoucle := true;
  estDansBoucleE :=!estDansBoucle;
 if !vDEBUG then 
  begin
    (*afficherListeAS( appel);new_line () ;*)
    Printf.eprintf "1 traiterBouclesInternes num %d nom eng %d ou stopper %d sa eng %d tete nid %d ispred %d\n" id  nomE idEng saBENG (getBoucleIdB nT.infoNid.laBoucle) idpred ;

    (* afficheNidEval !docEvalue.maListeNidEval; *)
  (*  Printf.printf "FIN NID ENG COURANT \n"*)
  end;
  let conte = match typeE with  TBOUCLE(n,ap,_,_,_,_,_,_,_) -> ap |_-> 0 in
  
  if nomE = idEng (*|| (nomE = (getBoucleIdB nT.infoNid.laBoucle) && nomE != 0)*) then
  begin
    let fini = ((nomE = idEng) && (nomE =  (getBoucleIdB nT.infoNid.laBoucle)))  in
    if fini then estDansBoucleLast := true else   estDansBoucleLast:= false;

   (* if nomE = 0 then Printf.printf "fin de la remontée\n";*)
    let info = (getBoucleInfoB nEC.infoNid.laBoucle) in
    let nbEngl =getNombreIt (nEC.infoNid.expressionBorne)
          info.conditionConstante info.typeBoucle  info.conditionI  info.conditionMultiple  [] info.estPlus
          info.infoVariation  nEC.varDeBoucleNid []  in

   (*print_expVA (EXP(info.conditionI)); flush(); space(); new_line ();*)

    aslAux := [];
   if !vDEBUG then  
    begin
      Printf.eprintf "2 traiterBouclesInternes num %d nom eng %d \n"  id nomE ;
      (*afficherNid nEC;*) Printf.eprintf "FIN NID ENG COURANT \n";

      if lt <> [] then begin Printf.eprintf "IF true :\n"; List.iter (fun e-> Printf.eprintf "%s "e) lt end;

      if lf <> [] then begin Printf.eprintf "IF false :\n"; List.iter (fun e-> Printf.eprintf "%s "e) lf end
    end;
    (*Soit VDij l'ensemble des variables modifiées par Bi dont dépend la borne TN *)
    let tni = rechercheNbTotalIti nomE numAp !docEvalue.maListeNidEval in
      
    (*Printf.printf "av traiterBouclesInternes num %d nom eng %d \n"  id nomE ;*)
    let(varTN,varmax,varTni,l, output) =   creerLesAffect tN max tni id conte in
    isExeBoucle := isExeE;
    (*if !isExeBoucle then  Printf.printf "la boucle englobante est exécutée\n"
    else  Printf.printf "la boucle englobante n'est pas exécutée\n";*)
    let varIfN =  Printf.sprintf "%s-%d" "TWH" id  in
    let (nlt,nlf,exeloop) = if id = idpred then   creerLesAffectEXECUTED lt lf "Loop" id idEng !cptFunctiontestIntoLoop else (lt,lf, lcond) in
    let (lesAsf, intofunction,newlt, newlf,intocondcall) =
    ( if (!dernierAppelFct <> !predDernierAppelFct)
      then
      begin
        match !dernierAppelFct with
        TFONCTION (_, _,_,_, _,_,_,_,_,_,_,_) ->
          (*let numB  = id in*)
          let (pred, trouve) =
          listejusquafonction (List.rev listeEng) idpred !dernierAppelFct in
          let calllist = (reecrireCallsInLoop  nEC.varDeBoucleNid nEC.lesAffectationsBNid ) in

          (match pred with
            TFONCTION (nomf, numF,corps,listeInputInst, contexteAvantAppel,appelF,lFt,lFf,_,_,_,_) ->
        (*  Printf.printf"traiterboucleinterne Dans evaluation de la fonction...%s %d %s \n "nomf id nEC.varDeBoucleNid ;*)
        (*    Printf.printf"traiterboucleinterne Dans evaluation de la fonction...%s %d %s \n "nomf id nEC.varDeBoucleNid ;*)
            if appelF = [] then (Printf.printf "ces appel vide\n"; ([], true,lt,lf,!isExecutedOneTimeOrMore))
            else
            begin
              (match List.hd appelF with
              APPEL (i,e,nomFonc,s,CORPS c,v,_,_) ->
                let ainserer = listeSAUFIDB  (reecrireCallsInLoop  nEC.varDeBoucleNid corps ) idpred l in
                (*Printf.printf "ces as\n";*)
                (*afficherLesAffectations( ainserer);new_line () ;*)
                (*Printf.printf "ces as\n";*)

                (*afficherLesAffectations( ainserer);new_line () ;*)
                (*Printf.printf "ces as\n";*)
                (*Printf.printf "evalUneBoucleOuAppel  appel FONCTION %s:\n appel : call %d\n " nomf numF;*)


                let listBefore = List.rev (listBeforeCall !listeAppels nomf numF ) in
                listBeforeFct := listBefore;
                (*Printf.printf "listBefore\n";
                List.iter (fun f-> let (name, num, _,_) =f in Printf.printf"name %s num %d \n" name num)listBefore;*)

                (*Printf.printf"traiterboucleinterne   fonction...%s %d %s \n "nomf id nEC.varDeBoucleNid ;Printf.printf "listBefore\n";
                List.iter (fun (nom,nim,_,_)-> Printf.printf " %s %d\n"nom nim;)listBefore;*)

                let nb = numberOfCall listBefore nomf numF in


                  let listtestfct =   creerLesAffectEXECUTEDFct ([(nomf, numF,lFt,lFf)] ) in


                (*let (newlt, newlf,exeassign) =creerLesAffectEXECUTED lFt lFf  nomf numF id !cptFunctiontestIntoLoop in*)
                let aSC =  evalSIDA nT nEC calllist numF  (List.append   ainserer exeloop)
                  (List.append listtestfct listeInputInst) listeEng nb listBefore   in
                (*Printf.printf "ces as 2\n";*)
                let ifassign = filterIF aSC in (*afficherListeAS ifassign;  *)
                let isExecutedF = if ifassign = [] then true    else    isExecutedFunction   ifassign  in
                  (*let (before, _) =    roavant aSC assignBefore  [] in*)

                (*afficherListeAS( aSC);new_line () ;*)
                (*Printf.printf"Fin traiterboucleinterne Dans evaluation de la fonction...%s %d %s \n "nomf id nEC.varDeBoucleNid ;*)
                (*if isExecutedF = false then  (Printf.printf " Into loop function %s appel %d not executed \n" nomf numF;
                  (*listeInstNonexe := List.append [pred] !listeInstNonexe*));*)

                isExeBoucle := isExeE && isExecutedF;
                (*              Printf.printf "ces as 3\n";*)
                (aSC, nb > 0, nlt,nlf,!isExecutedOneTimeOrMore && lFt = [] && lFf = [])
              | _-> ([], true,lt,lf,!isExecutedOneTimeOrMore))
            end
            |_->([], true,lt,lf,!isExecutedOneTimeOrMore))
        |_->(*Printf.printf "lesAS NON par fonction valeur\n"; *)  (lesVardeiSansj nT nEC idpred    (List.append   l exeloop) , false,lt,lf,!isExecutedOneTimeOrMore)
      end
      else begin (*Printf.printf "cas3\n"; *) (lesVardeiSansj nT nEC idpred   (List.append   l exeloop) , false,lt,lf,!isExecutedOneTimeOrMore)end
    )in

   (* isExecutedOneTimeOrMore := intocondcall;*)
    let lesAs =  lesAsf in
    let ii = (nEC.varDeBoucleNid) in
    let vij =  rechercheLesVar  lesAs [] in
    let new_cond = filterIF lesAs in

    let resExptN  =    rechercheAffectVDsListeAS varTN lesAs in
    (*Printf.printf "varTN varmax %s %s\n" varTN varmax;

    print_expVA resExptN; new_line();Printf.printf "CALCUL DE AS fin\n";*)
    let recExptMax = rechercheAffectVDsListeAS  varmax  lesAs in
    let restest = rechercheAffectVDsListeAS  varIfN  lesAs in
    

  isIntoIfLoop := false;

  isEnd := false;
  isEndNONZERO := false;
  let resauxmax = calculer max  !infoaffichNull [] 1 in
  isEnd := if estDefExp resauxmax then if  estNul resauxmax then true else false else false ;

  (*Printf.printf "av traiterBouclesInternes num %d nom eng %d AVANT\n"  id nomE ;*)
  resAuxTN :=
  (  match resExptN with
    MULTIPLE ->(* Printf.printf"resAuxTN MULTIPLEdef\n";*)
      if sansP = false then
      begin
        if estDefExp resauxmax then
        begin
          isIntoIfLoop := true ;
          EXP(BINARY(MUL,expVaToExp borne,(expressionEvalueeToExpression resauxmax)))
        end
        else (*if estDefExp resauxmax2  then
            EXP(BINARY(MUL,expVaToExp borne,expVaToExp max))
           else*) EXP(NOTHING)
      end
      else MULTIPLE
    |EXP  ( exptN) ->
      if sansP = false then
      begin
      (*  Printf.printf"resAuxTN def\n";*)
        (*let resExptni  =  rechercheAffectVDsListeAS  varTni lesAs in*)
        (*Printf.printf "CALCUL DE TNi num %d nom eng %d\n" (getBoucleIdB n.infoNid.laBoucle) nomE ;*)
        (*print_expVA resExptni; new_line();Printf.printf "CALCUL DE AS fin\n";*)
        let listeDesVar = listeDesVarsDeExpSeules exptN in
        (*let listeDesVarSansBit = onlyNotLoopVar listeDesVar in*)
        (*if listeDesVarSansBit = [] then Printf.printf "on peut arreter indépendant des autres b eng\n"
        else   Printf.printf "  peut etre dépendant des autres b eng\n";*)


        let vdij = ( intersection listeDesVar  ( union [ii]  vij)) in

        let estIndependantTN  =   if vdij = []
                      then true
                      else (  let isindependent = independantDesVarsDeExpSeules exptN lesAs vdij in
                          (*if isindependent then Printf.printf "exptN is independant \n"
                          else Printf.printf "exptN is dependant \n";*) (*false*)isindependent ) in

        (* idenpendant*)
        if estIndependantTN then
        begin
          if !vDEBUG then  Printf.eprintf "intersection vide\n";
          (* si les deux contiennent une même variable max * max ici ou dans evaluation ???*)
           (match nbEngl with
            MULTIPLE->(*Printf.printf"borne  multiple\n";*)  MULTIPLE
            |EXP(exptni)->
              if estDefExp resauxmax then
              begin
                (*Printf.printf"borne  MUL 1\n";*)
                isEndNONZERO := true;
                EXP(BINARY(MUL,expVaToExp borne,(expressionEvalueeToExpression resauxmax)))
              end
              else
                  if estNothing nbEngl || estNothing (EXP(exptN)) then
                  begin (*Printf.printf"borne  NOTHING 1\n";*) EXP(NOTHING) end
                  else begin (*Printf.printf"borne  MUL 2\n";*) EXP(BINARY(MUL,exptni,exptN)) end)
        end
        else
        begin
          (*tant que vD != [ii] faire begin pour toute variable x appartenant à vD faire
          remplacer dans tN x
           par l'expression qui lui est associée dans la liste des vij  end *)
          (*remplacerVar tN vD vij;*)
          (* avant il faut modifiee lesAs mais uniquement pour les variables*)
          (*let lesAs = (if vdij <> [ii] then majAs lesAs vdij ii else lesAs) in*)
          if !vDEBUG then Printf.eprintf("!!!Depend de la boucle englobante sans var \n");
          match nbEngl with
          MULTIPLE -> (*Printf.printf"borne  multiple 2\n";*)MULTIPLE;
          | EXP(exptni) ->
            begin
              (* si tN contient lui meme un SYGMA de i il faut composer *)
              (*Printf.printf"borne  SYGMA 2\n";*)



              if estNothing nbEngl || estNothing (EXP(exptN)) then
              begin (*Printf.printf"borne  NOTHING 2\n";*) EXP(NOTHING) end
              else
                EXP(CALL(VARIABLE("SYGMA") ,
                  (List.append
                    (List.append [VARIABLE (ii)]
                        [BINARY(SUB, exptni,CONSTANT (CONST_INT "1"))])  [ exptN])));

            end
            (* remarque la seule variable modifiée par la boucle englobante courante dont
             doit dépendre TN à ce stade est ii *)
        end
      end
      else resExptN) ;
    maxAuxTN :=
    ( if sansP = false then
      begin
        match recExptMax with
        MULTIPLE->
          if estDefExp resauxmax then
          begin isIntoIfLoop := true ;
             EXP(expressionEvalueeToExpression resauxmax)
          end
          else  EXP(NOTHING)
          | EXP (e)->(*Printf.printf"resAuxTN def\n";*)

          let le =listeDesVarsDeExpSeules e in
          let listVarOfExp = intersection  le (union [ii] vij) in
          (*let (unique, var) =  if le = [] then (false,"") else if List.tl le = [] then (true, List.hd le) else (false,"") in

          let ne = if unique && simplifierAffineCte var recExptMax then remplacerValPar0  var e else e in*)

          let estIndependantE  =  if listVarOfExp = []
                      then true
                      else (  let isindependent = independantDesVarsDeExpSeules e lesAs listVarOfExp in
                          (*if isindependent then Printf.printf "e is independant \n"
                          else Printf.printf "e is dependant \n";*) (*false*)isindependent ) in

          if estIndependantE then
          begin (* Printf.printf "la borne max ne contient pas de var fct de ii :%s boucle %d\n" ii id;*)
            (*if estDefExp resauxmax then EXP(expressionEvalueeToExpression resauxmax)
            else
            begin
              let res = calculer recExptMax  !infoaffichNull [] 1 in
              if estDefExp res then EXP(expressionEvalueeToExpression res)
              else *) EXP (e)
          (*  end*)

          end
          else
          begin

            (match nbEngl with
            MULTIPLE ->   MULTIPLE
            | EXP(exp) ->
              if estNothing nbEngl || estNothing (EXP(e)) then  EXP(NOTHING)
              else
                EXP(CALL(VARIABLE("MAX") , (List.append (List.append
                  [VARIABLE (ii)]
                  [BINARY(SUB,  exp, (CONSTANT (CONST_INT "1")))])  [e])))
                  )
          end
        end
        else recExptMax
      );

       (* VOIR ????*)
      if sansP then  (match !maxAuxTN with EXP(e) -> resAuxTN := if estNothing (borne) then  borne else EXP(BINARY(MUL,expVaToExp borne,e))|_->());

        (*Printf.printf"appel rec de traiterBouclesInternes   \n";*)
        (*Printf.printf "1 traiterBouclesInternes %d nom eng %d ou stopper %d sa eng %d tete nid %d\n" id nomE idEng saBENG (getBoucleIdB nT.infoNid.laBoucle);
        Printf.printf"traiter calcul MAX pour %s =\n" ii; print_expVA !maxAuxTN; new_line ();Printf.printf"\n";*)
        (*Printf.printf"traiter calcul Total pour %s =\n" ii; print_expVA !resAuxTN; new_line ();Printf.printf"\n";*)

    dernierAppelFct := !predDernierAppelFct;
(*Printf.printf "av traiterBouclesInternes num %d nom eng %d AVANT AR\n"  id nomE ;*)

    if   !isIntoIfLoop = false && !isEnd  = false && !isEndNONZERO = false && fini = false then
      traiterBouclesInternes nT  nT saBENG
      id    ( !resAuxTN)  appel listeEng typeE numAp  ( !maxAuxTN) isExeE newlt newlf borne   sansP globales maxinit varLoop direction nomE (listeAsToListeAffect new_cond) iscompo
    else
    begin
      (*Printf.printf"traiter calcul MAX pour %s =\n" ii; print_expVA !maxAuxTN;  new_line ();Printf.printf"\n";    *)
      (*Printf.printf "av traiterBouclesInternes num %d nom eng %d \n" id nomE ;afficherListeAS(endcontexte);new_line();afficherListeAS new_cond; *)

      let ncc = List.map(fun assign -> match assign with
        ASSIGN_SIMPLE (id, e) -> ASSIGN_SIMPLE (id,applyStoreVA(applyStoreVA (applyStoreVA e !aslAux) appel) globales) |_-> assign) new_cond  in
 
 
        let next_cond = ncc in (* afficherListeAS next_cond;*)
		 
 		let valrestest = calculer (applyStoreVA(applyStoreVA (applyStoreVA (restest) !aslAux) appel) globales)   !infoaffichNull  [](*appel*) 1 in

 
		let isExecutedV = (match valrestest with Boolean(b)	->  if b = false then  false  else  true 
										|_->if estDefExp valrestest then if estNul valrestest then  false  else  true else  true  

						) in 

(*if isExecutedV = true then Printf.printf "!isExeBoucle= true %s" varIfN else Printf.printf "!isExeBoucle= false %s " varIfN;*)

          let isexeN = !isExeBoucle && isExecutedFunction next_cond && isExecutedV in

        (*if isexeN = true then Printf.printf "!isExeBoucle= true" else Printf.printf "!isExeBoucle= false" ;*)
		 
          if isexeN = false || !isEnd then
          begin
          (*  Printf.printf "la boucle n'est pas exécutée\n"; *)
            maxAuxTN :=EXP(CONSTANT (CONST_INT "0"));
            resAuxTN:=EXP(CONSTANT (CONST_INT "0"));
            listeInstNonexe := List.append [typeE] !listeInstNonexe
          end;

          let nTN =  applyif(applyif (applyif (!resAuxTN) !aslAux) appel) globales in
          let inter = applyif(applyif (applyif (!maxAuxTN) !aslAux) appel) globales in

          (*Printf.printf"traiter calcul MAX pour %s =\n" ii; print_expVA !maxAuxTN; new_line ();Printf.printf"\n";*)


          let listeIntersection = (intersection (listeDesVarsDeExpSeules maxinit ) (union [ii] vij)) in
        (*  Printf.printf "av traiterBouclesInternes num %d nom eng %d AVANT AR\n"  id nomE ;*)
(*if id = 62 then print_expression maxinit 0; new_line ();Printf.printf"\n";flush(); space(); new_line();*)

          let estIndependantM  =  if listeIntersection = []
                      then true
                      else (  let isindependent = independantDesVarsDeExpSeules maxinit lesAs  listeIntersection in
                           (*false*)  isindependent ) in
 
          (*List.iter (fun elem -> Printf.printf "%s " elem)listeIntersection;*)
          (*Printf.printf "av traiterBouclesInternes num %d nom eng %d AVANT AR\n"  id nomE ;*)
          let expmaxinit = if estIndependantM then
                      (applyif( applyif ( applyif (EXP( maxinit)) !aslAux) appel) globales)
                     else  (EXP(NOTHING) ) in


          (*Printf.printf"traiter calcul MAX pour %s =\n" ii; print_expVA (EXP(maxinit)); new_line ();Printf.printf"\n";*)

          let resauxmax2 = calculer expmaxinit   !infoaffichNull [] 1 in
          let nMax =
            ( match inter with
              MULTIPLE ->
                   if estDefExp resauxmax2  then  expmaxinit else EXP(NOTHING)
              | EXP(exp) ->
                if estNothing inter then
                  if estDefExp resauxmax2  then  expmaxinit else EXP(NOTHING)
                else inter

               ) in
      let (nMaxn,nTNn)= getMaxTotalArraySizeDep (expVaToExp nMax) (expVaToExp nTN)  appel globales in
      (*
      Printf.printf "1 traiterBouclesInternes  %d nom eng %d ou stopper %d sa eng %d tete nid %d\n" id  nomE idEng saBENG
      (getBoucleIdB nT.infoNid.laBoucle);
      Printf.printf"traiter calcul MAX pour %s =\n" ii; print_expVA nMax; new_line ();Printf.printf"\n";
      Printf.printf"traiter calcul Total pour %s =\n" ii; print_expVA nTN; new_line ();Printf.printf"\n";
      if !vDEBUG then Printf.printf "evalNid contexte  boucle: %d\n" id  ; *)
          (*afficherListeAS (appel);flush(); space(); new_line();*)

      (*Printf.printf "av traiterBouclesInternes num %d nom eng %d AVANT AR\n"  id nomE ; *)
          let nouNidEval = new_nidEval
                  typeE
                  (EXP(nTNn))
                  varLoop (*n.varDeBoucleNid *)
                  direction (*info.infoVariation.direction  *)
                  (EXP(nMaxn))   isexeN !isIntoIfLoop 0 in
      (*Printf.printf "AJOUTER 1 traiterBouclesInternes  %d nom eng %d ou stopper %d sa eng %d tete nid %d \nNID EVAL" id nomE idEng
       saBENG (getBoucleIdB nT.infoNid.laBoucle);*)
          (*isExecutedOneTimeOrMore := !isExecutedOneTimeOrMore && (nouNidEval.isIntoIf = false);*)
          compNotInnerDependentLoop nouNidEval iscompo;

          (*Printf.printf "av traiterBouclesInternes num %d nom eng %d AVANT FIN\n"  id nomE ;*)
          docEvalue := new_documentEvalue
                (List.append  [ nouNidEval] !docEvalue.maListeNidEval) !docEvalue.maListeEval;
              nouBoucleEval := [nouNidEval]
    end
  end
  else
  begin
    if (existeNidContenant nEC.listeTripletNid  idEng) then
    begin
      let liste = (rechercheNidContenant  nEC.listeTripletNid idEng) in
      if liste <> [] then
      begin
        let (_,_,nid) =List.hd liste in
(*Printf.printf "TRAITEMENT  DE %d AAA\n" id;*)
        traiterBouclesInternes nT  nid idEng id  tN appel listeEng typeE  numAp max isExeE lt lf borne false globales(* true = sans prod*) maxinit varLoop direction idpred lcond iscompo
      end
    end
    else

    begin

      if  ((*idEng <> 0) &&*) (idEng <> (getBoucleIdB nT.infoNid.laBoucle))) then
      (*if (nomE = (getBoucleIdB nT.infoNid.laBoucle) && nomE != 0) || (idEng != (getBoucleIdB nT.infoNid.laBoucle)) then*)
        begin
          let reverseliste = List.rev listeEng in
          listeAppels := [];
          let (nbou, nab, _,_,_,_,_) =
          rechercheDerniereBoucleApresId (getBoucleIdB nEC.infoNid.laBoucle) reverseliste in
          (*Printf.printf "TRAITEMENT  DE %d \n"  id;
          Printf.printf "REMONTER JUSQU4A SUIVANT DE %d suivant %d\n" (getBoucleIdB nEC.infoNid.laBoucle) nbou;*)
          let nidCourantCC = (rechercheNid nbou) in
          (match !dernierAppelFct with
              TFONCTION (nomf, numF,corps,listeInputInst, contexteAvantAppel,appelF,lFt,lFf,_,_,_,_) ->
                let fin = nbou = nomE in
                (*Printf.printf "REMONTER JUSQU4A SUIVANT DE %d suivant %d dans %s\n" (getBoucleIdB nEC.infoNid.laBoucle) nbou nomf; *)
                (*if fin then Printf.printf "on continu\n" else Printf.printf "derniere passe\n";*)
                  traiterBouclesInternes nT  nidCourantCC nomE id
                tN appel listeEng typeE  numF
                max isExeE lt lf borne  fin globales(* true = sans prod*) maxinit varLoop direction  idpred lcond iscompo
              |_->
               (* Printf.printf "FIN 1 pas de boucle englobante fin traiterBouclesInternes apres creer\n"*)())
                (*traiterBouclesInternes nT  nT nomE id
                !resAuxTN appel listeEng typeE  numAp
                !maxAuxTN isExeE lt lf borne  false globales(* true = sans prod*) maxinit varLoop direction nomE)*)

        end
        else
        begin
          (*Printf.printf "fin...pas compo\n";*)
        (*let reverseliste = List.rev listeEng in
          let (nbou, nab, _,_,_,_,_) =
          rechercheDerniereBoucleApresId (getBoucleIdB nEC.infoNid.laBoucle) reverseliste in
          Printf.printf "TRAITEMENT  DE %d \n"  id;
          Printf.printf "REMONTER JUSQU4A SUIVANT DE %d suivant %d\n" (getBoucleIdB nEC.infoNid.laBoucle) nbou;*)

           traiterBouclesInternes nT  nT idEng id
                tN appel listeEng typeE  numAp
                max isExeE lt lf borne  false  (* to exec the last path => false *) globales(* true = sans prod*) maxinit varLoop direction  idpred lcond iscompo
        end
    end
  end






(*   ---------------------------------------------------------------------------------------------------------------------------------COMPONENTS*)


let rec traiterBouclesInternesComposant     nT (*tete nid contenant bi*)  nEC (*noeud englobantcourant *)
                idEng (*id noeud englobant  où stopper *)
                id (*courant à  évaluer bi*)  tN
                appel (*contexte appel pour le moment fonction puis doc *)
                listeEng typeE numAp max isExeE lt lf borne    sansP globales corpsCompo maxinit varLoop direction idPred lcond=
  let info = (getBoucleInfoB (nEC.infoNid.laBoucle)) in

  let nomE = info.identifiant  in
  estDansBoucle := true;
  estDansBoucleE :=!estDansBoucle;
  let saBENG = (if aBoucleEnglobante info then info.nomEnglobante else 0) in
  if !vDEBUG then
  begin

    Printf.eprintf "1 traiterBouclesInternes num %d nom eng %d ou stopper %d sa eng %d tete nid %d\n" id  nomE idEng saBENG (getBoucleIdB nT.infoNid.laBoucle);
    (* afficheNidEval !docEvalue.maListeNidEval; *)
  (*  Printf.printf "FIN NID ENG COURANT \n"*)
  end;

  if nomE = idEng (*|| (nomE = (getBoucleIdB nT.infoNid.laBoucle) && nomE != 0*) then
  begin
  let fini = ((nomE = idEng) && (nomE =  (getBoucleIdB nT.infoNid.laBoucle)))  in
  if fini then estDansBoucleLast := true else   estDansBoucleLast:= false;
  let saBENG = (if aBoucleEnglobante info then info.nomEnglobante else 0) in


  let nbEngl =getNombreIt (nEC.infoNid.expressionBorne)    info.conditionConstante info.typeBoucle  info.conditionI  info.conditionMultiple  [] info.estPlus   info.infoVariation  nEC.varDeBoucleNid []  in

(*print_expVA (EXP(info.conditionI)); flush(); space(); new_line ();*)

  (*let varTN =  Printf.sprintf "%s-%d" "total" id in
  let varmax =  Printf.sprintf "%s-%d" "max" id in *)

    let(varTN,varmax,varTni,l, output) =   creerLesAffect tN max (EXP(NOTHING)) id 0 in
	let varIfN =  Printf.sprintf "%s-%d" "TWH" id  in
           
  isExeBoucle := isExeE;
 
  let (nlt,nlf,exeloop) = if id = idPred then   creerLesAffectEXECUTED lt lf "Loop" id idEng !cptFunctiontestIntoLoop else (lt,lf, lcond) in

  let (lesAs, intofunction,newlt, newlf) =
    ( if (!dernierAppelFct <> !predDernierAppelFct)
      then
      begin
        (match !dernierAppelFct with
          TFONCTION (nomf, numF,corps,listeInputInst, contexteAvantAppel,appelF,lFt,lFf,_,_,_,_) ->
            (*Printf.printf "cherche dernier appel nomfonc %s\n" nomf;*)
            let calllist = (reecrireCallsInLoop  nEC.varDeBoucleNid nEC.lesAffectationsBNid ) in

                  let ainserer =   (List.append l corps  ) in
                  let listBefore = List.rev ( (listBeforeCall !listeAppels nomf numF )) in

                  listBeforeFct := listBefore;

                  let nb = numberOfCall listBefore nomf numF in


                  let listtestfct =   creerLesAffectEXECUTEDFct ([(nomf, numF,lFt,lFf)] ) in
                  let aSC =  evalSIDA nT nEC calllist numF    (List.append   ainserer exeloop)
                    (List.append listtestfct listeInputInst) listeEng nb  listBefore in

                  let ifassign = filterIF aSC in
                      (*afficherListeAS aSC;*)
                  let isExecutedF = if ifassign = [] then true  else    isExecutedFunction   ifassign  in

                  (*if isExecutedF = false then listeInstNonexe := List.append [!dernierAppelFct] !listeInstNonexe;*)
                  isExeBoucle := isExeE && isExecutedF;
  (* afficherListeAS aSC;
                Printf.printf "apres evalSIDA compo\n";*)
                  (aSC, nb > 0, nlt,nlf )
            (*    | _-> ([], true))
             end*)
          |_->(*Printf.printf"cas 1\n";*)(lesVardeiSansj nT nEC idPred    (List.append   l exeloop)  , false,lt ,lf))
      end
      else  ((*Printf.printf"cas 2\n";*) (lesVardeiSansj nT nEC idPred    (List.append   l exeloop)  , false,lt ,lf))
    )in
   (*afficherListeAS lesAs; *)
      let ii = (nEC.varDeBoucleNid) in
      let vij =  rechercheLesVar  lesAs [] in

    let new_cond = filterIF lesAs in
    let resExptN  =    rechercheAffectVDsListeAS varTN lesAs in
    let recExptMax = rechercheAffectVDsListeAS  varmax  lesAs in

	let restest = rechercheAffectVDsListeAS  varIfN  lesAs in
    
    isIntoIfLoop := false;
    isEnd := false;
    isEndNONZERO := false;
    let resauxmax = calculer max  !infoaffichNull [] 1 in
    isEnd := if estDefExp resauxmax then if  estNul resauxmax then true else false else false ;

    resAuxTN :=
    (  match resExptN with
    MULTIPLE ->
      if sansP = false then
      begin
        if estDefExp resauxmax then
        begin
          isIntoIfLoop := true ;
          EXP(BINARY(MUL,expVaToExp borne,(expressionEvalueeToExpression resauxmax)))
        end
        else EXP(NOTHING)
      end
      else MULTIPLE
    |EXP  ( exptN) ->
      if sansP = false then
      begin
        let vdij = ( intersection  (listeDesVarsDeExpSeules exptN)  ( union [ii]  vij)) in

        let estIndependantTN  =   if vdij = []
                      then true
                      else (  let isindependent = independantDesVarsDeExpSeules exptN lesAs vdij in
                          (*if isindependent then Printf.printf "exptN is independant \n"
                          else Printf.printf "exptN is dependant \n";*) (*false*)isindependent ) in

        (* idenpendant*)
        if estIndependantTN then
        begin
           (match nbEngl with
            MULTIPLE->  MULTIPLE
            |EXP(exptni)->
              if estDefExp resauxmax then
              begin
                isEndNONZERO := true;
                EXP(BINARY(MUL,expVaToExp borne,(expressionEvalueeToExpression resauxmax)))
              end
              else if estNothing nbEngl || estNothing (EXP(exptN)) then   EXP(NOTHING) else    EXP(BINARY(MUL,exptni,exptN)) )
        end
        else
        begin
          match nbEngl with
          MULTIPLE -> MULTIPLE;
          | EXP(exptni) ->
              if estNothing nbEngl || estNothing (EXP(exptN)) then  EXP(NOTHING)
              else
                EXP(CALL(VARIABLE("SYGMA") ,
                  (List.append
                    (List.append [VARIABLE (ii)]
                        [BINARY(SUB, exptni,CONSTANT (CONST_INT "1"))])  [ exptN])))

        end
      end
      else resExptN) ;
    maxAuxTN :=
    ( if sansP = false then
      begin
        match recExptMax with
        MULTIPLE->
          if estDefExp resauxmax then
          begin isIntoIfLoop := true ;
             EXP(expressionEvalueeToExpression resauxmax)
          end
          else  EXP(NOTHING)
          | EXP (e)->


          let listVarOfExp = intersection (listeDesVarsDeExpSeules e) (union [ii] vij) in

          let estIndependantE  =  if listVarOfExp = []
                      then true
                      else (  let isindependent = independantDesVarsDeExpSeules e lesAs listVarOfExp in
                          (*if isindependent then Printf.printf "e is independant \n"
                          else Printf.printf "e is dependant \n";*) (*false*)isindependent ) in

          if estIndependantE then  EXP (e)
          else
            (match nbEngl with
              MULTIPLE ->   MULTIPLE
              | EXP(exp) ->
                if estNothing nbEngl || estNothing (EXP(e)) then  EXP(NOTHING)
                else EXP(
                      CALL(
                          VARIABLE("MAX") ,
                          (List.append (
                                  List.append [VARIABLE (ii)] [BINARY(SUB,exp, (CONSTANT (CONST_INT "1")))]
                                 )
                                [e]
                          )
                        )))


        end
        else recExptMax
      );

    dernierAppelFct := !predDernierAppelFct;

    if   !isIntoIfLoop = false && !isEnd  = false && !isEndNONZERO = false && fini = false then
      traiterBouclesInternesComposant     nT  nT saBENG id    ( !resAuxTN)  appel listeEng typeE numAp  ( !maxAuxTN) isExeE newlt newlf borne   sansP
      globales corpsCompo maxinit varLoop direction nomE (listeAsToListeAffect new_cond)
    else
    begin
          (*Printf.printf "dans le else 1\n";*)

          let ncc = List.map(fun assign -> match assign with ASSIGN_SIMPLE (id, e)->    ASSIGN_SIMPLE (id,applyStoreVA(applyStoreVA e appel) globales) |_-> assign) new_cond  in

          let next_cond = ncc in (* afficherListeAS next_cond;*)

		let valrestest = calculer (applyStoreVA(applyStoreVA (applyStoreVA (restest) !aslAux) appel) globales)   !infoaffichNull  [](*appel*) 1 in


		let isExecutedV = (match valrestest with Boolean(b)	->  if b = false then  false  else  true 
										|_->if estDefExp valrestest then if estNul valrestest then  false  else  true else  true  

						) in 
         

          let isExe2 = !isExeBoucle &&
            isExecutedFunction next_cond && isExecutedV in

          if isExe2 = false || !isEnd then
          begin
            maxAuxTN :=EXP(CONSTANT (CONST_INT "0"));
            resAuxTN:=EXP(CONSTANT (CONST_INT "0"));
            listeInstNonexe := List.append [typeE] !listeInstNonexe
          end;


          let nTN =  applyif(applyif (!resAuxTN) appel) globales in
          let inter = applyif(applyif (!maxAuxTN) appel) globales in
          let listeIntersection = (intersection (listeDesVarsDeExpSeules maxinit ) (union [ii] vij)) in



          let estIndependantM  =  if listeIntersection = []
                      then true
                      else (  let isindependent = independantDesVarsDeExpSeules maxinit lesAs  listeIntersection in
                          (*if isindependent then Printf.printf "maxinit is independant \n"
                          else Printf.printf "maxinit is dependant \n";*) (*false*)isindependent ) in

          (*List.iter (fun elem -> Printf.printf "%s " elem)listeIntersection;*)


          let expmaxinit = if estIndependantM then
                      applyif( applyif (EXP( maxinit)) appel) globales
                     else  EXP(NOTHING)  in
          (*Printf.printf "dans le else 2\n";   *)
          let resauxmax2 = calculer expmaxinit   !infoaffichNull [] 1 in
          let (nMax, isExeLoop) =
            ( match inter with
              MULTIPLE ->

                   if estDefExp resauxmax2  then
                   begin
                    let executed = if  estNul resauxmax2 then false else true in

                      ( expmaxinit , executed)

                   end else (EXP(NOTHING) , true)
              | EXP(exp) ->
                if estNothing inter then
                  if estDefExp resauxmax2  then
                  begin
                    let executed = if  estNul resauxmax2 then false else true in
                    ( expmaxinit , executed)
                  end
                  else (EXP(NOTHING) , true)
                else
                begin
                  let res = calculer inter    !infoaffichNull [] 1 in
                  let executed = if estDefExp res  then
                            if  estNul res then false else true
                           else true in
                  (inter,executed)
                end
               ) in
          if isExeLoop = false then  listeInstNonexe := List.append [typeE] !listeInstNonexe  ;

          (*Printf.printf "dans le else 3\n"; *)
          let nouNidEval = new_nidEval typeE nTN  varLoop direction nMax   isExeLoop !isIntoIfLoop 0 in

          (* ignore (afficherNidUML nouNidEval  [] 1 Listener.null) ; *)
(*Printf.printf "dans le else 3 \nNEW NID EVAL\n"; *)
          docEvalue := new_documentEvalue  (List.append  [ nouNidEval] !docEvalue.maListeNidEval) !docEvalue.maListeEval;
          (*Printf.printf "dans le else 4\n";*)
          nouBoucleEval := [nouNidEval]
      end


  end
  else
  begin
    if (existeNidContenant nEC.listeTripletNid  idEng) then
    begin
      let liste = (rechercheNidContenant  nEC.listeTripletNid idEng) in
      if liste <> [] then
      begin
        let (_,_,nid) =List.hd liste in
(*Printf.printf "TRAITEMENT  DE %d AAA\n" id;*)
        traiterBouclesInternes nT  nid idEng id  !resAuxTN appel listeEng typeE  numAp !maxAuxTN isExeE lt lf borne false globales(* true = sans prod*) maxinit varLoop direction idPred lcond true
      end
    end
    else

    begin

      if  ((*idEng <> 0) &&*) (idEng <> (getBoucleIdB nT.infoNid.laBoucle))) then
      (*if (nomE = (getBoucleIdB nT.infoNid.laBoucle) && nomE != 0) || (idEng != (getBoucleIdB nT.infoNid.laBoucle)) then*)
        begin
          listeAppels := [];
          let reverseliste = List.rev listeEng in
          let (nbou, nab, _,_,_,_,_) =
          rechercheDerniereBoucleApresId (getBoucleIdB nEC.infoNid.laBoucle) reverseliste in
          (*Printf.printf "TRAITEMENT  DE %d \n"  id;
          Printf.printf "REMONTER JUSQU4A SUIVANT DE %d suivant %d\n" (getBoucleIdB nEC.infoNid.laBoucle) nbou;*)
          let nidCourantCC = (rechercheNid nbou) in
          (match !dernierAppelFct with
              TFONCTION (nomf, numF,corps,listeInputInst, contexteAvantAppel,appelF,lFt,lFf,_,_,_,_) ->
                let fin = nbou = nomE in
                (*Printf.printf "REMONTER JUSQU4A SUIVANT DE %d suivant %d dans %s\n" (getBoucleIdB nEC.infoNid.laBoucle) nbou nomf; *)
                (*if fin then Printf.printf "on continu\n" else Printf.printf "derniere passe\n";*)
                  traiterBouclesInternes nT  nidCourantCC nomE id
                !resAuxTN appel listeEng typeE  numF
                !maxAuxTN isExeE lt lf borne  fin globales(* true = sans prod*) maxinit varLoop direction  idPred lcond true
              |_->
                Printf.printf "FIN 1 pas de boucle englobante fin traiterBouclesInternes apres creer\n")
                (*traiterBouclesInternes nT  nT nomE id
                !resAuxTN appel listeEng typeE  numAp
                !maxAuxTN isExeE lt lf borne  false globales(* true = sans prod*) maxinit varLoop direction nomE)*)

        end
        else
        begin
        (*  Printf.printf "fin...\n";*)
        (*let reverseliste = List.rev listeEng in
          let (nbou, nab, _,_,_,_,_) =
          rechercheDerniereBoucleApresId (getBoucleIdB nEC.infoNid.laBoucle) reverseliste in
          Printf.printf "TRAITEMENT  DE %d \n"  id;
          Printf.printf "REMONTER JUSQU4A SUIVANT DE %d suivant %d\n" (getBoucleIdB nEC.infoNid.laBoucle) nbou;*)

           traiterBouclesInternes nT  nT idEng id
                !resAuxTN appel listeEng typeE  numAp
                !maxAuxTN isExeE lt lf borne  false (* to exec the last path => false *)globales(* true = sans prod*) maxinit varLoop direction
           idPred lcond true
        end
    end
  end



let rechercheTriplet n liste=
List.find (fun e -> let (_,_,nid) =e in (getBoucleIdB nid.infoNid.laBoucle) = n) liste

let rec existeTFonction liste nom numF=
if liste <> [] then
begin
  let pred = (List.hd liste) in
  match pred with
  TBOUCLE(_, _,_,_,_,_,_,_,_) -> existeTFonction (List.tl liste) nom numF
  | TFONCTION(n,num,_,_,_,_,_,_,_,_,_,_) ->  if n = nom && num = numF then true else existeTFonction (List.tl liste) nom numF
end
else false

let rec existeTBoucle liste id appel=
if liste <> [] then
begin
  let pred = (List.hd liste) in
  match pred with
  TBOUCLE(ide, appele,_,_,_,_,_,_,_)  ->
      if (id = ide) && (appele = appel)   then true else existeTBoucle (List.tl liste) id appel
  | TFONCTION(_,_,_,_,_,_,_,_,_,_,_,_) ->  existeTBoucle (List.tl liste) id appel
end
else false

let rec reecrireCorpsNonExe  listeinst listeTypeNonExe numAppel=

if listeinst = [] || listeTypeNonExe = [] then listeinst
else
begin
  let i = List.hd listeinst in
  let suite = List.tl listeinst in
  match i with
    VAR (id, exp,_,_) -> List.append [i] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
    | TAB (id, exp1, exp2,_,_) -> List.append [i] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
    | MEMASSIGN(id, exp1, exp2,_,_) -> List.append [i] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
    | BEGIN liste ->
        List.append [BEGIN(reecrireCorpsNonExe  liste listeTypeNonExe numAppel)]
             (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
    | IFVF (t, i1, i2) ->
      let liste1 = match i1 with BEGIN(e)-> e |_->[] in
      let res1 = reecrireCorpsNonExe  liste1 listeTypeNonExe numAppel in
      let liste2 = match i2 with BEGIN(e)-> e |_->[]  in
      let res2 = reecrireCorpsNonExe  liste2 listeTypeNonExe numAppel in
      List.append  [IFVF(t, BEGIN(res1), BEGIN(res2))] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
    | IFV ( t, i1)    ->
      let liste1 = match i1 with BEGIN(e)-> e |_->[] in
      let res1 = reecrireCorpsNonExe  liste1 listeTypeNonExe numAppel in
      List.append [IFV ( t, BEGIN(res1))] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
    | FORV (num,id, e1, e2, e3, nbIt, inst,cp)  ->
      if existeTBoucle listeTypeNonExe num numAppel then List.append [FORV (num,id, e1, e2, e3, nbIt, BEGIN([]),[])](reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
      else
      begin
        let liste1 = match inst with BEGIN(e)-> e |_->[] in
        let res1 = reecrireCorpsNonExe  liste1 listeTypeNonExe numAppel in memAssign res1;
        List.append [FORV (num,id, e1, e2, e3, nbIt,  BEGIN(res1),!myChangePtr)]
          (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
      end
    | APPEL (i,e,nomFonc,s,CORPS c,var,r,u)->
      if List.mem_assoc  nomFonc !alreadyEvalFunctionAS = false then
      begin


        if existeTFonction listeTypeNonExe nomFonc i then
          List.append [APPEL (i,e,nomFonc,s,CORPS (BEGIN([])),var,r,u)](reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
        else
        begin
          let liste1 = match c with BEGIN(e)-> e |e->[e] in
          let res1 = reecrireCorpsNonExe  liste1 listeTypeNonExe i in
          List.append [APPEL (i, e ,nomFonc,s,CORPS(BEGIN(res1)),var,r,u)] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
        end
      end
      else
      begin
          if existeTFonction listeTypeNonExe nomFonc i then (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
          else
          begin
          List.append [APPEL (i, e ,nomFonc,s, ABSSTORE  ( List.assoc nomFonc !alreadyEvalFunctionAS),var,r,u)] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
          end
      end
    | APPEL (i,e,nomFonc,s,ABSSTORE c,var,r,u)->
      if existeTFonction listeTypeNonExe nomFonc i then (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
      else
      begin
        List.append [APPEL (i, e ,nomFonc,s, ABSSTORE c,var,r,u)] (reecrireCorpsNonExe  suite listeTypeNonExe numAppel)
      end
end

let rec sansIfCorps boucleOuAppelB =
if boucleOuAppelB = [] then []
else
begin
  let (first, next) = (List.hd boucleOuAppelB, List.tl boucleOuAppelB) in

  let beginLoA =
    (match first with
      IDBOUCLE (_, _,_,_,_) | IDAPPEL (_,_,_,_, _,_,_,_)-> [first ]
      |IDIF (_,_, treethen,_, treeelse,_,_,_,_,_,_,_) -> List.append (sansIfCorps treethen ) (sansIfCorps treeelse )
    ) in
  List.append beginLoA (sansIfCorps next)
end

 let isexeEnglobant = ref true

(* ANNOTATION *)
type ifAnnotation = TAKETHEN | TAKEELSE | TAKEANY
(* ANNOTATION For IF-n variable store in var look for annotation *)
let hasIFAnnotation var = !hasCondListFile_name && existeAffectationVarListe var !condAnnotated

(* ANNOTATION When it exists an annotation the return 3 no valid annotation, 1 the then branch is executed, 2 the else branch is executed*)
let getIFAnnotation var =
        let affect = rechercheAffectVDsListeAS  var  !condAnnotated in
        let cond = calculer  affect !infoaffichNull  [] 1 in
        match cond with Boolean(false)  | ConstInt("0")-> TAKEELSE | Boolean(true)  | ConstInt("1")-> TAKETHEN |_-> TAKEANY
(* ANNOTATION *)
let ifAnnotationToInt annot =
 match annot with  TAKETHEN ->1 | TAKEELSE ->2 | TAKEANY ->3


(** jz: evaluate body of a function or loop (includes if-statements as well)
        [?] a body can consist of a list of 'bodies', function then recurses on those
        [?] this can be, for example, a list of if-statements in a function body:
        [?] then, (first, next) consists of first and following if-statements
        [?] is Cvarabs.typeListeAS wrapping Util.abstractStore

        returns: triple = ( context, lastLoopOrCall, globals )
        @corps: the body or a list of bodies [Cextraireboucle.elementCorpsFonction list]
        @affectations: [?] list of statements/instructions that affect the body [Cvarabs.listeDesInst]
        @contexte: [?] abstract state [Cvarabs.typeListeAS]
        @listeEng [?] [evaluationType]
        @estexeEng: [?] is executed [bool]
        @lastLoopOrCall: list of callers/outer loops [Cextraireboucle.elementCorpsFonction list]
        @intoLoop: [?] [bool]
        @globales: global variables [Util.abstractStore list]

        [!] modification: the first time anything is executed, the globals should've already been
              set. so we just use the list that contains additional information (SCENARIO)
              and append it to the globals, if necessary, overwriting global
              assignments/initialization; afterwards we reset the additional information
              to [], such that it gets only reflected in the first call.
              this needs verification as there could be some issues:
                1) does this propagate over all functions that are called?
                2) does it start with main then?
                3) do we need to re-add information?
*)
let rec evalCorpsFOB corps affectations contexte listeEng estexeEng lastLoopOrCall intoLoop globales =
  (* this is a workaround to get global variable values -- FIXME :: do it only ONCE!! *)
  List.iter (
    fun g -> globalesPtr := !globalesPtr @ [g]; ()
  ) globales;
  let ncorps =
    if intoLoop = false then
      corps
    else
      corps (*sansIfCorps*)
    in
    if ncorps <> [] then begin
      let (first, next) = ((List.hd ncorps), (List.tl ncorps)) in   (* first and next bodies *)
      (* jz: SCEN-HACK for static scenarios: sort globals and constraints
             then merge them giving priority to the constraints *)
      (* jz NOTE: maybe give prio only the first time s.t. it is respected always?
            doesn't help for outputting ... 
            NOTE :: alternative are ghost instructions (ghost-affectations, 
                    i.e. effects of instrumented instructions. cannot be used with delta *) 
      let in_ffx_given = String.length !in_ffx_file in
      globalsAndConstraints :=
        if not !ghost && in_ffx_given > 0 then begin
          if !scenarioAsGlobalsUsedOnce = 1 then begin
            let sortedGlobals = List.sort cmpScenarioASAsgns globales in
            let sortedScenario = List.sort cmpScenarioASAsgns !scenarioAsGlobals in
            scenarioAsGlobalsUsedOnce := 0 ; (* jz: don't reuse scenario info *)
            mergePrioA_AB sortedScenario sortedGlobals [];
          end else !globalsAndConstraints
        end else globales
      ;
      (* new context and globals from evaluation of body, used in recursive call *)
      let (new_cont, new_globale) = evalUneBoucleOuAppel first affectations contexte listeEng estexeEng lastLoopOrCall !globalsAndConstraints in     (* jz: original = globales *)
      if next != [] then begin (* more than one body-element *)
        let (next_cont, last, next_globales) = evalCorpsFOB next affectations new_cont listeEng estexeEng [ first ] intoLoop new_globale in
        (next_cont, last, next_globales)
      end else
        (new_cont, [first], new_globale) (* no recursive call: context, globals from first body *)
    end else begin
      (contexte, [], globales) (* no body *)
    end

(** jz: invoked only from two callsites: evalCorpsFOB and evaluerSN
        retruns: pair = ( context, globals )
        @param elem the statement evaluated, possible: IDBOUCLE, IDIF, APPEL
    jz: added some simple mechanism to get a few addresses of fct ptrs -- still
        only added for if *)
and evalUneBoucleOuAppel elem affectations contexte listeEng estexeEng lastLoopOrCall globale =
(match elem with
  IDBOUCLE (num, lt,lf,fic,lig) ->
    if (existeNid num) then begin
      (* Printf.printf"Dans evalUneBoucleOuAppel de la boucle...%d \n"num;*)
      let nid = (rechercheNid num) in
      if (getBoucleInfoB (nid.infoNid.laBoucle)).infoVariation.operateur = NE then
        listNotEQ := List.append [(fic,lig)] !listNotEQ;
        let asL =
          if !estDansBoucle = false then
            (jusquaBaux affectations num contexte lastLoopOrCall globale)
          else
            contexte in
            estDansBoucleLast := true;
            (evalNid nid asL listeEng lt lf estexeEng globale fic lig, globale)
    end else begin
      if !vDEBUG then
        Printf.eprintf "eval corps fonction nid %d non trouve\n" num;
        (contexte, globale)
    end
  | IDIF (var, instthen, treethen, instelse, treeelse, lt, lf, fic, lig, infothen, infoelse, e) ->
    let asL = (jusquaIaux affectations var contexte lastLoopOrCall globale) in
    if !estDansBoucle = false then begin
      (* let asL = (jusquaIaux affectations var contexte lastLoopOrCall globale) in*)
      let prevPtrct = !myCurrentPtrContextAux in
      let isExecutedIf = estexeEng && isExecuted lt lf asL [] [] true in
      let annotRes =
        if hasIFAnnotation var then
          getIFAnnotation var
        else TAKEANY in
          let (executedBranch,affect) =
            if (existeAffectationVarListe var asL)
               || (existeAffectationVarListe var globale) then begin
              let affect =
                if (existeAffectationVarListe var asL) then
                  applyStoreVA (rechercheAffectVDsListeAS var asL) globale
                else rechercheAffectVDsListeAS var globale in
                  if annotRes = TAKEANY then
                    (let cond = calculer affect !infoaffichNull [] 1 in
                    (match cond with
                      Boolean(true) (*then only excuted*)
                      | ConstInt("1") -> (TAKETHEN, expVaToExp affect)
                      | Boolean(false) (*else only excuted*)
                      | ConstInt("0") -> (TAKEELSE, expVaToExp affect)
                      | _ -> (TAKEANY, expVaToExp affect))
                      (* ??? indifined branch is executed*)
                    )
                  else
                    (annotRes, expVaToExp affect)
    end else (annotRes, NOTHING) in
      let isExcutedThen = isExecutedIf && (executedBranch != TAKEELSE) in
      let isExcutedElse = isExecutedIf && (executedBranch != TAKETHEN) in
      let listeInstNonexePred = !listeInstNonexe in
      listeInstNonexe := [];
      let corpsEvalTMPPred = !corpsEvalTMP in corpsEvalTMP := [];
      let (ifthencontexte, lastthen,globalesThen) =
        evalCorpsFOB treethen instthen asL listeEng isExcutedThen [] false globale in
      let nonexethen = !listeInstNonexe in
      listeInstNonexe := [];
      let corpsEvalThen = !corpsEvalTMP  in corpsEvalTMP := [];
      let (ifelsecontexte, lastelse, globalesElse) =
        evalCorpsFOB treeelse instelse asL listeEng isExcutedElse [] false globale in
      let nonexelse = !listeInstNonexe in
      listeInstNonexe := listeInstNonexePred;
      let corpsEvalElse = !corpsEvalTMP  in
      let (nas, ng) =
        if isExecutedIf = false then begin
          (*debut com *)
          let new_if =
            [new_elementEvali affect  corpsEvalThen corpsEvalElse e fic lig false 4
             var (CONSTANT(CONST_INT("0"))) (CONSTANT(CONST_INT("1"))) infothen infoelse asL]
          in
          corpsEvalTMP := List.append corpsEvalTMPPred new_if;
          docEvalue :=
            new_documentEvalue !docEvalue.maListeNidEval
            (List.append !docEvalue.maListeEval new_if);
          (*fin comp*)
            
          myCurrentPtrContext:=prevPtrct;
          (asL, globale)
        end else begin
          (match executedBranch with
            TAKETHEN ->
              let fc = (localPtrAnalyse [new_instBEGIN (instthen)]  prevPtrct   !estDansBoucle false) in
              let res = endOfcontexte instthen  lastthen  ifthencontexte globalesThen, globalesThen in
              myCurrentPtrContext := (LocalAPContext.joinSet fc prevPtrct ); 
              res
              
            | TAKEELSE  ->
              let fc = (localPtrAnalyse [new_instBEGIN (instelse)]  prevPtrct   !estDansBoucle false) in
              let res = endOfcontexte instelse  lastelse  ifelsecontexte globalesElse, globalesElse in
              myCurrentPtrContext:=(LocalAPContext.joinSet fc prevPtrct );    res
              
            | TAKEANY ->
              
              (*isExecutedOneTimeOrMore:= false;*)
              let nthen = reecrireCorpsNonExe  instthen nonexethen !numAppel in
              let nelse = reecrireCorpsNonExe  instelse nonexelse !numAppel in
              let fc  = (localPtrAnalyse [new_instBEGIN (nthen)]  prevPtrct   !estDansBoucle false) in
              let  ec= (localPtrAnalyse [new_instBEGIN (nelse)]  prevPtrct   !estDansBoucle false) in
              myCurrentPtrContext:=LocalAPContext.joinSet   fc ec;
              if nelse = [] then
                (evalStore (IFV(EXP(VARIABLE(var)),BEGIN(nthen))) asL globale prevPtrct, globale)
              else
                (evalStore (IFVF(EXP(VARIABLE(var)),BEGIN(nthen),BEGIN(nelse))) asL globale prevPtrct, globale)
          )
        end in
          (*debut com*)
          let (vt, vf) = creerVarTF lt lf asL [] in
          let new_if = [new_elementEvali affect  corpsEvalThen corpsEvalElse e fic lig false
                        (ifAnnotationToInt executedBranch) var vt vf infothen infoelse asL]
          in
          corpsEvalTMP := List.append corpsEvalTMPPred new_if;
          docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_if);
          (*fin comp*)
          (*isExecutedOneTimeOrMore := isIntoIf;*)
          (nas, ng)
    end else begin (* into loop body*)
      (*debut com*)
      let corpsEvalTMPPred = !corpsEvalTMP in
      corpsEvalTMP := [];
      let (_, _,_) =  evalCorpsFOB treethen instthen contexte listeEng true [] true globale in
      let corpsEvalThen = !corpsEvalTMP  in
      corpsEvalTMP := [];
      let (_, _,_) =  evalCorpsFOB treeelse instelse contexte listeEng true [] true globale in
      let corpsEvalElse = !corpsEvalTMP  in
      let new_if = [new_elementEvali NOTHING corpsEvalThen corpsEvalElse e fic lig true 3 var
                    (CONSTANT(CONST_INT("1"))) ( CONSTANT(CONST_INT("0"))) infothen infoelse asL]
      in
      corpsEvalTMP := List.append corpsEvalTMPPred   new_if;
      docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_if);
      (*fin comp*)
      (*Printf.printf"if dans boucle %s %s %d\n" var  fic lig ;*)
      (contexte, globale)
    end
  | IDAPPEL (numf,appel,listeInputInstruction,var, lt,lf,fic,lig) ->
    let numAppelPred = !numAppel in
    let nomFonction = (match appel with  CALL(exp,_) -> (match exp with VARIABLE(nomFct) -> nomFct |_ -> "") |_ -> "") in
    if !vDEBUG then Printf.eprintf "evalUneBoucleOuAppel Eval appel FONCTION %s: num appel %d \n" nomFonction numf;
    let dansBoucle = !estDansBoucle in
    let asf = (jusquaFaux affectations numf  contexte lastLoopOrCall globale) in
    let (prevPtrct,contexteAvantAppel,(myCall, hasCall)) =
      (if dansBoucle = false then
         (!myCurrentPtrContextAux,asf,(!appelcourant,true))
       else if !appelcourant <> [] then
         (!myCurrentPtrContext, contexte, ([getIntoAffectB nomFonction numf (List.hd !appelcourant) affectations], true))
       else
         (!myCurrentPtrContext, contexte,( [], false)))
      in
    (* jz: fct ptr *) 
    let b = match elem with 
        IDAPPEL (_, CALL (UNARY (MEMOF, VARIABLE varn), []), [], "", [], [], filen, lin) -> (varn, filen, lin)
      | _ -> ("", "", -1) 
    in
    (* jz: ::FIXME:: this could be done better -- we search a single assignment to the ptr
           and stores info about this call in some global data structure. that way we can fill 
           in the necessary info when we do the respective onCall, identified by line/src *)
    let currentlyPointsToExclusivelyOneAddr = match contexteAvantAppel with
        [ASSIGN_SIMPLE (varname, EXP (UNARY (ADDROF, VARIABLE fctname)))] -> (* verify that fctname is actually the name of a fct *)
          fctname
      | _ -> ""
    in (* jz: continue *)
    let (v1,v2,v3) = b in Hashtbl.add varsPointingToFcts (v2, v3) (v1, currentlyPointsToExclusivelyOneAddr) ;
    let (lappel, entrees, lesAffectations, isCompo, output) =
        if myCall <> [] then begin
          match List.hd myCall with
          APPEL (n,e,nomFonc, s,CORPS c,v,r,u) ->
            if ((existeFonctionParNom nomFonction doc)
                && (not (Cextraireboucle.is_in_use_partial nomFonction))) then begin
              let (_, func) = (rechercherFonctionParNom nomFonction doc) in
              let ne = (match e with BEGIN(eee)-> (List.append listeInputInstruction eee) |_->listeInputInstruction) in
              (* Printf.printf "evalUneBoucleOuAppel FONCTION %s: num appel EXISTE %d \n" nomFonction numf;*)
              ([APPEL (n,e,nomFonc,s,CORPS(BEGIN(func.lesAffectations)),v,r,u)], ne,func.lesAffectations, false,[])
            end else
              (let corps = (match c with BEGIN(ccc)-> ccc |ccc-> [ccc] ) in
               let ne = (match e with BEGIN(eee)-> (List.append listeInputInstruction eee) |_->listeInputInstruction) in
               if corps != [] then begin
                 (*Printf.printf "evalUneBoucleOuAppel Eval appel FONCTION %s: num appel EXISTE VOIR COMPO %d \n" nomFonction numf;*)
                 if (Cextraireboucle.is_in_use_partial nomFonc) then
                   ([APPEL (n,e,nomFonc,s,ABSSTORE (Cextraireboucle.getAbsStoreFromComp nomFonc),v,r,u)], ne, [],true,[])
                 else
                   ([APPEL (n,e,nomFonc,s,CORPS c,v,r,u)], ne,[], false,[])
               end else
                 (* jz: ::TODO:: this could also be a call via a fct ptr ::TODO:: *)
                 ((*Printf.printf "evalUneBoucleOuAppel Eval appel FONCTION %s: num appel EXTERN %d \n" nomFonction numf;*)
                  if (Cextraireboucle.is_in_use_partial nomFonc) then
                    ([APPEL (n,e,nomFonc,s,ABSSTORE (Cextraireboucle.getAbsStoreFromComp nomFonc),v,r,u)], ne, [],true,[])
                  else begin
                   let sorties = (match s with BEGIN(sss)-> sss |_->[]) in ([],listeInputInstruction,[], false,sorties)
                  end
                 )
              )
          | APPEL (n,e,nomFonc,s,ABSSTORE a,v,r,u) ->
            let ne = (match e with BEGIN(eee)-> (List.append listeInputInstruction eee) |_->listeInputInstruction) in
            (*Printf.printf "evalUneBoucleOuAppel Eval appel FONCTION %s: num appel COMPOSANT %d \n" nomFonction numf; *)
            ([APPEL (n,e,nomFonc,s,ABSSTORE a,v,r,u)], ne, [],true,[])  | _ -> failwith "function type error (component, extern...)"
          end else ([], listeInputInstruction,[], false,[]) in

      (* non le contexte de l'appel se réduit à la valeur des *)
      let (asLAppel,others, globalesBefore, input) =
      ( if dansBoucle = false then
         begin
          let (gc,others) = filterGlobalesAndOthers contexteAvantAppel !globalesVar in
          let newGlobales = rond globale gc in
          let (input, xx) = evalInputFunction others   entrees newGlobales prevPtrct in

          ( input ,others, newGlobales, entrees )
        end
        else (contexte,[], globale, []))  in


   let (vt, vf) =  if dansBoucle = false then  creerVarTF lt lf contexteAvantAppel []
          else  (( CONSTANT(CONST_INT("1")))) , (( CONSTANT(CONST_INT("0")))) in

(*  Printf.printf "evalUneBoucleOuAppel  appel FONCTION %s \n" nomFonction ;
  Printf.printf "isExecuted : list of true conditions variables\n"; List.iter (fun e-> Printf.printf "%s "e) lt;
  Printf.printf "isExecuted : list of false conditions variables\n"; List.iter (fun e-> Printf.printf "%s "e) lf;*)


  let isExecutedCall =  if dansBoucle = false then   estexeEng && isExecuted lt lf contexteAvantAppel [] [] true else estexeEng in
  (* if depends on before context but not global because of instruction order is not ok*)
  (*if isExecuted lt lf contexteAvantAppel [] []  false then Printf.printf "est exécutée %s\n" nomFonction
  else  Printf.printf "n'est exécutée%s\n"nomFonction;*)

  (*Printf.printf "evalUneBoucleOuAppel  appel FONCTION %s:\n Globales :\n" nomFonction ;afficherListeAS( globalesBefore);new_line () ;*)

  if  lappel <> [] && isCompo = false then
  begin
    let appelC = List.hd lappel in
    match appelC with
        APPEL (_,e,nomFonc,s,c,v,_,usedAfter) ->
          if ((existeFonctionParNom nomFonction doc) && (not (Cextraireboucle.is_in_use_partial nomFonction))) then
        begin
            numAppel := numf;
          let (_, func) = (rechercherFonctionParNom nomFonction doc) in
          let affec = if dansBoucle = false then  func.lesAffectations
                  else reecrireCallsInLoop !varDeBoucleBoucle func.lesAffectations  in
            if !vDEBUG then Printf.eprintf "evalUneBoucleOuAppel FIN Eval appel FONCTION %s:\n ENTREES :\n" nomFonction ;
            let typeE =  TFONCTION(nomFonction,!numAppel,affec , entrees, asLAppel,lappel,lt,lf,
                    isExecutedCall  , dansBoucle,fic,lig) in
            let (new_contexte,last, globalesAA) =
            evaluerFonction nomFonction func asLAppel (EXP(appel))  (List.append [typeE] listeEng) typeE
                 isExecutedCall globalesBefore vt vf prevPtrct input in

          (*Printf.printf "evalUneBoucleOuAppel FIN Eval appel FONCTION %s:\n ENTREES :\n" nomFonction ;*)
            numAppel := numAppelPred ;
            if dansBoucle = false then
            begin
              if isExecutedCall (*&& nomFonction != !(!mainFonc)*) then (*not extern not into loop executed*)
              begin
                let rc = endOfcontexte affec  last  new_contexte globalesAA in
              let (globalUsedPtr, inputPtr) = (( changeGlobalListPtr  nomFonc c []),(getParamPtr entrees)) in
              let before =LocalAPContext.initIntervalAnalyse
                (union globalUsedPtr inputPtr)  inputPtr  prevPtrct globalUsedPtr  in

              let getNextassign =   getNextaa nomFonction entrees usedAfter before c  "c orange" prevPtrct false in
              myCurrentPtrContext :=  LocalAPContext.joinSetSubSet prevPtrct !myCurrentPtrContextAux  ;

                listeASCourant := [];
                let sorties = (match s with BEGIN(sss)-> sss |_->[]) in
                if sorties <> [] then
                begin

(*afficherLesAffectations func.lesAffectations ;
                Printf.printf "evalUneBoucleOuAppel var SORTIE %s \n" nomFonction ;afficherListeAS( rc);new_line () ; *)
                List.iter (
                  fun sortie ->
                  (match sortie with
                  VAR (id, e,_,_)->
                    (*Printf.printf "evalUneBoucleOuAppel var SORTIE %s %s\n" nomFonction id;afficherListeAS( rc);new_line () ;*)
                    let (isOkSortie, isnotchange) =  isOkSortie e rc [] id in
                    if isnotchange = false then
                    begin
                      if isOkSortie then
                        if existAssosArrayIDsize id  then   (getTabAssign sortie rc globalesAA MULTIPLE !myCurrentPtrContext)
                        else
                          listeASCourant :=
                           (new_assign_simple id ( getSortie e rc  globalesAA id !myCurrentPtrContext))::!listeASCourant
                      else  listeASCourant :=  (new_assign_simple id MULTIPLE )::!listeASCourant
                    end;   ()
                  | TAB (id, e1, e2,_,_) ->
                    let (isOkSortie, isnotchange) =  isOkSortie e2 rc [] id in
                    if isnotchange = false then
                      if isOkSortie then
                        getTabAssign sortie rc globalesAA MULTIPLE !myCurrentPtrContext
                      else
                        listeASCourant := (ASSIGN_DOUBLE (id, MULTIPLE, MULTIPLE))::!listeASCourant;  ()
                  |MEMASSIGN (id, e1, e2,_,_)-> (*Printf.printf "sortie %s mem\n" id;*)
                    let (isOkSortie, isnotchange) =  isOkSortie e2 rc [] id in
                    if isnotchange = false then
                    begin
                      let index = if   List.mem "_ALL-"  (listeDesVarsDeExpSeules ( expVaToExp e1) )
                          then EXP(applyStore (expVaToExp e1) asLAppel)
                          else  EXP(BINARY (ADD, (applyStore (expVaToExp e1) asLAppel), VARIABLE "_ALL-")) in
                      if isOkSortie then
                        getMemAssign sortie rc  globalesAA index !myCurrentPtrContext
                      else
                        listeASCourant := (ASSIGN_MEM (id, index,  MULTIPLE))::!listeASCourant
                    end;  ()
                  |_->())
                  )sorties  ;
(*Printf.printf "\nevalUneBoucleOuAppel var SORTIE RES \n";
                  afficherListeAS !listeASCourant;Printf.printf "\nevalUneBoucleOuAppel var SORTIE FIN \n";*)
              end   ;
              let returnf = Printf.sprintf "res-%s"  nomFonc in

               (*afficherListeAS rc; afficherListeAS !listeASCourant;*)
              let nginterne = filterGlobales rc !globalesVar in

              if existeAffectationVarListe returnf rc then
              begin
                let affectres = ro returnf rc in
                listeASCourant :=  rond [affectres] (List.append nginterne !listeASCourant )
              end
              else listeASCourant :=     (List.append nginterne  !listeASCourant );
              let ncont =  if getNextassign = [] then rond others  !listeASCourant
               else rond others   (List.append !listeASCourant  getNextassign) in(*voir remarque cvarabs.ml*)
              (ncont, globalesAA)

            end
            else begin myCurrentPtrContext := prevPtrct;listeInstNonexe := typeE::!listeInstNonexe; (contexte ,globale) end(*not extern not into loop not exe*)
          end
          else
          begin (*Printf.printf "FIN Eval appel FONCTION 3%s:\n" nomFonction ; *)  (contexte,globale)  end(*not extern  *)
        end
        else
        begin
          if  dansBoucle = false &&  isExecutedCall (*  extern not into loop*) then
          begin
              myCurrentPtrContext := prevPtrct;
              listeASCourant := [];
              let sorties = (match s with BEGIN(sss)-> sss |_->[]) in
              if sorties <> [] then
              begin
                List.iter
                ( fun sortie ->
                  (match sortie with
                    VAR (id, _,_,_) ->
                      listeASCourant :=  (new_assign_simple id  MULTIPLE)::!listeASCourant;  ()
                    | TAB (id, e1, _,_,_) ->
                      listeASCourant :=(ASSIGN_DOUBLE (id,  MULTIPLE,  MULTIPLE))::!listeASCourant; ()
                    |MEMASSIGN (id, e1,_,_,_)->

                      let index = if   List.mem "_ALL-"  (listeDesVarsDeExpSeules (expVaToExp e1) )
                            then EXP(applyStore (expVaToExp e1) asLAppel)
                            else  EXP(BINARY (ADD, (applyStore (expVaToExp e1) asLAppel), VARIABLE "_ALL-")) in
                      listeASCourant :=(ASSIGN_MEM (id, index,  MULTIPLE)):: !listeASCourant; ()
                    |_->())
                )sorties
              end  ;
              (rond contexteAvantAppel !listeASCourant,globalesBefore)
            end

            else (*Printf.printf "FIN Eval appel FONCTION 4%s:\n" nomFonction ; *) (contexteAvantAppel, globale)
        end(* extern  *)
        |_-> begin (*Printf.printf "FIN Eval appel FONCTION 5%s:\n" nomFonction ;  *)(contexteAvantAppel, globale) end;
      end
      else
      begin
      numAppel := numf;
      let (nextcont, neg)=
        if dansBoucle = false then (* compo or extern not into loop*)
        begin
          if isExecutedCall then (* compo or extern not into loop executed*)
          begin
              (*  Printf.printf "FIN Eval appel FONCTION 6%s:\n" nomFonction ;*)
            if isCompo then (*  compo not into loop executed*)
            begin
              let (e, corpsOuAppel) = match List.hd lappel with APPEL(_, e, _, _, corpsOuAppel, _ ,_,_) ->
                                (e, corpsOuAppel)  |_ -> failwith "function type error (component, extern...)" in
              match corpsOuAppel with
                CORPS c ->  (*Printf.printf " FONCTION externe%s:\n" nomFonction ; *)(contexte, globale)
                |ABSSTORE a ->(*Printf.printf "FIN Eval appel FONCTION composant%s:\n" nomFonction ;*)
                  myCurrentPtrContext :=LocalAPContext.setGlobalToTop prevPtrct ( changeGlobalListPtr  nomFonction (ABSSTORE a) []);
                  let nc = rond   others asLAppel in
                  let nextnum= getfirtFreeCompParCall !numAppel nomFonction in
                  let typeE =  TFONCTION(nomFonction,!numAppel,listeAsToListeAffect a , entrees, nc,lappel,lt,lf,
                             isExecutedCall, dansBoucle, fic,lig)  in
                  dernierAppelFct := typeE;
                  let comp_base = (!idBoucle + 1) in
                  compEvalue := (nextnum, nomFonction,
                          (evaluerComposant nomFonction nc isExecutedCall dansBoucle globalesBefore
                        (List.append [typeE]  listeEng) typeE comp_base ))::(!compEvalue);
                  let new_fct = [ new_elementEvala typeE (EXP(appel)) [] vt vf] in
                  corpsEvalTMP := List.append !corpsEvalTMP  new_fct;
                  docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_fct);
                  let inter =   (evalStore (List.hd lappel) nc  globalesBefore) prevPtrct in    (*OK*)
                  (* afficherListeAS( inter);new_line () ;  *)
                (*  Printf.printf "FIN Eval appel FONCTION composant%s:\n" nomFonction ;*)
                  ( inter ,globalesBefore)
            end
            else
            begin (*  extern not into loop executed*)
              (*if output != [] then Printf.eprintf "WARNING  FONCTION EXTERN BUT NOT DECLARED %s  :  EXECUTED NOT INTO\n" nomFonction ;  *)
              myCurrentPtrContext := prevPtrct;
              let typeE =  TFONCTION(nomFonction,!numAppel,[] , listeInputInstruction, contexteAvantAppel,lappel,lt,lf,
                         isExecutedCall, dansBoucle,fic,lig) in
              let new_fct = [ new_elementEvala typeE (EXP(appel)) [] vt vf] in
              corpsEvalTMP := List.append !corpsEvalTMP  new_fct;
              docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_fct);
                if output != [] then
                begin
                List.iter
                ( fun sortie ->
                  (match sortie with
                    VAR (id, _,_,_) ->(*  Printf.printf "%s \n" id;*)
                      listeASCourant :=  (new_assign_simple id  MULTIPLE )::!listeASCourant;  ()
                    | TAB (id, e1, _,_,_) ->  (* Printf.printf "%s \n" id;*)
                      listeASCourant := (ASSIGN_DOUBLE (id,  MULTIPLE,  MULTIPLE))::!listeASCourant; ()
                    |MEMASSIGN (id, e1,_,_,_)->   (* Printf.printf "%s \n" id;*)

                      let index = if   List.mem "_ALL-"  (listeDesVarsDeExpSeules (expVaToExp e1) )
                            then EXP(applyStore (expVaToExp e1) asLAppel)
                            else  EXP(BINARY (ADD, (applyStore (expVaToExp e1) asLAppel), VARIABLE "_ALL-")) in
                      listeASCourant :=

                         (ASSIGN_MEM (id, index,  MULTIPLE))::!listeASCourant; ()
                    |_->())
                )output
              end  ;
              (rond contexteAvantAppel !listeASCourant,globale)
            end
          end
          else
          begin (* compo or extern not into loop not executed*)
            myCurrentPtrContext := prevPtrct;
            if isCompo then
            begin
              let typeE =  TFONCTION(nomFonction,!numAppel,[] , entrees, [],lappel,lt,lf,  isExecutedCall, dansBoucle,fic,lig)  in
              dernierAppelFct := typeE;
              let comp_base = (!idBoucle + 1) in
              let nextnum= getfirtFreeCompParCall !numAppel nomFonction in
              compEvalue := (nextnum, nomFonction,
                  (evaluerComposant nomFonction [] isExecutedCall dansBoucle []
                    (List.append [typeE] listeEng) typeE comp_base ))::(!compEvalue);
              (*Printf.printf "FONCTION composant%s: NON EXECUTED FIN\n" nomFonction ;*)
              let new_fct=[new_elementEvala typeE (EXP(appel)) [] (CONSTANT(CONST_INT("0"))) (CONSTANT(CONST_INT("0")))]
               in
              corpsEvalTMP := List.append !corpsEvalTMP  new_fct;
              docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_fct);
            end;
            (contexte, globale)
          end
        end
        else (* compo or extern  into *)
          if isCompo then (* composant *)
          begin
            let (e, nom, corpsOuAppel) =
              match List.hd lappel with APPEL(_, e, nom, _, corpsOuAppel, _ ,_,_) -> (e,nom, corpsOuAppel)  |_ -> failwith "function type error (component, extern...)" in
            match corpsOuAppel with
              CORPS c ->  Printf.printf "IMPOSSIBLE%s %s:\n" nomFonction nom; (contexte, globale)
              |ABSSTORE a -> (*Printf.printf "LOOP FIN Eval appel FONCTION composant %s:\n" nomFonction ;*)

              let typeE =
              TFONCTION(nomFonction,!numAppel,listeAsToListeAffect a,entrees, asLAppel,lappel,lt,lf, isExecutedCall, dansBoucle,fic,lig) in
            let comp_base = (!idBoucle + 1) in
            let nextnum= getfirtFreeCompParCall !numAppel nomFonction in
            compEvalue := (nextnum, nomFonction,
                      (evaluerComposant nomFonction contexte  true dansBoucle globale (List.append [typeE]
                     listeEng) typeE comp_base ))::(!compEvalue);
            let new_fct = [ new_elementEvala typeE (EXP(appel)) [] vt vf] in
            corpsEvalTMP := List.append !corpsEvalTMP  new_fct;
            docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_fct);

            (* Printf.printf "On ajoute au compEvalue un nouvel element, qui a maintenant %d elements\n" (List.length !compEvalue);*)
            (contexte, globale)
          end
          else
          begin
            let typeE =
                TFONCTION(nomFonction,!numAppel,[] , entrees, contexte,lappel,lt,lf,    isExecutedCall, dansBoucle,fic,lig) in
            let new_fct=[new_elementEvala typeE (EXP(appel)) [] (CONSTANT(CONST_INT("1"))) (CONSTANT(CONST_INT("0")))] in
            corpsEvalTMP := List.append !corpsEvalTMP  new_fct;
            docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_fct);
            (contexte, globale)
          end
      in
      numAppel := numAppelPred ;
          (* Printf.printf "FIN Eval appel FONCTION 6%s:\n" nomFonction ; *)
      (nextcont, neg) (*asLAppel REVOIR !!!*)
      end )




and evaluerComposant nomComp contexte isExecutedCall dansBoucle globales listeEng typeE comp_base =
  let absolutize valname =
    try
      Scanf.sscanf valname "bIt-%d" (fun x -> (sprintf "bIt-%d" (x + comp_base)))
    with Scanf.Scan_failure str -> valname
  in
  let absolutizeTotal valname =
    try
      Scanf.sscanf valname "total-%d" (fun x -> (sprintf "total-%d" (x + comp_base)))
    with Scanf.Scan_failure str -> valname
  in
  let absolutizeMax valname =
    try
      Scanf.sscanf valname "max-%d" (fun x -> (sprintf "max-%d" (x + comp_base)))
    with Scanf.Scan_failure str -> valname
  in
  let absolutizeTotalMax x = absolutizeTotal (absolutizeMax x) in
  let rec evalAuxBoucle = function
    Doc subtree -> Doc (List.map evalAuxBoucle subtree)
  | Function (x, subtree) ->  Function (x, List.map evalAuxBoucle subtree)
  | Call (x, subtree) -> Call (x, List.map evalAuxBoucle subtree)
  | If  (( name,  line, source, inloop, executed ,condVa, cond ,lt, lf, asL), subtree)  ->   If (( name,  line, source, inloop, executed ,condVa, cond ,lt, lf, asL), List.map evalAuxBoucle subtree)
  | IfT (x, subtree) -> IfT (x,  List.map evalAuxBoucle subtree)
  | IfF (x,  subtree) -> IfF ( x,List.map evalAuxBoucle subtree)
  | Loop ((id, line, source, exact, max, total, expMax, expTotal,expinit, sens,lt,lf), subtree) ->
    idBoucle := (!idBoucle + 1);
    let expMax = mapVar absolutize expMax in
    let expTotal = mapVar absolutize expTotal in
    let id = id + (!idBoucle) + 1 in
    let varLoop = sprintf "bIt-%d" id in
  let direction = sens in
  let corpsCompo =  (mapListAffect absolutizeTotalMax (getInstListFromPartial (getPartialResult nomComp))) in
(*Printf.printf "ON ESSAYE D APPLIQUER LE CONTEXTE SUR:loop %s %u \n"source line ;  print_expression expMax 0;*)
  (*Printf.printf "ON ESSAYE D APPLIQUER LE CONTEXTE SUR: " ; print_expression expMax 0;*)
  (*let appelP = !dernierAppelFct in*)
    dernierAppelFct:= typeE;
    let (finstanciedTotal,finstanciedMax) = evalNidComposant id contexte listeEng [] [] true globales expMax expTotal varLoop direction corpsCompo source line in
  let (nmax,ntotal) = getMaxTotalArraySizeDep finstanciedMax finstanciedTotal contexte globales in


   let instanciedMax =  nmax in
   let instanciedTotal =ntotal in

    (*Printf.printf "TOTAL: %s MAX: %s\n" (string_from_expr instanciedTotal) (string_from_expr instanciedMax);  *)
    let res = Loop ((id, line, source, exact, NOCOMP, NOCOMP, instanciedMax, instanciedTotal, expinit, sens,lt,lf), List.map evalAuxBoucle subtree) in
        (*  dernierAppelFct := appelP;*)
    res
    in


  let rec evalAuxPasBoucle = function
  Doc subtree -> Doc (List.map evalAuxPasBoucle subtree)
  | Function ((name, inloop, executed, extern ), subtree) ->
    let isexeEnglobantPred = !isexeEnglobant in
    (*isexeEnglobant:= isexeEnglobantPred && executed;*)
    let isexe = !isexeEnglobant in
  (*  if !isexeEnglobant then Printf.printf "ON A COMPOSE compo %s name EXECUTED \n"name  else Printf.printf "ON A COMPOSE l'appel %s name  NOT EXECUTED \n"name ;*)
    let res = Function ((name, inloop, isexe, extern ), List.map evalAuxPasBoucle subtree)in
    isexeEnglobant:= isexeEnglobantPred;
    res
  | Call ((name, numCall, line, source, inloop, executed, extern,lt,lf) , subtree) ->
    let (elt,elf) = (applyStore (applyStore lt contexte ) globales,applyStore (applyStore lf contexte ) globales) in

    let isexeEnglobantPred = !isexeEnglobant in
    isexeEnglobant:= isexeEnglobantPred && executed && creerVarTFE elt elf;
    let isexe = !isexeEnglobant in
    (*if isexe then Printf.printf "ON A COMPOSE l'appel %s name %d EXECUTED \n"name numCall else Printf.printf "ON A COMPOSE l'appel %s name %d NOT EXECUTED \n"name numCall;*)
    let res = Call ((name, numCall, line, source, inloop, isexe, extern,elt,elf), List.map evalAuxPasBoucle subtree) in
    isexeEnglobant:= isexeEnglobantPred;
    res
  |If (( name,  line, source, inloop, executed ,condVa, cond ,lt, lf, asL), subtree) ->
    let (elt,elf) = (applyStore (applyStore lt contexte ) globales,applyStore (applyStore lf contexte ) globales) in
    let isexeEnglobantPred = !isexeEnglobant in

    isexeEnglobant:= if (executed = 4) then false else isexeEnglobantPred &&  creerVarTFE elt elf;

 
    let (isexe, valcond) = if !isexeEnglobant then
          begin
            let valcond =  (( applyStoreVA(applyStoreVA (EXP condVa) contexte)globales))   in
            let resaux = calculer valcond  !infoaffichNull [] 1 in
            if estDefExp resaux  then match resaux with Boolean(false)  | ConstInt("0")-> (2 ,expVaToExp valcond)| Boolean(true)  | ConstInt("1")->(1 ,expVaToExp valcond)|_-> (3 ,expVaToExp valcond)else  (3 ,expVaToExp valcond)
          end 
		  else  (4 ,CONSTANT (CONST_INT "0")) in
 
     let res =
      if !isexeEnglobant   then     If ((name, line, source, inloop, isexe, valcond, cond, elt, elf, asL), List.map evalAuxPasBoucle subtree)
	  else If ((name, line, source, inloop, 4, (CONSTANT (CONST_INT "0")),cond ,elt, elf, asL), List.map evalAuxPasBoucle subtree)
       in
    isexeEnglobant:= isexeEnglobantPred;
    res
  |IfT ((isexe, info), subtree) ->

    let isexeEnglobantPred = !isexeEnglobant in
    (*isexeEnglobant:= isexeEnglobantPred && executed;*)
    let isexe = !isexeEnglobant in
  (*  if !isexeEnglobant then Printf.printf "ON A COMPOSE compo %s name EXECUTED \n"name  else Printf.printf "ON A COMPOSE l'appel %s name  NOT EXECUTED \n"name ;*)
    let res = IfT ((isexe, info),  List.map evalAuxBoucle subtree)  in
    isexeEnglobant:= isexeEnglobantPred;
    res
  |IfF ((isexe, info),  subtree) ->

    let isexeEnglobantPred = !isexeEnglobant in
    (*isexeEnglobant:= isexeEnglobantPred && executed;*)
    let isexe = !isexeEnglobant in
  (*  if !isexeEnglobant then Printf.printf "ON A COMPOSE compo %s name EXECUTED \n"name  else Printf.printf "ON A COMPOSE l'appel %s name  NOT EXECUTED \n"name ;*)
    let res = IfF ((isexe, info),  List.map evalAuxBoucle subtree)    in
    isexeEnglobant:= isexeEnglobantPred;
    res

  | Loop ((id, line, source, exact, max, total, expMax, expTotal,expinit, sens,lt,lf), subtree) ->
    begin
        idBoucle := (!idBoucle + 1);
(*Printf.printf "ON ESSAYE D APPLIQUER LE CONTEXTE SUR:loop %s %u "source line ;*)
  (*   Printf.printf "ON ESSAYE D APPLIQUER LE CONTEXTE SUR: " ; print_expression expMax 0;*)
    (*let appelP = !dernierAppelFct in*)
    let (elt,elf) = (applyStore (applyStore lt contexte ) globales,applyStore (applyStore lf contexte ) globales) in
    let isexeEnglobantPred = !isexeEnglobant in
    isexeEnglobant:= isexeEnglobantPred  && creerVarTFE elt elf;

    dernierAppelFct:= typeE;(*VOIR*)
    let res =

      if !isexeEnglobant = false then
      begin
       (* Printf.printf "ON A COMPOSE la boucle ID %u , non executed\n" id ;*)
        Loop ((id + (!idBoucle), line, source, exact, ConstInt("0"), ConstInt("0"), (CONSTANT (CONST_INT "0")), (CONSTANT (CONST_INT "0")), expinit, sens,(CONSTANT (CONST_INT "0")),(CONSTANT (CONST_INT "0"))), List.map evalAuxPasBoucle subtree)
      end
      else
      begin
        let expMax = mapVar absolutize expMax in
        let expTotal = mapVar absolutize expTotal in
        let finstanciedMax =  (expVaToExp( applyStoreVA(applyStoreVA (EXP(expMax)) contexte)globales))   in
      (*Printf.printf "ON ESSAYE D APPLIQUER LE CONTEXTE SUR: " ; print_expression finstanciedMax 0;*)
        let finstanciedTotal =   (expVaToExp(applyStoreVA (applyStoreVA (EXP(expTotal)) contexte)globales))  in
        let (nmax,ntotal) = getMaxTotalArraySizeDep finstanciedMax finstanciedTotal contexte globales in
        let instanciedMax =if (not exact) then nmax else (expMax) in
        let instanciedTotal = if (not exact) then ntotal else (expTotal) in
        let total = if (exact) then total else NOCOMP in
        let max = if (exact) then max else NOCOMP in

       (* Printf.printf "ON A COMPOSE la boucle ID %u , ca a donne total=%s max=%s\n" id
        (string_from_expr instanciedMax) (string_from_expr instanciedTotal);*)
              Loop ((id + (!idBoucle), line, source, exact, max, total, instanciedMax, instanciedTotal, expinit, sens, elt,elf),
           List.map evalAuxPasBoucle subtree)
      end  in
      (*dernierAppelFct := appelP;*)isexeEnglobant:= isexeEnglobantPred;
    res
    end in
   (*Printf.printf "ON A COMPOSE le compo ID %s\n" nomComp; *)
  (*print_string "ICI ON KONPOZE LE KONPOZAN \n";*)
  let mytree = getExpBornesFromComp nomComp in
  if dansBoucle then evalAuxBoucle mytree
  else
    (
      isexeEnglobant := isExecutedCall ;
      let res = evalAuxPasBoucle mytree in
      isexeEnglobant:= true;
      res
    )


and evalNidComposant id  appel  listeEng lt lf estexeEng globales expMax expTotal varLoop direction corpsCompo fic lig=
  (*Printf.printf "evalNidComposant NID av eval nid de %d \n" id  ;*)
  dernierAppelFct :=   !predDernierAppelFct;
    if existeDerniereBoucle listeEng then
    begin
      (* Printf.printf "il existe la boucle extern\n";*)
      let  (numTete, numApp, cont, isExe, ltB,lfB)=
      match !typeNidTeteCourant with TBOUCLE (numT, numA, _,cont, b, ltB,lfB,_,_) -> (numT, numA, cont, b, ltB,lfB)
                     |_-> (0, 0,[], false, [],[]) in
      listeAppels := [];
      let (numBouclePred, numAppBP, _, _,listeAvantAppel,_,_,_) =  rechercheDerniereBoucle listeEng in
      let borneP = rechercheNbTotalIti numBouclePred numAppBP !docEvalue.maListeNidEval in

      let (ouStopper, isExeE) =
        if numBouclePred = 0 then (numTete,estexeEng&&(isExecutedNidEval numTete numApp  !docEvalue.maListeNidEval) )
          else (numBouclePred, estexeEng&&  ( isExecutedNidEval numBouclePred numAppBP  !docEvalue.maListeNidEval)) in

      let varDeBouclePred = !varDeBoucleBoucle in
      varDeBoucleBoucle :=varLoop;

      let typeEval = TBOUCLE(id, !numAppel,   [] ,appel, true, [],[],fic , lig)
      in
      if (existeNid numTete) then
      begin
        if (existeNid numBouclePred) then
        begin
          let nidTETE = (rechercheNid numTete) in
          let nidPred = (rechercheNid numBouclePred) in
          let courcont =  cont in
          let nle = (List.append  [typeEval] listeEng) in
          (*Printf.printf "ici on appelle la fonction traiterboucleinternecomposant\n";*)
          (*dernierAppelFct := match  List.hd listeEng with TFONCTION(nomFonction,numA,_ , _, _,_,_,_,   _, _)->(nomFonction,numA)|->("UNKNOWN",0)*)
          dernierAppelFct :=  List.hd listeEng ;
           (*dernierAppelFct := !predDernierAppelFct;*)
          traiterBouclesInternesComposant
              nidTETE  nidPred  ouStopper
              id   (EXP( expTotal))
              courcont nle typeEval numAppBP (EXP(expMax))  isExeE lt lf borneP   false globales corpsCompo expMax  varLoop  direction id [];
          let nouNidEval = List.hd !nouBoucleEval in
          let borne  =  nouNidEval.expressionBorneToutesIt  in
          let maxi  =  nouNidEval.maxUneIt in
           varDeBoucleBoucle :=  varDeBouclePred;
          (expVaToExp borne,expVaToExp maxi)
        end
        else  (NOTHING, NOTHING)
      end
      else (NOTHING, NOTHING)
    end
    else (NOTHING, NOTHING)


(* jz: this function is called from evaluerFonctionsDuDoc, which is called in main,
       with quite some (un)initialized datastructures that are probably used here. *)
and evaluerFonction id f contexte exp listeEng typeA estexeEng globales et ef ptrInterval input =
        (*isPartialisationEval := !isPartialisation;*)
  let corpsEvalTMPPred = !corpsEvalTMP in
  corpsEvalTMP := [];
  let (corps, intoLoop, aff) =
    (match typeA with
      TFONCTION(_, _, lesAff, _, _, _, _, _, _, intoLoop, _, _) ->
        if intoLoop = true then
          ((*sansIfCorps*) f.corps.boucleOuAppel, true, lesAff)
        else
          (f.corps.boucleOuAppel, false, lesAff)
    | _ -> (f.corps.boucleOuAppel, false, [])
    ) in
        (*Printf.printf"Dans evaluerFonction %s  \n"id;
        if intoLoop = false then LocalAPContext.print ptrInterval;*)
        (*let aff =
            if !varDeBoucleBoucle = "" then f.lesAffectations
            else reecrireCallsInLoop !varDeBoucleBoucle f.lesAffectations in*)
        (*Printf.printf"Dans evaluerFonction %s  \nLES AFFECTATIONS"id;
        afficherLesAffectations (aff) ;
        new_line () ;
        Printf.printf"Dans evaluerFonctions FIN  \n";*)
    if intoLoop = false then begin
      let (inp, used) =
        if AFContext.mem !myAF id = false then
          ([],[])
        else begin
          let f = AFContext.get !myAF id in
          (MyFunCont.getInput f, MyFunCont.getOtherUsed f)
        end
      in
      let poiteurUsed = List.filter (fun x-> if isPtr x then true else false) used in
      let inputPtrParam = (getParamPtr input) in
      myCurrentPtrContext := localPtrAnalyse input ptrInterval false true ;
      myCurrentPtrContext := LocalAPContext.initIntervalAnalyse poiteurUsed inputPtrParam !myCurrentPtrContext !globalPtr;
      (*myCurrentPtrContext := LocalAPContext.unionPL !myCurrentPtrContext ptrInterval globalPtr inputPtrParam*)
      (*LocalAPContext.print !myCurrentPtrContext;*)
    end;
    let (new_contexte, next, new_globales) =
                  evalCorpsFOB corps aff contexte listeEng estexeEng [] intoLoop globales in
    let corpsEvalPourAppel = !corpsEvalTMP in
    let new_fct = [new_elementEvala typeA exp corpsEvalPourAppel et ef] in
    corpsEvalTMP := List.append corpsEvalTMPPred new_fct;
    docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_fct);
    let endOfcontext =
      if intoLoop then
        new_contexte
      else
        filterwithoutWH new_contexte
    in
    (endOfcontext, next, new_globales)


and completevaluerFonction id f contexte exp listeEng typeA estexeEng globales et ef ptrInterval input =
  let lesAff =
  (match typeA with
    TFONCTION(_,_, lesAff, _, _, _, _, _, _, intoLoop, _, _) -> lesAff
  | _ -> []
  ) in
  let (new_contexte, next, new_globales) = evaluerFonction id f contexte exp listeEng typeA estexeEng globales et ef ptrInterval input in
  endOfcontexte lesAff next new_contexte new_globales


and evalNid nid appel (*appelée pour une mere de nid*) listeEng lt lf estexeEng globales fic lig =
  if !vDEBUG then
    Printf.eprintf "evalNid NID av eval nid de %d \n" (getBoucleIdB nid.infoNid.laBoucle);
  dernierAppelFct := !predDernierAppelFct;
  let info = getBoucleInfoB nid.infoNid.laBoucle in
  let mesBouclesOuAppel = (*sansIfCorps*) info.boucleOuAppelB in
  if !estDansBoucle = false then begin
        (*isExecutedOneTimeOrMoreList := [];*)
    let prevPtr = !myCurrentPtrContextAux in
    let aSC = appel in
    if !vDEBUG then
      Printf.eprintf "evalNid contexte  boucle: \n";
          (* afficherListeAS aSC;flush(); space(); new_line();
             Printf.printf "evalNid contexte  boucle: tete\n";
             Printf.printf "FIN CONTEXTE globale \n";*)
          (* if lt <> [] then begin Printf.printf "liste des variables IF true :\n"; List.iter (fun e-> Printf.printf "%s "e) lt end;
             if lf <> [] then begin Printf.printf "liste des variables IF false :\n"; List.iter (fun e-> Printf.printf "%s "e) lf end;*)
    let listeInstNonexePred = !listeInstNonexe in
    listeInstNonexe :=[];
    if !vDEBUG then
      Printf.eprintf "NID av eval nid de %d pas dans autre boucle\n" (getBoucleIdB nid.infoNid.laBoucle);
    let (vt, vf) = creerVarTF lt lf appel [] in
    let isExe = estexeEng && isExecuted lt lf aSC [] [] true in
          (*  Printf.printf"evalNid : valeur de isexe dans evalboucel pas dans autre : ";
              if isExe then Printf.printf"vrai\n" else Printf.printf"false\n" ;*)
    estDansBoucle := true;
    estDansBoucleE :=!estDansBoucle;
    let varDeBouclePred = !varDeBoucleBoucle in
    varDeBoucleBoucle := nid.varDeBoucleNid;
            (*let id = nid.varDeBoucleNid in*)
    let corpsEvalTMPPred = !corpsEvalTMP in
    corpsEvalTMP := [];
            (*print_expression nid.infoNid.expressionBorne 0;*)
    let nb =
      if isExe then
        (getNombreIt (nid.infoNid.expressionBorne)
              info.conditionConstante info.typeBoucle  info.conditionI info.conditionMultiple
              aSC info.estPlus  info.infoVariation nid.varDeBoucleNid globales)
       else
        EXP(CONSTANT (CONST_INT "0")) in
            (*print_expVA (EXP(info.conditionI)); flush(); space(); new_line ();*)
    let num = getBoucleIdB nid.infoNid.laBoucle in
    let typeEval = TBOUCLE(num, !numAppel,
          (reecrireCallsInLoop nid.varDeBoucleNid nid.lesAffectationsBNid),
              aSC, isExe, [], [], fic, lig)
    in
    if !vDEBUG then begin
      Printf.eprintf "evalNid contexte  boucle: %d\n" (getBoucleIdB nid.infoNid.laBoucle);
            (*afficherListeAS aSC;flush(); space(); new_line()*)
    end;
    let nouNidEval =
      new_nidEval typeEval nb (* borne total *) (nid.varDeBoucleNid)
          info.infoVariation.direction nb isExe false 0
    in
    docEvalue := new_documentEvalue (List.append [ nouNidEval] !docEvalue.maListeNidEval) !docEvalue.maListeEval;
    if !vDEBUG then
      Printf.eprintf "av evaluerSN de %d dans nid tete appel %d\nNEW NID EVAL\n" (getBoucleIdB nid.infoNid.laBoucle) !numAppel;
                (*ignore (afficherNidUML nouNidEval  [] 1 Listener.null);*)
    isExecutedOneTimeOrMoreList := [];
    compNotInnerDependentLoop nouNidEval false;
    let borne = nouNidEval.expressionBorneToutesIt in
    let tetePred = (TBOUCLE(0,0,[],[], true,[],[],"",0)) in
    typeNidTeteCourant := typeEval;
    let resaux = calculer nb  !infoaffichNull [] 1 in
    let isNull =
      if estDefExp resaux then
        if getDefValue resaux <= 0.0 then
          true
        else false
      else
        false
    in
    let fc = (localPtrAnalyse [new_instBEGIN (nid.lesAffectationsBNid)] prevPtr true false) in
    memAssign nid.lesAffectationsBNid;
    myChangeUpperLoopPtr := !myChangePtr;
                    (*List.iter(fun x -> Printf.printf "ptr %s  " x)!myChangePtr ;*)
    myCurrentPtrContext := (LocalAPContext.joinSet fc prevPtr);
    let previsExecutedOneTimeOrMore = !isExecutedOneTimeOrMore in
                  (*let listeSauf =*)
    evaluerSN nid nid aSC mesBouclesOuAppel (List.append [typeEval] listeEng) isExe borne nid globales;
                  (*  resaux in*)
    isExecutedOneTimeOrMore := previsExecutedOneTimeOrMore;
    if !vDEBUG then
      Printf.eprintf "ap evaluerSN de %d dans nid tete appel %d\n" (getBoucleIdB nid.infoNid.laBoucle) !numAppel;
    typeNidTeteCourant := tetePred;
    let corpsEvalPourB = !corpsEvalTMP  in
    let new_b = [new_elementEvalb nouNidEval typeEval corpsEvalPourB vt vf] in
    corpsEvalTMP := List.append corpsEvalTMPPred new_b;
    docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_b);
    if !vDEBUG then
      Printf.eprintf "ajout dans liste corpsEval %d\n" (getBoucleIdB nid.infoNid.laBoucle);
    varDeBoucleBoucle := varDeBouclePred;

    estDansBoucle := false; estDansBoucleE :=!estDansBoucle;
    if isExe && isNull = false then begin
                (*Printf.printf "est exécutée\n" ;*)
                (*supprimer tous ceux qui sont dans  !listeInstNonexe; *)
      let ni = reecrireCorpsNonExe  nid.lesAffectationsBNid !listeInstNonexe !numAppel in
      listeInstNonexe := listeInstNonexePred;
      firstLoop := 0;
      listeDesVarDependITitcour := [];
      let fc = (localPtrAnalyse [new_instBEGIN (ni)] prevPtr true false) in
      let nc = (LocalAPContext.joinSet fc prevPtr ) in
                (*Printf.printf "CONTEXTE RES ajout dans liste corpsEval %d \n"  (getBoucleIdB nid.infoNid.laBoucle) ;
                afficherUneAffect (new_instBEGIN ni); Printf.printf "evalSIDA fin\n";(* c'est bien une instruction for*)*)
      estDansBoucleLast := false;(* to omit some Rond of abstact store *)
      let res = evalStore ((new_instBEGIN ni)) aSC globales nc in
      estDansBoucleLast := true ;
                (*afficherListeAS res;flush(); space(); new_line();*)
      res
    end else begin (* Printf.printf "n'est pas exécutée\n";*)
      myCurrentPtrContext:=prevPtr;
      listeInstNonexe := List.append [typeEval] listeInstNonexePred;
      appel;
    end
  end else begin
                (*Printf.printf "EVAL evaluation fonction nid dans boucle\n"  ;*)
                (*Printf.printf "contexte appel boucle\n" ; afficherListeAS appel; Printf.printf "fin contexte\n" ;*)
                (* Printf.printf "NID av eval nid de %d dans autre boucle\n" (getBoucleIdB nid.infoNid.laBoucle)  ;*)
                (*if lt <> [] then begin Printf.printf "liste des variables IF true :\n"; List.iter (fun e-> Printf.printf "%s "e) lt end;
                if lf <> [] then begin Printf.printf "liste des variables IF false :\n"; List.iter (fun e-> Printf.printf "%s "e) lf end;*)
    if existeDerniereBoucle listeEng then begin
      let (numTete, numApp, cont, isExe, ltB,lfB) =
      match !typeNidTeteCourant with
        TBOUCLE (numT, numA, _,cont, b, ltB,lfB,_,_) -> (numT, numA, cont, b, ltB,lfB)
      | _ -> (0, 0,[], false, [],[]) in
      listeAppels := [];
      let (numBouclePred, numAppBP, _, _, listeAvantAppel, _, _, _) = rechercheDerniereBoucle listeEng in
      let borneP = rechercheNbTotalIti numBouclePred numAppBP !docEvalue.maListeNidEval in
      let (ouStopper, isExeE) =
        if numBouclePred = 0 then
          (numTete,estexeEng&&(isExecutedNidEval numTete numApp  !docEvalue.maListeNidEval) )
        else
          (numBouclePred, estexeEng && (isExecutedNidEval numBouclePred numAppBP !docEvalue.maListeNidEval)) in
      (*  if isExeE then Printf.printf"isExeE = vrai\n" else Printf.printf"isExeE = false\n" ;*)
      let varDeBouclePred = !varDeBoucleBoucle in
      varDeBoucleBoucle := nid.varDeBoucleNid;
      let typeEval = TBOUCLE((getBoucleIdB nid.infoNid.laBoucle), !numAppel,
        (reecrireCallsInLoop nid.varDeBoucleNid nid.lesAffectationsBNid), appel, isExeE, lt, lf, fic, lig)
      in
      if (existeNid numTete) then begin
        if (existeNid numBouclePred) then begin
          let nidTETE = (rechercheNid numTete) in
          let nidAppel = (rechercheNid numBouclePred) in
          let info = (getBoucleInfoB nid.infoNid.laBoucle) in
          let valBorne =  if isExeE then getNombreIt (nid.infoNid.expressionBorne)
                     info.conditionConstante info.typeBoucle  info.conditionI
                    info.conditionMultiple [] info.estPlus  info.infoVariation nid.varDeBoucleNid []
        else
          EXP(CONSTANT (CONST_INT "0")) in
          let courcont =  cont in
                 (* Printf.printf "NID av eval nid de %d dans autre boucle\n" (getBoucleIdB nid.infoNid.laBoucle) ;*)
                 (* Printf.printf "valeur initiale borne :\n";print_expVA valBorne; new_line();*)
          let nle = (List.append [typeEval] listeEng) in
          traiterBouclesInternes nidTETE nidAppel ouStopper
                  (* le noeud englobant où il faut s'arreter ici id boucle englobante *)
              (getBoucleIdB nid.infoNid.laBoucle)
              (*(EXP(n.infoNid.expressionBorne)) *)valBorne
              courcont nle typeEval numAppBP valBorne isExeE lt lf borneP false globales nid.infoNid.expressionBorne nid.varDeBoucleNid info.infoVariation.direction
              (getBoucleIdB nid.infoNid.laBoucle) [] false;
              (*Printf.printf "NEW NID EVAL\n";*)
          let nouNidEval = List.hd !nouBoucleEval in
          let borne = nouNidEval.expressionBorneToutesIt in
          let corpsEvalTMPPred = !corpsEvalTMP in
          corpsEvalTMP := [];
              (*Printf.printf "NEW NID EVAL\n";*)
          if !vDEBUG then
            Printf.eprintf "evalNid av evaluerSN de %d dans nid tete %d appel %d\n" (getBoucleIdB nid.infoNid.laBoucle)
              (getBoucleIdB nidTETE.infoNid.laBoucle) !numAppel;
          let previsExecutedOneTimeOrMore = !isExecutedOneTimeOrMore in
          evaluerSN nidTETE nid courcont mesBouclesOuAppel (List.append [typeEval] listeEng) isExeE borne nid globales;
          isExecutedOneTimeOrMore := previsExecutedOneTimeOrMore;
          if !vDEBUG then
            Printf.eprintf "ap evaluerSN de %d dans nid tete appel %d\n" (getBoucleIdB nid.infoNid.laBoucle)  !numAppel;
            let corpsEvalPourB = !corpsEvalTMP in
            let (vt, vf) = creerVarTF lt lf [] [] in
            let new_b = [new_elementEvalb nouNidEval typeEval corpsEvalPourB vt vf] in
            corpsEvalTMP := List.append corpsEvalTMPPred   new_b;
            docEvalue := new_documentEvalue !docEvalue.maListeNidEval (List.append !docEvalue.maListeEval new_b);
          varDeBoucleBoucle :=varDeBouclePred;
          appel
        end else begin
          if !vDEBUG then Printf.eprintf "pb pas de nid pour boucle %d :\n" numTete;
            appel
          end
        end else begin
          if !vDEBUG then Printf.eprintf "pb pas de nid pour boucle %d :\n" numBouclePred;
          appel
        end
      end else begin
        if !vDEBUG then Printf.printf "evalNID : pas de boucle englobante  boucle autre nid\n";
        appel
      end
    end;
            (*if !vDEBUG then Printf.printf "ap evalNID\n" ;*)


(*  pour toutes les boucles b de niveau niveau courant +1 du nid N faire  RechercherExpressionTNj B N *)
and evaluerSN nid  (*tete*) niddepart (*tete niveau courant *) appel mesBouclesOuAppel listeEng isExeE borne tetePred globales=
List.iter
  (fun  c ->
    match c with
      IDBOUCLE (num,lt,lf,fic,lig) ->
       (* if !vDEBUG then Printf.eprintf "NID av sous nid de %d  dans autre boucle\n" num;*)
 

	
        (*if isExeE then Printf.printf"evaluerSN isExeE = vrai\n" else Printf.printf"evaluerSN isExeE = false\n" ;*)
(*Printf.printf "evaluerSN contexte dans eval sous nid: \n";
          afficherListeAS appel;
          Printf.printf "FIN CONTEXTE \n";*)


        let ((*listeId*)_,_,n) = rechercheTriplet num niddepart.listeTripletNid in
        if !vDEBUG then
          Printf.eprintf "1 evaluerSN nid %d nid depart %d  nid tete %d\n" num  (getBoucleIdB niddepart.infoNid.laBoucle)
          (getBoucleIdB nid.infoNid.laBoucle);
        let varDeBouclePred = !varDeBoucleBoucle in
        varDeBoucleBoucle :=n.varDeBoucleNid;
        if  (getBoucleInfoB (n.infoNid.laBoucle)).infoVariation.operateur = NE then listNotEQ := List.append [(fic,lig)] !listNotEQ;
        let info = getBoucleInfoB n.infoNid.laBoucle in
        let valBorne =  if isExeE then getNombreIt (n.infoNid.expressionBorne)
                  info.conditionConstante  info.typeBoucle  info.conditionI
                  info.conditionMultiple [] info.estPlus  info.infoVariation n.varDeBoucleNid []
                else EXP(CONSTANT (CONST_INT "0")) in

 (*if num = 62 then print_expVA ( (valBorne)); flush(); space(); new_line (); *)

        let corps = (*sansIfCorps*) info.boucleOuAppelB in
        let typeEval = TBOUCLE ( (getBoucleIdB n.infoNid.laBoucle), !numAppel,
          (reecrireCallsInLoop n.varDeBoucleNid   n.lesAffectationsBNid ),appel, isExeE, lt,lf,fic,lig) in
        dernierAppelFct := !predDernierAppelFct;

        traiterBouclesInternes
              nid (*le noeud complet qui la contient *)
              niddepart (* noeud courant *)
              ((getBoucleInfoB (n.infoNid.laBoucle)).nomEnglobante)
              (* le noeud englobant où il faut s'arreter ici id boucle englobante *)
              (getBoucleIdB n.infoNid.laBoucle)  (*sous noeud conserné*)
              (*(EXP(n.infoNid.expressionBorne)) *)valBorne
              appel listeEng typeEval !numAppel valBorne isExeE lt lf borne   false globales  n.infoNid.expressionBorne n.varDeBoucleNid info.infoVariation.direction  (getBoucleIdB n.infoNid.laBoucle) [] false;
(*Printf.printf "NEW NID EVAL 2\n";*)
        let nouNidEval = List.hd !nouBoucleEval in
        let borneN  =  nouNidEval.expressionBorneToutesIt  in
        let corpsEvalTMPPred = !corpsEvalTMP in
        corpsEvalTMP := [];
(* Printf.printf "AP EVALUERSN ajout sousnid de %d = %d
          dans liste des boucle de %d\n" (getBoucleIdB n.infoNid.laBoucle)
          num (getBoucleIdB nid.infoNid.laBoucle);*)
        if !vDEBUG then
        begin
          Printf.eprintf "av eval sous nid de %d\n" (getBoucleIdB n.infoNid.laBoucle) ;
          Printf.eprintf "CORPS boucles %d:\n"  (getBoucleIdB n.infoNid.laBoucle);
          (*afficherElementCorpsFonction corps ;  *)
          Printf.eprintf "av eval sous nid de FIN%d\n"  (getBoucleIdB n.infoNid.laBoucle) ;
          (*Printf.printf "contexte dans eval sous nid: \n";
          afficherListeAS appel;
          Printf.printf "FIN CONTEXTE \n"*)
        end;

        let previsExecutedOneTimeOrMore = !isExecutedOneTimeOrMore in
        evaluerSN nid (*tete*) n  appel  corps  (* passer au niveau suivant *) (List.append [typeEval] listeEng) isExeE   borneN tetePred globales;

        isExecutedOneTimeOrMore := previsExecutedOneTimeOrMore;
         if !vDEBUG then
          Printf.eprintf "AP EVALUERSN ajout sousnid de %d = %d
          dans liste des boucle de %d\n" (getBoucleIdB n.infoNid.laBoucle)
          num (getBoucleIdB nid.infoNid.laBoucle);
          (*  if lt <> [] then begin Printf.printf "liste des variables IF true :\n";
          List.iter (fun e-> Printf.printf "%s "e) lt end;
          if lf <> [] then begin Printf.printf "liste des variables IF false :\n";
          List.iter (fun e-> Printf.printf "%s "e) lf end;*)
        let (vt, vf) =    creerVarTF lt lf [] []   in
        let new_b =  new_elementEvalb nouNidEval typeEval !corpsEvalTMP vt vf in
        corpsEvalTMP := List.append corpsEvalTMPPred  [ new_b];

        varDeBoucleBoucle :=varDeBouclePred;()
      | IDAPPEL (_,_,_,_,_,_,_,_)
      | IDIF (_,_, _,_, _,_,_,_,_,_,_,_)-> (* Printf.printf"reecrire corps boucle appel ou\n";*)
        let _ =
          evalUneBoucleOuAppel c (reecrireCallsInLoop niddepart.varDeBoucleNid niddepart.lesAffectationsBNid ) appel listeEng isExeE  [] globales in ()
     (* |IDIF (_,_, treethen,_, treeelse,_,_)->  Printf.printf "av eval sous nid de DANS IF %d\n" (getBoucleIdB niddepart.infoNid.laBoucle) ;
      evaluerSN nid  (*tete*) niddepart (*tete niveau courant *) appel treethen listeEng isExeE borne tetePred;
      evaluerSN nid  (*tete*) niddepart (*tete niveau courant *) appel treeelse listeEng isExeE borne tetePred;()*)

  ) mesBouclesOuAppel



let printendExp l =
  if List.mem NOTHING l then
    Printf.printf "NOTHING"
  else
    print_commas
  true
  (fun max -> print_expression (remplacerNOTHINGParAux(EXP(max))) 0) l


let afficherInfoFonctionDuDocUML listeF =
  compEvalue := List.rev !compEvalue;
  if listeF <> [] then begin
    valeurEng :=  NOCOMP ;
    borneAux :=  NOCOMP ;
    estNulEng := false;
    isExactEng := true;
    listeDesMaxParIdBoucle :=[];
    let result = (Listener.onBegin Listener.null) in
    match ((rechercherEvalParNomAppel !(!mainFonc) 0 0 listeF)) with
      APPELEVAL(tyc,_,corps,_,_) ->
        let (isExe,isInLoop) =
          (match tyc with
            TFONCTION(_, _,_,_,_, _,_,_,a,b,_,_) ->
              (a,b)
          | _ ->
              (true,false)) in
              let isExtern = (not (existeFonctionParNom !(!mainFonc) doc)) in
              let result = Listener.onFunction result !(!mainFonc) isInLoop isExe isExtern in
              let result = if (not isExtern) then (afficherCorpsUML corps 5 true result) else result in
              let result = Listener.onFunctionEnd result in
              let result = Listener.onEnd result in
              Printf.printf"\n<loopsfacts>\n";
              if ( !listeDesMaxParIdBoucle != [] ) then
                List.iter(
                  fun (id,max) ->
                    Printf.printf "\t <loopId=\"%d\" maxcountAnyCalls=\"" id ;
                    (match max with
                      EVALEXP(oldMax) ->
                        if estDefExp oldMax then begin
                          print_expTerm oldMax;
                          Printf.printf  "\" >"   ; new_line();
                        end else begin
                          Printf.printf "NOCOMP\" expmaxcountAnyCalls=\"maximum(";
                          print_expTerm oldMax;
                          space();
                          flush();
                          Printf.printf ")\" >";
                          new_line();
                        end;
                    | EXPMAX(l) ->
                        Printf.printf "NOCOMP\" expmaxcountAnyCalls=\"maximum(";
                        printendExp l;
                        space();
                        flush();
                        Printf.printf  ")\" >";
                        new_line();)
                ) !listeDesMaxParIdBoucle;
                Printf.printf "</loopsfacts>\n";
                flush();
                new_line();
                result
      | _ -> Listener.null;
  end else
    Listener.null
  let ptrInterval = ref []
  let integerInterval = ref []


let evaluerFonctionsDuDoc doc =
  (* jz FIXME :: should be done only once!! should be reset afterwards NO becose partial anyse??? *)
  List.iter (fun (_, f) -> allFunctionNames := !allFunctionNames @ [f.nom] ; () ) !doc.laListeDesFonctions;

  if !doc.laListeDesFonctions <> [] then begin
    if existeFonctionParNom !(!mainFonc) doc = false then
      failwith "no entry point";

    let (_, f) = (rechercherFonctionParNom !(!mainFonc) doc) in
    listeASCourant := [];
    globalesVar := !alreadyAffectedGlobales;
    (*!listeDesVolatiles;*)
            (*Printf.printf"GLOBALE\n";
            List.iter(fun var->Printf.printf "%s  "var)!globalesVar;*)
            (*Printf.printf"Dans evaluerFonction %s  \nLES AFFECTATIONS" !(!mainFonc);
            Printf.printf"Dans evaluerFonctionsDuDoc  \n";
            afficherLesAffectations (  f.lesAffectations) ;new_line () ;*)
            (*
            Printf.printf"GLOBALE\n";
            afficherLesAffectations (!listeDesEnum) ;new_line () ;new_line () ;flush(); space();
            Printf.printf"FIN GLOBALE\n";*)
   (* Printf.printf"Dans evaluerFonction %s  \nLES AFFECTATIONS" !(!mainFonc);*)
    let globalInst(*TMP*) =
      if !notwithGlobalAndStaticInit = false then
        (myCurrentPtrContext := (localPtrAnalyse [new_instBEGIN (!listeDesInstGlobales)] [] false true);
        ptrInterval := !myCurrentPtrContext;
        !listeDesInstGlobales)
      else
        (myCurrentPtrContext := (localPtrAnalyse [new_instBEGIN (!listeLocalStatic)] [] false true);
        ptrInterval := !myCurrentPtrContext;
        List.append (!listeLocalStatic) (!listeDesEnum))
    in

    (* jz: TODO :: remove -- old scenario code piece
      let scenGlobalInst = scenarioAsGlobalInsts in
      let sortedGlobalInst = List.sort cmpScenarioInstAsgns globalInstTMP in
      let sortedScenarioInst = List.sort cmpScenarioInstAsgns !scenarioAsGlobalInsts in
      let globalInst = mergePrioA_AB_Insts sortedScenarioInst sortedGlobalInst [] in
    *)
    let typeE = TFONCTION(!(!mainFonc), !numAppel, f.lesAffectations, globalInst, [], [], [], [], true, false,"",0) in
    dernierAppelFct := typeE;
    predDernierAppelFct := typeE;
    let (_, _, _) = evaluerFonction !(!mainFonc) f [] (EXP(NOTHING)) [typeE] typeE true
                          (evalStore (new_instBEGIN globalInst) [] [] []) (* globales in evaluerFonction *)
                          (( CONSTANT(CONST_INT("1")))) (( CONSTANT(CONST_INT("0"))))
                          !ptrInterval [](*input*) in ()

  end else ()

let rec afficherFonction corps tab =
  List.iter (fun a->
    match a with
      IDBOUCLE (num, _, _, _, _) ->
        if (existeBoucle num) then begin
          estNulEng := false;
          let b = rechercheBoucle num in
          Printf.printf "lOOP nid...%d\n" num;
          afficherFonction (getBoucleInfoB b).boucleOuAppelB (tab+3);
          Printf.printf "end lOOP nid...%d \n" num;
        end else
          Printf.printf "lOOP nid %d non trouve\n" num;
    | IDIF (var, _, treethen, _, treeelse, _, _, _, _, _, _, _) ->
        Printf.printf "IF...%s\n" var;
        afficherFonction treethen (tab+3);
        Printf.printf "IF...%s else\n" var;
        afficherFonction treeelse (tab+3);
    | IDAPPEL (numf, appel, _, _, _, _, fichier, ligne) ->
        let nomFonction =
          (match appel with
            CALL(exp, _) ->
              (match exp with
                VARIABLE(nomFct) -> nomFct
              | _ -> "")
            | _ -> "")
        in
        print_tab (tab);
        Printf.printf  "<call name=\"%s\" numcall=\"%d\"" nomFonction numf;
        if (existeFonctionParNom nomFonction doc) = false then begin
          Printf.printf " line=\"%d\" source=\"%s\" extern=\"true\">" ligne fichier
        end else begin
          Printf.printf " line=\"%d\" source=\"%s\">" ligne fichier;
          new_line();
          let (_, f) = (rechercherFonctionParNom nomFonction doc) in
          afficherFonction f.corps.boucleOuAppel (tab+3)
        end;
        print_tab (tab);
        Printf.printf "</call>";
        new_line()
  ) corps


let afficherFonctionsDuDoc doc =
  Printf.printf "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
  Printf.printf "<flowfacts>\n";
  if !doc.laListeDesFonctions <> [] then begin
    if existeFonctionParNom !(!mainFonc) doc then begin
      let (_, f) = (rechercherFonctionParNom !(!mainFonc) doc) in
      print_tab (1);
      Printf.printf "<function name=\"%s\">" !(!mainFonc);
      new_line ();
      afficherFonction f.corps.boucleOuAppel 5;
      print_tab (1);
      Printf.printf "</function>";
    end else
      Printf.printf "\n/* \t fonction main %s  non trouvee*/\n" !(!mainFonc)
    end;
    new_line();
    Printf.printf "</flowfacts>";
    new_line()

let listCaseFonction = ref []
let existCaseAssosFunction id = List.mem_assoc id  !listCaseFonction
let getCaseAssosFunction id = List.assoc id !listCaseFonction
let setNewCaseAssosFunction id na =
List.map (fun (idFunc, list) -> if id = idFunc then (id,na) else (idFunc,list))! listCaseFonction
let rec consCaseCorpsFOB corps numCall =
List.iter (fun e -> consCaseUneBoucleOuAppel e numCall) corps


and oneFunction id numCall lt lf =
  if existCaseAssosFunction id then begin
    let old_assos = getCaseAssosFunction id in
    listCaseFonction := setNewCaseAssosFunction id (List.append [(lt,lf)] old_assos);
  end else
    listCaseFonction := List.append [(id, [(lt,lf)])] !listCaseFonction;

and consCaseUneBoucleOuAppel elem numCall =
  match elem with
    IDBOUCLE (num, lt,lf,_,_) ->
      if (existeNid num) then begin
        let nid = (rechercheNid num) in
        let info = getBoucleInfoB nid.infoNid.laBoucle in
        let mesBouclesOuAppel = info.boucleOuAppelB in
        consCaseCorpsFOB mesBouclesOuAppel  numCall
      end
  | IDIF (_,_, treethen,_, treeelse,_,_,_,_,_,_,_)->
      consCaseCorpsFOB treethen numCall;
      consCaseCorpsFOB treeelse numCall
  | IDAPPEL (numf,appel,_,_, lt,lf,_,_) ->
      let nomFonction =
      (match appel with
        CALL(exp,_) ->
          (match exp with
            VARIABLE(nomFct) ->
              nomFct
          | _ -> "")
        | _ -> "")
      in
      if existeFonctionParNom nomFonction doc then begin
        let (_, func) = (rechercherFonctionParNom nomFonction doc) in
        consCaseFonction nomFonction func (numCall+1) lt lf;
      end else
        oneFunction nomFonction numCall lt lf


and consCaseFonction id f numCall lt lf =
  oneFunction id numCall lt lf;
  consCaseCorpsFOB f.corps.boucleOuAppel numCall

let evaluerCaseFonctionsDuDoc doc=
  if !doc.laListeDesFonctions <> [] then begin
    let (_, f) = (rechercherFonctionParNom !(!mainFonc) doc) in
    consCaseFonction !(!mainFonc) f 0 (*num appel*) [] []
  end

let printFuncCaseAssos l =
  List.iter(fun (name,listCase) ->
    Printf.printf "\n%s :\n" name;
    List.iter(fun (lt,lf) ->
      Printf.printf "\tIF true : ";
      List.iter (fun e-> Printf.printf "%s "e) lt;
      Printf.printf "\tIF false : ";
      List.iter (fun e-> Printf.printf "%s "e) lf;
      Printf.printf "\n"
    ) listCase
  ) l

let rec consProdT l b=
if b then
  if List.tl l = [] then VARIABLE( List.hd l) else  BINARY (MUL , VARIABLE(  List.hd l), consProdT (List.tl l) b)
else
  if List.tl l = [] then UNARY( NOT ,VARIABLE( List.hd l)) else  BINARY (MUL ,  UNARY( NOT ,VARIABLE( List.hd l)), consProdT (List.tl l) b)

let consProdCaseTF lt lf=
if lt = [] && lf = [] then CONSTANT( CONST_INT "1")
else if lt = [] then consProdT lf false
   else if lf = [] then consProdT lt true else BINARY(MUL, consProdT lt true,consProdT lf false)

let nb_loop = ref 0
let rec consNbCorpsFOB corps numCall evalFunction =

  if corps = [] then CONSTANT( CONST_INT "0")
  else
  begin
    let v1 = consNBUneBoucleOuAppel (List.hd corps) numCall evalFunction in
    let v2 = consNbCorpsFOB (List.tl corps) numCall evalFunction in

    let add1 = calculer (EXP(v1)) !infoaffichNull [] 1 in
    let add2 = calculer (EXP(v2)) !infoaffichNull [] 1 in
    if estNoComp add1 = false && estNoComp add2 =false &&  estNul add1 && estNul add2 then CONSTANT( CONST_INT "0")
      else if estNoComp add1 = false && estNul add1 then v2 else if estNoComp add2 = false && estNul add2 then v1 else BINARY(ADD, v1 ,v2)
  end



and consNBUneBoucleOuAppel elem  numCall evalFunction=
match elem with
  IDBOUCLE (num, lt,lf,_,_) ->
    let nvar =  Printf.sprintf "%s_%d_%d_%d" "NBIT" num numCall  !nb_loop in
    nb_loop := !nb_loop +1;
    if  (existeBoucle   num) then
    begin
      let b = (rechercheBoucle  num) in
      let info = getBoucleInfoB b in
      let mesBouclesOuAppel = info.boucleOuAppelB in
      let profinit = consProdCaseTF lt lf in
      let valprod = (calculer  (EXP(profinit)) !infoaffichNull  [] 1) in
      let prod = if estNoComp valprod = false && estUn valprod then VARIABLE(nvar) else
           BINARY(MUL, VARIABLE(nvar), profinit) in
      let other = consNbCorpsFOB mesBouclesOuAppel  numCall evalFunction in
      let value = calculer  (EXP(other)) !infoaffichNull  [] 1 in
      if estNoComp value = false &&  estNul value then CONSTANT( CONST_INT "0")
      else if estNoComp value = false &&  estUn value then prod else  BINARY(MUL,prod,other)
    end
    else begin  CONSTANT( CONST_INT "0") end
   |IDIF (_,_, treethen,_, treeelse,_,_,_,_,_,_,_)->
  let nb1 = consNbCorpsFOB  treethen  numCall evalFunction in
  let nb2 = consNbCorpsFOB  treeelse numCall evalFunction in
  let res = CALL(VARIABLE("MAXIMUM"), List.append [nb1] [nb2]) in
  let value = calculer  (EXP(res)) !infoaffichNull  [] 1 in
      if estNoComp value = false   then expressionEvalueeToExpression value
      else res
  | IDAPPEL (numf,appel,_,_, lt,lf,_,_) ->
    let nomFonction =
      (match appel with
        CALL(exp,_)->(match exp with VARIABLE(nomFct)->nomFct|_-> "")|_->"") in
        if existeFonctionParNom nomFonction doc then
        begin
          let (_, func) = (rechercherFonctionParNom nomFonction doc) in
          consNBFonction nomFonction func numf lt lf evalFunction;
        end
        else if nomFonction = evalFunction then  consProdCaseTF lt lf else CONSTANT( CONST_INT "0")



and consNBFonction id f   numCall lt lf evalFunction=
  let prod = consProdCaseTF lt lf in
  let valprod = (calculer (EXP(prod)) !infoaffichNull [] 1) in
  if id = evalFunction then
    (* fonction non recursive*) prod
  else begin
    let other =consNbCorpsFOB f.corps.boucleOuAppel   numCall evalFunction in
    let value = calculer  (EXP(other)) !infoaffichNull  [] 1 in
    if estNoComp value = false && estNul value then
      CONSTANT( CONST_INT "0")
    else
      if estNoComp value = false && estUn value then
        prod
      else
        if estNoComp valprod = false && estUn valprod then
          other
        else
          BINARY(MUL,prod,other)
  end


let evaluerOneFunctionOfDoc doc evalFunction=
  if !doc.laListeDesFonctions <> [] then begin
    if existeFonctionParNom !(!mainFonc) doc  = false then
      failwith "no entry point";
    let (_, f) = (rechercherFonctionParNom !(!mainFonc) doc) in
    nb_loop :=  0;
    Printf.printf "\nnbofCallof %s = \n" evalFunction ;
    print_expression (consNBFonction !(!mainFonc) f 0 [] [] evalFunction) 0;
    flush();
    space();
    new_line();
    flush();
    space();
    new_line()
  end


let rec evaluerNbFunctionOfDoc doc evalFunction l2 =
  match evalFunction with
    []-> new_line()
  | e::l -> begin
      trace := true;
        if List.mem !e l2 = false then
          evaluerOneFunctionOfDoc doc !e;
          evaluerNbFunctionOfDoc doc l (!e::l2);
          flush();
          space();
          new_line()
        end


let initref (result : out_channel) (defs : file) =
  out := result;
  enTETE :=  false;
  numAppel :=  0;
  estNulEng :=  false;
  estDansBoucle :=  false; estDansBoucleE :=!estDansBoucle;
  varDeBoucleBoucle := "";
  isPartialisation := false;
  isPrint_Expression := false;
  analyse_defsPB defs(*;
  print_AssosIdLoopRef  !listLoopIdRef;
  print_listIdCallFunctionRef !listIdCallFunctionRef*)

let initorange verbose =
if verbose then Printf.printf "reset initorange";
  compEvalue := [];
  listeAppels := [];
  cptFunctiontestIntoLoop :=  0;
  enTETE := false;
  numAppel := 0;
  estNulEng := false;
  estDansBoucle := false; estDansBoucleE :=!estDansBoucle;
  varDeBoucleBoucle :="";
  listeDesMaxParIdBoucle :=  [];
  typeNidTeteCourant := (TBOUCLE(0,0,[], [], true,[], [],"",0));
  dernierAppelFct := (TFONCTION(!(!mainFonc),0,[], [], [], [],[], [], true,false,"",0));
  predDernierAppelFct := (TFONCTION(!(!mainFonc),0,[], [], [], [],[],[], true, false,"",0));
  corpsEvalTMP := [] ;
  nouBoucleEval:= [];
  docEvalue := new_documentEvalue [] [];
  appelcourant := [] ;
  estTROUVEID := false;
  estTROUVEIDO := false;
  listBeforeFct := [];
  resAuxTN := MULTIPLE;
  maxAuxTN := MULTIPLE;
  isIntoIfLoop:= false;
  isEnd := false;
  isEndNONZERO := false;
  valeurEng := NOCOMP;
  borneAux := NOCOMP ;
  borneMaxAux := NOCOMP ;
  listeVB := [];
  listeVBDEP := [];
  isProd := false;
  isExactEng := true;
  curassocnamesetList := [];
  isExeBoucle := true;
  listeInstNonexe := [];
  aslAux := [];
  listCaseFonction := []


let listnoteqLoop l =
  List.iter (
    fun (fic,lig) ->
      Printf.eprintf
        "WARNING != condition => bound is either this one or infinite line %d into source %s \n"
        lig fic
  ) l
(*


let rec constructGhostInsts scenInsts decdef originalInsts =
  match scenInsts with
    (h::[]) ->
      analyse_expressionaux h; (* affectations!?! *)
      SEQUENCE(STAT_LINE(
               COMPUTATION(EXPR_LINE(h, "", 0)), "", 0), originalInsts)
  | (h::t) ->
      analyse_expressionaux h; (* affectations!?! *)
      BLOCK(decdef, SEQUENCE(STAT_LINE(
            COMPUTATION(EXPR_LINE(h, "", 0)), "", 0), (constructGhostInsts t decdef originalInsts)))
  | _ -> originalInsts

*)
let rec constructGhostInsts scenInsts decdef originalInsts =
  match scenInsts with
    (h::[]) ->
			if !vDEBUG then 
			begin
				 Printf.printf
						"constructGhostInsts\n";
				print_expression h 0;
					 (* analyse_expressionaux h; *)(* affectations!?! *)
				 Printf.printf
						"Fin analyse constructGhostInsts\n";
				print_expression h 0;
			end;
      SEQUENCE(STAT_LINE(
               COMPUTATION(EXPR_LINE(h, "", 0)), "", 0), originalInsts)
  | (h::t) ->
			if !vDEBUG then 
			begin
				 Printf.printf
						"constructGhostInsts\n";
				(*print_expression h 0;*)
					  (*analyse_expressionaux h;*) (* affectations!?! A QUOI CELA LUI SERT ???? *)

				 Printf.printf
				"Fin analyse constructGhostInsts\n"; 
			end;
     (* BLOCK(decdef, *)SEQUENCE(STAT_LINE(
            COMPUTATION(EXPR_LINE(h, "", 0)), "", 0), (constructGhostInsts t (*decdef*)[] originalInsts))(* ...)... *)
  | _ -> originalInsts

let rec constructSenaInstaux scenInsts =
		( match scenInsts with
				(h::[]) ->
					if !vDEBUG then begin Printf.printf "constructSenaInst\n";			print_expression h 0; end;
				  	analyse_expressionaux h; (* affectations!?! *)
			 		if !vDEBUG then Printf.printf 	"Fin analyse constructSenaInst\n";
			 
			
			  | (h::t) ->
			 		if !vDEBUG then begin Printf.printf "constructSenaInst\n";			print_expression h 0; end;
				  	analyse_expressionaux h; (* affectations!?! *)
			 		if !vDEBUG then Printf.printf 	"Fin analyse constructSenaInst\n"; 
			 		constructSenaInstaux t;
				 
			  | _ ->if !vDEBUG then  Printf.printf 	"Fin analyse constructSenaInst\n"; 
        )

	 


let   constructSenaInst scenInsts =
	let scenInstsPred = !listeDesInstCourantes in
    listeDesInstCourantes := [];
	
	constructSenaInstaux scenInsts;	 

	let res = !listeDesInstCourantes in
	listeDesInstCourantes := scenInstsPred;
	res



(* jz: will target main first iterates over list of functions,
       when we find main, we fixup the scenario annots as instructions.
       (!! we also do that for the global variable assignments, as they're merged into the scenario.)
*)
let rec fixupScenarioInFunction functionName functionList senainst =
  match functionList with
    [] -> []
  | ((someint, h)::t) ->
      (match h with
        {nom = functionName; declaration = b; corps = c; lesAffectations = d; listeES = e} -> (* main found *)
          let newmain =
            (match c with
              {boucleOuAppel = boua; corpsS = stmts} ->
                (match stmts with
                  BLOCK(decdef, seq) ->
                    (* generic: do for all *)
                  (*  let newblock = BLOCK(decdef,constructGhostInsts !scenarioAsDocInsts decdef seq) in *)
                    let laffwobeg =
                      (match d with
						
                        [BEGIN(lesaff)] -> let affectations = List.append !listeDesInstCourantes  senainst in [BEGIN(affectations @ lesaff)]
                      | _ -> d)
                    in
                    (someint, {nom = functionName; declaration = b;
                               corps = {boucleOuAppel = boua; corpsS = (*newblock*) stmts };
                     lesAffectations = laffwobeg; listeES = e})
                | _ -> (someint, h))
            ) (*| _ -> (someint, h))*)
          in
            newmain::(fixupScenarioInFunction functionName t senainst)
     ) (* | {nom = no; declaration = b; corps = c; lesAffectations = d; listeES = e} ->
         (someint, h)::(fixupScenarioInFunction functionName t)) *)

(* translate global variable assignments to expressions so we can ghost instruct them *)
let rec globAsgnsToInst globalInsts res =
  match globalInsts with
    (h::t) ->
      (match h with
        VAR(varname, EXP(exp), _, _) ->
          (globAsgnsToInst t (res @ [BINARY(ASSIGN,VARIABLE(varname), exp)]))
      | _ ->
          (globAsgnsToInst t res))
  | [] -> res



(* jz: balance *)
(* helper data structures in util *)

(* refs to docs *)
let docToAnalyze = ref (new_document [] [] [] [])
let docAnalyzed = ref (new_documentEvalue  [] [])


(* matching could be cleaner ... *)
(* FIXME -- HACK -- problem: breaks/returns
 * introduce a further IF-x statement and the
 * original is lost in what is to be analyzed.
 * so, the IF-x actually identifies another
 * (artificially introduced) part of the code.
 * this could be identified and fixed here? 
 *)
let rec unwrapBody body_stmts =
  match body_stmts with
    STAT_LINE(any, src, line) ->
      unwrapBody any
  | SEQUENCE(any, restOfSeq) ->
      unwrapBody any;
      unwrapBody restOfSeq
  | FOR(init, cond, incr, forBody) ->
      vanillaLoopStmts := [FOR(init, cond, incr, forBody)] @ !vanillaLoopStmts;
      unwrapBody forBody
  | WHILE(cond, whileBody) ->
      vanillaLoopStmts := [WHILE(cond, whileBody)] @ !vanillaLoopStmts;
      unwrapBody whileBody
  | IF(cond, thenBody, elseBody) ->
      vanillaIfStmts := [IF(cond, thenBody, elseBody)] @ !vanillaIfStmts;
      unwrapBody thenBody;
      unwrapBody elseBody
  | BLOCK(decls, blockstmts) ->
      unwrapBody blockstmts
  | _ -> () 


(* jz: balanced -- look up necessary information *)
let mapDocAnalyzedOntoDocToAnalyze (docAnalyzed : documentEvalue ref) (docToAnalyze : document ref) =
  (match !docToAnalyze with
   	{ laListeDesBoucles = listeDesBoucles; laListeDesFonctions = listeDesFonctions;
      laListeDesAssosBoucleBorne = listeDesAssosBoucleBorne; laListeDesNids = nidDeBoucle } ->
        (* iterate list of functions *)
        (List.iter
          (fun func ->
            (match func with
              (* get its id and all data *)
              (id, {nom = name; declaration = decls;
                    corps = body; lesAffectations = laff;
                    listeES = les}) ->
                 (* check fct body stmts for loops*)
                 (match body with
                   (* retrieve info about body and statements *)
                   {boucleOuAppel = boa; corpsS = body_stmts} ->
                     unwrapBody body_stmts))
          ) listeDesFonctions));
  (* bound look-up on evaluated doc *)
  (match !docEvalue with
    { maListeNidEval = nidEval; maListeEval = elementEval } ->
      (* iterate over all found loops *)
      (List.iter
        (fun nid ->
          (match nid with
            (* fetch information for each one *)
            { idBoucleN = idbouclen; expressionBorneToutesIt = expessionbornetoutesit;
              maxUneIt = maxuneit; isExecuted = isexecuted; isIntoIf = isintoif; num = num} ->
                (* fetch corresponding loop bound *)
                (List.iter
                  (fun info ->
                    (* loop info stores for each loop id its loop bounds, this is set
                       in orange, just before outputting it to ffx. loopInfo is found in util *)
                    (match info with
                      (** jz: 11/06/14 -- total and max were swapped in the delta analysis, FIXED. *)
                      (id, max, total) ->
                        (match idbouclen with
                          TBOUCLE (lid, appel, _, _, _, _, _, _, _) ->
                            if lid = id then
                              completeLoopStmts := [(id, max)] @ !completeLoopStmts
                              (* completeLoopStmts := [(id, total, max)] @ !completeLoopStmts *)
                        | _ -> ()))
                  ) !loopInfo))
        ) nidEval));
  (* jz: balance -- in hashtables we have (id, stmt), in completeLoopStmts and
         ifInfo we have the analysis results (loopId, bnd), (ifId, [t,f]).
         thus, we need to create a list, matching 
            loop = (id, STMT) + (loopid, BND) and cond = (ifid, STMT) + (ifId, [T,F])
         to do so: iterate over completeLoopStmts, for each id get the bound
         and find corresponding stmt in the hash tbl
  *)
  (List.iter  
    (fun analyzedLoop ->
      let (id, bnd) = analyzedLoop in
      let stmt = Hashtbl.find looptbl id in
      balanceLoops := [(stmt, bnd)] @ !balanceLoops;
    ) !completeLoopStmts);
  (* ifInfo contains the ``high'' id, i.e.  (ifInfo->id - tmpIfCnt)
     gives the id we need to look up in the htbl *)
  (List.iter
    (fun analyzedCond ->
      (* jz: FIXME -- missing matches, due to break/return *)
      (match !analyzedCond with
        (id, [texec; fexec]) -> 
          let stmt = Hashtbl.find condtbl id in
          balanceIfs := !balanceIfs @ [(stmt, (id,[texec; fexec]))];
      | _ -> ())
    ) !ifInfo);
  ()

let initPrint blabla =
	  idBoucle := 0;
	  idIf := 0;
	  idAppel:=0;
	  nbImbrications := 0;
	  
	  enTETE := false;
	  numAppel := 0;
	  estNulEng := false;
	  estDansBoucle := false; estDansBoucleE :=!estDansBoucle;
	  getOnlyBoolAssignment := true;
	  ptrInterval := [];
	  integerInterval := ["x"]



let evalOneTree senainst globalInst globalAsgnsAsGhost funct =
 	  let scenarioDoc =
				match !doc with
				  {laListeDesBoucles = a; laListeDesFonctions = b;
				   laListeDesAssosBoucleBorne = c; laListeDesNids = d} ->
					ref {laListeDesBoucles = a;
						 
						 laListeDesFonctions = (fixupScenarioInFunction funct b senainst);
						 laListeDesAssosBoucleBorne = c;
						 laListeDesNids = d}
	in 

(*Printf.printf "evalOneTree!!!! main =%s\n" funct ;*)
			  idAppel:=0;
			   (* afficherFonctionsDuDoc doc; Listener.null*)
				getOnlyBoolAssignment := false;
				listNotEQ := [];
				Printf.printf "\nEVALUATION BEGIN %s\n" funct;  

(*docEvalueCour*)
				evaluerFonctionsDuDoc scenarioDoc;(*scenAndGlobalDoc;*)
				(* jz: balanced *)
				docToAnalyze := !scenarioDoc;
				listnoteqLoop !listNotEQ;
				Printf.printf "\nEVALUATION END\n";
				(* jz: balanced *)
				docAnalyzed := !docEvalue;
				(* continue doing stuff ... *)


				let res = (afficherInfoFonctionDuDocUML !docEvalue.maListeEval) in
			   
			  mapDocAnalyzedOntoDocToAnalyze docAnalyzed docToAnalyze;
			  let ff = let open Balance in { 
				loop_bound = (function lstmt -> Some (List.assoc lstmt !balanceLoops));
				branch_feasibility = 
				   (function istmt -> 
						if !balanceIfs != [] then 
						  (* HACK HACK HACK: quick break/return fix --
						     the problem is that it will get counted multiple times
						     in loops -- which it shouldn't be *)
						    try snd (List.assoc istmt !balanceIfs) with Not_found -> [true;true] 
						else (* Some *) [true; true]);
				branch_id = (fun istmt ->
				  try fst (List.assoc istmt !balanceIfs) with Not_found -> "IF-?");
			  } in
			  (* to get from document to definition list we need to extract 
				 for the required fct the corpsS *)
			  (* JZ: fix entry point: let entry = "main" in *)
			  let entry = funct in 
			  let fctlist = !docToAnalyze.laListeDesFonctions in
			  (* reconstruct frontc output from doc *)
			  let allFcts =
				(List.map
				  (fun fct ->
					(match fct with
					  (someint, f) ->
						(match f with
						  {nom = entry; declaration = decls;
						   corps = body; lesAffectations = d; listeES = e} -> 
						    (match body with
						      {boucleOuAppel = boua; corpsS = stmts} ->
						        FUNDEF(decls, ([], stmts))
						   )
						)
					)  
				  )
				  fctlist) 
			  in
			  begin match !delta, !ghost with
			  | `Skip, _ -> ()
			  | `Run _, true ->
			    Format.printf "Skipping the balance analysis (ghost instructions)@\n"
			  | `Run (`OutDir dir), false ->
			    Format.printf "Computing the balance information from the %s function@\n" entry;
			    let balance = Balance.analysis ff allFcts entry in
			    let (_,eval) = balance in
			    Format.printf ">>> Estimated costs of function: %d\n" (Balance.CodeEval.weight eval);
			    Format.printf "%d accessible conditional statements have been listed@\n" (Balance.CodeEval.cond_count eval);
			    List.iter (Format.printf "%a@\n" Balance.CondEval.delta_print) (Balance.CodeEval.sorted_conds eval);
			    let output = open_out (dir^"/"^"deltas_"^funct^".ffx") in
			    Balance.Output.to_ffx (output) balance;
			    Format.printf "\nparent - child relations:\n";
			    Hashtbl.iter (
			      fun parent children ->
				Format.printf "\t%s: " parent;
				List.iter (fun child -> Format.printf "%s " child) children;
				Format.printf "\n"
			    ) parents;
			    close_out output;
			  end ;
			  if !wcee then begin
			    Format.printf "Now performing the worst case execution estimation@\n";
			    let _ = Wcee.analysis ff allFcts entry in
			    ()
			  end ;
			  res

let rec applyToOthersTREE liste memscenarioAsDocInsts senainst globalAsgnsAsGhost globalInst=
 
	let result =			
		(match liste with 
			[] ->  []
                        	                
			| a::b ->      if b = [] then print_FIN := true;
					let hd = !(a)in
    					let tl = b in
					setMainFct hd;
					(*Printf.printf "applyToOthersTREE!!!! main =%s\n" !(!mainFonc) ;*)
               

			 	 	dernierAppelFct:= (TFONCTION(!(!mainFonc),0,[], [], [], [],[], [], true,false,"",0));
			 		predDernierAppelFct  :=  (TFONCTION(!(!mainFonc),0,[], [], [], [],[],[], true, false,"",0));
					 
					(*Printf.printf "applyToOthersTREE!!!! main =%s\n" !(!mainFonc) ;*)
					scenarioAsDocInsts := memscenarioAsDocInsts ;
		            initorange false ;
					initPrint "blabla";
					(*Printf.printf "!!!! main =%s" !(!mainFonc) ;*)
					let first =  ( evalOneTree senainst globalInst globalAsgnsAsGhost !(!mainFonc)  ) in
					 
  					flush();
         			 space();
      				
  					new_line();
		            List.append [ first] 
		               (applyToOthersTREE tl memscenarioAsDocInsts senainst globalAsgnsAsGhost globalInst  )
		           
		)
in
result


(*
let printFile (result : out_channel)  (defs2 : file) need_analyse_defs mode =
  initPrint "blabla";
  out := result;
 

  if (!isPrint_Expression) then
    exp_VERBOSE := true
  else
    exp_VERBOSE := false;

  if need_analyse_defs then
    analyse_defs defs2; (*step 1*)

  phaseinit := false;
   
  flush ();
  if mode = "resume" && !evalFunction != [] then
    (evaluerNbFunctionOfDoc doc !evalFunction [];
     afficherFonctionsDuDoc doc;);
  if !hasCondListFile_name then begin
    condAnnotated := renameListeIF (getAbsStoreFromComp !condListFile_name) ;
    afficherListeAS !condAnnotated;
  end;
  if mode = "nothing" && !evalFunction != [] then Printf.eprintf "They are more than one function name but only the first one is taken into account. See --resume / --multiTree options.\n";

  (* jz: this might be kinda f*ked: we add to the ``program instructions'' the
      necessary instructions from the scenario. additionally, we compute the affectations 
      of the new statements, by supplying them to analyse_expressionaux. this call will 
      put the affects of the stmts into Cvarabs.listeDesInst (listeDesInstCourantes).
        from there, we take them and merge them into the data we already have in ``doc''
     FIXME/TODO :: (0) first find main function and add it ONLY there
                   (1) parse the scenario information to the requirements
                   (2) do it for every scenario information
  *)

  (* translate global assignments to ghost instruction: first acquire global assignments *)
  let globalInst =
    if !notwithGlobalAndStaticInit = false then
      (myCurrentPtrContext := (localPtrAnalyse [new_instBEGIN (!listeDesInstGlobales)] [] false true);
      ptrInterval := !myCurrentPtrContext;
      !listeDesInstGlobales)
    else
      (myCurrentPtrContext := (localPtrAnalyse [new_instBEGIN (!listeLocalStatic)] [] false true);
      ptrInterval := !myCurrentPtrContext;
      List.append (!listeLocalStatic) (!listeDesEnum))
  in
  (* now construct construct ghost instrs from global assignments by
     merging them with scenario notes, scenario has precedence *)
  let globalAsgnsAsGhost = globAsgnsToInst globalInst [] in
  scenarioAsDocInsts := globalAsgnsAsGhost @ !scenarioAsDocInsts;
  let memscenarioAsDocInsts = !scenarioAsDocInsts in
  let senainst = constructSenaInst !scenarioAsDocInsts in

  (*Printf.printf"MODE !!!!! mode %s\n" mode;*) 
  if  mode = "multitree"  && !evalFunction != [] then ( print_FIN := false; print_EnTETE := true) ;
    



  let result =   (evalOneTree senainst globalInst globalAsgnsAsGhost  !(!mainFonc)) in
  flush();
          space();
      
  new_line();
  (*Printf.printf"MODE !!!!! mode %s\n" mode;*)
	let resultList = if  mode = "multitree"  && !evalFunction != []  then
	( let liste = !evalFunction in
          print_EnTETE := false;

	  let suite =  (applyToOthersTREE liste memscenarioAsDocInsts senainst globalAsgnsAsGhost globalInst) in
       List.append [result] suite)
		else [result] in
 resultList 
  end;;
 
*)

(*
let printFile (result : out_channel)  (defs2 : file) need_analyse_defs mode =
  initPrint "blabla";
  out := result;
 

  if (!isPrint_Expression) then
    exp_VERBOSE := true
  else
    exp_VERBOSE := false;

  if need_analyse_defs then
    analyse_defs defs2; (*step 1*)

  phaseinit := false;
   
  flush ();
  if mode = "resume" && !evalFunction != [] then
    (evaluerNbFunctionOfDoc doc !evalFunction [];
     afficherFonctionsDuDoc doc;);
  if !hasCondListFile_name then begin
    condAnnotated := renameListeIF (getAbsStoreFromComp !condListFile_name) ;
    afficherListeAS !condAnnotated;
  end;
  if mode = "nothing" && !evalFunction != [] then Printf.eprintf "They are more than one function name but only the first one is taken into account. See --resume / --multiTree options.\n";

  (* jz: this might be kinda f*ked: we add to the ``program instructions'' the
      necessary instructions from the scenario. additionally, we compute the affectations 
      of the new statements, by supplying them to analyse_expressionaux. this call will 
      put the affects of the stmts into Cvarabs.listeDesInst (listeDesInstCourantes).
        from there, we take them and merge them into the data we already have in ``doc''
     FIXME/TODO :: (0) first find main function and add it ONLY there
                   (1) parse the scenario information to the requirements
                   (2) do it for every scenario information
  *)

  (* translate global assignments to ghost instruction: first acquire global assignments *)
  let globalInst =
    if !notwithGlobalAndStaticInit = false then
      (myCurrentPtrContext := (localPtrAnalyse [new_instBEGIN (!listeDesInstGlobales)] [] false true);
      ptrInterval := !myCurrentPtrContext;
      !listeDesInstGlobales)
    else
      (myCurrentPtrContext := (localPtrAnalyse [new_instBEGIN (!listeLocalStatic)] [] false true);
      ptrInterval := !myCurrentPtrContext;
      List.append (!listeLocalStatic) (!listeDesEnum))
  in
  (* now construct construct ghost instrs from global assignments by
     merging them with scenario notes, scenario has precedence *)
  let globalAsgnsAsGhost = globAsgnsToInst globalInst [] in
  scenarioAsDocInsts := globalAsgnsAsGhost @ !scenarioAsDocInsts;
  let memscenarioAsDocInsts = !scenarioAsDocInsts in
  let senainst = constructSenaInst !scenarioAsDocInsts in

  (*Printf.printf"MODE !!!!! mode %s\n" mode;*)

  let result =   (evalOneTree senainst globalInst globalAsgnsAsGhost  !(!mainFonc)) in
  flush();
          space();
      
  new_line();
  (*Printf.printf"MODE !!!!! mode %s\n" mode;*)
	let resultList = if  mode = "multitree"  && !evalFunction != []  then
	( let liste = !evalFunction in
	  let suite =  (applyToOthersTREE liste memscenarioAsDocInsts senainst globalAsgnsAsGhost globalInst) in
       List.append [result] suite)
		else [result] in
 resultList 
  end;;
*)

let printFile (result : out_channel)  (defs2 : file) need_analyse_defs mode=
if (!doc.laListeDesFonctions = []) then 
	Printf.printf "printFile liste fonction vide 1\n"
else  Printf.printf "printFile liste fonction NON vide 1\n";

  idBoucle := 0;	idIf := 0;
  idAppel:=0;
  nbImbrications := 0;
  out := result;
  enTETE :=  false;
  numAppel :=  0;
  estNulEng :=  false;
  estDansBoucle :=  false;
	getOnlyBoolAssignment := true;
  ptrInterval :=   [];
  integerInterval :=   ["x"];

  if ( !isPrint_Expression ) then exp_VERBOSE :=  true else exp_VERBOSE :=  false;

  if need_analyse_defs
  	then  
  	begin Printf.printf "orange step1\n";

		analyse_defs defs2; (*step 1*)
	end;
	
	 Printf.printf "orange step1 analyse_defs terminé\n";
 phaseinit := false;
  (*afficherNidDeBoucle doc;	*)
  (*Printf.printf "les globales\n";
  List.iter(fun x->Printf.printf "%s\t" x)!alreadyAffectedGlobales;
  Printf.printf "les tableaux\n";
print_AssosArrayIDsize !listAssosArrayIDsize;
  Printf.printf "les typesdefs tableaux\n";
  print_AssosArrayIDsize !listAssosTypeDefArrayIDsize;
  Printf.printf "les pointeurs\n";
 *)

(*	evaluerCaseFonctionsDuDoc  doc;
  printFuncCaseAssos !listCaseFonction;*)
  
   




  flush ();


 (* if !evalFunction != [] then( evaluerNbFunctionOfDoc  doc  !evalFunction []; afficherFonctionsDuDoc doc;);
 	if !hasCondListFile_name then
	begin
		condAnnotated := renameListeIF (getAbsStoreFromComp !condListFile_name) ;
		afficherListeAS   !condAnnotated; 
	end;
	*)
	
	if mode = "resume" && !evalFunction != [] then
    (evaluerNbFunctionOfDoc doc !evalFunction [];
     afficherFonctionsDuDoc doc;);
  if !hasCondListFile_name then begin
    condAnnotated := renameListeIF (getAbsStoreFromComp !condListFile_name) ;
    afficherListeAS !condAnnotated;
  end;
  if mode = "nothing" && !evalFunction != [] then 
  Printf.eprintf "They are more than one function name but only the first one is taken into account. See --resume / --multiTree options.\n";

if (!doc.laListeDesFonctions = []) then Printf.printf "liste fonction vide\n"
else  Printf.printf "liste fonction non vide\n";
 

 let result = (* afficherFonctionsDuDoc doc; Listener.null*)

		 
		  getOnlyBoolAssignment := false;
		 listNotEQ := [];
		  Printf.printf "\nEVALUATION BEGIN\n";
		  evaluerFonctionsDuDoc doc ;
		  listnoteqLoop		!listNotEQ;
 		  Printf.printf "\nEVALUATION END\n";
  		  afficherInfoFonctionDuDocUML !docEvalue.maListeEval
  in
  print_newline () ;
  flush ();
 [result]
  end;;
  
  
  
  
 
