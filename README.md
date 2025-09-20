[![Win](https://github.com/aiekick/ImCoolBar/actions/workflows/win.yml/badge.svg)](https://github.com/aiekick/ImCoolBar/actions/workflows/win.yml)
[![Linux](https://github.com/aiekick/ImCoolBar/actions/workflows/linux.yml/badge.svg)](https://github.com/aiekick/ImCoolBar/actions/workflows/linux.yml)
[![Osx](https://github.com/aiekick/ImCoolBar/actions/workflows/osx.yml/badge.svg)](https://github.com/aiekick/ImCoolBar/actions/workflows/osx.yml)

# ImCoolbar

# Minimal Sample

```cpp
	ImGuiContext& g = *GImGui;
	const float& ref_font_scale = g.FontSizeBase;

	static bool show_imgui_demo{false};
	static bool show_implot_demo{false};
	static bool show_imgui_metrics{false};
	static bool show_imcoolbar_metrics{false};
	static std::array<const char*, 4U> arr = {"ImGui\nDemo", "ImPlot\nDemo", "ImGui\nMetrics", "ImCoolBar\nMetrics"};
	static ImGui::ImCoolBarConfig _config;
	_config.normal_size = 35.0f;
	_config.hovered_size = 100.0f;
	_config.anchor = ImVec2(0.5f, 1.0f);
	
	if (ImGui::BeginCoolBar("CoolBarMainWin", ImCoolBarFlags_Horizontal, _config)) {
		for (size_t idx = 0U; idx < arr.size(); ++idx) {
			if (ImGui::CoolBarItem()) {
				const char* label = arr.at(idx);
				const float w = ImGui::GetCoolBarItemWidth();
				const float s = ImGui::GetCoolBarItemScale();
				ImGui::PushFont(nullptr, ref_font_scale * s);
				const auto ret = ImGui::Button(label, ImVec2(w * 2.0f, w));
				ImGui::PopFont();
				if (ret) {
					switch (idx) {
						case 0: show_imgui_demo = !show_imgui_demo; break;
						case 1: show_implot_demo = !show_implot_demo; break;
						case 2: show_imgui_metrics = !show_imgui_metrics; break;
						case 3: show_imcoolbar_metrics = !show_imcoolbar_metrics; break;
					}
				}
			}
		}
		ImGui::EndCoolBar();
	}

	if (show_imgui_demo) {
		ImGui::ShowDemoWindow(&show_imgui_demo);
	}
	if (show_implot_demo) {
		ImPlot::ShowDemoWindow(&show_implot_demo);
	}
	if (show_imgui_metrics) {
		ImGui::ShowMetricsWindow(&show_imgui_metrics);
	}
	if (show_imcoolbar_metrics) {
		ImGui::ShowCoolBarMetrics(&show_imcoolbar_metrics);
	}
```

Result : 

![alt text](https://github.com/aiekick/ImCoolBar/blob/DemoApp/doc/sample.gif)

# Demo App 

![alt text](https://github.com/aiekick/ImCoolBar/blob/DemoApp/doc/DemoApp.gif)
