// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <shader_compiler/common/bit_field.h>
#include <shader_compiler/common/common_types.h>
#include <shader_compiler/frontend/maxwell/translate/impl/impl.h>

namespace Shader::Maxwell {
namespace {
void POPC(TranslatorVisitor& v, u64 insn, const IR::U32& src) {
    union {
        u64 raw;
        BitField<0, 8, IR::Reg> dest_reg;
        BitField<40, 1, u64> tilde;
    } const popc{insn};

    const IR::U32 operand = popc.tilde == 0 ? src : v.ir.BitwiseNot(src);
    const IR::U32 result = v.ir.BitCount(operand);
    v.X(popc.dest_reg, result);
}
} // Anonymous namespace

void TranslatorVisitor::POPC_reg(u64 insn) {
    POPC(*this, insn, GetReg20(insn));
}

void TranslatorVisitor::POPC_cbuf(u64 insn) {
    POPC(*this, insn, GetCbuf(insn));
}

void TranslatorVisitor::POPC_imm(u64 insn) {
    POPC(*this, insn, GetImm20(insn));
}

} // namespace Shader::Maxwell
