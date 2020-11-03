#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/alloc.h>

#include "ocaml_helpers.h"
#include "wasm.h"

// BTW
// Any reference to INSTANCE is a tuple of (Module, Instance)

/* Allocating an OCaml custom block */
static value alloc_wasm_engine_t(wasm_engine_t* engine)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_engine_t*), 0, 1);
  wasm_engine_t_val(v) = engine;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_engine_new(value unit) {
  CAMLparam1(unit);
  wasm_engine_t* engine = wasm_engine_new();
  CAMLreturn(alloc_wasm_engine_t(engine));
}

static value alloc_wasm_store_t(wasm_store_t* store)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_store_t*), 0, 1);
  wasm_store_t_val(v) = store;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_store_new(value _engine) {
  CAMLparam1(_engine);
  wasm_engine_t* engine = wasm_engine_t_val(_engine);
  wasm_store_t* store = wasm_store_new(engine);
  CAMLreturn(alloc_wasm_store_t(store));
}

static value alloc_wasm_module_t(wasm_module_t* module)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_module_t*), 0, 1);
  wasm_module_t_val(v) = module;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_module_new(value _store, value _bytes) {
  CAMLparam2(_store, _bytes);
  wasm_store_t* store = wasm_store_t_val(_store);
  const char* bytes = Safe_String_val(_bytes);
  int bytesLen = caml_string_length(_bytes);
  wasm_byte_vec_t wasm;
  wasm_byte_vec_new(&wasm, bytesLen, bytes);
  wasm_module_t* module = wasm_module_new(store, &wasm);
  wasm_byte_vec_delete(&wasm);
  CAMLreturn(alloc_wasm_module_t(module));
}

static value alloc_wasm_instance_t(wasm_instance_t* instance)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_instance_t*), 0, 1);
  wasm_instance_t_val(v) = instance;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_instance_new(value _store, value _module) {
  CAMLparam2(_store, _module);
  wasm_store_t* store = wasm_store_t_val(_store);
  wasm_module_t* module = wasm_module_t_val(_module);
  wasm_trap_t* trap;
  wasm_instance_t* instance = wasm_instance_new(store, module, NULL, &trap);

  CAMLlocal1(tuple);
  tuple = caml_alloc_tuple(2);
  Store_field(tuple, 0, _module);
  Store_field(tuple, 1, alloc_wasm_instance_t(instance));
  CAMLreturn(tuple);
}

static value alloc_wasm_extern_t(wasm_extern_t* _extern)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_extern_t*), 0, 1);
  wasm_extern_t_val(v) = _extern;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_instance_get_extern(value _instanceTuple, value _lookupName) {
  CAMLparam2(_instanceTuple, _lookupName);

  const char* lookupName = Safe_String_val(_lookupName);
  size_t lookupLen = strlen(lookupName);

  wasm_module_t* module = wasm_module_t_val(Field(_instanceTuple, 0));
  wasm_exporttype_vec_t exporttypes;
  wasm_module_exports(module, &exporttypes);

  wasm_instance_t* instance = wasm_instance_t_val(Field(_instanceTuple, 1));
  wasm_extern_vec_t externs;
  wasm_instance_exports(instance, &externs);

  CAMLlocal1(result);
  result = Val_none;
  for (size_t i = 0; i < exporttypes.size; i++) {
    wasm_exporttype_t* exporttype = exporttypes.data[i];
    const char* exportName = (const char*) wasm_exporttype_name(exporttype)->data;
    if (lookupLen == strlen(exportName) && memcmp(lookupName, exportName, lookupLen) == 0) {
      CAMLreturn(caml_alloc_some(alloc_wasm_extern_t(externs.data[i])));
    }
  }

  CAMLreturn(result);
}

static value alloc_wasm_func_t(wasm_func_t* func)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_func_t*), 0, 1);
  wasm_func_t_val(v) = func;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_extern_as_func(value _extern) {
  CAMLparam1(_extern);
  wasm_extern_t* ext = wasm_extern_t_val(_extern);
  wasm_func_t* func = wasm_extern_as_func(ext);
  if (func != NULL) {
    CAMLreturn(caml_alloc_some(alloc_wasm_func_t(func)));
  } else {
    CAMLreturn(Val_none);
  }
}

static value alloc_wasm_val_t(wasm_val_t val)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(wasm_val_t), 0, 1);
  wasm_val_t_val(v) = val;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_int32_to_val(value _i32) {
  CAMLparam1(_i32);
  int32_t i32 = Int32_val(_i32);
  wasm_val_t val;
  val.kind = WASM_I32;
  val.of.i32 = i32;
  CAMLreturn(alloc_wasm_val_t(val));
}

static value alloc_int32(int32_t i32)
{
  value v = caml_alloc_custom(&wasmtime_ops, sizeof(int32_t), 0, 1);
  Int32_val(v) = i32;
  return v;
}

CAMLprim value
caml_wasmtime_wasm_val_to_int32(value _val) {
  CAMLparam1(_val);
  wasm_val_t val = wasm_val_t_val(_val);
  if (val.kind == WASM_I32) {
    CAMLreturn(caml_alloc_some(alloc_int32(val.of.i32)));
  } else {
    CAMLreturn(Val_none);
  }
}

CAMLprim value
caml_wasmtime_wasm_func_call(value _func, value _args) {
  CAMLparam2(_func, _args);
  wasm_func_t* func = wasm_func_t_val(_func);
  _args = array_of_list(_args);
  int argsLength = array_length(_args);
  wasm_val_t args[argsLength];
  for (int i = 0; i < argsLength; i++) {
    args[i] = wasm_val_t_val(Field(_args, i));
  }
  wasm_val_t results[1];
  /*wasm_trap_t* trap = */wasm_func_call(func, args, results);
  CAMLreturn(alloc_wasm_val_t(results[0]));
}
