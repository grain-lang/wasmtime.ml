type t

external create : Store.t -> bytes -> t = "caml_wasmtime_wasm_module_new"
