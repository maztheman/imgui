// dear imgui: Platform Backend for GLFM
// This needs to be used along with a Renderer (e.g. OpenGL3 )
// (Info: GLFM is a Android / iOS general purpose library for handling windows, inputs, OpenGL context creation, etc.)
// (Requires: GLFM 3.1+)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Keyboard support. Since 1.87 we are using the io.AddKeyEvent() function. Pass ImGuiKey values to all key functions e.g. ImGui::IsKeyPressed(ImGuiKey_Space). [Legacy GLFW_KEY_* values will also be supported unless IMGUI_DISABLE_OBSOLETE_KEYIO is set]
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange' (note: the resizing cursors requires GLFW 3.4+).

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2025-03-11: Copied glfw as a base for glfm support
//  2022-11-22: Perform a dummy glfwGetError() read to cancel missing names with glfwGetKeyName(). (#5908)
//  2022-10-18: Perform a dummy glfwGetError() read to cancel missing mouse cursors errors. Using GLFW_VERSION_COMBINED directly. (#5785)
//  2022-10-11: Using 'nullptr' instead of 'NULL' as per our switch to C++11.
//  2022-09-26: Inputs: Renamed ImGuiKey_ModXXX introduced in 1.87 to ImGuiMod_XXX (old names still supported).
//  2022-09-01: Inputs: Honor GLFW_CURSOR_DISABLED by not setting mouse position.
//  2022-04-30: Inputs: Fixed ImGui_ImplGlfw_TranslateUntranslatedKey() for lower case letters on OSX.
//  2022-03-23: Inputs: Fixed a regression in 1.87 which resulted in keyboard modifiers events being reported incorrectly on Linux/X11.
//  2022-02-07: Added ImGui_ImplGlfw_InstallCallbacks()/ImGui_ImplGlfw_RestoreCallbacks() helpers to facilitate user installing callbacks after initializing backend.
//  2022-01-26: Inputs: replaced short-lived io.AddKeyModsEvent() (added two weeks ago) with io.AddKeyEvent() using ImGuiKey_ModXXX flags. Sorry for the confusion.
//  2021-01-20: Inputs: calling new io.AddKeyAnalogEvent() for gamepad support, instead of writing directly to io.NavInputs[].
//  2022-01-17: Inputs: calling new io.AddMousePosEvent(), io.AddMouseButtonEvent(), io.AddMouseWheelEvent() API (1.87+).
//  2022-01-17: Inputs: always update key mods next and before key event (not in NewFrame) to fix input queue with very low framerates.
//  2022-01-12: *BREAKING CHANGE*: Now using glfwSetCursorPosCallback(). If you called ImGui_ImplGlfw_InitXXX() with install_callbacks = false, you MUST install glfwSetCursorPosCallback() and forward it to the backend via ImGui_ImplGlfw_CursorPosCallback().
//  2022-01-10: Inputs: calling new io.AddKeyEvent(), io.AddKeyModsEvent() + io.SetKeyEventNativeData() API (1.87+). Support for full ImGuiKey range.
//  2022-01-05: Inputs: Converting GLFW untranslated keycodes back to translated keycodes (in the ImGui_ImplGlfw_KeyCallback() function) in order to match the behavior of every other backend, and facilitate the use of GLFW with lettered-shortcuts API.
//  2021-08-17: *BREAKING CHANGE*: Now using glfwSetWindowFocusCallback() to calling io.AddFocusEvent(). If you called ImGui_ImplGlfw_InitXXX() with install_callbacks = false, you MUST install glfwSetWindowFocusCallback() and forward it to the backend via ImGui_ImplGlfw_WindowFocusCallback().
//  2021-07-29: *BREAKING CHANGE*: Now using glfwSetCursorEnterCallback(). MousePos is correctly reported when the host platform window is hovered but not focused. If you called ImGui_ImplGlfw_InitXXX() with install_callbacks = false, you MUST install glfwSetWindowFocusCallback() callback and forward it to the backend via ImGui_ImplGlfw_CursorEnterCallback().
//  2021-06-29: Reorganized backend to pull data from a single structure to facilitate usage with multiple-contexts (all g_XXXX access changed to bd->XXXX).
//  2020-01-17: Inputs: Disable error callback while assigning mouse cursors because some X11 setup don't have them and it generates errors.
//  2019-12-05: Inputs: Added support for new mouse cursors added in GLFW 3.4+ (resizing cursors, not allowed cursor).
//  2019-10-18: Misc: Previously installed user callbacks are now restored on shutdown.
//  2019-07-21: Inputs: Added mapping for ImGuiKey_KeyPadEnter.
//  2019-05-11: Inputs: Don't filter value from character callback before calling AddInputCharacter().
//  2019-03-12: Misc: Preserve DisplayFramebufferScale when main window is minimized.
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-11-07: Inputs: When installing our GLFW callbacks, we save user's previously installed ones - if any - and chain call them.
//  2018-08-01: Inputs: Workaround for Emscripten which doesn't seem to handle focus related calls.
//  2018-06-29: Inputs: Added support for the ImGuiMouseCursor_Hand cursor.
//  2018-06-08: Misc: Extracted imgui_impl_glfw.cpp/.h away from the old combined GLFW+OpenGL/Vulkan examples.
//  2018-03-20: Misc: Setup io.BackendFlags ImGuiBackendFlags_HasMouseCursors flag + honor ImGuiConfigFlags_NoMouseCursorChange flag.
//  2018-02-20: Inputs: Added support for mouse cursors (ImGui::GetMouseCursor() value, passed to glfwSetCursor()).
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-02-06: Inputs: Added mapping for ImGuiKey_Space.
//  2018-01-25: Inputs: Added gamepad support if ImGuiConfigFlags_NavEnableGamepad is set.
//  2018-01-25: Inputs: Honoring the io.WantSetMousePos by repositioning the mouse (when using navigation and ImGuiConfigFlags_NavMoveMouse is set).
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-18: Inputs: Added mapping for ImGuiKey_Insert.
//  2017-08-25: Inputs: MousePos set to -FLT_MAX,-FLT_MAX when mouse is unavailable/missing (instead of -1,-1).
//  2016-10-15: Misc: Added a void* user_data parameter to Clipboard function handlers.

