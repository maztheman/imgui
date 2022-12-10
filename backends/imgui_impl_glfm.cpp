// dear imgui: Platform Backend for GLFW
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan, WebGPU..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (Requires: GLFW 3.1+)

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

static ImGuiKey ImGui_ImplGlfm_KeyToImGuiKey(int key)
{
    switch (key)
    {
        case GLFMKeyBackspace: return ImGuiKey_Backspace;
    case GLFMKeyTab: return ImGuiKey_Tab;
    case GLFMKeyEnter: return ImGuiKey_Enter;
    case GLFMKeyEscape: return ImGuiKey_Escape;
    case GLFMKeySpace: return ImGuiKey_Space;
    case GLFMKeyPageUp: return ImGuiKey_PageUp;
    case GLFMKeyPageDown: return ImGuiKey_PageDown;
    case GLFMKeyEnd: return ImGuiKey_End;
    case GLFMKeyHome: return ImGuiKey_Home;
    case GLFMKeyLeft: return ImGuiKey_LeftArrow;
    case GLFMKeyUp: return ImGuiKey_UpArrow;
    case GLFMKeyRight: return ImGuiKey_RightArrow;
    case GLFMKeyDown: return ImGuiKey_DownArrow;
    case GLFMKeyDelete: return ImGuiKey_Delete;
    //case GLFMKeyNavBack: return ImGuiKey_Tab;
    case GLFMKeyNavMenu: return ImGuiKey_Menu;
    //case GLFMKeyNavSelect: return ImGuiKey_Tab;
    //case GLFMKeyPlayPause: return ImGuiKey_Tab;
       default: return ImGuiKey_None;
    }
}

static int ImGui_ImplGlfm_KeyToModifier(int key)
{
    return 0;
}

static void ImGui_ImplGlfm_UpdateKeyModifiers(int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl, (mods & GLFMKeyModifierCtrl) != 0);
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

void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
    if (bd->PrevUserCallbackScroll != nullptr && window == bd->Window)
        bd->PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
}

#endif

#if 1


