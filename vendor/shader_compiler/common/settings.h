// SPDX-License-Identifier: MPL-2.0
// Copyright © 2021 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include "common_types.h"

namespace Shader::Settings {
    /**
     * @note Only contains the settings relevant to the shader compiler
     */
    struct Values {
        bool renderer_debug;
        bool disable_shader_loop_safety_checks;
        struct ResolutionScalingInfo {
            u32 up_scale{1};
            u32 down_shift{0};
            f32 up_factor{1.0f};
            f32 down_factor{1.0f};
            bool active{};
            bool downscale{};
        } resolution_info;
    };

    static inline Values values{}; //!< A static structure with the values set by Skyline code
}