#include "imgui.h"
#include "imgui_impl_glfm.h"

// Clang warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"     // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wsign-conversion"    // warning: implicit conversion changes signedness
#endif

// GLFM6
#include <GLFM/glfm.h>
#include <android/keycodes.h>
#include <android/input.h>
// We gather version tests as define in order to easily see which features are version-dependent.
#define GLFM_VERSION_COMBINED           (GLFM_VERSION_MAJOR * 1000 + GLFM_VERSION_MINOR * 100 + GLFM_VERSION_REVISION)
#ifdef GLFM_RESIZE_NESW_CURSOR          // Let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2019-11-29 (cursors defines) // FIXME: Remove when GLFW 3.4 is released?
#define GLFM_HAS_NEW_CURSORS            (GLFM_VERSION_COMBINED >= 3400) // 3.4+ GLFW_RESIZE_ALL_CURSOR, GLFW_RESIZE_NESW_CURSOR, GLFW_RESIZE_NWSE_CURSOR, GLFW_NOT_ALLOWED_CURSOR
#else
#define GLFM_HAS_NEW_CURSORS            (0)
#endif
#define GLFM_HAS_GAMEPAD_API            (GLFM_VERSION_COMBINED >= 3300) // 3.3+ glfwGetGamepadState() new api
#define GLFM_HAS_GETKEYNAME             (GLFM_VERSION_COMBINED >= 3200) // 3.2+ glfwGetKeyName()

// GLFW data
enum GlfmClientApi
{
    GlfmClientApi_Unknown,
    GlfmClientApi_OpenGL,
    GlfmClientApi_Metal
};