static ImGuiKey ImGui_ImplGlfm_ScanCodeToImGuiKey(int32_t scancode)
{
    switch (scancode)
    {
        case AKEYCODE_TAB:                  return ImGuiKey_Tab;
        case AKEYCODE_DPAD_LEFT:            return ImGuiKey_LeftArrow;
        case AKEYCODE_DPAD_RIGHT:           return ImGuiKey_RightArrow;
        case AKEYCODE_DPAD_UP:              return ImGuiKey_UpArrow;
        case AKEYCODE_DPAD_DOWN:            return ImGuiKey_DownArrow;
        case AKEYCODE_PAGE_UP:              return ImGuiKey_PageUp;
        case AKEYCODE_PAGE_DOWN:            return ImGuiKey_PageDown;
        case AKEYCODE_MOVE_HOME:            return ImGuiKey_Home;
        case AKEYCODE_MOVE_END:             return ImGuiKey_End;
        case AKEYCODE_INSERT:               return ImGuiKey_Insert;
        case AKEYCODE_FORWARD_DEL:          return ImGuiKey_Delete;
        case AKEYCODE_DEL:                  return ImGuiKey_Backspace;
        case AKEYCODE_SPACE:                return ImGuiKey_Space;
        case AKEYCODE_ENTER:                return ImGuiKey_Enter;
        case AKEYCODE_ESCAPE:               return ImGuiKey_Escape;
        case AKEYCODE_APOSTROPHE:           return ImGuiKey_Apostrophe;
        case AKEYCODE_COMMA:                return ImGuiKey_Comma;
        case AKEYCODE_MINUS:                return ImGuiKey_Minus;
        case AKEYCODE_PERIOD:               return ImGuiKey_Period;
        case AKEYCODE_SLASH:                return ImGuiKey_Slash;
        case AKEYCODE_SEMICOLON:            return ImGuiKey_Semicolon;
        case AKEYCODE_EQUALS:               return ImGuiKey_Equal;
        case AKEYCODE_LEFT_BRACKET:         return ImGuiKey_LeftBracket;
        case AKEYCODE_BACKSLASH:            return ImGuiKey_Backslash;
        case AKEYCODE_RIGHT_BRACKET:        return ImGuiKey_RightBracket;
        case AKEYCODE_GRAVE:                return ImGuiKey_GraveAccent;
        case AKEYCODE_CAPS_LOCK:            return ImGuiKey_CapsLock;
        case AKEYCODE_SCROLL_LOCK:          return ImGuiKey_ScrollLock;
        case AKEYCODE_NUM_LOCK:             return ImGuiKey_NumLock;
        case AKEYCODE_SYSRQ:                return ImGuiKey_PrintScreen;
        case AKEYCODE_BREAK:                return ImGuiKey_Pause;
        case AKEYCODE_NUMPAD_0:             return ImGuiKey_Keypad0;
        case AKEYCODE_NUMPAD_1:             return ImGuiKey_Keypad1;
        case AKEYCODE_NUMPAD_2:             return ImGuiKey_Keypad2;
        case AKEYCODE_NUMPAD_3:             return ImGuiKey_Keypad3;
        case AKEYCODE_NUMPAD_4:             return ImGuiKey_Keypad4;
        case AKEYCODE_NUMPAD_5:             return ImGuiKey_Keypad5;
        case AKEYCODE_NUMPAD_6:             return ImGuiKey_Keypad6;
        case AKEYCODE_NUMPAD_7:             return ImGuiKey_Keypad7;
        case AKEYCODE_NUMPAD_8:             return ImGuiKey_Keypad8;
        case AKEYCODE_NUMPAD_9:             return ImGuiKey_Keypad9;
        case AKEYCODE_NUMPAD_DOT:           return ImGuiKey_KeypadDecimal;
        case AKEYCODE_NUMPAD_DIVIDE:        return ImGuiKey_KeypadDivide;
        case AKEYCODE_NUMPAD_MULTIPLY:      return ImGuiKey_KeypadMultiply;
        case AKEYCODE_NUMPAD_SUBTRACT:      return ImGuiKey_KeypadSubtract;
        case AKEYCODE_NUMPAD_ADD:           return ImGuiKey_KeypadAdd;
        case AKEYCODE_NUMPAD_ENTER:         return ImGuiKey_KeypadEnter;
        case AKEYCODE_NUMPAD_EQUALS:        return ImGuiKey_KeypadEqual;
        case AKEYCODE_CTRL_LEFT:            return ImGuiKey_LeftCtrl;
        case AKEYCODE_SHIFT_LEFT:           return ImGuiKey_LeftShift;
        case AKEYCODE_ALT_LEFT:             return ImGuiKey_LeftAlt;
        case AKEYCODE_META_LEFT:            return ImGuiKey_LeftSuper;
        case AKEYCODE_CTRL_RIGHT:           return ImGuiKey_RightCtrl;
        case AKEYCODE_SHIFT_RIGHT:          return ImGuiKey_RightShift;
        case AKEYCODE_ALT_RIGHT:            return ImGuiKey_RightAlt;
        case AKEYCODE_META_RIGHT:           return ImGuiKey_RightSuper;
        case AKEYCODE_MENU:                 return ImGuiKey_Menu;
        case AKEYCODE_0:                    return ImGuiKey_0;
        case AKEYCODE_1:                    return ImGuiKey_1;
        case AKEYCODE_2:                    return ImGuiKey_2;
        case AKEYCODE_3:                    return ImGuiKey_3;
        case AKEYCODE_4:                    return ImGuiKey_4;
        case AKEYCODE_5:                    return ImGuiKey_5;
        case AKEYCODE_6:                    return ImGuiKey_6;
        case AKEYCODE_7:                    return ImGuiKey_7;
        case AKEYCODE_8:                    return ImGuiKey_8;
        case AKEYCODE_9:                    return ImGuiKey_9;
        case AKEYCODE_A:                    return ImGuiKey_A;
        case AKEYCODE_B:                    return ImGuiKey_B;
        case AKEYCODE_C:                    return ImGuiKey_C;
        case AKEYCODE_D:                    return ImGuiKey_D;
        case AKEYCODE_E:                    return ImGuiKey_E;
        case AKEYCODE_F:                    return ImGuiKey_F;
        case AKEYCODE_G:                    return ImGuiKey_G;
        case AKEYCODE_H:                    return ImGuiKey_H;
        case AKEYCODE_I:                    return ImGuiKey_I;
        case AKEYCODE_J:                    return ImGuiKey_J;
        case AKEYCODE_K:                    return ImGuiKey_K;
        case AKEYCODE_L:                    return ImGuiKey_L;
        case AKEYCODE_M:                    return ImGuiKey_M;
        case AKEYCODE_N:                    return ImGuiKey_N;
        case AKEYCODE_O:                    return ImGuiKey_O;
        case AKEYCODE_P:                    return ImGuiKey_P;
        case AKEYCODE_Q:                    return ImGuiKey_Q;
        case AKEYCODE_R:                    return ImGuiKey_R;
        case AKEYCODE_S:                    return ImGuiKey_S;
        case AKEYCODE_T:                    return ImGuiKey_T;
        case AKEYCODE_U:                    return ImGuiKey_U;
        case AKEYCODE_V:                    return ImGuiKey_V;
        case AKEYCODE_W:                    return ImGuiKey_W;
        case AKEYCODE_X:                    return ImGuiKey_X;
        case AKEYCODE_Y:                    return ImGuiKey_Y;
        case AKEYCODE_Z:                    return ImGuiKey_Z;
        case AKEYCODE_F1:                   return ImGuiKey_F1;
        case AKEYCODE_F2:                   return ImGuiKey_F2;
        case AKEYCODE_F3:                   return ImGuiKey_F3;
        case AKEYCODE_F4:                   return ImGuiKey_F4;
        case AKEYCODE_F5:                   return ImGuiKey_F5;
        case AKEYCODE_F6:                   return ImGuiKey_F6;
        case AKEYCODE_F7:                   return ImGuiKey_F7;
        case AKEYCODE_F8:                   return ImGuiKey_F8;
        case AKEYCODE_F9:                   return ImGuiKey_F9;
        case AKEYCODE_F10:                  return ImGuiKey_F10;
        case AKEYCODE_F11:                  return ImGuiKey_F11;
        case AKEYCODE_F12:                  return ImGuiKey_F12;
        default:                            return ImGuiKey_None;
    }
}



