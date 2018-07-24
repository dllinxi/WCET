(* 
 * ffx2pval: FFX 2 PVAL Translation;
 * author:   jakob zwirchmayr (jakob.zwirchmayr@irit.fr)
 * date:     2014-09-24
 *)

open Xml

(* 
 * Parse an FFX file and output the corresponding 
 *   Principle Value Annotation Language (PVAL).
 *
 * The following translation is currently performed: 
 * (entry function is called "main", PC stores line,source and ids,
 *  IPET counters are referred by #construct-ID identifiers)
 *
 * +loops: maxcount and totalcount attributes:
 *    <loop id="x" line="y" source="z" maxcount="M" totalcount="T"/>
 *   maxcount expression: 
 *      PC="main" -> PC="L-x,y,z" => #L-x <= M  (enter loop -> iterate max)
 *   maxcount expression:
 *      PC="main" -> #L-x <= T              (enter prog -> iterate total)
 *   if the loop is within a call (contextual loop bounds), this must
 *   be reflected in the PC expression:
 *     <call name="n" numcall="o" line="p" source="q">
 *       <func> 
 *         <loop id="a" line="b" source="c" max="D" tot="E"/>
 *   PC="n_o,p,q" -> PC="L-a,b," => #L-a <= D
 *   and this should be a generalization of the above (main) case.
 *
 * parent stack consists of PVAL LABELS
 *
 * FIXME:
 *   Problem with this piece of code is that it doesn't use
 *   any proper data types but strings to identify stuff.
 *
 * TODO:
 *   analysis entry point hardcoded (main)
 *   ffx input file hardcoded (small.ffx)
 *
 * ILP TRANSLATION: each ``through'' string needs to identify a distinct 
 *                  ILP variable. (The best thing to do is to first ``merge''
 *                  all the thru clauses to the same ILP vars.)
 * 
 * FOUND OUT: the order in a scope doesn't matter (i.e. we don't care that
 *   FFX might not respect stmt order -- it's all about flow, therefore the
 *   results are correct no matter how we swap the order of elements within 
 *   a scope.
 *)


(* assume entry is called main *)
let entry = "main" 
let (tmpflow : string ref) = ref "" 
let (curCase : string Stack.t) = Stack.create ();;

(* stores parent relations of constructs in order to
 * translate to pval (the program counter variable) *)
let (parentstack : (string) Stack.t) = Stack.create ();;
let (funcstack : (string) Stack.t) = Stack.create ();;

(* pval for loops, calls and cond (together w. cases) *)
let (pvalLoopMaxstack : (string) Stack.t) = Stack.create ();;
let (pvalLoopTtlstack : (string) Stack.t) = Stack.create ();;
let (pvalExecdstack : (string) Stack.t) = Stack.create ();;

(* std ipet clauses *)
let (flowfactstack : (string) Stack.t) = Stack.create ();;
(* store siblings for ipet clauses *)
let (predflowstack: (string list) Stack.t) = Stack.create ();;

(* 
 * THE FOLLOWING FUNCTIONS EXTRACT ATTRIBUTES REQUIRED TO CONSTRUCT PVAL
 * LABELS (exception-safe) AND CONSTRUCT PVAL LABELS AND IPET COUNTERS 
 * FOR FFX ELEMENTS.
 *
 * ELEM IS THE XML ELEMENT OF SOME FFX CONSTRUCT.
 *)

(* 
 * INPUT: xml element and attribute string                   
 * DESCR: gets attribute value as string, guarded s.t. missing 
 *        attributes are translated to something_NOT_LOCATED.        
 *)
let guardedAttr (elem:Xml.xml) (attrib:string) =
  try (Xml.attrib elem attrib) 
  with Xml.No_attribute attrib -> String.uppercase(attrib) ^ "_NOT_LOCATED" 
     | Xml.Not_element elem -> String.uppercase(attrib) ^ "_NOT_LOCATED" 

(* DESCR: guarded xml attribute getter for loops (totalcount) *)
let maxCountFromLoopElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "loop") = 0);
  guardedAttr elem "maxcount"

(* DESCR: guarded xml attribute getter for loops (maxcount) *)
let ttlCountFromLoopElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "loop") = 0);
  guardedAttr elem "totalcount"

