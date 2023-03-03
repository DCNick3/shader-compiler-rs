// SPDX-License-Identifier: MPL-2.0
// Copyright © 2018 yuzu Emulator Project (https://github.com/yuzu-emu/yuzu/)

#pragma once

#include <bit>
#include <climits>
#include <cstddef>

#include <shader_compiler/common/common_types.h>

namespace Common {

/// Gets the size of a specified type T in bits.
    template <typename T>
    [[nodiscard]] constexpr std::size_t BitSize() {
        return sizeof(T) * CHAR_BIT;
    }

    [[nodiscard]] constexpr u32 MostSignificantBit32(const u32 value) {
        return 31U - static_cast<u32>(std::countl_zero(value));
    }

    [[nodiscard]] constexpr u32 MostSignificantBit64(const u64 value) {
        return 63U - static_cast<u32>(std::countl_zero(value));
    }

    [[nodiscard]] constexpr u32 Log2Floor32(const u32 value) {
        return MostSignificantBit32(value);
    }

    [[nodiscard]] constexpr u32 Log2Floor64(const u64 value) {
        return MostSignificantBit64(value);
    }

    [[nodiscard]] constexpr u32 Log2Ceil32(const u32 value) {
        const u32 log2_f = Log2Floor32(value);
        return log2_f + static_cast<u32>((value ^ (1U << log2_f)) != 0U);
    }

    [[nodiscard]] constexpr u32 Log2Ceil64(const u64 value) {
        const u64 log2_f = Log2Floor64(value);
        return static_cast<u32>(log2_f + static_cast<u64>((value ^ (1ULL << log2_f)) != 0ULL));
    }

} // namespace Common
