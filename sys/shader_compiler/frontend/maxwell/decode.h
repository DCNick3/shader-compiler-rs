// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <shader_compiler/common/common_types.h>
#include <shader_compiler/frontend/maxwell/opcodes.h>

namespace Shader::Maxwell {

[[nodiscard]] Opcode Decode(u64 insn);

} // namespace Shader::Maxwell