bool ImGui_ImplGlfm_KeyCallback(GLFMDisplay* display, int keycode, int scancode, int action, int mods)
{
    ImGui_ImplGlfm_Data* bd = ImGui_ImplGlfm_GetBackendData();
    if (bd->PrevUserCallbackKey != nullptr && display == bd->Display)
        bd->PrevUserCallbackKey(display, keycode, scancode, action, mods);

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


    ImGuiKey imgui_key = ImGui_ImplGlfm_ScanCodeToImGuiKey(scancode);
    io.AddKeyEvent(imgui_key, (action == GLFMKeyActionPressed));
    io.SetKeyEventNativeData(imgui_key, keycode, scancode); // To support legacy indexing (<1.87 user code)
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

    //bd->PrevUserCallbackWindowFocus = glfwSetWindowFocusCallback(display, ImGui_ImplGlfw_WindowFocusCallback);
    //bd->PrevUserCallbackCursorEnter = glfwSetCursorEnterCallback(display, ImGui_ImplGlfw_CursorEnterCallback);
    bd->PrevUserCallbackTouch = glfmSetTouchFunc(display, ImGui_ImplGlfm_TouchCallback);
    //bd->PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(display, ImGui_ImplGlfw_MouseButtonCallback);
    //bd->PrevUserCallbackScroll = glfwSetScrollCallback(display, ImGui_ImplGlfw_ScrollCallback);
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
    //io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    //io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = bd->Display;

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
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

static void ImGui_ImplGlfw_UpdateMouseData()
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
            glfwSetCursorPos(bd->Window, (double)io.MousePos.x, (double)io.MousePos.y);

        // (Optional) Fallback to provide mouse position when focused (ImGui_ImplGlfw_CursorPosCallback already provides this when hovered or captured)
        if (is_app_focused && bd->MouseWindow == nullptr)
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos(bd->Window, &mouse_x, &mouse_y);
            io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
            bd->LastValidMousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
    }
}

#endif

#if 0

