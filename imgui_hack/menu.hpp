#pragma once
#include "singleton.hpp"
#include "imgui/imgui.h"

struct IDirect3DDevice9;

class Menu
    : public Singleton<Menu>
{
public:
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void Render();

    void Show();
    void Hide();
    void Toggle();

    bool IsVisible() const { return _visible; }

private:
    void create_style();

    ImGuiStyle        _style;
    bool              _visible;
};