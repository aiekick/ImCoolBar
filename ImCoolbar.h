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

#include "imgui.h"

typedef int ImCoolBarFlags;  //
enum ImCoolBarFlags_ {       //
    ImCoolBar_Vertical = 0,  //
    ImCoolBar_Horizontal     //
};

namespace ImGui {

struct IMGUI_API ImCoolBarConfig {
    ImVec2 anchor      = ImVec2(-1.0f, -1.0f);
    float normal_size  = 40.0f;
    float hovered_size = 60.0f;
    float anim_step    = 0.05f;
    ImCoolBarConfig(const ImVec2 vAnchor = ImVec2(-1.0f, -1.0f), const float& vNormalSize = 40.0f, const float& vHoveredSize = 60.0f, const float& vAnimStep = 0.05f) 
        : anchor(vAnchor), normal_size(vNormalSize), hovered_size(vHoveredSize), anim_step(vAnimStep) {}
};

IMGUI_API bool BeginCoolBar(const char* vLabel, const ImCoolBarFlags& vFlags = ImCoolBar_Vertical, const ImCoolBarConfig& vConfig = {});
IMGUI_API void EndCoolBar();
IMGUI_API bool CoolBarItem(const char* vLabel);
IMGUI_API float GetCoolBarItemWidth();

}  // namespace ImGui
