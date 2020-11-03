type t

external as_func : t -> Func.t option = "caml_wasmtime_wasm_extern_as_func"