struct ImGui_ImplGlfm_Data
{
    GLFMDisplay*            Display;
    GlfmClientApi           ClientApi;
    double                  Time;
    GLFMDisplay*             MouseDisplay;
    //GLFWcursor*             MouseCursors[ImGuiMouseCursor_COUNT];
    ImVec2                  LastValidMousePos;
    bool                    InstalledCallbacks;

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    //GLFWwindowfocusfun      PrevUserCallbackWindowFocus;
    //GLFWcursorposfun        PrevUserCallbackCursorPos;
    //GLFWcursorenterfun      PrevUserCallbackCursorEnter;
    GLFMTouchFunc             PrevUserCallbackTouch;
    //GLFWscrollfun           PrevUserCallbackScroll;
    GLFMKeyFunc               PrevUserCallbackKey;
    GLFMCharFunc              PrevUserCallbackChar;
    //GLFWmonitorfun          PrevUserCallbackMonitor;

    ImGui_ImplGlfm_Data()   { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
// FIXME: multi-context support is not well tested and probably dysfunctional in this backend.
// - Because glfwPollEvents() process all windows and some events may be called outside of it, you will need to register your own callbacks
//   (passing install_callbacks=false in ImGui_ImplGlfw_InitXXX functions), set the current dear imgui context and then call our callbacks.
// - Otherwise we may need to store a GLFWWindow* -> ImGuiContext* map and handle this in the backend, adding a little bit of extra complexity to it.
// FIXME: some shared resources (mouse cursor shape, gamepad) are mishandled when using multi-context.
static ImGui_ImplGlfm_Data* ImGui_ImplGlfm_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplGlfm_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

static int ImGui_ImplGlfm_KeyToModifier(int key)
{
    return 0;
}

static void ImGui_ImplGlfm_UpdateKeyModifiers(int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl, (mods & GLFMKeyModifierControl) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (mods & GLFMKeyModifierShift) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (mods & GLFMKeyModifierAlt) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (mods & GLFMKeyModifierMeta) != 0);
}

/*void ImGui_ImplGlfm_TouchCallback(GLFMDisplay* window, int button, int action, int mods)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    if (bd->PrevUserCallbackMousebutton != nullptr && window == bd->Window)
        bd->PrevUserCallbackMousebutton(window, button, action, mods);

    ImGui_ImplGlfm_UpdateKeyModifiers(mods);

    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < ImGuiMouseButton_COUNT)
        io.AddMouseButtonEvent(button, action == GLFW_PRESS);
}*/

#if 0

void ImGui_ImplGlfm_ScrollCallback(GLFMDisplay* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    if (bd->PrevUserCallbackScroll != nullptr && window == bd->Window)
        bd->PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
}

#endif

#if 1

static ImGuiKey ImGui_ImplGlfm_ScanCodeToImGuiKey(GLFMKeyCode scancode)
{
    switch (scancode)
    {
        case GLFMKeyCodeTab:                  return ImGuiKey_Tab;
        case GLFMKeyCodeArrowLeft:            return ImGuiKey_LeftArrow;
        case GLFMKeyCodeArrowRight:           return ImGuiKey_RightArrow;
        case GLFMKeyCodeArrowUp:              return ImGuiKey_UpArrow;
        case GLFMKeyCodeArrowDown:            return ImGuiKey_DownArrow;
        case GLFMKeyCodePageUp:               return ImGuiKey_PageUp;
        case GLFMKeyCodePageDown:             return ImGuiKey_PageDown;
        case GLFMKeyCodeHome:                 return ImGuiKey_Home;
        case GLFMKeyCodeEnd:                  return ImGuiKey_End;
        case GLFMKeyCodeInsert:               return ImGuiKey_Insert;
        case GLFMKeyCodeDelete:               return ImGuiKey_Delete;
        case GLFMKeyCodeBackspace:            return ImGuiKey_Backspace;
        case GLFMKeyCodeSpace:                return ImGuiKey_Space;
        case GLFMKeyCodeEnter:                return ImGuiKey_Enter;
        case GLFMKeyCodeEscape:               return ImGuiKey_Escape;
        case GLFMKeyCodeQuote:                return ImGuiKey_Apostrophe;
        case GLFMKeyCodeComma:                return ImGuiKey_Comma;
        case GLFMKeyCodeMinus:                return ImGuiKey_Minus;
        case GLFMKeyCodePeriod:               return ImGuiKey_Period;
        case GLFMKeyCodeSlash:                return ImGuiKey_Slash;
        case GLFMKeyCodeSemicolon:            return ImGuiKey_Semicolon;
        case GLFMKeyCodeEqual:                return ImGuiKey_Equal;
        case GLFMKeyCodeBracketLeft:          return ImGuiKey_LeftBracket;
        case GLFMKeyCodeBackslash:            return ImGuiKey_Backslash;
        case GLFMKeyCodeBracketRight:         return ImGuiKey_RightBracket;
        case GLFMKeyCodeBackquote:            return ImGuiKey_GraveAccent;
        case GLFMKeyCodeCapsLock:             return ImGuiKey_CapsLock;
        case GLFMKeyCodeScrollLock:           return ImGuiKey_ScrollLock;
        case GLFMKeyCodeNumLock:              return ImGuiKey_NumLock;
        case GLFMKeyCodePrintScreen:          return ImGuiKey_PrintScreen;
        case GLFMKeyCodePause:                return ImGuiKey_Pause;
        case GLFMKeyCodeNumpad0:              return ImGuiKey_Keypad0;
        case GLFMKeyCodeNumpad1:              return ImGuiKey_Keypad1;
        case GLFMKeyCodeNumpad2:              return ImGuiKey_Keypad2;
        case GLFMKeyCodeNumpad3:              return ImGuiKey_Keypad3;
        case GLFMKeyCodeNumpad4:              return ImGuiKey_Keypad4;
        case GLFMKeyCodeNumpad5:              return ImGuiKey_Keypad5;
        case GLFMKeyCodeNumpad6:              return ImGuiKey_Keypad6;
        case GLFMKeyCodeNumpad7:              return ImGuiKey_Keypad7;
        case GLFMKeyCodeNumpad8:              return ImGuiKey_Keypad8;
        case GLFMKeyCodeNumpad9:              return ImGuiKey_Keypad9;
        case GLFMKeyCodeNumpadDecimal:        return ImGuiKey_KeypadDecimal;
        case GLFMKeyCodeNumpadDivide:         return ImGuiKey_KeypadDivide;
        case GLFMKeyCodeNumpadMultiply:       return ImGuiKey_KeypadMultiply;
        case GLFMKeyCodeNumpadSubtract:       return ImGuiKey_KeypadSubtract;
        case GLFMKeyCodeNumpadAdd:            return ImGuiKey_KeypadAdd;
        case GLFMKeyCodeNumpadEnter:          return ImGuiKey_KeypadEnter;
        case GLFMKeyCodeNumpadEqual:          return ImGuiKey_KeypadEqual;
        case GLFMKeyCodeControlLeft:          return ImGuiKey_LeftCtrl;
        case GLFMKeyCodeShiftLeft:            return ImGuiKey_LeftShift;
        case GLFMKeyCodeAltLeft:              return ImGuiKey_LeftAlt;
        case GLFMKeyCodeMetaLeft:             return ImGuiKey_LeftSuper;
        case GLFMKeyCodeControlRight:         return ImGuiKey_RightCtrl;
        case GLFMKeyCodeShiftRight:           return ImGuiKey_RightShift;
        case GLFMKeyCodeAltRight:             return ImGuiKey_RightAlt;
        case GLFMKeyCodeMetaRight:            return ImGuiKey_RightSuper;
        case GLFMKeyCodeMenu:                 return ImGuiKey_Menu;
        case GLFMKeyCode0:                    return ImGuiKey_0;
        case GLFMKeyCode1:                    return ImGuiKey_1;
        case GLFMKeyCode2:                    return ImGuiKey_2;
        case GLFMKeyCode3:                    return ImGuiKey_3;
        case GLFMKeyCode4:                    return ImGuiKey_4;
        case GLFMKeyCode5:                    return ImGuiKey_5;
        case GLFMKeyCode6:                    return ImGuiKey_6;
        case GLFMKeyCode7:                    return ImGuiKey_7;
        case GLFMKeyCode8:                    return ImGuiKey_8;
        case GLFMKeyCode9:                    return ImGuiKey_9;
        case GLFMKeyCodeA:                    return ImGuiKey_A;
        case GLFMKeyCodeB:                    return ImGuiKey_B;
        case GLFMKeyCodeC:                    return ImGuiKey_C;
        case GLFMKeyCodeD:                    return ImGuiKey_D;
        case GLFMKeyCodeE:                    return ImGuiKey_E;
        case GLFMKeyCodeF:                    return ImGuiKey_F;
        case GLFMKeyCodeG:                    return ImGuiKey_G;
        case GLFMKeyCodeH:                    return ImGuiKey_H;
        case GLFMKeyCodeI:                    return ImGuiKey_I;
        case GLFMKeyCodeJ:                    return ImGuiKey_J;
        case GLFMKeyCodeK:                    return ImGuiKey_K;
        case GLFMKeyCodeL:                    return ImGuiKey_L;
        case GLFMKeyCodeM:                    return ImGuiKey_M;
        case GLFMKeyCodeN:                    return ImGuiKey_N;
        case GLFMKeyCodeO:                    return ImGuiKey_O;
        case GLFMKeyCodeP:                    return ImGuiKey_P;
        case GLFMKeyCodeQ:                    return ImGuiKey_Q;
        case GLFMKeyCodeR:                    return ImGuiKey_R;
        case GLFMKeyCodeS:                    return ImGuiKey_S;
        case GLFMKeyCodeT:                    return ImGuiKey_T;
        case GLFMKeyCodeU:                    return ImGuiKey_U;
        case GLFMKeyCodeV:                    return ImGuiKey_V;
        case GLFMKeyCodeW:                    return ImGuiKey_W;
        case GLFMKeyCodeX:                    return ImGuiKey_X;
        case GLFMKeyCodeY:                    return ImGuiKey_Y;
        case GLFMKeyCodeZ:                    return ImGuiKey_Z;
        case GLFMKeyCodeF1:                   return ImGuiKey_F1;
        case GLFMKeyCodeF2:                   return ImGuiKey_F2;
        case GLFMKeyCodeF3:                   return ImGuiKey_F3;
        case GLFMKeyCodeF4:                   return ImGuiKey_F4;
        case GLFMKeyCodeF5:                   return ImGuiKey_F5;
        case GLFMKeyCodeF6:                   return ImGuiKey_F6;
        case GLFMKeyCodeF7:                   return ImGuiKey_F7;
        case GLFMKeyCodeF8:                   return ImGuiKey_F8;
        case GLFMKeyCodeF9:                   return ImGuiKey_F9;
        case GLFMKeyCodeF10:                  return ImGuiKey_F10;
        case GLFMKeyCodeF11:                  return ImGuiKey_F11;
        case GLFMKeyCodeF12:                  return ImGuiKey_F12;
        default:                              return ImGuiKey_None;
    }
}

bool ImGui_ImplGlfm_KeyCallback(GLFMDisplay* display, GLFMKeyCode keycode, GLFMKeyAction action, int mods)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    if (bd->PrevUserCallbackKey != nullptr && display == bd->Display)
        bd->PrevUserCallbackKey(display, keycode, action, mods);

