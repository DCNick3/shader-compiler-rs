extern crate shader_compiler_sys;

#[cxx::bridge(namespace = "Bridge")]
mod ffi {
    extern "Rust" {
        fn log_debug(message: &CxxString);
        fn log_warning(message: &CxxString);
        fn log_error(message: &CxxString);
    }
    unsafe extern "C++" {
        include!("shader-compiler-bridge/src/bridge.hpp");
        include!(<shader_compiler/environment.h>);

        /// Translates a binary maxwell shader into a SPIR-V shader
        ///
        /// Expects the shader to start with the [Shader Program Header]
        ///
        /// [Shader Program Header] https://download.nvidia.com/open-gpu-doc/Shader-Program-Header/1/Shader-Program-Header.html
        fn translate_shader(shader: Vec<u8>) -> Vec<u32>;
    }
}

fn log_debug(message: &CxxString) {
    tracing::debug!("{}", message);
}

fn log_warning(message: &CxxString) {
    tracing::warn!("{}", message);
}

fn log_error(message: &CxxString) {
    tracing::error!("{}", message);
}

use cxx::CxxString;
pub use ffi::translate_shader;
