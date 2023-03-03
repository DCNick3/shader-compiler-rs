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
    cxx_build::bridge("src/lib.rs")
        .file("src/bridge.cc")
        .flag_if_supported("-std=c++20")
        .flag_if_supported("-Wno-pragmas")
        .include(vendor_dir().join("range-v3/include"))
        .include(vendor_dir().join("sirit/externals/SPIRV-Headers/include"))
        .include(vendor_dir().join("sirit/include"))
        .include(vendor_dir()) // to make sure shader_compiler is on include path
        .compile("shader-compiler-bridge");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/bridge.cc");
    println!("cargo:rerun-if-changed={}", vendor_dir().to_str().unwrap());
}