    if (action != GLFMKeyActionPressed && action != GLFMKeyActionReleased)
        return false;

 //   ImGui_ImplGlfm_UpdateKeyModifiers(mods);
 // At some point fix key func to pass glfm mods not android
 //Also invent more keycodes to handle all scancodes
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl,  (mods & AMETA_CTRL_ON)  != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (mods & AMETA_SHIFT_ON) != 0);
    io.AddKeyEvent(ImGuiMod_Alt,   (mods & AMETA_ALT_ON)   != 0);
    io.AddKeyEvent(ImGuiMod_Super, (mods & AMETA_META_ON)  != 0);


    ImGuiKey imgui_key = ImGui_ImplGlfm_ScanCodeToImGuiKey(keycode);
    io.AddKeyEvent(imgui_key, (action == GLFMKeyActionPressed));
    io.SetKeyEventNativeData(imgui_key, keycode, keycode); // To support legacy indexing (<1.87 user code)
    return true;
}

#endif

bool ImGui_ImplGlfm_TouchCallback(GLFMDisplay* display, int touch, GLFMTouchPhase phase, double x, double y)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    bool handled = false;
     if (bd->PrevUserCallbackTouch != nullptr && display == bd->Display)
        handled = bd->PrevUserCallbackTouch(display, touch, phase, x, y);

    if (!glfmHasTouch(display))
        return false;

    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)x, (float)y);
    bd->LastValidMousePos = ImVec2((float)x, (float)y);

    if (touch == 0) {
        io.AddMouseButtonEvent(0, phase == GLFMTouchPhaseBegan || phase == GLFMTouchPhaseMoved);
    }

    return handled;
}

