#include "render.h"

#include <mutex>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/math.hpp"
#include "imgui/directx9/imgui_impl_dx9.h"
#include "imgui/imgui_internal.h"

ImFont* g_pDefaultFont;

Render::Render()
{

}

ImDrawListSharedData _data;

std::mutex render_mutex;

void Render::Initialize()
{
	ImGui::CreateContext();

	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	_data = ImDrawListSharedData();

	draw_list = new ImDrawList(&_data);
	draw_list_act = new ImDrawList(&_data);
	draw_list_rendering = new ImDrawList(&_data);

	GetFonts();
}

void Render::GetFonts() 
{
	//ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(blocky, blocky_size, 11.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	//g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(blocky, blocky_size, 11.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
}

void Render::ClearDrawList() {
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() {
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();
	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* Render::RenderScene() {

	if (render_mutex.try_lock()) 
	{
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}