(* DESCR: construct label and ipet counter for loop elements *)
let labelFromLoopElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "loop") = 0);
  "Lp-" ^ (guardedAttr elem "loopId") ^ "-" ^ 
          (guardedAttr elem "line") ^ "-" ^ 
          (guardedAttr elem "source")

(* DESCR: ipet counter represenatation = # + label representation *)
let ipetCntFromLoopElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "loop") = 0);
  "#" ^ (labelFromLoopElem elem)

(* DESCR: guarded xml attribute getters for calls *)
let numCallFromCallElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "call") = 0);
  guardedAttr elem "numcall"

(* DESCR: label representation for call elements *)
let labelFromCallElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "call") = 0);
  "Ca-" ^ (guardedAttr elem "name") ^ "-" ^ (guardedAttr elem "numcall") ^ "-" ^
          (guardedAttr elem "line") ^ "-" ^ (guardedAttr elem "source")

(* DESCR: ipet counter representation for call elements *)
let ipetCntFromCallElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "call") = 0);
  "#" ^ (labelFromCallElem elem)

(* DESCR: used for cond, call, case elems and extracts executed attr *)
let execdFromElem (elem:Xml.xml) = 
  assert (((String.compare (Xml.tag elem) "condition") = 0)
          || ((String.compare (Xml.tag elem) "call") = 0)
          || ((String.compare (Xml.tag elem) "case") = 0));
  match guardedAttr elem "executed" with
      "false" -> 0
    | _ -> 1 (* default to executed, safe *)

(* DESCR: label for case elements *)
let labelFromCaseElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "case") = 0);
  "Cs-" ^ (guardedAttr elem "line") ^ "-" ^ (guardedAttr elem "source")

(* DESCR: ipet counter for case elements *)
let ipetCntFromCaseElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "case") = 0);
  "#" ^ (labelFromCaseElem elem)

(* 
 * INPUT: xml element and conditionalID (of parent)
 * DESCR: label for condition elements; the conditionalId is the id
 *        we get from the parent conditional elem which we usually 
 *        extract from the parentstack.
 *)
let labelFromCondElem (elem:Xml.xml) (conditionalId:string) = 
  assert ((String.compare (Xml.tag elem) "condition") = 0);
  assert ((String.compare (String.sub conditionalId 0 3) "IF-") = 0);
  "Cn-" ^ conditionalId ^ "-" ^
          (guardedAttr elem "line") ^ "-" ^ (guardedAttr elem "source")

(* DESCR: ipet counter for conditional element *)
let ipetCntFromCondElem elem conditionalId = 
  assert ((String.compare (Xml.tag elem) "condition") = 0);
  assert ((String.compare (String.sub conditionalId 0 3) "IF-") = 0);
  "#" ^ (labelFromCondElem elem conditionalId)

(* 
 * DESCR: we don't treat those really but we need label construction --
 *        what we push is ``Ci-ID'' so that we can use the ID part for 
 *        condition label construction *)
let labelFromCondiElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "conditional") = 0);
  "Ci-" ^ (guardedAttr elem "id")

(* DESCR: label for function elements *)
let labelFromFuncElem (elem:Xml.xml) = 
  assert ((String.compare (Xml.tag elem) "function") = 0);
  "Fn-" ^ (guardedAttr elem "name")

(*
 * INPUT: xml element and its string representation 
 * DESCR: and extract the attribute information necessary to construct 
 *        the label representation. is used to get rid of a match-with
 *        constructs in other functions which saves some lines, nevertheless
 *        the elem type needs to be known. (we could do without but its less
 *        hassle like this)
 *)
let labelFromElem (str_elem:string) (elem:Xml.xml) = 
  match str_elem with 
      "call" -> labelFromCallElem elem
    | "loop" -> labelFromLoopElem elem
    | "conditional" -> labelFromCondiElem elem
    | "condition" -> 
        let parentLab = Stack.top parentstack in
        let parentId = String.sub parentLab 3 (String.length parentLab - 3) in
        labelFromCondElem elem parentId
    | "function" -> labelFromFuncElem elem
    | "case" -> labelFromCaseElem elem
    | _ -> ignore(assert (false)); "XX" ^ str_elem




let sstkp stack = 
  Printf.printf "     ";
  Stack.iter (fun f ->  List.iter (fun g -> Printf.printf "%s " g)  f; Printf.printf "\n") stack



