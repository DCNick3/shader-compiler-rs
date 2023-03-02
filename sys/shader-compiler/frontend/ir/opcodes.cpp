// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <shader_compiler/frontend/ir/opcodes.h>

namespace Shader::IR {

std::string_view NameOf(Opcode op) {
    return Detail::META_TABLE[static_cast<size_t>(op)].name;
}

} // namespace Shader::IR
