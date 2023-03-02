// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <shader_compiler/environment.h>
#include <shader_compiler/frontend/ir/abstract_syntax_list.h>
#include <shader_compiler/frontend/ir/basic_block.h>
#include <shader_compiler/frontend/ir/value.h>
#include <shader_compiler/frontend/maxwell/control_flow.h>
#include <shader_compiler/object_pool.h>

namespace Shader {
struct HostTranslateInfo;
namespace Maxwell {

[[nodiscard]] IR::AbstractSyntaxList BuildASL(ObjectPool<IR::Inst>& inst_pool,
                                              ObjectPool<IR::Block>& block_pool, Environment& env,
                                              Flow::CFG& cfg, const HostTranslateInfo& host_info);

} // namespace Maxwell
} // namespace Shader
