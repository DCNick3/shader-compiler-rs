// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <shader_compiler/backend/glsl/emit_glsl_instructions.h>
#include <shader_compiler/backend/glsl/glsl_emit_context.h>
#include <shader_compiler/frontend/ir/value.h>

namespace Shader::Backend::GLSL {

void EmitLogicalOr(EmitContext& ctx, IR::Inst& inst, std::string_view a, std::string_view b) {
    ctx.AddU1("{}={}||{};", inst, a, b);
}

void EmitLogicalAnd(EmitContext& ctx, IR::Inst& inst, std::string_view a, std::string_view b) {
    ctx.AddU1("{}={}&&{};", inst, a, b);
}

void EmitLogicalXor(EmitContext& ctx, IR::Inst& inst, std::string_view a, std::string_view b) {
    ctx.AddU1("{}={}^^{};", inst, a, b);
}

void EmitLogicalNot(EmitContext& ctx, IR::Inst& inst, std::string_view value) {
    ctx.AddU1("{}=!{};", inst, value);
}
} // namespace Shader::Backend::GLSL
