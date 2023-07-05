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
#include "ImCoolbar.h"
#include "imgui_internal.h"
#include <cmath>
#include <vector>
#include <array>

//#define ENABLE_DEBUG

static float bubbleEffect(const float& vValue, const float& vStength) {
    return pow(cos(vValue * IM_PI * vStength), 12.0f);
}

// https://codesandbox.io/s/motion-dock-forked-hs4p8d?file=/src/Dock.tsx
static float getHoverSize(const float& vValue, const float& vNormalSize, const float& vHoveredSize, const float& vStength, const float& vScale) {
    return vNormalSize + (vHoveredSize - vNormalSize) * bubbleEffect(vValue, vStength) * vScale;
}

static bool isWindowHovered(ImGuiWindow* vWindow) {
    return ImGui::IsMouseHoveringRect(vWindow->Rect().Min, vWindow->Rect().Max);
}

IMGUI_API bool ImGui::BeginCoolBar(const char* vLabel, ImCoolBarFlags vCBFlags, const ImCoolBarConfig& vConfig, ImGuiWindowFlags vFlags) {
    ImGuiWindowFlags flags = vFlags |                               //
                             ImGuiWindowFlags_NoTitleBar |          //
                             ImGuiWindowFlags_NoScrollbar |         //
                             ImGuiWindowFlags_AlwaysAutoResize |    //
                             ImGuiWindowFlags_NoCollapse |          //
                             ImGuiWindowFlags_NoMove |              //
                             ImGuiWindowFlags_NoSavedSettings |     //
                             ImGuiWindowFlags_NoBackground |        //
                             ImGuiWindowFlags_NoFocusOnAppearing |  //
                             ImGuiWindowFlags_DockNodeHost |        //
                             ImGuiWindowFlags_NoDocking;            //
    bool res = ImGui::Begin(vLabel, nullptr, flags);
    if (!res) {
        ImGui::End();
    } else {
        ImGuiContext& g     = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        window->StateStorage.SetInt(window->GetID("##CoolBarItemIndex"), 0);
        window->StateStorage.SetInt(window->GetID("##CoolBarFlags"), vCBFlags);
        window->StateStorage.SetInt(window->GetID("##CoolBarID"), window->GetID(vLabel));
        window->StateStorage.SetFloat(window->GetID("##CoolBarAnchorX"), vConfig.anchor.x);
        window->StateStorage.SetFloat(window->GetID("##CoolBarAnchorY"), vConfig.anchor.y);
        window->StateStorage.SetFloat(window->GetID("##CoolBarNormalSize"), vConfig.normal_size);
        window->StateStorage.SetFloat(window->GetID("##CoolBarHoveredSize"), vConfig.hovered_size);
        window->StateStorage.SetFloat(window->GetID("##CoolBarEffectStrength"), vConfig.effect_strength);

        const auto& bar_size   = ImGui::GetWindowSize();
        const float& offset_x = (viewport->Size.x - bar_size.x) * vConfig.anchor.x;
        const float& offset_y  = (viewport->Size.y - bar_size.y) * vConfig.anchor.y;
        ImGui::SetWindowPos(ImVec2(offset_x, offset_y) + viewport->Pos);

        if (isWindowHovered(window)) {
            const float& anim_scale = window->StateStorage.GetFloat(window->GetID("##CoolBarAnimScale"));
            if (anim_scale < 1.0f) {
                window->StateStorage.SetFloat(window->GetID("##CoolBarAnimScale"), anim_scale + vConfig.anim_step);
            }
        } else {
            const float& anim_scale = window->StateStorage.GetFloat(window->GetID("##CoolBarAnimScale"));
            if (anim_scale > 0.0f) {
                window->StateStorage.SetFloat(window->GetID("##CoolBarAnimScale"), anim_scale - vConfig.anim_step);
            }
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

IMGUI_API bool ImGui::CoolBarItem() {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const auto& idx               = window->StateStorage.GetInt(window->GetID("##CoolBarItemIndex"));
    const auto& coolbar_id        = window->StateStorage.GetInt(window->GetID("##CoolBarID"));
    const auto& coolbar_item_id   = window->GetID(coolbar_id + idx + 1);
    const auto& current_item_size = window->StateStorage.GetFloat(coolbar_item_id);

    const auto& flags             = window->StateStorage.GetInt(window->GetID("##CoolBarFlags"));
    const auto& anim_scale        = window->StateStorage.GetFloat(window->GetID("##CoolBarAnimScale"));
    const auto& normal_size       = window->StateStorage.GetFloat(window->GetID("##CoolBarNormalSize"));
    const auto& hovered_size      = window->StateStorage.GetFloat(window->GetID("##CoolBarHoveredSize"));
    const auto& effect_strength   = window->StateStorage.GetFloat(window->GetID("##CoolBarEffectStrength"));

    assert(normal_size > 0.0f);

    if (flags & ImCoolBarFlags_Horizontal) {
        if (idx) {
            ImGui::SameLine();
        }
    }

    float w         = normal_size;
    ImGuiContext& g = *GImGui;

    // Can be Horizontal or Vertical, not both
    assert((flags & ImCoolBarFlags_Horizontal) || (flags & ImCoolBarFlags_Vertical));

#ifdef ENABLE_DEBUG
    char buf[256 + 1];
    const auto& color = ImGui::GetColorU32(ImVec4(0, 0, 1, 1));
#endif

    if (isWindowHovered(window) || anim_scale > 0.0f) {
        if (flags & ImCoolBarFlags_Horizontal) {
            const float& btn_center_x = ImGui::GetCursorScreenPos().x + current_item_size * 0.5f;
            const float& diff_pos     = (ImGui::GetMousePos().x - btn_center_x) / window->Size.x;
            w                         = getHoverSize(diff_pos, normal_size, hovered_size, effect_strength, anim_scale);
            const float& anchor_y     = window->StateStorage.GetFloat(window->GetID("##CoolBarAnchorY"));
            const float& bar_height   = getBarSize(window, normal_size, hovered_size, anim_scale);
            float btn_offset_y        = (bar_height - w) * anchor_y + g.Style.WindowPadding.y;
            ImGui::SetCursorPosY(btn_offset_y);
#ifdef ENABLE_DEBUG
            ImFormatString(buf, 256, "%.3f", diff_pos);
            const auto& bug_size = ImGui::CalcTextSize(buf);
            window->DrawList->AddText(ImVec2(btn_center_x - bug_size.x * 0.5f, window->Pos.y + window->Size.y * (1.0f - anchor_y)), color, buf);
            window->DrawList->AddLine(ImVec2(btn_center_x, window->Pos.y), ImVec2(btn_center_x, window->Pos.y + window->Size.y), color, 2.0f);
#endif
        } else {
            const float& btn_center_y = ImGui::GetCursorScreenPos().y + current_item_size * 0.5f;
            const float& diff_pos     = (ImGui::GetMousePos().y - btn_center_y) / window->Size.y;
            w                         = getHoverSize(diff_pos, normal_size, hovered_size, effect_strength, anim_scale);
            const float& anchor_x     = window->StateStorage.GetFloat(window->GetID("##CoolBarAnchorX"));
            const float& bar_width    = getBarSize(window, normal_size, hovered_size, anim_scale);
            float btn_offset_x        = (bar_width - w) * anchor_x + g.Style.WindowPadding.x;
            ImGui::SetCursorPosX(btn_offset_x);
#ifdef ENABLE_DEBUG
            ImFormatString(buf, 256, "%.3f", diff_pos);
            const auto& bug_size = ImGui::CalcTextSize(buf);
            window->DrawList->AddText(ImVec2(window->Pos.x + window->Size.x * (1.0f - anchor_x), btn_center_y), color, buf);
            window->DrawList->AddLine(ImVec2(window->Pos.x, btn_center_y), ImVec2(window->Pos.x + window->Size.x, btn_center_y), color, 2.0f);
#endif
        }
    }

    window->StateStorage.SetInt(window->GetID("##CoolBarItemIndex"), idx + 1);
    window->StateStorage.SetFloat(coolbar_item_id, w);
    window->StateStorage.SetFloat(window->GetID("##CoolBarItemCurrentSize"), w);
    window->StateStorage.SetFloat(window->GetID("##CoolBarItemCurrentScale"), w / normal_size);

    return true;
}

IMGUI_API float ImGui::GetCoolBarItemWidth() {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return 0.0f;
    return window->StateStorage.GetFloat(window->GetID("##CoolBarItemCurrentSize"));
}

IMGUI_API float ImGui::GetCoolBarItemScale() {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return 0.0f;
    return window->StateStorage.GetFloat(window->GetID("##CoolBarItemCurrentScale"));
}
