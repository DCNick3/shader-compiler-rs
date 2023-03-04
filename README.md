
## shader-compiler-rs

A thin wrapper around the Hades - Yuzu Shader Recompiler, which allows to recompile binary NVN shaders into GLSL, found in many Nintendo Switch games.

## Installation & Usage

Install with:

```bash
git clone https://github.com/DCNick3/shader-compiler-rs
cd shader-compiler-rs
cargo install --path cli
```

Usage: 

```bash
shader-compiler-rs shader.bin shader.glsl
```

`shader.bin` is the data part of the shader you pass to NVN. Note, that, unlike the Ryujinx shader compiler, this tool accepts shaders with the 0x30-bytes NVN-specific header before the [SPH](https://download.nvidia.com/open-gpu-doc/Shader-Program-Header/1/Shader-Program-Header.html).

`shader.glsl` is the output file. It will be overwritten if it already exists.

## Thanks

A big thanks to the [yuzu](https://github.com/yuzu-emu/yuzu) contributors for their work on the shader recompiler, which is used in this tool.

Also, personal thanks to ByLaws and Pharynx on the ReSwitched Discord for patiently answering my questions =)

## License

Same as the skyline's fork of Yuzu Shader Recompiler (Hades), this tool is licensed under the MPL-2.0 license. 