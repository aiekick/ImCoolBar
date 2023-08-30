/*
MIT License

Copyright (c) 2023 Stephane Cuillerdier (aka Aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "../imgui.h"

namespace ImGui
{
    enum ImCoolBarFlags
    {
        None       = 0,
        Vertical   = 1,
        Horizontal = 2
    };

    struct IMGUI_API ImCoolBarConfig
    {
        ImVec2 anchor         = ImVec2(-1.0f, -1.0f);
        float normal_size     = 40.0f;
        float hovered_size    = 60.0f;
        float anim_step       = 0.15f;
        float effect_strength = 0.50f;

        ImCoolBarConfig(
            ImVec2 vAnchor         = ImVec2(-1.0f, -1.0f),
            float vNormalSize      = 40.0f,
            float vHoveredSize     = 60.0f,
            float vAnimStep        = 0.15f,
            float vEffectStrength  = 0.50f
        ):
            anchor(vAnchor),
            normal_size(vNormalSize),
            hovered_size(vHoveredSize),
            anim_step(vAnimStep),
            effect_strength(vEffectStrength)
        {
        }
    };

    IMGUI_API bool BeginCoolBar(
        const char* vLabel,
        ImCoolBarFlags vCBFlags        = ImCoolBarFlags::Vertical,
        const ImCoolBarConfig& vConfig = ImCoolBarConfig(),
        ImGuiWindowFlags vFlags        = ImGuiWindowFlags_None
    );

    IMGUI_API void EndCoolBar();
    IMGUI_API bool CoolBarItem();
    IMGUI_API float GetCoolBarItemWidth();
    IMGUI_API float GetCoolBarItemScale();
}