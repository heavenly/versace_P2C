#include "particles.h"
#include "options.hpp"
#include "menu.hpp"

//modify for more or less dots
constexpr int DOTS_SIZE = 512;

std::vector<dot> dots = { };

void dot::update() 
{
	m_pos += m_vel;
}

void dot::draw() 
{
	g_VGuiSurface->DrawSetColor(255, 255, 255, 55);
	g_VGuiSurface->DrawFilledRect(m_pos.x, m_pos.y, m_pos.x + 2, m_pos.y + 2);
}

void dot_draw() 
{
	struct screen_size {
		int x, y;
	}; static screen_size sc;

	if (!g_vars.misc_menu_snow)
		return;

	if (sc.x == 0 || sc.y == 0)
		g_EngineClient->GetScreenSize(sc.x, sc.y);


	const static auto random_float = [=](float min, float max) -> float
	{
		typedef float(*random_float_t)(float, float);
		static random_float_t rand_float = (random_float_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat");
		return rand_float(min, max);
	};

	//this is only used once, to save performance I don't remove the dots
	static int i = 0;
	
	if (dots.empty())
		i = 0;
	
	while (i < DOTS_SIZE)
	{
		dots.emplace_back(dot(Vector2D(rand() % (int)sc.x, -16), Vector2D(0, random_float(.3, .8))));
		i++;
	}

	//drawing shaded backdrop
	g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 135));
	g_VGuiSurface->DrawFilledRect(0, 0, sc.x, sc.y);

	for (auto& dot : dots) 
	{
		auto y = dot.m_pos.y;
		const static auto y_bound = sc.y + 20;

		if (y > y_bound) 
		{
			dot.m_pos.y = dot.m_orig_pos.y;
		}

		dot.update();

		//for whatever retarded reason this uses a lot of performance:
		dot.draw();
	}
}

void dot_destroy() 
{
	dots.clear();
}