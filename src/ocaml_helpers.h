#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/callback.h>
#include <caml/alloc.h>
#include <caml/custom.h>

#include <string.h>

#include "wasm.h"

static struct custom_operations wasmtime_ops;

#define wasm_engine_t_val(v) (*((wasm_engine_t**) Data_custom_val(v)))
#define wasm_store_t_val(v) (*((wasm_store_t**) Data_custom_val(v)))
#define wasm_module_t_val(v) (*((wasm_module_t**) Data_custom_val(v)))
#define wasm_instance_t_val(v) (*((wasm_instance_t**) Data_custom_val(v)))
#define wasm_extern_t_val(v) (*((wasm_extern_t**) Data_custom_val(v)))
#define wasm_func_t_val(v) (*((wasm_func_t**) Data_custom_val(v)))
#define wasm_val_t_val(v) (*((wasm_val_t*) Data_custom_val(v)))

#define Safe_String_val(v) memcpy(malloc(caml_string_length(v) + 1), String_val(v), caml_string_length(v) + 1)

#define Val_none Val_int(0)
#define Some_val(v) Field(v, 0)
#define Tag_some 0
#define Is_none(v) ((v) == Val_none)
#define Is_some(v) Is_block(v)

CAMLprim value
caml_alloc_some(value v);

CAMLprim value
array_of_list(value list);

int array_length(value array);
