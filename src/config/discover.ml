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
        | Some "mingw" ->
            {
              cflags = [ "-DWASM_API_EXTERN="; "-DWASI_API_EXTERN=" ];
              libs =
                [ "-luserenv"; "-lole32"; "-lntdll"; "-lws2_32"; "-lkernel32" ];
            }
        | Some _ -> default
      in

      C.Flags.write_sexp "c_flags.sexp" conf.cflags;
      C.Flags.write_sexp "c_library_flags.sexp" conf.libs)
