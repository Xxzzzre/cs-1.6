#pragma comment(lib, "legacy_stdio_definitions.lib")
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;
#include "imgui/soil.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_internal.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "bass/dll.h"
#include "bass/bass.h"
#include "Injector.h"
#include "image.h"
#include "color.h"
#include "radio.h"