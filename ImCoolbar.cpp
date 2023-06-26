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

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "ImCoolBar.h"
#include "imgui_internal.h"
#include <cmath>
#include <vector>
#include <array>

static float bubbleEffect(const float& v) {
    return ImMax(pow(cos(v * IM_PI * 0.5f), 24.0f), 0.0f);
}

// https://codesandbox.io/s/motion-dock-forked-hs4p8d?file=/src/Dock.tsx
static float getHoverSize(const float& v, const float& vNormalSize, const float& vHoveredSize, const float& vScale) {
    return vNormalSize + vHoveredSize * bubbleEffect(v) * vScale;
}

static bool isWindowHovered(ImGuiWindow* vWindow) {
    return ImGui::IsMouseHoveringRect(vWindow->Rect().Min, vWindow->Rect().Max);
}

static float getCursorOffset() {
    ImGuiContext& g     = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    auto mp = ImGui::GetMousePos().x;

    float offset = 0.0f;

    return offset;
}

IMGUI_API bool ImGui::BeginCoolBar(const char* vLabel, const ImCoolBarFlags& vFlags, const ImCoolBarConfig& vConfig) {
    ImGuiWindowFlags flags =                 //
        ImGuiWindowFlags_NoTitleBar |        //
        ImGuiWindowFlags_NoScrollbar |       //
        ImGuiWindowFlags_AlwaysAutoResize |  //
        ImGuiWindowFlags_NoDocking;          //

    bool res = ImGui::Begin(vLabel, nullptr, flags);

    if (!res) {
        ImGui::End();
    } else {
        ImGuiContext& g     = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();
        window->StateStorage.SetInt(window->GetID("##CoolBarItemIndex"), 0);
        window->StateStorage.SetInt(window->GetID("##CoolBarFlag"), vFlags);
        window->StateStorage.SetFloat(window->GetID("##CoolBarAnchorX"), vConfig.anchor.x);
        window->StateStorage.SetFloat(window->GetID("##CoolBarAnchorY"), vConfig.anchor.y);
        //window->StateStorage.SetFloat(window->GetID("##CoolBarAnimStep"), vConfig.anim_step);
        window->StateStorage.SetFloat(window->GetID("##CoolBarNormalSize"), vConfig.normal_size);
        window->StateStorage.SetFloat(window->GetID("##CoolBarHoveredSize"), vConfig.hovered_size);

        const auto& bar_size   = ImGui::GetWindowSize();
        const float& offset_x = (ImGui::GetIO().DisplaySize.x - bar_size.x) * vConfig.anchor.x;
        const float& offset_y  = (ImGui::GetIO().DisplaySize.y - bar_size.y) * vConfig.anchor.y;
        ImGui::SetWindowPos(ImVec2(offset_x, offset_y));

        if (isWindowHovered(window)) {
            const float& anim_value = window->StateStorage.GetFloat(window->GetID("##CoolBarAnimValue"));
            if (anim_value < 1.0f) {
                window->StateStorage.SetFloat(window->GetID("##CoolBarAnimValue"), anim_value + vConfig.anim_step);
            }
        } else {
            const float& anim_value = window->StateStorage.GetFloat(window->GetID("##CoolBarAnimValue"));
            if (anim_value > 0.0f) {
                window->StateStorage.SetFloat(window->GetID("##CoolBarAnimValue"), anim_value - vConfig.anim_step);
            }
        }

        if (vFlags & ImCoolBar_Horizontal) {
            ImGui::SetCursorPosY(g.Style.WindowPadding.y);
        } else {
            ImGui::SetCursorPosX(g.Style.WindowPadding.x);
        }
    }

    return res;
}

IMGUI_API void ImGui::EndCoolBar() {
    ImGui::End();
}

static float getBarSize(ImGuiWindow* vWindow, const float& vNormalSize, const float& vHoveredSize, const float& vScale) {
    ImGuiContext& g = *GImGui;
    return vNormalSize + vHoveredSize * vScale;
}

IMGUI_API bool ImGui::CoolBarItem(const char* vLabel) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const auto& flag         = window->StateStorage.GetInt(window->GetID("##CoolBarFlag"));
    const auto& idx          = window->StateStorage.GetInt(window->GetID("##CoolBarItemIndex"));
    const auto& anim_value   = window->StateStorage.GetFloat(window->GetID("##CoolBarAnimValue"));
    const auto& normal_size  = window->StateStorage.GetFloat(window->GetID("##CoolBarNormalSize"));
    const auto& hovered_size = window->StateStorage.GetFloat(window->GetID("##CoolBarHoveredSize"));

    if (flag & ImCoolBar_Horizontal) {
        if (idx) {
            ImGui::SameLine();
        }
    }

    float w         = normal_size;
    ImGuiContext& g = *GImGui;

    if (flag & ImCoolBar_Horizontal) {
        const float& mouse_pos    = ImGui::GetMousePos().x;
        const float& dock_width   = window->Size.x;
        const float& btn_center_x = ImGui::GetCursorScreenPos().x + window->StateStorage.GetFloat(window->GetID(vLabel)) * 0.5f;
        const float& diff_pos     = (mouse_pos - btn_center_x) / dock_width;
        w                         = getHoverSize(diff_pos, normal_size, hovered_size, anim_value);
        const float& anchor_y   = window->StateStorage.GetFloat(window->GetID("##CoolBarAnchorY"));
        const float& bar_height   = getBarSize(window, normal_size, hovered_size, anim_value);
        float btn_offset_y      = (bar_height - w) * anchor_y + g.Style.WindowPadding.y;
        ImGui::SetCursorPosY(btn_offset_y);
    } else {
        const float& mouse_pos    = ImGui::GetMousePos().y;
        const float& dock_height  = window->Size.y;
        const float& btn_center_y = ImGui::GetCursorScreenPos().y + window->StateStorage.GetFloat(window->GetID(vLabel)) * 0.5f;
        const float& diff_pos     = (mouse_pos - btn_center_y) / dock_height;
        w                         = getHoverSize(diff_pos, normal_size, hovered_size, anim_value);
        const float& anchor_x  = window->StateStorage.GetFloat(window->GetID("##CoolBarAnchorX"));
        const float& bar_width    = getBarSize(window, normal_size, hovered_size, anim_value);
        float btn_offset_x     = (bar_width - w) * anchor_x + g.Style.WindowPadding.x;
        ImGui::SetCursorPosX(btn_offset_x);
    }

    window->StateStorage.SetInt(window->GetID("##CoolBarItemIndex"), idx + 1);
    window->StateStorage.SetFloat(window->GetID(vLabel), w);
    window->StateStorage.SetFloat(window->GetID("##CoolBarItemCurrentSize"), w);
    return true;
}

IMGUI_API float ImGui::GetCoolBarItemWidth() {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return 0.0f;
    return window->StateStorage.GetFloat(window->GetID("##CoolBarItemCurrentSize"));
}
