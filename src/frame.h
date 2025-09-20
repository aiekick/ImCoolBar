#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <glad/glad.h>

#include <imgui.h>
#include <ImCoolBar.h>

#include <string>
#include <vector>
#include <array>

class Frame {
private:
    struct AppDatas {
        bool show_app_metrics{false};
        bool show_app_demo{false};
        bool show_graph_demo{false};
        bool show_imcoolbar_metrics{false};
        std::vector<std::pair<std::string, GLuint>> textures;
        GLuint bg_id{};
        ImGui::ImCoolBarConfig coolBarConfig;
    } m_datas;

public:
    bool init();
    void unit();

    void update(const ImVec2& vDisplaySize);

private:
    GLuint m_loadTexture(const std::string& vFilePathName);
    void m_drawBackground(const GLuint& vTexId, const ImVec2& vDisplaySize);
    void m_drawCoolBar(const size_t& vMaxIcons, const char* vLabel, const ImCoolBarFlags& vFlags = ImCoolBarFlags_Vertical);
};