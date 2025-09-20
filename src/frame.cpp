#include "frame.h"
#include "stb_image.h"
#include <imgui_internal.h>
#include "implot.h"

bool Frame::init() {
    const std::array<std::string, 11> icons_name = {
        "Settings",   //
        "Activa",     //
        "Magnet",     //
        "Blender",    //
        "CLion",      //
        "Firefox",    //
        "Gimp",       //
        "Godot",      //
        "VLC",        //
        "Wikipedia",  //
        "GeoGebra"    //
    };

    m_datas.bg_id = m_loadTexture("res/desert.jpg");
    for (const auto& name : icons_name) {
        m_datas.textures.push_back(std::make_pair(name, m_loadTexture("res/" + name + ".png")));
    }

    m_datas.coolBarConfig.normal_size = 40.0f;
    m_datas.coolBarConfig.hovered_size = 150.0f;
    m_datas.coolBarConfig.anchor = ImVec2(0.5f, 1.0f);
    m_datas.coolBarConfig.anim_step = 0.05f;
#ifdef ENABLE_IMCOOLBAR_DEBUG
    m_datas.coolBarConfig.anim_step = 0.005f;
#endif

    return true;
}

void Frame::unit() {
}


void Frame::update(const ImVec2& vDisplaySize) {
    m_drawBackground(m_datas.bg_id, vDisplaySize);

    ImGuiContext& g = *GImGui;
    const float& ref_font_scale = g.FontSizeBase;

    m_datas.coolBarConfig.anchor = ImVec2(0.5f, 0.0f);
    m_drawCoolBar( 11, "Top##CoolBarMainWin", ImCoolBarFlags_Horizontal);

    m_datas.coolBarConfig.anchor = ImVec2(0.0f, 0.5f);
    m_drawCoolBar(6, "Left##CoolBarMainWin", ImCoolBarFlags_Vertical);

    m_datas.coolBarConfig.anchor = ImVec2(1.0f, 0.5f);
    m_drawCoolBar( 6, "Right##CoolBarMainWin", ImCoolBarFlags_Vertical);

    m_datas.coolBarConfig.anchor = ImVec2(0.5f, 1.0f);
    static std::array<const char*, 13U> arr = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M"};
    bool opened = ImGui::BeginCoolBar("Bottom##CoolBarMainWin", ImCoolBarFlags_Horizontal, m_datas.coolBarConfig);
    if (opened) {
        for (size_t idx = 0U; idx < 13; ++idx) {
            if (ImGui::CoolBarItem()) {
                const char* label = arr.at(idx);
                float w = ImGui::GetCoolBarItemWidth();
                float s = ImGui::GetCoolBarItemScale();
                ImGui::PushFont(nullptr, ref_font_scale * s);
                const auto ret = ImGui::Button(label, ImVec2(w, w));
                ImGui::PopFont();
#ifdef _DEBUG
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("scale : %.2f\nwidth : %.2f", s, w);
                }
#endif
                if (ret) {
                    if (idx == 3) {  // 'D'
                        m_datas.show_imcoolbar_metrics = !m_datas.show_imcoolbar_metrics;
                    }
                }
            }
        }
        ImGui::EndCoolBar();
    }

    if (m_datas.show_app_metrics) {
        ImGui::ShowMetricsWindow(&m_datas.show_app_metrics);
    }

    if (m_datas.show_app_demo) {
        ImGui::ShowDemoWindow(&m_datas.show_app_demo);
    }

    if (m_datas.show_graph_demo) {
        ImPlot::ShowDemoWindow(&m_datas.show_graph_demo);
    }

    if (m_datas.show_imcoolbar_metrics) {
        ImGui::ShowCoolBarMetrics(&m_datas.show_imcoolbar_metrics);
    }
}

GLuint Frame::m_loadTexture(const std::string& vFilePathName) {
    GLuint tex_id = 0U;
    int32_t w, h, chans;
    auto image = stbi_load(vFilePathName.c_str(), &w, &h, &chans, 0);
    if (image) {
        stbi_image_free(image);
        if (chans == 3) {
            image = stbi_load(vFilePathName.c_str(), &w, &h, &chans, STBI_rgb);
        } else if (chans == 4) {
            image = stbi_load(vFilePathName.c_str(), &w, &h, &chans, STBI_rgb_alpha);
        }
    }
    if (image) {
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (chans == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)w, (GLsizei)h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (chans == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)w, (GLsizei)h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        glFinish();
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);
    }
    return tex_id;
}

void Frame::m_drawBackground(const GLuint& vTexId, const ImVec2& vDisplaySize) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
    ImVec2 pos;
    ImGui::Begin("##background",
                 nullptr,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowPos(ImVec2());
    ImGui::SetWindowSize(vDisplaySize);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!window->SkipItems) {
        window->DrawList->AddImage((ImTextureID)(size_t)vTexId, pos, pos + vDisplaySize);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void Frame::m_drawCoolBar(const size_t& vMaxIcons, const char* vLabel, const ImCoolBarFlags& vFlags) {
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    bool opened = ImGui::BeginCoolBar(vLabel, vFlags, m_datas.coolBarConfig);
    ImGui::PopStyleVar();
    if (opened) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());
        size_t idx = 0U;
        for (const auto& arr : m_datas.textures) {
            if (idx++ < vMaxIcons) {
                if (ImGui::CoolBarItem()) {
#ifndef ENABLE_IMCOOLBAR_DEBUG
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4());
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
#endif
                    float w = ImGui::GetCoolBarItemWidth();
                    ImTextureRef tex;
                    tex._TexID = arr.second;
                    bool res = ImGui::ImageButton(arr.first.c_str(), tex, ImVec2(w, w));
#ifndef ENABLE_IMCOOLBAR_DEBUG
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor(2);
#endif

                    if (res) {
                        if (arr.first == "Settings") {
                            m_datas.show_app_demo = !m_datas.show_app_demo;
                        } else if (arr.first == "Activa") {
                            m_datas.show_graph_demo = !m_datas.show_graph_demo;
                        } else if (arr.first == "Magnet") {
                            m_datas.show_app_metrics = !m_datas.show_app_metrics;
                        } else if (arr.first == "Blender") {
                            m_datas.show_imcoolbar_metrics = !m_datas.show_imcoolbar_metrics;
                        }
                    }
                }
            }
        }
        ImGui::PopStyleVar();
        ImGui::EndCoolBar();
    }
}
