// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <shader_compiler/environment.h>
#include <shader_compiler/frontend/ir/program.h>

namespace Shader {
struct HostTranslateInfo;
}

namespace Shader::Optimization {

void CollectShaderInfoPass(Environment& env, IR::Program& program);
void ConstantPropagationPass(Environment& env, IR::Program& program);
void DeadCodeEliminationPass(IR::Program& program);
void GlobalMemoryToStorageBufferPass(IR::Program& program, const HostTranslateInfo& host_info);
void IdentityRemovalPass(IR::Program& program);
void LowerFp16ToFp32(IR::Program& program);
void LowerInt64ToInt32(IR::Program& program);
void RescalingPass(IR::Program& program);
void SsaRewritePass(IR::Program& program);
void PositionPass(Environment& env, IR::Program& program);
void TexturePass(Environment& env, IR::Program& program, const HostTranslateInfo& host_info);
void LayerPass(IR::Program& program, const HostTranslateInfo& host_info);
void VerificationPass(const IR::Program& program);

// Dual Vertex
void VertexATransformPass(IR::Program& program);
void VertexBTransformPass(IR::Program& program);
void JoinTextureInfo(Info& base, Info& source);
void JoinStorageInfo(Info& base, Info& source);

} // namespace Shader::Optimization
