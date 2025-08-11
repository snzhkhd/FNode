#ifndef IMGUI_IMPL_RAYLIB
#define IMGUI_IMPL_RAYLIB

/*	NOTE: I've added a few macros to deal with compatibility.
	Most are turned off by default to keep it as close as possible to the original code,
	but ImGui has its quirks when it comes to GL2. I'll be integrating part of WEREMSOFT's rendering code.

	(https://github.com/WEREMSOFT/c99-raylib-cimgui-template/).
*/

//	The compiler whines about IMGUI_IMPL_API not being defined, so I'm leaving this here.
#include <imgui.h>

//	Config macros
#include "imgui_impl_raylib_config.h"
static const char* CodepointToUtf8(int codepoint, int* length) {
    static char utf8[5] = { 0 };
    *length = 0;

    if (codepoint <= 0x7F) {
        utf8[0] = codepoint;
        *length = 1;
    }
    else if (codepoint <= 0x7FF) {
        utf8[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
        utf8[1] = 0x80 | (codepoint & 0x3F);
        *length = 2;
    }
    else if (codepoint <= 0xFFFF) {
        utf8[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
        utf8[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        utf8[2] = 0x80 | (codepoint & 0x3F);
        *length = 3;
    }
    else if (codepoint <= 0x10FFFF) {
        utf8[0] = 0xF0 | ((codepoint >> 18) & 0x07);
        utf8[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        utf8[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        utf8[3] = 0x80 | (codepoint & 0x3F);
        *length = 4;
    }

    utf8[*length] = 0;
    return utf8;
}
#if defined(__cplusplus)
extern "C" {
#endif

	IMGUI_IMPL_API bool     ImGui_ImplRaylib_Init();
	IMGUI_IMPL_API void     ImGui_ImplRaylib_Shutdown();
	IMGUI_IMPL_API void     ImGui_ImplRaylib_NewFrame();
	IMGUI_IMPL_API bool     ImGui_ImplRaylib_ProcessEvent();

#ifdef COMPATIBILITY_MODE
	IMGUI_IMPL_API void     ImGui_ImplRaylib_LoadDefaultFontAtlas();
	IMGUI_IMPL_API void     ImGui_ImplRaylib_Render(ImDrawData* draw_data);
#endif

#if defined(__cplusplus)
}
#endif

#endif