#if 0
void ImGui_ImplGlfw_CursorPosCallback(GLFWwindow* window, double x, double y)
{
    ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
    if (bd->PrevUserCallbackCursorPos != nullptr && window == bd->Window)
        bd->PrevUserCallbackCursorPos(window, x, y);
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)x, (float)y);
    bd->LastValidMousePos = ImVec2((float)x, (float)y);
}
// Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
// so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
void ImGui_ImplGlfw_CursorEnterCallback(GLFWwindow* window, int entered)
{
    ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
    if (bd->PrevUserCallbackCursorEnter != nullptr && window == bd->Window)
        bd->PrevUserCallbackCursorEnter(window, entered);
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiIO& io = ImGui::GetIO();
    if (entered)
    {
        bd->MouseWindow = window;
        io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
    }
    else if (!entered && bd->MouseWindow == window)
    {
        bd->LastValidMousePos = io.MousePos;
        bd->MouseWindow = nullptr;
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }
}
#endif

void ImGui_ImplGlfm_CharCallback(GLFMDisplay* display, const char *utf8, int modifiers)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    if (bd->PrevUserCallbackChar != nullptr && display == bd->Display)
        bd->PrevUserCallbackChar(display, utf8, modifiers);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharactersUTF8(utf8);
}

void ImGui_ImplGlfm_InstallCallbacks(GLFMDisplay* display)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    IM_ASSERT(bd->InstalledCallbacks == false && "Callbacks already installed!");
    IM_ASSERT(bd->Display == display);

    //bd->PrevUserCallbackWindowFocus = glfmSetWindowFocusCallback(display, ImGui_ImplGlfm_WindowFocusCallback);
    //bd->PrevUserCallbackCursorEnter = glfmSetCursorEnterCallback(display, ImGui_ImplGlfm_CursorEnterCallback);
    bd->PrevUserCallbackTouch = glfmSetTouchFunc(display, ImGui_ImplGlfm_TouchCallback);
    //bd->PrevUserCallbackMousebutton = glfmSetMouseButtonCallback(display, ImGui_ImplGlfm_MouseButtonCallback);
    //bd->PrevUserCallbackScroll = glfmSetScrollCallback(display, ImGui_ImplGlfm_ScrollCallback);
    bd->PrevUserCallbackKey = glfmSetKeyFunc(display, ImGui_ImplGlfm_KeyCallback);
    bd->PrevUserCallbackChar = glfmSetCharFunc(display, ImGui_ImplGlfm_CharCallback);
    bd->InstalledCallbacks = true;
}

