// SPDX-License-Identifier: MPL-2.0
// Copyright © 2021 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <fmt/format.h>

namespace Shader::Log {
    // Proxy logging framework, these functions are implemented by Skyline
    void Debug(const std::string& message);
    void Warn(const std::string& message);
    void Error(const std::string& message);
}

#define LOG_DEBUG(tag, message, ...) Shader::Log::Debug(fmt::format("Shader Compiler (" #tag "): " message __VA_OPT__(,) __VA_ARGS__))
#define LOG_WARNING(tag, message, ...) Shader::Log::Warn(fmt::format("Shader Compiler (" #tag "): " message __VA_OPT__(,) __VA_ARGS__))
#define LOG_ERROR(tag, message, ...) Shader::Log::Error(fmt::format("Shader Compiler (" #tag "): " message __VA_OPT__(,) __VA_ARGS__))
