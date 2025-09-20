// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

#include "3rdparty/imgui/backends/imgui_impl_opengl3.h"
#include "3rdparty/imgui/backends/imgui_impl_glfw.h"
#include <ImCoolBar/ImCoolBar.h>
#include "CustomFont.cpp"
#include <functional>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <clocale>
#include <string>
#include <vector>
#include <array>

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>  // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>  // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

int display_w, display_h;

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }

GLuint loadTexture(const std::string& vFilePathName) {
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

void drawBackground(const GLuint& vTexId) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
    ImVec2 pos;
    ImVec2 size((float)display_w, (float)display_h);
    ImGui::Begin("##background", nullptr, 
        ImGuiWindowFlags_NoBackground | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDocking | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowPos(ImVec2());
    ImGui::SetWindowSize(size);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!window->SkipItems) { window->DrawList->AddImage((ImTextureID)(size_t)vTexId, pos, pos + size); }
    ImGui::End();
    ImGui::PopStyleVar();
}

typedef std::vector<std::pair<std::string, GLuint>> TexturesContainer;

struct AppDatas {
    bool show_app_metrics = false;
    bool show_app_demo    = false;
    bool show_graph_demo = false;
    bool show_imcoolbar_metrics = false;
    TexturesContainer textures;
};

void drawCoolBar(AppDatas& vAppDatas,
                 const size_t& vMaxIcons,
                 const char* vLabel,
                 const ImCoolBarFlags& vFlags = ImCoolBarFlags_Vertical,
                 const ImGui::ImCoolBarConfig& vConfig = {}) {
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    bool opened = ImGui::BeginCoolBar(vLabel, vFlags, vConfig);
    ImGui::PopStyleVar();
    if (opened) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());
        size_t idx = 0U;
        for (const auto& arr : vAppDatas.textures) {
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
                            vAppDatas.show_app_demo = !vAppDatas.show_app_demo;
                        } else if (arr.first == "Activa") {
                            vAppDatas.show_graph_demo = !vAppDatas.show_graph_demo;
                        } else if (arr.first == "Magnet") {
                            vAppDatas.show_app_metrics = !vAppDatas.show_app_metrics;
                        } else if (arr.first == "Blender") {
                            vAppDatas.show_imcoolbar_metrics = !vAppDatas.show_imcoolbar_metrics;
                        }
                    }
                }
            }
        }
        ImGui::PopStyleVar();
        ImGui::EndCoolBar();
    }
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImToolbar", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGuiContext& g = *GImGui;
    g.IO.FontAllowUserScaling = true;  // zoom with ctrl + mouse wheel


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // load icon font file (CustomFont.cpp)
    g.IO.Fonts->AddFontDefault();
    static const ImWchar icons_ranges[] = {ICON_MIN_IGFD, ICON_MAX_IGFD, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    g.IO.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, 50.0f, &icons_config, icons_ranges);

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

    auto background_id = loadTexture("res/desert.jpg");

    AppDatas _appDatas;
    for (const auto& name : icons_name) {
        _appDatas.textures.push_back(std::make_pair(name, loadTexture("res/" + name + ".png")));
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetFramebufferSize(window, &display_w, &display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawBackground(background_id);

        static ImGui::ImCoolBarConfig _config;
        _config.normal_size = 30.0f;
        _config.hovered_size = 200.0f;
        _config.anchor = ImVec2(0.5f, 1.0f);
        _config.anim_step = 0.05f;
#ifdef ENABLE_IMCOOLBAR_DEBUG
        _config.anim_step = 0.005f;
#endif

        const float& ref_font_scale = g.FontSizeBase;

        _config.anchor = ImVec2(0.5f, 0.0f);
        drawCoolBar(_appDatas, 11, "Top##CoolBarMainWin", ImCoolBarFlags_Horizontal, _config);

        _config.anchor = ImVec2(0.0f, 0.5f);
        drawCoolBar(_appDatas, 6, "Left##CoolBarMainWin", ImCoolBarFlags_Vertical, _config);

        _config.anchor = ImVec2(1.0f, 0.5f);
        drawCoolBar(_appDatas, 6, "Right##CoolBarMainWin", ImCoolBarFlags_Vertical, _config);

        _config.normal_size = 25.0f;
        _config.anchor = ImVec2(0.5f, 1.0f);
        static std::array<const char*, 13U> arr = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M"};
        bool opened = ImGui::BeginCoolBar("Bottom##CoolBarMainWin", ImCoolBarFlags_Horizontal, _config);
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
                        if (idx == 3) { // 'D'
                            _appDatas.show_imcoolbar_metrics = !_appDatas.show_imcoolbar_metrics;
                        }
                    }
                }
            }
            ImGui::EndCoolBar();
        }

        if (_appDatas.show_app_metrics) {
            ImGui::ShowMetricsWindow(&_appDatas.show_app_metrics);
        }

        if (_appDatas.show_app_demo) {
            ImGui::ShowDemoWindow(&_appDatas.show_app_demo);
        }

        if (_appDatas.show_graph_demo) {
            ImPlot::ShowDemoWindow(&_appDatas.show_graph_demo);
        }

        if (_appDatas.show_imcoolbar_metrics) {
            ImGui::ShowCoolBarMetrics(&_appDatas.show_imcoolbar_metrics);
        }

        // Cpu Zone : prepare
        ImGui::Render();

        // GPU Zone : Rendering
        glfwMakeContextCurrent(window);

        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (g.IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
