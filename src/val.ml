type t

external i32 : int32 -> t = "caml_wasmtime_wasm_int32_to_val"

external to_int32 : t -> int32 option = "caml_wasmtime_wasm_val_to_int32"