(* 
 * INPUT: xml elem and its label representation (i.e. type + additional info)
 * DESCR: take the elem and treat what is on the parent stack as its parent.
 *        translate the element, then, take all the elements children and 
 *        translate hose, each child elem calling this function recursively.
 *)
let rec translateChild (str_label:string) (elem:Xml.xml) =
  assert ((String.compare str_label (Xml.tag elem)) != 0);
  let cur = Stack.top parentstack in
  let children = Xml.children elem in

  (* we get as argument the label representation of the element, but we 
   * need to look up it's general 'type' in order to construct the pval
   * expression. therefore, we use the tag to identify the type (which is
   * easier than to calculate back from the label representation).
   * most complex is the condition, we need the identifier from the conditional,
   * which is the outer scope (look up on parentstack) and extract ID from the
   * label string. elems parent is on top of stack, elem will 'become parent'
   * when treating all children. *)
  (* push on predflowstack, so we know where to attach the current ipet constraint *)
  (* we use the boolean to check whether we treated a call, if so, we need to pop from
     the function stack, which constitutes our calls trings *)
  let elm = Xml.tag elem in
  let thrustr = " through [" ^ (Stack.top funcstack) ^ "]" in
  let needPopFct = (match elm with
      "call" -> 
          tmpflow := !tmpflow ^ " " ^ str_label;
          let ff = "@exit -> " ^ "#" ^ str_label ^ " <= #" ^ cur ^ thrustr in
          Stack.push (labelFromCallElem elem) funcstack;
          Stack.push ff flowfactstack;
          let ex = execdFromElem elem in
          if (ex = 0) then begin
            let ffexd = "@exit -> " ^ (ipetCntFromCaseElem elem) ^ " = 0" ^ thrustr in
            Stack.push ffexd pvalExecdstack;
          end;
          true
    | "loop" ->  
          tmpflow := !tmpflow ^ " " ^ str_label;
          let n = maxCountFromLoopElem elem in
          let t = ttlCountFromLoopElem elem in
          let np1 = (string_of_int ((int_of_string n) + 1)) in
          let ff = "@exit -> " ^ "#" ^ str_label ^ " <= #" ^ cur ^ " * " ^ n ^ thrustr in 
          let ff2 = "@exit -> " ^ "#" ^ str_label ^ "_cond <= #" ^ cur ^ " * " ^ np1 ^ thrustr in
          let ff3 = "@exit -> " ^ "#" ^ str_label ^ "_cond <= #" ^ (Stack.top funcstack) ^ " * " ^ t ^ thrustr in
          Stack.push ff pvalLoopMaxstack; (* loop ff: maxcount *)
          Stack.push ff2 pvalLoopMaxstack;  (* lcond ff: maxcount + 1 *)
          Stack.push ff3 pvalLoopTtlstack;  (* lcond ff: totalcount *)
          false
    | "conditional" -> 
          tmpflow := !tmpflow ^ " " ^ str_label;
          let ff = "@exit -> " ^ "#" ^ str_label ^ " <= #" ^ cur in
          Stack.push ff flowfactstack;
          "conditional, parent: " ^ cur ^ "\n\t\tPVAL: executed (IGNORE CONDITIONAL)";
          false
    | "condition" -> 
          (* outer is conditional - take the id and mix into label/ipet stuff 
           * leave the conditional id on the stack, though, for removal;
           * need to 'cut' out id *)
          (*let parentLab = Stack.top parentstack in*)
          (*let parentId = String.sub parentLab 3 (String.length parentLab - 3) in*)
          tmpflow := !tmpflow ^ " " ^ str_label;
          let ff = "@exit -> " ^ "#" ^ str_label ^ " <= #" ^ cur in
          Stack.push ff flowfactstack;
          let ex = execdFromElem elem in
          if (ex = 0) then begin
            let ff = "@exit -> " ^ "#" ^ cur ^ " = 0" ^ thrustr in
            Stack.push ff pvalExecdstack;
          end;
          false
    | "function" 
    | "Fn-main" -> 
          "function, parent: " ^ cur ^ "\n\t\tPVAL: executed (IGNORE FCT)";
          false
    | "case" ->
          tmpflow := !tmpflow ^ " " ^ str_label;
          if ((Stack.length curCase) = 1) then begin
            let case0 = (Stack.pop curCase) in
            let cclause = 
              "@exit /\\ #" ^ cur ^ " = i -> " ^ case0 ^ " + " 
                ^ (ipetCntFromCaseElem elem) ^ " <= i" ^ thrustr
            in
            Stack.push cclause flowfactstack
          end else 
            Stack.push (ipetCntFromCaseElem elem) curCase; (* beware, we fill curStack with ipetcnt *)
          (* executed attribute *)
          let ex = execdFromElem elem in
          if (ex = 0) then begin
            let ff = "@exit -> " ^ (ipetCntFromCaseElem elem) ^ " = 0" ^ thrustr in
            Stack.push ff pvalExecdstack;
          end;
          false
    | _ -> false)
  in
  (* if siblings is empty, then use parent, otherwise sibling *) 
  translateChildren elm elem children elem;
  if needPopFct = true then
    ignore(Stack.pop funcstack)


