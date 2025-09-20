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

#ifdef _MSC_VER
#include <Windows.h>
#define ICB_DEBUG_BREAK      \
    if (IsDebuggerPresent()) \
    __debugbreak()
#else
#define ICB_DEBUG_BREAK
#endif

#define BREAK_ON_KEY(KEY)         \
    if (ImGui::IsKeyPressed(KEY)) \
    ICB_DEBUG_BREAK

static float bubbleEffect(const float vValue, const float vStrength) {
    return pow(cos(vValue * IM_PI * vStrength), 8.0f);
}

static float getBarSize(const float vNormalSize, const float vHoveredSize, const float vScale) {
    return ImClamp(vNormalSize + (vHoveredSize - vNormalSize) * vScale, vNormalSize, vHoveredSize);
}

// https://codesandbox.io/s/motion-dock-forked-hs4p8d?file=/src/Dock.tsx
static float getHoverSize(const float vValue, const float vNormalSize, const float vHoveredSize, const float vStength, const float vScale) {
    return getBarSize(vNormalSize, vHoveredSize, bubbleEffect(vValue, vStength) * vScale);
}

static bool isWindowHovered(ImGuiWindow* vWindow) {
    return ImGui::IsMouseHoveringRect(vWindow->Rect().Min, vWindow->Rect().Max);
}

static float getChannel(const ImVec2& vVec, const ImCoolBarFlags vCBFlags) {
    if (vCBFlags & ImCoolBarFlags_Horizontal) {
        return vVec.x;
    }
    return vVec.y;
}

static float getChannelInv(const ImVec2& vVec, const ImCoolBarFlags vCBFlags) {
    if (vCBFlags & ImCoolBarFlags_Horizontal) {
        return vVec.y;
    }
    return vVec.x;
}

IMGUI_API bool ImGui::BeginCoolBar(const char* vLabel, ImCoolBarFlags vCBFlags, const ImCoolBarConfig& vConfig, ImGuiWindowFlags vFlags) {
    ImGuiWindowFlags flags =                   //
        vFlags                                 //
        | ImGuiWindowFlags_NoTitleBar          //
        | ImGuiWindowFlags_NoScrollbar         //
        | ImGuiWindowFlags_AlwaysAutoResize    //
        | ImGuiWindowFlags_NoCollapse          //
        | ImGuiWindowFlags_NoMove              //
        | ImGuiWindowFlags_NoSavedSettings     //
        | ImGuiWindowFlags_NoFocusOnAppearing  //
#ifndef ENABLE_IMCOOLBAR_DEBUG
        | ImGuiWindowFlags_NoBackground  //
#endif                                   //
#ifdef IMGUI_HAS_DOCK
        | ImGuiWindowFlags_DockNodeHost  //
        | ImGuiWindowFlags_NoDocking     //
#endif
        ;
    bool res = ImGui::Begin(vLabel, nullptr, flags);
    if (!res) {
        ImGui::End();
    } else {
        // Can be Horizontal or Vertical, not both
        const bool isVertical = (vCBFlags & ImCoolBarFlags_Vertical);
        const bool isHorizontal = (vCBFlags & ImCoolBarFlags_Horizontal);
        IM_ASSERT((isHorizontal && !isVertical) || (!isHorizontal && isVertical));

        ImGuiWindow* pWindow = GetCurrentWindow();
        pWindow->StateStorage.SetVoidPtr(pWindow->GetID(ICB_PREFIX "Type"), (void*)"ImCoolBar");
        pWindow->StateStorage.SetInt(pWindow->GetID(ICB_PREFIX "ItemIdx"), 0);
        pWindow->StateStorage.SetInt(pWindow->GetID(ICB_PREFIX "Flags"), vCBFlags);
        const float anchor = ImClamp(getChannelInv(vConfig.anchor, vCBFlags), 0.0f, 1.0f);
        pWindow->StateStorage.SetFloat(pWindow->GetID(ICB_PREFIX "Anchor"), anchor);
        const auto normal_size_id = pWindow->GetID(ICB_PREFIX "NormalSize");
        const auto hovered_size_id = pWindow->GetID(ICB_PREFIX "HoveredSize");
        pWindow->StateStorage.SetFloat(normal_size_id, vConfig.normal_size);
        pWindow->StateStorage.SetFloat(hovered_size_id, vConfig.hovered_size);
        pWindow->StateStorage.SetFloat(pWindow->GetID(ICB_PREFIX "EffectStrength"), vConfig.effect_strength);

        const auto anim_scale_id = pWindow->GetID(ICB_PREFIX "AnimScale");
        float anim_scale = pWindow->StateStorage.GetFloat(anim_scale_id);
        if (isWindowHovered(pWindow)) {
            if (anim_scale < 1.0f) {
                anim_scale += vConfig.anim_step;
            }
        } else {
            if (anim_scale > 0.0f) {
                anim_scale -= vConfig.anim_step;
            }
        }

        anim_scale = ImClamp(anim_scale, 0.0f, 1.0f);
        pWindow->StateStorage.SetFloat(anim_scale_id, anim_scale);

        // --- Position with predicted cross-axis size for THIS frame ---
        ImVec2 pad = ImGui::GetStyle().WindowPadding * 2.0f;
        ImVec2 bar_size = pWindow->ContentSize + pad;  // along main axis ok
        const float normal_size = pWindow->StateStorage.GetFloat(normal_size_id);
        const float hovered_size = pWindow->StateStorage.GetFloat(hovered_size_id);
        const float cross = getBarSize(normal_size, hovered_size, anim_scale);
        if (isHorizontal) {
            bar_size.y = cross + pad.y;
        } else {
            bar_size.x = cross + pad.x;
        }

        const ImGuiViewport* pViewport = pWindow->Viewport;
        ImVec2 new_pos = ImFloor(pViewport->Pos + (pViewport->Size - bar_size) * vConfig.anchor);
        ImGui::SetWindowPos(new_pos);
    }

    return res;
}