void ImGui_ImplGlfm_RestoreCallbacks(GLFMDisplay* display)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    IM_ASSERT(bd->InstalledCallbacks == true && "Callbacks not installed!");
    IM_ASSERT(bd->Display == display);

    //glfwSetWindowFocusCallback(display, bd->PrevUserCallbackWindowFocus);
    //glfwSetCursorEnterCallback(display, bd->PrevUserCallbackCursorEnter);
    glfmSetTouchFunc(display, bd->PrevUserCallbackTouch);
    //glfwSetMouseButtonCallback(display, bd->PrevUserCallbackMousebutton);
    //glfwSetScrollCallback(display, bd->PrevUserCallbackScroll);
    //glfwSetKeyCallback(display, bd->PrevUserCallbackKey);
    glfmSetCharFunc(display, bd->PrevUserCallbackChar);
    bd->InstalledCallbacks = false;
    //bd->PrevUserCallbackWindowFocus = nullptr;
    //bd->PrevUserCallbackCursorEnter = nullptr;
    bd->PrevUserCallbackTouch = nullptr;
    //bd->PrevUserCallbackMousebutton = nullptr;
    //bd->PrevUserCallbackScroll = nullptr;
    //bd->PrevUserCallbackKey = nullptr;
    bd->PrevUserCallbackChar = nullptr;
}

