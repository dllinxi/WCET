type result = Success of string 
            | Error of string 
type engine
external glue_init : unit -> engine = "glue_init";;
external glue_exec : engine -> string -> result = "glue_exec";;

exception YacasError of string ;;

let create () = 
  let e = glue_init () in 
  function str -> match (glue_exec e str) with
    Success(result) -> result
    |Error(errmsg) -> raise (YacasError(errmsg)) ;;
  
let exec = glue_exec ;;
