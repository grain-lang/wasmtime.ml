module C = Configurator.V1

let () =
  C.main ~name:"wasmtime" (fun c ->
      let default : C.Pkg_config.package_conf =
        { libs = []; cflags = [ "-O2"; "-Wall"; "-Wextra" ] }
      in
      let conf =
        match C.ocaml_config_var c "system" with
        | None -> default
        | Some ("linux" | "macosx") ->
            { default with libs = [ "-lpthread"; "-ldl"; "-lm" ] }
        | Some "mingw64" ->
            (* Taken from https://github.com/bytecodealliance/wasmtime-go/commit/21d072af9cc286718c093b3df43f2948e3926d8d *)
            {
              cflags = [ "-DWASM_API_EXTERN="; "-DWASI_API_EXTERN=" ];
              libs = [ "-luserenv"; "-lole32"; "-lws2_32"; "-lkernel32" ];
            }
        | Some unknown ->
            failwith ("We don't know how to build for platform" ^ unknown)
      in

      C.Flags.write_sexp "c_flags.sexp" conf.cflags;
      C.Flags.write_sexp "c_library_flags.sexp" conf.libs)
