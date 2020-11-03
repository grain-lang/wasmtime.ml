open Wasmtime

let engine = Engine.create ()

(* A `Store` is a sort of "global object" in a sense, but for now it suffices
to say that it's generally passed to most constructors. *)
let store = Store.create engine

let read_whole_file filename =
  let ch = open_in filename in
  let s = really_input_string ch (in_channel_length ch) in
  close_in ch;
  Bytes.of_string s

let wasm = read_whole_file "adder.wasm"

(* We start off by creating a `Module` which represents a compiled form
of our input wasm module. In this case it'll be JIT-compiled after
we parse the text format. *)
let mod_ = Module.create store wasm

(* After we have a compiled `Module` we can then instantiate it, creating
an `Instance` which we can actually poke at functions on. *)
let instance = Instance.create store mod_

(* The `Instance` gives us access to various exported functions and items,
which we access here to pull out our `answer` exported function and run it. *)
let add =
  Instance.get_extern instance "add"
  |> Option.fold ~some:Extern.as_func ~none:None
  |> Option.get

(* And finally we can call our function! Note that the error propagation
with `?` is done to handle the case where the wasm function traps. *)
let result =
  Func.call add [ Val.i32 88l; Val.i32 2l ] |> Val.to_int32 |> Option.get

let _ = Printf.printf "result: %ld" result