IMGUI_API void ImGui::EndCoolBar() {
    ImGui::End();
}

IMGUI_API bool ImGui::CoolBarItem() {
    ImGuiWindow* pWindow = GetCurrentWindow();
    if (pWindow->SkipItems) {
        return false;
    }

    const auto item_index_id = pWindow->GetID(ICB_PREFIX "ItemIdx");
    const auto idx = pWindow->StateStorage.GetInt(item_index_id);
    const auto coolbar_item_id = pWindow->GetID(pWindow->ID + idx + 1);
    float current_item_size = pWindow->StateStorage.GetFloat(coolbar_item_id);
    const auto flags = pWindow->StateStorage.GetInt(pWindow->GetID(ICB_PREFIX "Flags"));
    const auto anim_scale = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "AnimScale"));
    const auto normal_size = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "NormalSize"));
    const auto hovered_size = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "HoveredSize"));
    const auto effect_strength = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "EffectStrength"));
    const auto last_mouse_pos_id = pWindow->GetID(ICB_PREFIX "LastMousePos");
    auto last_mouse_pos = pWindow->StateStorage.GetFloat(last_mouse_pos_id);

    assert(normal_size > 0.0f);

    if (flags & ImCoolBarFlags_Horizontal) {
        if (idx) {
            ImGui::SameLine();
        }
    }
    ImGuiContext& g = *GImGui;

    if (isWindowHovered(pWindow)) {
        last_mouse_pos = getChannel(g.IO.MousePos, flags);
    }

    if (current_item_size <= 0.0f) {
        current_item_size = normal_size;
    }

    float current_size = normal_size;
    if (anim_scale > 0.0f) {
        const auto csp = getChannel(pWindow->DC.CursorPos, flags);
        const auto ws = getChannel(pWindow->Size, flags);
        const auto wp = getChannel(g.Style.WindowPadding, flags);
        const float btn_center = csp + current_item_size * 0.5f;
        const float diff_pos = (last_mouse_pos - btn_center) / ws;
        current_size = getHoverSize(diff_pos, normal_size, hovered_size, effect_strength, anim_scale);
        const float anchor = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "Anchor"));
        const float bar_height = getBarSize(normal_size, hovered_size, anim_scale);
        const float btn_offset = ImFloor((bar_height - current_size) * anchor + wp);
        if (flags & ImCoolBarFlags_Horizontal) {
            ImGui::SetCursorPosY(btn_offset);
        } else if (flags & ImCoolBarFlags_Vertical) {
            ImGui::SetCursorPosX(btn_offset);
        }
    }

    pWindow->StateStorage.SetInt(item_index_id, idx + 1);
    pWindow->StateStorage.SetFloat(coolbar_item_id, current_size);
    pWindow->StateStorage.SetFloat(last_mouse_pos_id, last_mouse_pos);
    pWindow->StateStorage.SetFloat(pWindow->GetID(ICB_PREFIX "ItemCurrentSize"), current_size);
    pWindow->StateStorage.SetFloat(pWindow->GetID(ICB_PREFIX "ItemCurrentScale"), current_size / normal_size);

    return true;
}

