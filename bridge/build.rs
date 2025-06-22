use std::path::PathBuf;

// TODO: note that this is not suitable for crates.io, as in that case we no longer have access to the workspace root
fn vendor_dir() -> PathBuf {
    // the vendor dir is the root of the workspace
    let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        // the parent is the workspace root
        .parent()
        .unwrap()
        .to_path_buf();
    path.push("vendor");
    path
}

fn main() {
    let mut build = cxx_build::bridge("src/lib.rs");

    build
        .file("src/bridge.cc")
        .flag_if_supported("-std=c++20")
        .flag_if_supported("-Wno-pragmas")
        .include(vendor_dir().join("range-v3/include"))
        .include(vendor_dir().join("sirit/externals/SPIRV-Headers/include"))
        .include(vendor_dir().join("sirit/include"))
        .include(vendor_dir().join("fmt/include"))

        // technically boost container has some .cpp files for us to compile, but the recompiler works without them, so :shrug:
        .include(vendor_dir().join("boost/config/include"))
        .include(vendor_dir().join("boost/assert/include"))
        .include(vendor_dir().join("boost/move/include"))
        .include(vendor_dir().join("boost/intrusive/include"))
        .include(vendor_dir().join("boost/container/include"))
        .include(vendor_dir()) // to make sure shader_compiler is on include path
        .flag("-mno-ms-bitfields"); // this is needed to make MinGW build succeed

    if build.get_compiler().is_like_msvc() {
        build.flag("/std:c++20");
    } else {
        build.flag("-std=c++20");
    }

    build.compile("shader-compiler-bridge");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/bridge.cc");
    println!("cargo:rerun-if-changed=src/bridge.hpp");
    println!("cargo:rerun-if-changed={}", vendor_dir().to_str().unwrap());
}
