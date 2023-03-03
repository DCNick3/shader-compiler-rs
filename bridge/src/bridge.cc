#include "bridge.hpp"

#include <shader_compiler/environment.h>
#include <shader_compiler/common/log.h>
#include <shader_compiler/host_translate_info.h>
#include <shader_compiler/frontend/maxwell/translate_program.h>
#include <shader_compiler/backend/spirv/emit_spirv.h>

#include <shader-compiler-bridge/src/lib.rs.h>

// implement the logging functions
namespace Shader::Log {
    void Debug(const std::string& message) {
        Bridge::log_debug(message);
    }
    void Warn(const std::string& message) {
        Bridge::log_warning(message);
    }
    void Error(const std::string& message) {
        Bridge::log_error(message);
    }
}

namespace Bridge {

    class DummyEnvironment : public Shader::Environment {
    private:
        boost::container::vector<u8> binary;
        u32 baseOffset;
        // those can be ignored
        u32 textureBufferIndex;
        bool viewportTransformEnabled;
        // TODO: stub those
        // ShaderManager::ConstantBufferRead constantBufferRead;
        // ShaderManager::GetTextureType getTextureType;

    public:
        DummyEnvironment(Shader::Stage pStage,
                         boost::container::vector<u8> pBinary, u32 baseOffset
//                            ShaderManager::ConstantBufferRead constantBufferRead, ShaderManager::GetTextureType getTextureType
        )
                : binary{std::move(pBinary)}, baseOffset{baseOffset},
                textureBufferIndex{0},
                viewportTransformEnabled{false}
//                constantBufferRead{std::move(constantBufferRead)}, getTextureType{std::move(getTextureType)}
        {
            stage = pStage;
            sph = *reinterpret_cast<Shader::ProgramHeader *>(binary.data());
            start_address = baseOffset;
            is_propietary_driver = textureBufferIndex == 2;
        }

        [[nodiscard]] u64 ReadInstruction(u32 address) final {
            address -= baseOffset;
            if (binary.size() < (address + sizeof(u64)))
                throw std::runtime_error("Out of bounds instruction read: 0x{:X}"); // TODO: fmt?
            return *reinterpret_cast<u64 *>(binary.data() + address);
        }

        [[nodiscard]] u32 ReadCbufValue(u32 index, u32 offset) final {
            (void)index;
            (void)offset;
            return 0; // TODO: STUB
//            return constantBufferRead(index, offset);
        }

        [[nodiscard]] Shader::TexturePixelFormat ReadTexturePixelFormat(u32 handle) final {
            (void)handle;
            throw std::runtime_error("ReadTexturePixelFormat not implemented");
        }

        [[nodiscard]] Shader::TextureType ReadTextureType(u32 handle) final {
            (void)handle;
            return Shader::TextureType::Color2D; // TODO: ???
        }

        [[nodiscard]] u32 ReadViewportTransformState() final {
            return viewportTransformEnabled ? 1 : 0; // Only relevant for graphics shaders
        }

        [[nodiscard]] u32 TextureBoundBuffer() const final {
            return textureBufferIndex;
        }

        [[nodiscard]] u32 LocalMemorySize() const final {
            return static_cast<u32>(sph.LocalMemorySize()) + sph.common3.shader_local_memory_crs_size;
        }

        [[nodiscard]] u32 SharedMemorySize() const final {
            return 0; // Only relevant for compute shaders
        }

        [[nodiscard]] std::array<u32, 3> WorkgroupSize() const final {
            return {0, 0, 0}; // Only relevant for compute shaders
        }

        [[nodiscard]] bool HasHLEMacroState() const final {
            return false;
        }

        [[nodiscard]] std::optional<Shader::ReplaceConstant> GetReplaceConstBuffer(u32 bank, u32 offset) final {
            (void)bank;
            (void)offset;
            return std::nullopt;
        }

        void Dump(u64 hash) final {
            (void)hash;
        }
    };

