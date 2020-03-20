#pragma once
#include "valve_sdk/csgostructs.hpp"

//credits to null and k4m
class dot {
public:
	dot(const Vector2D p, const Vector2D v) 
	{
		m_vel = v;
		m_pos = p;
		m_orig_pos = p;
	}

	void update();
	void draw();

	Vector2D m_pos, m_vel, m_orig_pos;
};

extern void dot_draw();
extern void dot_destroy();