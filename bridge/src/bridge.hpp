#include <rust/cxx.h>

#include <shader_compiler/common/common_types.h>

namespace Bridge {
    rust::Vec<u32> translate_shader(rust::Vec<u8> shader);
}