static bool ImGui_ImplGlfm_Init(GLFMDisplay* display, bool install_callbacks, GlfmClientApi client_api)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");
    //printf("GLFW_VERSION: %d.%d.%d (%d)", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION, GLFW_VERSION_COMBINED);

    // Setup backend capabilities flags
    ImGui_ImplGlfm_Data* bd = IM_NEW(ImGui_ImplGlfm_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_glfm";
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

    bd->Display = display;
    bd->Time = 0.0;

    //TODO: we need to implement clipboard stuff in glfm
    //io.SetClipboardTextFn = ImGui_ImplGlfm_SetClipboardText;
    //io.GetClipboardTextFn = ImGui_ImplGlfm_GetClipboardText;
    io.ClipboardUserData = bd->Display;

    // Chain GLFM callbacks: our callbacks will call the user's previously installed callbacks, if any.
    if (install_callbacks)
        ImGui_ImplGlfm_InstallCallbacks(display);

    bd->ClientApi = client_api;
    return true;
}

bool ImGui_ImplGlfm_InitForOpenGL(GLFMDisplay* window, bool install_callbacks)
{
    return ImGui_ImplGlfm_Init(window, install_callbacks, GlfmClientApi_OpenGL);
}

bool ImGui_ImplGlfm_InitForMetal(GLFMDisplay* window, bool install_callbacks)
{
    return ImGui_ImplGlfm_Init(window, install_callbacks, GlfmClientApi_Metal);
}

bool ImGui_ImplGlfm_InitForOther(GLFMDisplay* window, bool install_callbacks)
{
    return ImGui_ImplGlfm_Init(window, install_callbacks, GlfmClientApi_Unknown);
}

void ImGui_ImplGlfm_Shutdown()
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    if (bd->InstalledCallbacks)
        ImGui_ImplGlfm_RestoreCallbacks(bd->Display);

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    IM_DELETE(bd);
}

#if 0

static void ImGui_ImplGlfm_UpdateMouseData()
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();

#ifdef __EMSCRIPTEN__
    const bool is_app_focused = true;
#else
    const bool is_app_focused = true;
#endif
    if (is_app_focused)
    {
        // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        if (io.WantSetMousePos)
            glfmSetCursorPos(bd->Window, (double)io.MousePos.x, (double)io.MousePos.y);

        // (Optional) Fallback to provide mouse position when focused (ImGui_ImplGlfw_CursorPosCallback already provides this when hovered or captured)
        if (is_app_focused && bd->MouseWindow == nullptr)
        {
            double mouse_x, mouse_y;
            glfmGetCursorPos(bd->Window, &mouse_x, &mouse_y);
            io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
            bd->LastValidMousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
    }
}

#endif

#if 0

// TODO: verify that these functions are not really needed for Android / iOS support.

