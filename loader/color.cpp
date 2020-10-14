#include "main.h"

float color_red = 1.f;
float color_green = 0.f;
float color_blue = 1.f;

ImColor Red() { return ImColor(1.f, 0.f, 0.f, 1.f); }
ImColor Green() { return ImColor(0.f, 1.f, 0.f, 1.f); }
ImColor Blue() { return ImColor(0.f, 0.f, 1.f, 1.f); }
ImColor White() { return ImColor(1.f, 1.f, 1.f, 1.f); }
ImColor Black() { return ImColor(0.f, 0.f, 0.f, 1.f); }
ImColor Wheel1() { return ImColor(color_red, color_green, color_blue, 1.f); }
ImColor Wheel2() { return ImColor(color_green, color_blue, color_red, 1.f); }
ImColor Wheel3() { return ImColor(color_blue, color_red, color_green, 1.f); }

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount > 0)
	{
		Color[0] += 0.01f;
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}