#include "bridge.hpp"

#include <shader_compiler/environment.h>
#include <shader_compiler/frontend/maxwell/translate_program.h>
#include <shader_compiler/backend/spirv/emit_spirv.h>

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

    rust::Vec<u8> translate_shader(rust::Vec<u8> shader) {
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

        DummyEnvironment environment{stage, std::move(binary), 0x10030};

//        Shader::Maxwell::TranslateProgram()

    }
}