static void ImGui_ImplGlfm_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfmGetInputMode(bd->Window, GLFW_CURSOR) == GLFM_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        glfmSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        // Show OS mouse cursor
        // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
        glfmSetCursor(bd->Window, bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
        glfmSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

#endif

#if 0
// Update gamepad inputs
static inline float Saturate(float v) { return v < 0.0f ? 0.0f : v  > 1.0f ? 1.0f : v; }
static void ImGui_ImplGlfm_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
        return;

    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
#if GLFM_HAS_GAMEPAD_API
    GLFWgamepadstate gamepad;
    if (!glfmGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
        return;
    #define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED)          do { io.AddKeyEvent(KEY_NO, gamepad.buttons[BUTTON_NO] != 0); } while (0)
    #define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1)    do { float v = gamepad.axes[AXIS_NO]; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#else
    int axes_count = 0, buttons_count = 0;
    const float* axes = glfmGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    if (axes_count == 0 || buttons_count == 0)
        return;
    #define MAP_BUTTON(KEY_NO, _UNUSED, BUTTON_NO)          do { io.AddKeyEvent(KEY_NO, (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS)); } while (0)
    #define MAP_ANALOG(KEY_NO, _UNUSED, AXIS_NO, V0, V1)    do { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#endif
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    MAP_BUTTON(ImGuiKey_GamepadStart,       GLFM_GAMEPAD_BUTTON_START,          7);
    MAP_BUTTON(ImGuiKey_GamepadBack,        GLFM_GAMEPAD_BUTTON_BACK,           6);
    MAP_BUTTON(ImGuiKey_GamepadFaceLeft,    GLFM_GAMEPAD_BUTTON_X,              2);     // Xbox X, PS Square
    MAP_BUTTON(ImGuiKey_GamepadFaceRight,   GLFM_GAMEPAD_BUTTON_B,              1);     // Xbox B, PS Circle
    MAP_BUTTON(ImGuiKey_GamepadFaceUp,      GLFM_GAMEPAD_BUTTON_Y,              3);     // Xbox Y, PS Triangle
    MAP_BUTTON(ImGuiKey_GamepadFaceDown,    GLFM_GAMEPAD_BUTTON_A,              0);     // Xbox A, PS Cross
    MAP_BUTTON(ImGuiKey_GamepadDpadLeft,    GLFM_GAMEPAD_BUTTON_DPAD_LEFT,      13);
    MAP_BUTTON(ImGuiKey_GamepadDpadRight,   GLFM_GAMEPAD_BUTTON_DPAD_RIGHT,     11);
    MAP_BUTTON(ImGuiKey_GamepadDpadUp,      GLFM_GAMEPAD_BUTTON_DPAD_UP,        10);
    MAP_BUTTON(ImGuiKey_GamepadDpadDown,    GLFM_GAMEPAD_BUTTON_DPAD_DOWN,      12);
    MAP_BUTTON(ImGuiKey_GamepadL1,          GLFM_GAMEPAD_BUTTON_LEFT_BUMPER,    4);
    MAP_BUTTON(ImGuiKey_GamepadR1,          GLFM_GAMEPAD_BUTTON_RIGHT_BUMPER,   5);
    MAP_ANALOG(ImGuiKey_GamepadL2,          GLFM_GAMEPAD_AXIS_LEFT_TRIGGER,     4,      -0.75f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadR2,          GLFM_GAMEPAD_AXIS_RIGHT_TRIGGER,    5,      -0.75f,  +1.0f);
    MAP_BUTTON(ImGuiKey_GamepadL3,          GLFM_GAMEPAD_BUTTON_LEFT_THUMB,     8);
    MAP_BUTTON(ImGuiKey_GamepadR3,          GLFM_GAMEPAD_BUTTON_RIGHT_THUMB,    9);
    MAP_ANALOG(ImGuiKey_GamepadLStickLeft,  GLFM_GAMEPAD_AXIS_LEFT_X,           0,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickRight, GLFM_GAMEPAD_AXIS_LEFT_X,           0,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickUp,    GLFM_GAMEPAD_AXIS_LEFT_Y,           1,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickDown,  GLFM_GAMEPAD_AXIS_LEFT_Y,           1,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickLeft,  GLFM_GAMEPAD_AXIS_RIGHT_X,          2,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickRight, GLFM_GAMEPAD_AXIS_RIGHT_X,          2,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickUp,    GLFM_GAMEPAD_AXIS_RIGHT_Y,          3,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickDown,  GLFM_GAMEPAD_AXIS_RIGHT_Y,          3,      +0.25f,  +1.0f);
    #undef MAP_BUTTON
    #undef MAP_ANALOG
}

#endif

void ImGui_ImplGlfm_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplGlfw_InitForXXX()?");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfmGetDisplaySize(bd->Display, &w, &h);
    //I'm unaware that the frame buffer can be different size then the display.
    //glfmGetFramebufferSize(bd->Window, &display_w, &display_h);
    display_w = w;
    display_h = h;
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / (float)w, (float)display_h / (float)h);

    // Setup time step
    double current_time = glfmGetTime();
    io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time = current_time;

    //ImGui_ImplGlfm_UpdateMouseData();
    //ImGui_ImplGlfm_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    //ImGui_ImplGlfm_UpdateGamepads();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