(* 
 * INPUT: (current top level) xml element; it's type and list of children; 
 * DESCR: for each children we descend by recursively calling translateChild.
 *        To do so, we construct the label representation for the element, 
 *        translate it  and supply it to translateChild. It is here that the
 *        (already translated) element becomes parent by putting it on top 
 *        of the parentstack.
 *)
and translateChildren (str_elem:string) (elem:Xml.xml) children flow_attach =
  match children with
      h :: tl -> 
          (* construct the label string for the parent elem.
           * carries additional info depending on elem type *)
          let parent_label_str = labelFromElem str_elem elem in
          Stack.push parent_label_str parentstack; 
          Stack.push [""] predflowstack; 
          (* label representation for current child *)
          let str_label = labelFromElem (Xml.tag h) h in
          (* label and elem for current child, translate it, 
           * then continue with its siblings. *)
          translateChild str_label h;
          (* translate rest of siblings, supplying the same parent elem *)
          ignore (translateChildren str_elem elem tl h) 
    | [] -> 
          ignore(Stack.pop parentstack) 
          (* pop preds *)
  

(* DESCR: stack printer *)
let outputClauses stack = 
  Stack.iter (fun f -> Printf.printf "  %s\n" f) stack

(* 
 * INPUT: TODO argument the main fct; verbose arg;
 * DESCR: translateChild gets the top level elem and its label representation
 *        and starts translation. the string representation is always the label
 *        representation which might differ for different C constructs. 
 *        basic idea: take some top level element(s), and translate it
 *        (translateChild), it will take care of all its child nodes.
 *)
let _ =
  let ffx_flowfacts = Xml.parse_file "small.ffx" in 
  let ffx_context = Xml.children ffx_flowfacts in   (* <flowfacts ... > *)
  (* there should only be one top level element, which is the main function *)
  Stack.push "Fn-main" funcstack;
  Stack.push "@exit -> #exit = 1" flowfactstack;
  assert ((List.length ffx_context) = 1);
  List.iter (
    fun f -> 
      (* there should be only one entry, and its name is main *)
      assert ((String.compare (guardedAttr f "name") "main") = 0);
      (* TODO: parameterize entry; we manually construct label representation
       * here. (function call 'Fn', name 'main'. this will go wrong as
       * soon as there is more than one top level element, which shouldn't
       * be the case. nevertheless, take care. *)
      Stack.push "Fn-main" parentstack;  
      (*let c = translateChild "Fn-main" f in ()*)
      ignore (translateChild "Fn-main" f)
  ) (ffx_context);

  (* std ipet clauses *)
  Stack.push "@exit -> #Fn-main = 1" flowfactstack;
  (* when we're done, also the first elem is pop'd off the stack: 
   * the list of children is finite, so after the last entry, we pop 
   * this initial stack elem *)
  assert ((Stack.length parentstack) = 0);
  assert ((Stack.length funcstack) = 1);
  Printf.printf "\n\nPVAL Loop Max Clauses:\n";
  outputClauses pvalLoopMaxstack;
  Printf.printf "\n\nPVAL Loop Total Clauses:\n";
  outputClauses pvalLoopTtlstack;
  Printf.printf "\n\nPVAL IPET Clauses:\n";
  outputClauses flowfactstack;
  Printf.printf "\n\nPVAL Executed Clauses:\n";
  outputClauses pvalExecdstack

