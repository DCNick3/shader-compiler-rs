use cc::Build;

const LIBSIRIT: &str = "libsirit.a";
const LIBSHADER_COMPILER: &str = "libshader_compiler.a";

fn build_sirit() {
    let mut build = Build::new();

    build
        .cpp(true)
        .include("sirit/externals/SPIRV-Headers/include")
        .include("sirit/include")
        .include("sirit/src")
        .files(
            glob::glob("sirit/src/**/*.cpp")
                .unwrap()
                .map(|v| v.unwrap()),
        );

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
        .include("range-v3/include")
        .include("sirit/externals/SPIRV-Headers/include")
        .include("sirit/include")
        .include(".") // to make sure shader_compiler is on include path
        .files(
            glob::glob("shader_compiler/**/*.cpp")
                .unwrap()
                .map(|v| v.unwrap()),
        );

    if build.get_compiler().is_like_msvc() {
        build.flag("/std:c++20");
    } else {
        build.flag("-std=c++20").flag("-Wno-pragmas");
    }

    build.compile(LIBSHADER_COMPILER);
}

fn main() {
    build_sirit();
    build_shader_compiler();

    println!("cargo:rustc-link-lib=static={}", LIBSIRIT);
    println!("cargo:rustc-link-lib=static={}", LIBSHADER_COMPILER);
}
