#undef HAVE_CONFIG_H
#define STACK_SIZE 65536
#include <yacas.h>
#include <glue.h>
extern "C" {

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/intext.h>
#include <stdio.h>
}

enum {
  RESULT_EXPR,
  RESULT_ERROR
};

typedef struct {
  CYacas *yacasEngine;
} engineWrapper;

void cyacas_finalize(value v) {
  delete ((engineWrapper*) Data_custom_val(v))->yacasEngine;
  ((engineWrapper*) Data_custom_val(v))->yacasEngine = NULL;
}

static struct custom_operations cyacas_ops = {
  "fr.irit.caml.cyacas",
  cyacas_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

extern "C" CAMLprim value glue_init(value unit) {
  CAMLparam1(unit);
  CAMLlocal1 (e);  
  CYacas *yacasEngine = CYacas::NewL(STACK_SIZE);
  if (yacasEngine == NULL) {
    caml_failwith("Failed to initialize.");
  }
  char cmd[1024];
  e = caml_alloc_custom(&cyacas_ops, sizeof(engineWrapper), 0, 1);
  ((engineWrapper*) Data_custom_val(e))->yacasEngine = yacasEngine;
  snprintf(cmd, 1024, "DefaultDirectory(\"%s/share/yacas/\");", YACASPATH);
  yacasEngine->Evaluate(cmd);
  if (yacasEngine->IsError()) {
    delete yacasEngine;
    caml_failwith("Failed to access script directory");
  }
  yacasEngine->Evaluate("Load(\"yacasinit.ys\");");
  if (yacasEngine->IsError()) {
    delete yacasEngine;
    caml_failwith("Failed to load initialization script");
  }
  printf("YACAS engine created.\n");
  CAMLreturn (e);
}

extern "C" CAMLprim value glue_exec(value e, value str) {
  CAMLparam2(e, str);
  CAMLlocal1(res);
  CYacas *yacasEngine = ((engineWrapper*) Data_custom_val(e))->yacasEngine;
  yacasEngine->Evaluate(String_val(str));
  if (yacasEngine->IsError()) {
    res = caml_alloc(1, RESULT_ERROR);
    Store_field(res, 0, caml_copy_string(yacasEngine->Error()));
  } else {
    res = caml_alloc(1, RESULT_EXPR);
    Store_field(res, 0, caml_copy_string(yacasEngine->Result()));
  }
  CAMLreturn( res);
}
