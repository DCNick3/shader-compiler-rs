// naga produces unreadable code =(
// so, it's commented out for now
/*

   let module = naga::front::spv::parse_u8_slice(&spirv, &Default::default()).unwrap();

   let info = naga::valid::Validator::new(
       naga::valid::ValidationFlags::all(),
       naga::valid::Capabilities::empty(),
   )
   .validate(&module)
   .expect("Validation failed");

   let pipeline_options = naga::back::glsl::PipelineOptions {
       entry_point: "main".to_string(),
       shader_stage: naga::ShaderStage::Fragment, // TODO: get the stage from the raw shader
       multiview: None,
   };

   let glsl_options = Default::default();

   let mut buffer = String::new();
   let mut writer = naga::back::glsl::Writer::new(
       &mut buffer,
       &module,
       &info,
       &glsl_options,
       &pipeline_options,
       naga::proc::BoundsCheckPolicies::default(),
   )
   .expect("Failed to create GLSL writer");
   writer.write().expect("Failed to write GLSL");

   println!("{}", buffer);
*/

use byte_slice_cast::AsByteSlice;
use clap::Parser;
use std::path::PathBuf;

#[derive(clap::Parser)]
struct Args {
    /// Path to the input binary maxwell shader
    ///
    /// Expects a 0x30-byte header before the SPH
    input: PathBuf,
    /// Path to the output GLSL shader
    output: PathBuf,
}

fn main() {
    let args = Args::parse();

    let shader = std::fs::read(args.input).unwrap();
    // skip the 0x30-bytes header before the SPH
    let shader = &shader[0x30..];

    let spirv = shader_compiler_bridge::translate_shader(shader.to_vec());

    // let spirv_bytes = spirv.as_byte_slice();
    // std::fs::write("shader.spv", spirv_bytes).unwrap();

    let module = spirv_cross::spirv::Module::from_words(&spirv);
    let mut ast = spirv_cross::spirv::Ast::<spirv_cross::glsl::Target>::parse(&module)
        .expect("Failed to parse SPIR-V");

    let mut options = spirv_cross::glsl::CompilerOptions::default();
    options.version = spirv_cross::glsl::Version::V4_50;
    options.vulkan_semantics = true;

    ast.set_compiler_options(&options)
        .expect("Failed to set compiler options");

    let glsl = ast.compile().expect("Failed to compile GLSL");

    std::fs::write(args.output, glsl).unwrap();
}
