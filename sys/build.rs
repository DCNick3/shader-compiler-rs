use cc::Build;
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

const LIBSIRIT: &str = "sirit";
const LIBSHADER_COMPILER: &str = "shader_compiler";

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

fn collect_cpp_files(path: &Path) -> impl Iterator<Item = PathBuf> {
    WalkDir::new(path)
        .into_iter()
        .map(|v| v.unwrap())
        .filter(|v| v.file_type().is_file())
        .map(|v| v.path().to_path_buf())
        .filter(|p| p.extension() == Some("cpp".as_ref()))
}

fn build_sirit() {
    let mut build = Build::new();

    build
        .cpp(true)
        .include(vendor_dir().join("sirit/externals/SPIRV-Headers/include"))
        .include(vendor_dir().join("sirit/include"))
        .include(vendor_dir().join("sirit/src"))
        .files(collect_cpp_files(&vendor_dir().join("sirit/src")));

    if build.get_compiler().is_like_msvc() {
        build.flag("/std:c++20");
    } else {
        build.flag("-std=c++20");
    }

    build.compile(LIBSIRIT);
}

fn build_shader_compiler() {
    let mut build = Build::new();

    build
        .cpp(true)
        .include(vendor_dir().join("range-v3/include"))
        .include(vendor_dir().join("sirit/externals/SPIRV-Headers/include"))
        .include(vendor_dir().join("sirit/include"))
        .include(vendor_dir()) // to make sure shader_compiler is on include path
        .files(collect_cpp_files(&vendor_dir().join("shader_compiler")));

    if build.get_compiler().is_like_msvc() {
        build.flag("/std:c++20");
    } else {
        build
            .flag("-std=c++20")
            .flag("-Wno-pragmas")
            .flag("-Wno-unused-parameter")
            .flag("-Wno-unused-function")
            .flag("-Wno-unused-variable")
            .flag("-Wno-unused-but-set-variable")
            .flag("-w"); // no warnings at all, as some are non-suppressible =(
    }

    build.compile(LIBSHADER_COMPILER);
}

fn main() {
    build_sirit();
    build_shader_compiler();

    println!("cargo:rustc-link-lib=static={}", LIBSIRIT);
    println!("cargo:rustc-link-lib=static={}", LIBSHADER_COMPILER);

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}", vendor_dir().to_str().unwrap());
}
