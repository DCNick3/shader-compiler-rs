extern crate shader_compiler_sys;

#[cxx::bridge(namespace = "Shader")]
mod ffi {
    unsafe extern "C++" {
        include!(<shader_compiler/environment.h>);
    }
}