    struct Pools {
        Shader::ObjectPool<Shader::Maxwell::Flow::Block> flowBlockPool;
        Shader::ObjectPool<Shader::IR::Inst> instructionPool;
        Shader::ObjectPool<Shader::IR::Block> blockPool;
    };

    rust::Vec<u32> translate_shader(rust::Vec<u8> shader) {
        boost::container::vector<u8> binary{shader.begin(), shader.end()};

        Shader::ProgramHeader program_header = *reinterpret_cast<Shader::ProgramHeader *>(binary.data());
        Shader::Stage stage;

        switch (program_header.common0.shader_type.Value()) {
            case 1:
                stage = Shader::Stage::VertexB;
                break;
            case 2:
                stage = Shader::Stage::TessellationControl;
                break;
            case 3:
                stage = Shader::Stage::TessellationEval;
                break;
            case 4:
                stage = Shader::Stage::Geometry;
                break;
            case 5:
                stage = Shader::Stage::Fragment;
                break;
            default:
                throw std::runtime_error("Unknown shader type");
        }

        const u32 BLOCK_OFFSET = 0x10030;

        Pools pools;

        DummyEnvironment environment{stage, std::move(binary), BLOCK_OFFSET};

        Shader::Maxwell::Flow::CFG cfg{
            environment,
            pools.flowBlockPool,
            Shader::Maxwell::Location{static_cast<u32>(BLOCK_OFFSET + sizeof(Shader::ProgramHeader))}
        };

        Shader::HostTranslateInfo hostTranslateInfo{
            .support_float16 = true,
            .support_int64 = true,
            .needs_demote_reorder = false,
            .support_snorm_render_buffer = true,
            .support_viewport_index_layer = true,
            .min_ssbo_alignment = 0x10,
            .support_geometry_shader_passthrough = false
        };
        Shader::IR::Program program = Shader::Maxwell::TranslateProgram(
            pools.instructionPool,
            pools.blockPool,
            environment,
            cfg,
            hostTranslateInfo
        );

        Shader::Profile profile{
            .supported_spirv = 0x00010400U,
            .unified_descriptor_binding = true,
            .support_descriptor_aliasing = true,
            .support_int8 = true,
            .support_int16 = true,
            .support_int64 = true,
            .support_vertex_instance_id = false,
            .support_float_controls = true,
            .support_separate_denorm_behavior = true,
            .support_separate_rounding_mode = true,
            .support_fp16_denorm_preserve = static_cast<bool>(true),
            .support_fp32_denorm_preserve = static_cast<bool>(true),
            .support_fp16_denorm_flush = static_cast<bool>(true),
            .support_fp32_denorm_flush = static_cast<bool>(true),
            .support_fp16_signed_zero_nan_preserve = static_cast<bool>(true),
            .support_fp32_signed_zero_nan_preserve = static_cast<bool>(true),
            .support_fp64_signed_zero_nan_preserve = static_cast<bool>(true),
            .support_explicit_workgroup_layout = false,
            .support_vote = true,
            .support_viewport_index_layer_non_geometry = true,
            .support_viewport_mask = false,
            .support_typeless_image_loads = true,
            .support_demote_to_helper_invocation = true,
            .support_int64_atomics = true,
            .support_derivative_control = true,
            .support_geometry_shader_passthrough = false,
            .support_native_ndc = false,
            .warp_size_potentially_larger_than_guest = false,
            .lower_left_origin_mode = false,
            .need_declared_frag_colors = false,
            .has_broken_spirv_position_input = false,
            .has_broken_spirv_subgroup_mask_vector_extract_dynamic = false,
            .has_broken_spirv_subgroup_shuffle = false,
            .max_subgroup_size = 1024, // random
            .disable_subgroup_shuffle = false
        };

        auto spirv = Shader::Backend::SPIRV::EmitSPIRV(profile, program);

        rust::Vec<u32> result;
        result.reserve(spirv.size());
        for (auto i : spirv) {
            result.push_back(i);
        }

        return result;
    }
}