static void ImGui_ImplGlfw_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        glfwSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        // Show OS mouse cursor
        // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
        glfwSetCursor(bd->Window, bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
        glfwSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

#endif

#if 0
// Update gamepad inputs
static inline float Saturate(float v) { return v < 0.0f ? 0.0f : v  > 1.0f ? 1.0f : v; }
static void ImGui_ImplGlfw_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
        return;

    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
#if GLFW_HAS_GAMEPAD_API
    GLFWgamepadstate gamepad;
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
        return;
    #define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED)          do { io.AddKeyEvent(KEY_NO, gamepad.buttons[BUTTON_NO] != 0); } while (0)
    #define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1)    do { float v = gamepad.axes[AXIS_NO]; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#else
    int axes_count = 0, buttons_count = 0;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    if (axes_count == 0 || buttons_count == 0)
        return;
    #define MAP_BUTTON(KEY_NO, _UNUSED, BUTTON_NO)          do { io.AddKeyEvent(KEY_NO, (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS)); } while (0)
    #define MAP_ANALOG(KEY_NO, _UNUSED, AXIS_NO, V0, V1)    do { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#endif
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    MAP_BUTTON(ImGuiKey_GamepadStart,       GLFW_GAMEPAD_BUTTON_START,          7);
    MAP_BUTTON(ImGuiKey_GamepadBack,        GLFW_GAMEPAD_BUTTON_BACK,           6);
    MAP_BUTTON(ImGuiKey_GamepadFaceLeft,    GLFW_GAMEPAD_BUTTON_X,              2);     // Xbox X, PS Square
    MAP_BUTTON(ImGuiKey_GamepadFaceRight,   GLFW_GAMEPAD_BUTTON_B,              1);     // Xbox B, PS Circle
    MAP_BUTTON(ImGuiKey_GamepadFaceUp,      GLFW_GAMEPAD_BUTTON_Y,              3);     // Xbox Y, PS Triangle
    MAP_BUTTON(ImGuiKey_GamepadFaceDown,    GLFW_GAMEPAD_BUTTON_A,              0);     // Xbox A, PS Cross
    MAP_BUTTON(ImGuiKey_GamepadDpadLeft,    GLFW_GAMEPAD_BUTTON_DPAD_LEFT,      13);
    MAP_BUTTON(ImGuiKey_GamepadDpadRight,   GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,     11);
    MAP_BUTTON(ImGuiKey_GamepadDpadUp,      GLFW_GAMEPAD_BUTTON_DPAD_UP,        10);
    MAP_BUTTON(ImGuiKey_GamepadDpadDown,    GLFW_GAMEPAD_BUTTON_DPAD_DOWN,      12);
    MAP_BUTTON(ImGuiKey_GamepadL1,          GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,    4);
    MAP_BUTTON(ImGuiKey_GamepadR1,          GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,   5);
    MAP_ANALOG(ImGuiKey_GamepadL2,          GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,     4,      -0.75f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadR2,          GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,    5,      -0.75f,  +1.0f);
    MAP_BUTTON(ImGuiKey_GamepadL3,          GLFW_GAMEPAD_BUTTON_LEFT_THUMB,     8);
    MAP_BUTTON(ImGuiKey_GamepadR3,          GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,    9);
    MAP_ANALOG(ImGuiKey_GamepadLStickLeft,  GLFW_GAMEPAD_AXIS_LEFT_X,           0,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickRight, GLFW_GAMEPAD_AXIS_LEFT_X,           0,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickUp,    GLFW_GAMEPAD_AXIS_LEFT_Y,           1,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickDown,  GLFW_GAMEPAD_AXIS_LEFT_Y,           1,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickLeft,  GLFW_GAMEPAD_AXIS_RIGHT_X,          2,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickRight, GLFW_GAMEPAD_AXIS_RIGHT_X,          2,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickUp,    GLFW_GAMEPAD_AXIS_RIGHT_Y,          3,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickDown,  GLFW_GAMEPAD_AXIS_RIGHT_Y,          3,      +0.25f,  +1.0f);
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
    //glfwGetFramebufferSize(bd->Window, &display_w, &display_h);
    display_w = w;
    display_h = h;
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / (float)w, (float)display_h / (float)h);

    // Setup time step
    double current_time = glfmGetTime();
    io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time = current_time;

    //ImGui_ImplGlfw_UpdateMouseData();
    //ImGui_ImplGlfw_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    //ImGui_ImplGlfw_UpdateGamepads();
}


#if 0

void ImGui_ImplGlfm_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

    // Backup GL state
    GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLuint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&last_program);
    GLuint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&last_texture);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
    GLuint last_sampler; if (bd->GlVersion >= 330) { glGetIntegerv(GL_SAMPLER_BINDING, (GLint*)&last_sampler); } else { last_sampler = 0; }
