type t

external create : Store.t -> Module.t -> t = "caml_wasmtime_wasm_instance_new"

external get_extern : t -> string -> Extern.t option
  = "caml_wasmtime_wasm_instance_get_extern"
