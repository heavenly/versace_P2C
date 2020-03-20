#include "etc_funcs.h"
#include "imgui/imgui.h"

namespace etc_functions
{

	bool color_edit_4(const char* label, Color* v, bool show_alpha)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}

	bool ColorPicker(const char *label, Color *col) {
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 32.0f - ImGui::GetStyle().FramePadding.x * 2.0f, ImGui::GetCursorPosY() + 1));

		auto clr = ImVec4{
	col->r() / 255.0f,
	col->g() / 255.0f,
	col->b() / 255.0f,
	col->a() / 255.0f
		};

		bool ret = ImGui::ColorEdit4(label, &clr.x, ImGuiColorEditFlags_NoInputs);

		if (ret) {
			col->SetColor(clr.x, clr.y, clr.z, clr.w);
		}

		return ret;
	}
};