IMGUI_API float ImGui::GetCoolBarItemWidth() {
    ImGuiWindow* pWindow = GetCurrentWindow();
    if (pWindow->SkipItems) {
        return 0.0f;
    }
    return pWindow->StateStorage.GetFloat(  //
        pWindow->GetID(ICB_PREFIX "ItemCurrentSize"));
}

IMGUI_API float ImGui::GetCoolBarItemScale() {
    ImGuiWindow* pWindow = GetCurrentWindow();
    if (pWindow->SkipItems) {
        return 0.0f;
    }
    return pWindow->StateStorage.GetFloat(  //
        pWindow->GetID(ICB_PREFIX "ItemCurrentScale"));
}

IMGUI_API void ImGui::ShowCoolBarMetrics(bool* vOpened) {
    if (ImGui::Begin("ImCoolBar Metrics", vOpened)) {
        ImGuiContext& g = *GImGui;
        for (auto* pWindow : g.Windows) {
            const char* type = (const char*)pWindow->StateStorage.GetVoidPtr(pWindow->GetID(ICB_PREFIX "Type"));
            if (type != nullptr && strcmp(type, "ImCoolBar") == 0) {
                if (!TreeNode(pWindow, "ImCoolBar %s", pWindow->Name)) {
                    continue;
                }

                const auto flags = pWindow->StateStorage.GetInt(pWindow->GetID(ICB_PREFIX "Flags"));
                const auto anchor = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "Anchor"));
                const auto max_idx = pWindow->StateStorage.GetInt(pWindow->GetID(ICB_PREFIX "ItemIdx"));
                const auto anim_scale = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "AnimScale"));
                const auto normal_size = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "NormalSize"));
                const auto hovered_size = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "HoveredSize"));
                const auto effect_strength = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "EffectStrength"));
                const auto item_last_size = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "ItemCurrentSize"));
                const auto item_last_scale = pWindow->StateStorage.GetFloat(pWindow->GetID(ICB_PREFIX "ItemCurrentScale"));

#define SetColumnLabel(a, fmt, v) \
    ImGui::TableNextColumn();     \
    ImGui::Text("%s", a);         \
    ImGui::TableNextColumn();     \
    ImGui::Text(fmt, v);          \
    ImGui::TableNextRow()

                if (ImGui::BeginTable("CoolbarDebugDatas", 2)) {
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    SetColumnLabel("MaxIdx ", "%i", max_idx);
                    SetColumnLabel("Anchor ", "%f", anchor);
                    SetColumnLabel("AnimScale ", "%f", anim_scale);
                    SetColumnLabel("NormalSize ", "%f", normal_size);
                    SetColumnLabel("HoveredSize ", "%f", hovered_size);
                    SetColumnLabel("EffectStrength ", "%f", effect_strength);
                    SetColumnLabel("ItemLastSize ", "%f", item_last_size);
                    SetColumnLabel("ItemLastScale ", "%f", item_last_scale);

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", "Flags ");
                    ImGui::TableNextColumn();
                    if (flags & ImCoolBarFlags_None) {
                        ImGui::Text("None");
                    }
                    if (flags & ImCoolBarFlags_Vertical) {
                        ImGui::Text("Vertical");
                    }
                    if (flags & ImCoolBarFlags_Horizontal) {
                        ImGui::Text("Horizontal");
                    }
                    ImGui::TableNextRow();

                    for (int idx = 0; idx < max_idx; ++idx) {
                        const auto coolbar_item_id = pWindow->GetID(pWindow->ID + idx + 1);
                        const auto current_item_size = pWindow->StateStorage.GetFloat(coolbar_item_id);
                        ImGui::TableNextColumn();
                        ImGui::Text("Item %i Size ", idx);
                        ImGui::TableNextColumn();
                        ImGui::Text("%f", current_item_size);
                        ImGui::TableNextRow();
                    }

                    ImGui::EndTable();
                }

#undef SetColumnLabel
                TreePop();
            }
        }
    }
    ImGui::End();
}
