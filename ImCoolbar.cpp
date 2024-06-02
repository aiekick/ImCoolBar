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

#define ICB_PREFIX "ICB"
//#define ENABLE_DEBUG

#ifdef _MSC_VER
#include <Windows.h>
#define IM_DEBUG_BREAK       \
    if (IsDebuggerPresent()) \
    __debugbreak()
#else
#define IM_DEBUG_BREAK
#endif

#define BREAK_ON_KEY(KEY)         \
    if (ImGui::IsKeyPressed(KEY)) \
    IM_DEBUG_BREAK

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
        // Can be Horizontal or Vertical, not both
        // this working for now, just because we have only H OR V flags
        IM_ASSERT(                                                                    //
            ((vCBFlags & ImCoolBarFlags_Horizontal) == ImCoolBarFlags_Horizontal) ||  //
            ((vCBFlags & ImCoolBarFlags_Vertical) == ImCoolBarFlags_Vertical)         //
        );

        ImGuiContext& g     = *GImGui;
        ImGuiWindow* window_ptr = GetCurrentWindow();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        window_ptr->StateStorage.SetInt(window_ptr->GetID(ICB_PREFIX "ItemIndex"), 0);
        window_ptr->StateStorage.SetInt(window_ptr->GetID(ICB_PREFIX "Flags"), vCBFlags);
        window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "AnchorX"), vConfig.anchor.x);
        window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "AnchorY"), vConfig.anchor.y);
        window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "NormalSize"), vConfig.normal_size);
        window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "HoveredSize"), vConfig.hovered_size);
        window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "EffectStrength"), vConfig.effect_strength);

        BREAK_ON_KEY(ImGuiKey_D);
        const auto& bar_size = window_ptr->ContentSize + ImGui::GetStyle().WindowPadding * 2.0f;
        const auto& new_pos = viewport->Pos + (viewport->Size - bar_size) * vConfig.anchor;
        ImGui::SetWindowPos(new_pos);

        const auto anim_scale_id = window_ptr->GetID(ICB_PREFIX "AnimScale");
        float anim_scale = window_ptr->StateStorage.GetFloat(anim_scale_id);
        if (isWindowHovered(window_ptr)) {
            if (anim_scale < 1.0f) {
                anim_scale += vConfig.anim_step;
            }
        } else {
            if (anim_scale > 0.0f) {
                anim_scale -= vConfig.anim_step;
            }
        }
        window_ptr->StateStorage.SetFloat(anim_scale_id, anim_scale);
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
    ImGuiWindow* window_ptr = GetCurrentWindow();
    if (window_ptr->SkipItems)
        return false;

    const auto& idx               = window_ptr->StateStorage.GetInt(window_ptr->GetID(ICB_PREFIX "ItemIndex"));
    const auto& coolbar_item_id = window_ptr->GetID(window_ptr->ID + idx + 1);
    const auto& current_item_size = window_ptr->StateStorage.GetFloat(coolbar_item_id);
    const auto& flags             = window_ptr->StateStorage.GetInt(window_ptr->GetID(ICB_PREFIX "Flags"));
    const auto& anim_scale        = window_ptr->StateStorage.GetFloat(window_ptr->GetID(ICB_PREFIX "AnimScale"));
    const auto& normal_size       = window_ptr->StateStorage.GetFloat(window_ptr->GetID(ICB_PREFIX "NormalSize"));
    const auto& hovered_size      = window_ptr->StateStorage.GetFloat(window_ptr->GetID(ICB_PREFIX "HoveredSize"));
    const auto& effect_strength   = window_ptr->StateStorage.GetFloat(window_ptr->GetID(ICB_PREFIX "EffectStrength"));

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

    if (isWindowHovered(window_ptr) || anim_scale > 0.0f) {
        if (flags & ImCoolBarFlags_Horizontal) {
            const float& btn_center_x = ImGui::GetCursorScreenPos().x + current_item_size * 0.5f;
            const float& diff_pos     = (ImGui::GetMousePos().x - btn_center_x) / window_ptr->Size.x;
            w                         = getHoverSize(diff_pos, normal_size, hovered_size, effect_strength, anim_scale);
            const float& anchor_y     = window_ptr->StateStorage.GetFloat(window_ptr->GetID(ICB_PREFIX "AnchorY"));
            const float& bar_height   = getBarSize(window_ptr, normal_size, hovered_size, anim_scale);
            float btn_offset_y        = (bar_height - w) * anchor_y + g.Style.WindowPadding.y;
            ImGui::SetCursorPosY(btn_offset_y);
#ifdef ENABLE_DEBUG
            ImFormatString(buf, 256, "%.3f", diff_pos);
            const auto& bug_size = ImGui::CalcTextSize(buf);
            window_ptr->DrawList->AddText(ImVec2(btn_center_x - bug_size.x * 0.5f, window_ptr->Pos.y + window_ptr->Size.y * (1.0f - anchor_y)), color, buf);
            window_ptr->DrawList->AddLine(ImVec2(btn_center_x, window_ptr->Pos.y), ImVec2(btn_center_x, window_ptr->Pos.y + window_ptr->Size.y), color, 2.0f);
#endif
        } else {
            const float& btn_center_y = ImGui::GetCursorScreenPos().y + current_item_size * 0.5f;
            const float& diff_pos     = (ImGui::GetMousePos().y - btn_center_y) / window_ptr->Size.y;
            w                         = getHoverSize(diff_pos, normal_size, hovered_size, effect_strength, anim_scale);
            const float& anchor_x     = window_ptr->StateStorage.GetFloat(window_ptr->GetID(ICB_PREFIX "AnchorX"));
            const float& bar_width    = getBarSize(window_ptr, normal_size, hovered_size, anim_scale);
            float btn_offset_x        = (bar_width - w) * anchor_x + g.Style.WindowPadding.x;
            ImGui::SetCursorPosX(btn_offset_x);
#ifdef ENABLE_DEBUG
            ImFormatString(buf, 256, "%.3f", diff_pos);
            const auto& bug_size = ImGui::CalcTextSize(buf);
            window_ptr->DrawList->AddText(ImVec2(window_ptr->Pos.x + window_ptr->Size.x * (1.0f - anchor_x), btn_center_y), color, buf);
            window_ptr->DrawList->AddLine(ImVec2(window_ptr->Pos.x, btn_center_y), ImVec2(window_ptr->Pos.x + window_ptr->Size.x, btn_center_y), color, 2.0f);
#endif
        }
    }

    window_ptr->StateStorage.SetInt(window_ptr->GetID(ICB_PREFIX "ItemIndex"), idx + 1);
    window_ptr->StateStorage.SetFloat(coolbar_item_id, w);
    window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "ItemCurrentSize"), w);
    window_ptr->StateStorage.SetFloat(window_ptr->GetID(ICB_PREFIX "ItemCurrentScale"), w / normal_size);

    return true;
}

IMGUI_API float ImGui::GetCoolBarItemWidth() {
    ImGuiWindow* window_ptr = GetCurrentWindow();
    if (window_ptr->SkipItems) {
        return 0.0f;
    }
    return window_ptr->StateStorage.GetFloat(  //
        window_ptr->GetID(ICB_PREFIX "ItemCurrentSize"));
}

IMGUI_API float ImGui::GetCoolBarItemScale() {
    ImGuiWindow* window_ptr = GetCurrentWindow();
    if (window_ptr->SkipItems) {
        return 0.0f;
    }

    return window_ptr->StateStorage.GetFloat(  //
        window_ptr->GetID(ICB_PREFIX "ItemCurrentScale"));
}
