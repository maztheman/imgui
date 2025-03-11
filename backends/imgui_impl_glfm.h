// dear imgui: Platform Backend for GLFM
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan, WebGPU..)
// (Info: GLFM is a mobile/web cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Keyboard support. Since 1.87 we are using the io.AddKeyEvent() function. Pass ImGuiKey values to all key functions e.g. ImGui::IsKeyPressed(ImGuiKey_Space). [Legacy GLFW_KEY_* values will also be supported unless IMGUI_DISABLE_OBSOLETE_KEYIO is set]

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

#include <glfm.h>

struct GLFMDisplay;

IMGUI_IMPL_API bool     ImGui_ImplGlfm_InitForOpenGL(GLFMDisplay* display, bool install_callbacks);
IMGUI_IMPL_API bool     ImGui_ImplGlfm_InitForMetal(GLFMDisplay* display, bool install_callbacks);
IMGUI_IMPL_API bool     ImGui_ImplGlfw_InitForOther(GLFMDisplay* display, bool install_callbacks);
IMGUI_IMPL_API void     ImGui_ImplGlfm_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplGlfm_NewFrame();

// GLFM callbacks (installer)
// - When calling Init with 'install_callbacks=true': ImGui_ImplGlfm_InstallCallbacks() is called. GLFM callbacks will be installed for you. They will chain-call user's previously installed callbacks, if any.
// - When calling Init with 'install_callbacks=false': GLFM callbacks won't be installed. You will need to call individual function yourself from your own GLFM callbacks.
IMGUI_IMPL_API void     ImGui_ImplGlfm_InstallCallbacks(GLFMDisplay* display);
IMGUI_IMPL_API void     ImGui_ImplGlfm_RestoreCallbacks(GLFMDisplay* display);

// GLFW callbacks (individual callbacks to call if you didn't install callbacks)
IMGUI_IMPL_API bool     ImGui_ImplGlfm_TouchCallback(GLFMDisplay* display, int touch, GLFMTouchPhase phase, double x, double y);
IMGUI_IMPL_API bool     ImGui_ImplGlfm_KeyCallback(GLFMDisplay* display, GLFMKeyCode keycode, GLFMKeyAction action, int mods);
IMGUI_IMPL_API void     ImGui_ImplGlfm_CharCallback(GLFMDisplay* display, const char *utf8, int modifiers);
