# ImCoolbar

# Minimal Sample

```cpp
	if (ImGui::BeginCoolBar("Bottom##CoolBarMainWin", ImCoolBar_Horizontal, ImVec2(0.5f, 1.0f))) {
		if (ImGui::CoolBarItem("ButtonA")) {
			float w = ImGui::GetCoolBarItemWidth();
			ImGui::Button("A", ImVec2(w, w));
		}
		if (ImGui::CoolBarItem("ButtonB")) {
			float w = ImGui::GetCoolBarItemWidth();
			ImGui::Button("B", ImVec2(w, w));
		}
		if (ImGui::CoolBarItem("ButtonC")) {
			float w = ImGui::GetCoolBarItemWidth();
			ImGui::Button("C", ImVec2(w, w));
		}
		if (ImGui::CoolBarItem("ButtonD")) {
			float w = ImGui::GetCoolBarItemWidth();
			ImGui::Button("D", ImVec2(w, w));
		}
		if (ImGui::CoolBarItem("ButtonE")) {
			float w = ImGui::GetCoolBarItemWidth();
			ImGui::Button("E", ImVec2(w, w));
		}
		if (ImGui::CoolBarItem("ButtonF")) {
			float w = ImGui::GetCoolBarItemWidth();
			ImGui::Button("F", ImVec2(w, w));
		}
		ImGui::EndCoolBar();
	}
```

Result : 

![alt text](https://github.com/aiekick/ImCoolBar/blob/DemoApp/doc/minimal_code.gif)

# Demo App 

![alt text](https://github.com/aiekick/ImCoolBar/blob/DemoApp/doc/DemoApp.gif)