#endif
    GLuint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    // This is part of VAO on OpenGL 3.0+ and OpenGL ES 3.0+.
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    ImGui_ImplOpenGL3_VtxAttribState last_vtx_attrib_state_pos; last_vtx_attrib_state_pos.GetState(bd->AttribLocationVtxPos);
    ImGui_ImplOpenGL3_VtxAttribState last_vtx_attrib_state_uv; last_vtx_attrib_state_uv.GetState(bd->AttribLocationVtxUV);
    ImGui_ImplOpenGL3_VtxAttribState last_vtx_attrib_state_color; last_vtx_attrib_state_color.GetState(bd->AttribLocationVtxColor);
#endif
#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    GLuint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&last_vertex_array_object);
#endif
#ifdef IMGUI_IMPL_HAS_POLYGON_MODE
    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
    GLboolean last_enable_primitive_restart = (bd->GlVersion >= 310) ? glIsEnabled(GL_PRIMITIVE_RESTART) : GL_FALSE;
#endif

    // Setup desired GL state
    // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
    // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
    GLuint vertex_array_object = 0;
#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    GL_CALL(glGenVertexArrays(1, &vertex_array_object));
#endif
    ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        // Upload vertex/index buffers
        // - OpenGL drivers are in a very sorry state nowadays....
        //   During 2021 we attempted to switch from glBufferData() to orphaning+glBufferSubData() following reports
        //   of leaks on Intel GPU when using multi-viewports on Windows.
        // - After this we kept hearing of various display corruptions issues. We started disabling on non-Intel GPU, but issues still got reported on Intel.
        // - We are now back to using exclusively glBufferData(). So bd->UseBufferSubData IS ALWAYS FALSE in this code.
        //   We are keeping the old code path for a while in case people finding new issues may want to test the bd->UseBufferSubData path.
        // - See https://github.com/ocornut/imgui/issues/4468 and please report any corruption issues.
        const GLsizeiptr vtx_buffer_size = (GLsizeiptr)cmd_list->VtxBuffer.Size * (int)sizeof(ImDrawVert);
        const GLsizeiptr idx_buffer_size = (GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx);
        if (bd->UseBufferSubData)
        {
            if (bd->VertexBufferSize < vtx_buffer_size)
            {
                bd->VertexBufferSize = vtx_buffer_size;
                GL_CALL(glBufferData(GL_ARRAY_BUFFER, bd->VertexBufferSize, nullptr, GL_STREAM_DRAW));
            }
            if (bd->IndexBufferSize < idx_buffer_size)
            {
                bd->IndexBufferSize = idx_buffer_size;
                GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bd->IndexBufferSize, nullptr, GL_STREAM_DRAW));
            }
            GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vtx_buffer_size, (const GLvoid*)cmd_list->VtxBuffer.Data));
            GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idx_buffer_size, (const GLvoid*)cmd_list->IdxBuffer.Data));
        }
        else
        {
            GL_CALL(glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW));
            GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW));
        }

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle (Y is inverted in OpenGL)
                GL_CALL(glScissor((int)clip_min.x, (int)((float)fb_height - clip_max.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y)));

                // Bind texture, Draw
                GL_CALL(glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->GetTexID()));
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
                if (bd->GlVersion >= 320)
                    GL_CALL(glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset));
                else
#endif
                GL_CALL(glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx))));
            }
        }
    }

    // Destroy the temporary VAO
#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    GL_CALL(glDeleteVertexArrays(1, &vertex_array_object));
#endif

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
    if (bd->GlVersion >= 330)
        glBindSampler(0, last_sampler);
#endif
    glActiveTexture(last_active_texture);
#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    glBindVertexArray(last_vertex_array_object);
#endif
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    last_vtx_attrib_state_pos.SetState(bd->AttribLocationVtxPos);
    last_vtx_attrib_state_uv.SetState(bd->AttribLocationVtxUV);
    last_vtx_attrib_state_color.SetState(bd->AttribLocationVtxColor);
#endif
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
    if (bd->GlVersion >= 310) { if (last_enable_primitive_restart) glEnable(GL_PRIMITIVE_RESTART); else glDisable(GL_PRIMITIVE_RESTART); }
#endif

#ifdef IMGUI_IMPL_HAS_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    (void)bd; // Not all compilation paths use this
}

#endif

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
