# ImCoolbar

# Minimal Sample

```cpp
auto CoolbarButton = [](const char* label) -> bool
{
    float w = ImGui::GetCoolBarItemWidth();
    auto font_ptr = ImGui::GetIO().Fonts->Fonts[0];
    font_ptr->Scale = ImGui::GetCoolBarItemScale();
    ImGui::PushFont(font_ptr);
    bool res = ImGui::Button(label, ImVec2(w, w));
    ImGui::PopFont();
    return res;
};

if (ImGui::BeginCoolBar("##CoolBarMain", ImGui::ImCoolBarFlags::Horizontal, ImVec2(0.5f, 1.0f)))
{
    if (ImGui::CoolBarItem())
    {
        if (CoolbarButton("A"))
        {
            // Do Stuff...
        }
    }
    if (ImGui::CoolBarItem())
    {
        if (CoolbarButton("B"))
        {
            // Do Stuff...
        }
    }
    if (ImGui::CoolBarItem())
    {
        if (CoolbarButton("C"))
        {
            // Do Stuff...
        }
    }
    if (ImGui::CoolBarItem())
    {
        if (CoolbarButton("D"))
        {
            // Do Stuff...
        }
    }
    ImGui::EndCoolBar();
}
```

# Result
![Result of Minimal Sample](https://github.com/aiekick/ImCoolBar/blob/DemoApp/doc/minimal_code.gif)

# Demo App 
![GIF of Demo App](https://github.com/aiekick/ImCoolBar/blob/DemoApp/doc/DemoApp.gif)