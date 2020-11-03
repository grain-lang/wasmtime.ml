let _ = Callback.register "array_of_list" Array.of_list

let _ = Callback.register "array_length" Array.length

type t

external call : t -> Val.t list -> Val.t = "caml_wasmtime_wasm_func_call"
