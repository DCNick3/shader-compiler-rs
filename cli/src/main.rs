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

fn main() {
    let shader = b"xV4\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00bT\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0f\x00\x00\x00\x00\x00\x00\x00\xf1\x07 \xfe\x00\xc4\x1f\x00\x00\x00\x07\x00\x8c\x07\x98L\x01\x00\x17\x00\x8c\x07\x98L\x02\x00'\x00\x8c\x07\x98L\xf0\x07\xe0\xff\x00\xfc\x1f\x00\x03\x007\x00\x8c\x07\x98L\x0f\x00\x07\x00\x00\x00\x00\xe3\x0f\x00\x87\xff\xff\x0f@\xe2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    // skip the 0x30-bytes header before the SPH
    let shader = &shader[0x30..];

    let spirv = shader_compiler_bridge::translate_shader(shader.to_vec());
    let spirv_bytes = spirv.as_byte_slice();

    std::fs::write("shader.spv", spirv_bytes).unwrap();

    let module = spirv_cross::spirv::Module::from_words(&spirv);
    let mut ast = spirv_cross::spirv::Ast::<spirv_cross::glsl::Target>::parse(&module)
        .expect("Failed to parse SPIR-V");

    let mut options = spirv_cross::glsl::CompilerOptions::default();
    options.version = spirv_cross::glsl::Version::V3_30;

    ast.set_compiler_options(&options)
        .expect("Failed to set compiler options");

    let glsl = ast.compile().expect("Failed to compile GLSL");

    println!("{}", glsl);
}
