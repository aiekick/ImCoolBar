// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

#include "src/frame.h"

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }

Frame m_frame;

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImCoolBarApp", NULL, NULL);
    if (window == NULL) {
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return EXIT_FAILURE;
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

    if (!m_frame.init()) {
        return EXIT_FAILURE;
    }

    int32_t display_w, display_h;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetFramebufferSize(window, &display_w, &display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_frame.update(ImVec2(display_w, display_h));

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

    m_frame.unit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
