//  lk_platform->h - public domain platform abstraction layer with live code editing support
//  no warranty is offered or implied

/*********************************************************************************************

Include this file in all places you need to refer to it. In one of your compilation units, write:
    #define LK_PLATFORM_IMPLEMENTATION
before including lk_platform->h, in order to paste in the source code.

This library provides a platform layer with support for live code editing. That enables you to
modify your source code, recompile it, and see the changes happen live (on the next frame),
without restarting the application. In order for this to work, you should compile this
platform layer to an EXE, and compile the rest of your application code to a DLL. The platform
layer will then load the DLL dynamically at runtime, and reload it if it detects a change.

If you want live code editing, you must specify the DLL name (not including the extension) by defining:
    #define LK_PLATFORM_DLL_NAME "my_application_dll"
before including the implementation of lk_platform->h.

If you don't want live code editing, you must disable it by defining:
    #define LK_PLATFORM_NO_DLL
before including the implementation of lk_platform->h.

As for the platform layer itself, it's designed to minimize interaction between the application
and the platform. All communication goes through a single LK_Platform context struct, and the only
way the platform ever interfaces with the application is by calling a small set of predefined functions.
In a way, this is a function-less library, the application exposes all the functions.

Your application doesn't necessarily have to expose any functions, but you probably want these:

    LK_CLIENT_EXPORT void lk_client_init(LK_Platform* platform);    // called once, before any platform stuff is created
    LK_CLIENT_EXPORT void lk_client_close(LK_Platform* platform);   // called once, before all platform stuff is destroyed
    LK_CLIENT_EXPORT void lk_client_frame(LK_Platform* platform);   // called each frame if your application has a window

You can also expose these, to get notified when your DLL was loaded or unloaded:

    LK_CLIENT_EXPORT void lk_client_dll_load(LK_Platform* platform);
    LK_CLIENT_EXPORT void lk_client_dll_unload(LK_Platform* platform);

Note that lk_client_dll_load() is the first of all functions to be called,
and lk_client_dll_unload() is the last, after everything is destroyed.

If you want to output audio, you should expose:

    LK_CLIENT_EXPORT void lk_client_render_audio(LK_Platform* platform, void* frames, LK_U32 frame_count);  // called on a separate thread

If you want to capture audio from a microphone, you should expose:

    LK_CLIENT_EXPORT void lk_client_capture_audio(LK_Platform* platform, void* frames, LK_U32 frame_count);  // called on a separate thread

If you want lk_platform to use your logging API, you should expose:

    LK_CLIENT_EXPORT void lk_client_log(LK_Platform* platform, const char* message, const char* file, int line);

Additional OS-specific callbacks:

    LK_CLIENT_EXPORT BOOL lk_client_win32_event_handler(LK_Platform* platform, HWND window, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* out_return);  // called on a separate thread

The platform pointer that's passed in all of the functions is unique and never changes during the lifetime of the application.
If you're using dynamic DLL loading, all of your global variables WILL BE DESTROYED after the DLL gets unloaded.
You can keep a pointer to persistent storage in platform->client_data.

QUICK NOTES
    @Incomplete

LICENSE
    This software is in the public domain. Anyone can use it, modify it,
    roll'n'smoke hardcopies of the source code, sell it to the terrorists, etc.
    No warranty is offered or implied; use this code at your own risk!

    See end of file for license information.

DOCUMENTATION
    @Incomplete

 *********************************************************************************************/


#ifndef LK_PLATFORM_HEADER
#define LK_PLATFORM_HEADER

#ifdef __cplusplus
#define LK_CLIENT_EXPORT extern "C" __declspec(dllexport)
#else
#define LK_CLIENT_EXPORT __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LK_SIMPLE_TYPES
#define LK_SIMPLE_TYPES
typedef signed char  LK_S8;
typedef signed short LK_S16;
typedef signed long  LK_S32;

typedef unsigned char      LK_U8;
typedef unsigned short     LK_U16;
typedef unsigned long      LK_U32;
typedef unsigned long long LK_U64;

typedef LK_U8  LK_B8;
typedef LK_U16 LK_B16;
typedef LK_U32 LK_B32;

typedef float  LK_F32;
typedef double LK_F64;
#endif

typedef enum
{
    LK_WINDOW_CANVAS,
    LK_WINDOW_OPENGL,
    LK_WINDOW_CUSTOM,
} LK_Window_Backend;

typedef struct
{
    LK_B8 down     : 1;
    LK_B8 pressed  : 1;
    LK_B8 released : 1;
    LK_B8 repeated : 1; // true when "pressed" is true, or when a repeat event happened
} LK_Digital_Button;

typedef enum
{
// The following constants match their ASCII encoding
    LK_KEY_BACKSPACE = '\b', // 8
    LK_KEY_TAB       = '\t', // 9
    LK_KEY_ENTER     = '\n', // 10
    LK_KEY_ESCAPE    = 27,   // 27 '\e'
    LK_KEY_SPACE     = ' ',  // 32
    LK_KEY_COMMA     = ',',  // 44
    LK_KEY_PERIOD    = '.',  // 46
    LK_KEY_GRAVE     = '`',  // 96
    LK_KEY_DELETE    = 127,

    LK_KEY_0 = '0', // 48
    LK_KEY_1,
    LK_KEY_2,
    LK_KEY_3,
    LK_KEY_4,
    LK_KEY_5,
    LK_KEY_6,
    LK_KEY_7,
    LK_KEY_8,
    LK_KEY_9,

    LK_KEY_A = 'A', // 65
    LK_KEY_B,
    LK_KEY_C,
    LK_KEY_D,
    LK_KEY_E,
    LK_KEY_F,
    LK_KEY_G,
    LK_KEY_H,
    LK_KEY_I,
    LK_KEY_J,
    LK_KEY_K,
    LK_KEY_L,
    LK_KEY_M,
    LK_KEY_N,
    LK_KEY_O,
    LK_KEY_P,
    LK_KEY_Q,
    LK_KEY_R,
    LK_KEY_S,
    LK_KEY_T,
    LK_KEY_U,
    LK_KEY_V,
    LK_KEY_W,
    LK_KEY_X,
    LK_KEY_Y,
    LK_KEY_Z,

// The following constants don't match any encoding.
// You may NOT assume that they remain the same in future versions.
// You may assume the RELATIVE order of constants that involve numbers.
// For example, you can do stuff like LK_KEY_F1 + i, or LK_KEY_NUMPAD_0 + i.
    LK_KEY_CAPS_LOCK = 128,
    LK_KEY_LEFT_SHIFT,
    LK_KEY_LEFT_CONTROL,
    LK_KEY_LEFT_WINDOWS,
    LK_KEY_LEFT_ALT,
    LK_KEY_RIGHT_SHIFT,
    LK_KEY_RIGHT_CONTROL,
    LK_KEY_RIGHT_WINDOWS,
    LK_KEY_RIGHT_ALT,

    LK_KEY_PRINT_SCREEN,
    LK_KEY_SCREEN_LOCK,
    LK_KEY_PAUSE,
    LK_KEY_INSERT,
    LK_KEY_HOME,
    LK_KEY_END,
    LK_KEY_PAGE_UP,
    LK_KEY_PAGE_DOWN,

    LK_KEY_ARROW_LEFT,
    LK_KEY_ARROW_RIGHT,
    LK_KEY_ARROW_UP,
    LK_KEY_ARROW_DOWN,

    LK_KEY_NUMLOCK,
    LK_KEY_NUMPAD_PLUS,
    LK_KEY_NUMPAD_MINUS,
    LK_KEY_NUMPAD_MULTIPLY,
    LK_KEY_NUMPAD_DIVIDE,
    LK_KEY_NUMPAD_PERIOD,
    LK_KEY_NUMPAD_ENTER,
    LK_KEY_NUMPAD_0,
    LK_KEY_NUMPAD_1,
    LK_KEY_NUMPAD_2,
    LK_KEY_NUMPAD_3,
    LK_KEY_NUMPAD_4,
    LK_KEY_NUMPAD_5,
    LK_KEY_NUMPAD_6,
    LK_KEY_NUMPAD_7,
    LK_KEY_NUMPAD_8,
    LK_KEY_NUMPAD_9,

    LK_KEY_F1,
    LK_KEY_F2,
    LK_KEY_F3,
    LK_KEY_F4,
    LK_KEY_F5,
    LK_KEY_F6,
    LK_KEY_F7,
    LK_KEY_F8,
    LK_KEY_F9,
    LK_KEY_F10,
    LK_KEY_F11,
    LK_KEY_F12,

    LK__KEY_COUNT = 256,
} LK_Key;

enum
{
    LK_MAX_GAMEPADS = 4,

    // Xbox controller buttons
    LK_GAMEPAD_XBOX_A = 0,
    LK_GAMEPAD_XBOX_B,
    LK_GAMEPAD_XBOX_X,
    LK_GAMEPAD_XBOX_Y,
    LK_GAMEPAD_XBOX_LB,
    LK_GAMEPAD_XBOX_RB,
    LK_GAMEPAD_XBOX_VIEW,
    LK_GAMEPAD_XBOX_MENU,
    LK_GAMEPAD_XBOX_GUIDE,
    LK_GAMEPAD_XBOX_LSB,
    LK_GAMEPAD_XBOX_RSB,

    // PlayStation controller buttons
    LK_GAMEPAD_PS_X = 0,
    LK_GAMEPAD_PS_CIRCLE,
    LK_GAMEPAD_PS_SQUARE,
    LK_GAMEPAD_PS_TRIANGLE,
    LK_GAMEPAD_PS_L1,
    LK_GAMEPAD_PS_R1,
    LK_GAMEPAD_PS_SELECT,
    LK_GAMEPAD_PS_START,
    LK_GAMEPAD_PS_LOGO,
    LK_GAMEPAD_PS_L3,
    LK_GAMEPAD_PS_R3,

    LK_GAMEPAD_BUTTON_COUNT,

    // Xbox controller analogs
    LK_GAMEPAD_XBOX_LEFT_STICK_X = 0,
    LK_GAMEPAD_XBOX_LEFT_STICK_Y,
    LK_GAMEPAD_XBOX_RIGHT_STICK_X,
    LK_GAMEPAD_XBOX_RIGHT_STICK_Y,
    LK_GAMEPAD_XBOX_LEFT_TRIGGER,
    LK_GAMEPAD_XBOX_RIGHT_TRIGGER,

    // PlayStation controller analogs
    LK_GAMEPAD_PS_LEFT_STICK_X = 0,
    LK_GAMEPAD_PS_LEFT_STICK_Y,
    LK_GAMEPAD_PS_RIGHT_STICK_X,
    LK_GAMEPAD_PS_RIGHT_STICK_Y,
    LK_GAMEPAD_PS_L2,
    LK_GAMEPAD_PS_R2,

    LK_GAMEPAD_ANALOG_COUNT
};

typedef struct LK_Gamepad_Structure
{
    LK_B32 connected;
    LK_F32 battery;  // Negative value means it doesn't have a battery, or we don't have battery information.
                     // Otherwise it's a number between 0.0f (empty) and 1.0f (full).
                     // (for xinput devices, full = 1.0f, medium = 0.6f, low = 0.3f, empty = 0.0f)

    LK_Digital_Button buttons[LK_GAMEPAD_BUTTON_COUNT];
    LK_F64 analog[LK_GAMEPAD_ANALOG_COUNT];

    struct
    {
        // Normalized vector in the direction of hat switch input. Zero if nothing is pressed.
        LK_F64 x;
        LK_F64 y;

        // Negative value means nothing is pressed. Otherwise, it's the angle in degrees.
        // Examples: 0 is right, 90 is up, 180 is left, 270 is down.
        LK_F64 angle;
    } hat;
} LK_Gamepad;

enum
{
    LK_DEFAULT_POSITION = 0x80000000,
};

typedef enum
{
    LK_AUDIO_SAMPLE_UNSPECIFIED,
    LK_AUDIO_SAMPLE_INT16,
    LK_AUDIO_SAMPLE_FLOAT32,
} LK_Audio_Sample_Type;

enum
{
    LK_EVENT_WINDOW_MOVE = 0x0001,  // window.x or window.y changed
    LK_EVENT_WINDOW_SIZE = 0x0002,  // window.width or window.height changed

    LK_EVENT_MOUSE_WHEEL   = 0x0008,  // mouse.delta_wheel is non-zero
    LK_EVENT_MOUSE_PRESS   = 0x0010,  // some mouse button pressed
    LK_EVENT_MOUSE_RELEASE = 0x0020,  // some mouse button released
    LK_EVENT_MOUSE_DOWN    = 0x0040,  // some mouse button is down
    LK_EVENT_MOUSE_MOVE    = 0x0080,  // mouse.x or mouse.y changed
    LK_EVENT_MOUSE         = LK_EVENT_MOUSE_WHEEL | LK_EVENT_MOUSE_MOVE | LK_EVENT_MOUSE_PRESS | LK_EVENT_MOUSE_RELEASE | LK_EVENT_MOUSE_DOWN,

    LK_EVENT_KEYBOARD_PRESS   = 0x0100,  // some key pressed
    LK_EVENT_KEYBOARD_RELEASE = 0x0200,  // some key released
    LK_EVENT_KEYBOARD_DOWN    = 0x0400,  // some key is down
    LK_EVENT_KEYBOARD_TEXT    = 0x0800,  // keyboard.text has content
    LK_EVENT_KEYBOARD         = LK_EVENT_KEYBOARD_PRESS | LK_EVENT_KEYBOARD_RELEASE | LK_EVENT_KEYBOARD_DOWN | LK_EVENT_KEYBOARD_TEXT,

    LK_EVENT_GAMEPAD_PRESS   = 0x1000,  // some gamepad button pressed
    LK_EVENT_GAMEPAD_RELEASE = 0x2000,  // some gamepad button released
    LK_EVENT_GAMEPAD_DOWN    = 0x4000,  // some gamepad button is down
    LK_EVENT_GAMEPAD_ANALOG  = 0x8000,  // some analog value is non-zero
    LK_EVENT_GAMEPAD         = LK_EVENT_GAMEPAD_PRESS | LK_EVENT_GAMEPAD_RELEASE | LK_EVENT_GAMEPAD_DOWN | LK_EVENT_GAMEPAD_ANALOG,
};

typedef struct LK_Platform_Structure
{
    LK_B32 break_frame_loop;
    void* client_data;

    LK_U32 event_mask;  // set before each frame

    // If pause_frame_loop is set, the frame loop will pause until the window receives
    // events, or the timeout interval elapses, whichever happens first.
    LK_B32 pause_frame_loop;        // set to false before each frame
    LK_F32 pause_timeout_seconds;   // set to 0.1f before each frame

    struct
    {
        LK_B32 no_window;           // read after init
        LK_Window_Backend backend;  // read after init

        // Transparency on Windows is only available for undecorated windows.
        LK_B32 transparent;         // read after init
        LK_B32 disable_animations;  // read after init

        void* handle;  // set after init, HWND on Windows

        char* title;  // reactive

        // These values don't include the window border, only the client area.
        LK_S32 x;       // reactive
        LK_S32 y;       // reactive
        LK_U32 width;   // reactive
        LK_U32 height;  // reactive

        LK_B32 fullscreen;          // reactive
        LK_B32 forbid_resizing;     // reactive
        LK_B32 undecorated;         // reactive
        LK_B32 invisible;           // reactive
        LK_B32 has_focus;           // set each frame

        // Icon input is read after init, if icon_pixels is not null.
        LK_U32 icon_width;
        LK_U32 icon_height;
        LK_U8* icon_pixels;  // must be RGBA, left to right, top to bottom

        // If no_resize_on_dpi_change is false, when DPI changes, the size of the window in inches will stay the same.
        // If no_resize_on_dpi_change is true, when DPI changes, the size of the window in pixels will stay the same.
        LK_B32 no_resize_on_dpi_change;  // reactive
        LK_U32 dpi;                      // set each frame
        LK_F32 width_in_inches;          // set each frame
        LK_F32 height_in_inches;         // set each frame

        LK_B32 swap_buffers;          // Set to 1 before each frame. Set this to 0 if you don't want to swap buffers.
        LK_U32 monitor_refresh_rate;  // In Hz, set each frame. If zero, the information could not be queried, or the monitor doesn't have a refresh rate.
    } window;

    struct
    {
        LK_S32 x;
        LK_S32 y;
        LK_S32 delta_x;
        LK_S32 delta_y;
        LK_S32 delta_wheel;

        // These are in a hardware-specific, native resolution, if available.
        // Otherwise they match delta_x and delta_y.
        LK_S32 delta_x_raw;
        LK_S32 delta_y_raw;

        LK_Digital_Button left_button;
        LK_Digital_Button right_button;
        LK_Digital_Button middle_button;
    } mouse;

    struct
    {
        LK_Digital_Button state[LK__KEY_COUNT];
        char* text; // UTF-8 formatted string.

        LK_B32 disable_windows_keys;
    } keyboard;

    LK_Gamepad gamepads[LK_MAX_GAMEPADS];

    struct
    {
        LK_U32 width;
        LK_U32 height;
        LK_U8* data;
    } canvas;

    struct
    {
        LK_U32 major_version;
        LK_U32 minor_version;
        LK_B32 debug_context;
        LK_B32 compatibility_context;

        LK_U32 swap_interval;

        LK_U32 color_bits;
        LK_U32 depth_bits;
        LK_U32 stencil_bits;
        LK_U32 sample_count;
    } opengl;

    struct
    {
        LK_B32 pause_render;
        LK_B32 pause_capture;

        // Settings for audio rendering and capture. By default, all settings are unspecified.
        // If you don't specify some setting, it is set to the optimal value before each call to client audio callbacks.
        // Note that optimal values might change when devices are disconnected or changed.

        LK_Audio_Sample_Type render_sample_type;
        LK_U32 render_channels;
        LK_U32 render_frequency;

        LK_Audio_Sample_Type capture_sample_type;
        LK_U32 capture_channels;
        LK_U32 capture_frequency;
    } audio;

    struct
    {
        LK_U64 delta_ticks;
        LK_U64 delta_nanoseconds;
        LK_U64 delta_microseconds;
        LK_U64 delta_milliseconds;
        LK_F64 delta_seconds;

        LK_U64 ticks;
        LK_U64 nanoseconds;
        LK_U64 microseconds;
        LK_U64 milliseconds;
        LK_F64 seconds;
    } time;

    struct
    {
        int argument_count;
        char** arguments;
    } command_line;

    struct
    {
        const char* vendor;

        LK_U32 logical_count;
        LK_U32 cache_line_size;

        LK_B8 has_rdtsc;
        LK_B8 has_mmx;
        LK_B8 has_sse;
        LK_B8 has_sse2;
        LK_B8 has_sse3;
        LK_B8 has_sse41;
        LK_B8 has_sse42;
        LK_B8 has_avx;
        LK_B8 has_avx2;
        LK_B8 has_avx512f;
        LK_B8 has_altivec;
        LK_B8 has_3dnow;
        LK_B8 has_hyperthreading;

        LK_U64 ram_bytes;
        LK_U64 ram_kilobytes;
        LK_U64 ram_megabytes;
    } system;

    void* private_pointer;

#ifdef LK_PLATFORM_USER_CONTEXT
    LK_PLATFORM_USER_CONTEXT
#endif
} LK_Platform;


typedef struct
{
    void(*init)               (LK_Platform* platform);
    void(*first_frame)        (LK_Platform* platform);
    void(*frame)              (LK_Platform* platform);
    void(*close)              (LK_Platform* platform);
    void(*render_audio)       (LK_Platform* platform, void* frames, LK_U32 frame_count);
    void(*capture_audio)      (LK_Platform* platform, void* frames, LK_U32 frame_count);
    void(*log)                (LK_Platform* platform, const char* message, const char* file, int line);
    void(*dll_load)           (LK_Platform* platform);
    void(*dll_unload)         (LK_Platform* platform);
    int (*win32_event_handler)(LK_Platform* platform, void* window, LK_U32 message, void* wparam, void* lparam, void** out_return);
} LK_Client_Functions;

void lk_entry(LK_Client_Functions* functions);


#ifdef __cplusplus
}
#endif

#endif



#ifdef LK_PLATFORM_IMPLEMENTATION
#ifndef LK_PLATFORM_IMPLEMENTED
#define LK_PLATFORM_IMPLEMENTED

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(_WIN32_WINNT) && !defined(WINVER)
// Targeting Windows XP by default.
#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define CINTERFACE
#define COBJMACROS
#include <windows.h>
#include <dbt.h>
#include <xinput.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#undef near
#undef far


////////////////////////////////////////////////////////////////////////////////
// Utilities
////////////////////////////////////////////////////////////////////////////////


#define LK_GetProc(module, destination, name)        \
{                                                    \
    destination = NULL;                              \
    if (module)                                      \
    {                                                \
        FARPROC proc = GetProcAddress(module, name); \
        if (proc)                                    \
            *(FARPROC*) &destination = proc;         \
    }                                                \
}


enum { LK_CIRCULAR_BUFFER_SIZE = 256 };

typedef struct
{
    LONG volatile read;
    LONG volatile write;
    LK_U8 data[LK_CIRCULAR_BUFFER_SIZE];
} LK_Circular_Buffer;

static LONG lk_available_in_circular_buffer(LK_Circular_Buffer* buffer)
{
    return buffer->write - buffer->read;
}

static LK_B32 lk_read_from_circular_buffer(LK_Circular_Buffer* buffer, void* data, LONG size)
{
    LONG read = buffer->read;
    LONG available = buffer->write - read;
    if (available < size)
        return 0;

    LK_U8* bytes = (LK_U8*) data;
    while (size-- > 0)
        *(bytes++) = buffer->data[read++ % sizeof(buffer->data)];

    InterlockedExchange(&buffer->read, read);
    return 1;
}

static LK_B32 lk_write_to_circular_buffer(LK_Circular_Buffer* buffer, void* data, LONG size)
{
    LONG write = buffer->write;
    LONG available = sizeof(buffer->data) - (write - buffer->read);
    if (available < size)
        return 0;

    LK_U8* bytes = (LK_U8*) data;
    while (size-- > 0)
        buffer->data[write++ % sizeof(buffer->data)] = *(bytes++);

    InterlockedExchange(&buffer->write, write);
    return 1;
}


typedef struct
{
    const char* message;
    int line;
} LK_Log_Line;

#define LK_Log(buffer, message)                               \
{                                                             \
    LK_Log_Line line = { message, __LINE__ };                 \
    lk_write_to_circular_buffer(buffer, &line, sizeof(line)); \
}


static WCHAR* lk_get_module_directory()
{
    // get the exe path
    SIZE_T size = MAX_PATH + 1;
    WCHAR* path = (WCHAR*) LocalAlloc(LMEM_FIXED, sizeof(WCHAR) * size);
    if (!path) return 0;
    while (GetModuleFileNameW(0, path, size) == size)
    {
        size *= 2;
        LocalFree(path);
        path = (WCHAR*) LocalAlloc(LMEM_FIXED, sizeof(WCHAR) * size);
        if (!path) return 0;
    }

    // find the last / or \ character
    WCHAR* last_slash = 0;
    WCHAR* cursor = path;
    while (*cursor)
    {
        if (*cursor == '/' || *cursor == '\\')
            last_slash = cursor;
        cursor++;
    }

    // remove the exe name, keep the trailing slash
    if (last_slash)
    {
        last_slash[1] = 0;
    }

    return path;
}

static WCHAR* lk_wchar_concatenate(WCHAR* str1, WCHAR* str2, WCHAR* str3, WCHAR* str4)
{
    int i;
    WCHAR* strings[4] = { str1, str2, str3, str4 };
    SIZE_T size = 1;

    // compute the concatenated string length
    for (i = 0; i < 4; i++)
    {
        WCHAR* cursor = strings[i];
        if (!cursor) continue;
        while (*(cursor++)) size++;
    }

    // allocate and copy
    WCHAR* result = (WCHAR*) LocalAlloc(LMEM_FIXED, sizeof(WCHAR) * size);
    if (!result) return 0;

    WCHAR* write = result;
    for (i = 0; i < 4; i++)
    {
        WCHAR* cursor = strings[i];
        if (!cursor) continue;
        while (*cursor) *(write++) = *(cursor++);
    }

    *write = 0;
    return result;
}


static void lk_update_digital_button(LK_Digital_Button* button, LK_B8 down)
{
    button->pressed  = ( down && !button->down);
    button->released = (!down &&  button->down);
    button->down     = down;
    button->repeated = button->pressed;
}

static void lk_get_client_rectangle(HWND hwnd, LK_S32* x, LK_S32* y, LK_U32* width, LK_U32* height)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    *width  = (LK_U32)(rect.right - rect.left);
    *height = (LK_U32)(rect.bottom - rect.top);

    POINT pos = { rect.left, rect.top };
    ClientToScreen(hwnd, &pos);
    *x = (LK_S32) pos.x;
    *y = (LK_S32) pos.y;
}


static void lk_get_monitor_rectangle(HWND hwnd, LK_S32* x, LK_S32* y, LK_U32* width, LK_U32* height)
{
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO info;
    info.cbSize = sizeof(info);
    info.rcMonitor.left = 0;
    info.rcMonitor.top = 0;
    info.rcMonitor.right = 1920;
    info.rcMonitor.bottom = 1080;
    GetMonitorInfoA(monitor, &info);
    *x      = info.rcMonitor.left;
    *y      = info.rcMonitor.top;
    *width  = info.rcMonitor.right  - info.rcMonitor.left;
    *height = info.rcMonitor.bottom - info.rcMonitor.top;
}

static LK_U32 lk_get_monitor_refresh_rate(HWND hwnd, HDC dc)
{
    if (hwnd)
    {
        HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (monitor)
        {
            MONITORINFOEXW monitor_info;
            monitor_info.cbSize = sizeof(monitor_info);
            if (GetMonitorInfoW(monitor, (MONITORINFO*) &monitor_info))
            {
                DEVMODEW monitor_settings = { 0 };
                monitor_settings.dmSize = sizeof(monitor_settings);
                if (EnumDisplaySettingsW(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &monitor_settings))
                {
                    LK_U32 refresh_rate = monitor_settings.dmDisplayFrequency;

                    // EnumDisplaySettings can return 0 or 1. That's either an error, or hardware default.
                    if (refresh_rate > 1)
                        return refresh_rate;
                }
            }
        }
    }

    if (dc)
    {
        LK_U32 refresh_rate = GetDeviceCaps(dc, VREFRESH);

        // GetDeviceCaps can return 0 or 1. That's either an error, or hardware default.
        if (refresh_rate > 1)
            return refresh_rate;
    }

    // Try to get default system info.
    DEVMODEW monitor_settings = { 0 };
    if (EnumDisplaySettingsW(0, ENUM_CURRENT_SETTINGS, &monitor_settings))
    {
        LK_U32 refresh_rate = monitor_settings.dmDisplayFrequency;

        // EnumDisplaySettings can return 0 or 1. That's either an error, or hardware default.
        if (refresh_rate > 1)
            return refresh_rate;
    }

    // Oh well...
    return 0;
}


static LK_Key lk_translate_key(USHORT virtual_key, USHORT scan_code, int e0, int e1)
{
    if (virtual_key == 0xFF)
    {
        return (LK_Key) 0;
    }
    else if (virtual_key == VK_SHIFT)
    {
        // left shift  vs  right shift
        virtual_key = MapVirtualKey(scan_code, MAPVK_VSC_TO_VK_EX);
    }
    else if (virtual_key == VK_NUMLOCK)
    {
        // pause/break  vs  numlock
        // MapVirtualKey is buggy, so we need to manually set the extended bit
        scan_code = MapVirtualKey(virtual_key, MAPVK_VK_TO_VSC) | 0x100;
    }

    if (e1)
    {
        // MapVirtualKey is buggy, so we need to set VK_PAUSE manually
        scan_code = (virtual_key == VK_PAUSE) ? 0x45 : MapVirtualKey(virtual_key, MAPVK_VK_TO_VSC);
    }

    switch (virtual_key)
    {
    case VK_CONTROL: virtual_key = e0 ? VK_RCONTROL : VK_LCONTROL; break;
    case VK_MENU:    virtual_key = e0 ? VK_RMENU    : VK_LMENU;    break;
    case VK_RETURN:  if ( e0) virtual_key = VK_RETURN;  break;
    case VK_INSERT:  if (!e0) virtual_key = VK_NUMPAD0; break;
    case VK_DELETE:  if (!e0) virtual_key = VK_DECIMAL; break;
    case VK_HOME:    if (!e0) virtual_key = VK_NUMPAD7; break;
    case VK_END:     if (!e0) virtual_key = VK_NUMPAD1; break;
    case VK_PRIOR:   if (!e0) virtual_key = VK_NUMPAD9; break;
    case VK_NEXT:    if (!e0) virtual_key = VK_NUMPAD3; break;
    case VK_LEFT:    if (!e0) virtual_key = VK_NUMPAD4; break;
    case VK_RIGHT:   if (!e0) virtual_key = VK_NUMPAD6; break;
    case VK_UP:      if (!e0) virtual_key = VK_NUMPAD8; break;
    case VK_DOWN:    if (!e0) virtual_key = VK_NUMPAD2; break;
    case VK_CLEAR:   if (!e0) virtual_key = VK_NUMPAD5; break;
    }


    LK_Key key = (LK_Key) 0;

    if (virtual_key >= '0' && virtual_key <= '9') key = (LK_Key)((int) LK_KEY_0 + (virtual_key - '0'));
    if (virtual_key >= 'A' && virtual_key <= 'Z') key = (LK_Key)((int) LK_KEY_A + (virtual_key - 'A'));
    if (virtual_key >=  96 && virtual_key <= 105) key = (LK_Key)((int) LK_KEY_NUMPAD_0 + (virtual_key - 96));
    if (virtual_key >= 112 && virtual_key <= 123) key = (LK_Key)((int) LK_KEY_F1 + (virtual_key - 112));

    switch (virtual_key)
    {
    case 8:   key = LK_KEY_BACKSPACE;       break;
    case 9:   key = LK_KEY_TAB;             break;
    case 13:  key = LK_KEY_ENTER;           break;
    case 19:  key = LK_KEY_PAUSE;           break;
    case 20:  key = LK_KEY_CAPS_LOCK;       break;
    case 27:  key = LK_KEY_ESCAPE;          break;
    case 32:  key = LK_KEY_SPACE;           break;
    case 33:  key = LK_KEY_PAGE_UP;         break;
    case 34:  key = LK_KEY_PAGE_DOWN;       break;
    case 35:  key = LK_KEY_END;             break;
    case 36:  key = LK_KEY_HOME;            break;
    case 37:  key = LK_KEY_ARROW_LEFT;      break;
    case 38:  key = LK_KEY_ARROW_UP;        break;
    case 39:  key = LK_KEY_ARROW_RIGHT;     break;
    case 40:  key = LK_KEY_ARROW_DOWN;      break;
    case 44:  key = LK_KEY_PRINT_SCREEN;    break;
    case 45:  key = LK_KEY_INSERT;          break;
    case 46:  key = LK_KEY_DELETE;          break;
    case 91:  key = LK_KEY_LEFT_WINDOWS;    break;
    case 92:  key = LK_KEY_RIGHT_WINDOWS;   break;
    case 106: key = LK_KEY_NUMPAD_MULTIPLY; break;
    case 107: key = LK_KEY_NUMPAD_PLUS;     break;
    case 109: key = LK_KEY_NUMPAD_MINUS;    break;
    case 110: key = LK_KEY_NUMPAD_PERIOD;   break;
    case 111: key = LK_KEY_NUMPAD_DIVIDE;   break;
    case 144: key = LK_KEY_NUMLOCK;         break;
    case 145: key = LK_KEY_SCREEN_LOCK;     break;
    case 160: key = LK_KEY_LEFT_SHIFT;      break;
    case 161: key = LK_KEY_RIGHT_SHIFT;     break;
    case 162: key = LK_KEY_LEFT_CONTROL;    break;
    case 163: key = LK_KEY_RIGHT_CONTROL;   break;
    case 164: key = LK_KEY_LEFT_ALT;        break;
    case 165: key = LK_KEY_RIGHT_ALT;       break;
    case 188: key = LK_KEY_COMMA;           break;
    case 190: key = LK_KEY_PERIOD;          break;
    case 192: key = LK_KEY_GRAVE;           break;
    }

    return key;
}


////////////////////////////////////////////////////////////////////////////////
// Client loading
////////////////////////////////////////////////////////////////////////////////


static void lk_client_init_stub(LK_Platform* platform) {}
static void lk_client_first_frame_stub(LK_Platform* platform) {}
static void lk_client_frame_stub(LK_Platform* platform) {}
static void lk_client_close_stub(LK_Platform* platform) {}
static void lk_client_dll_load_stub(LK_Platform* platform) {}
static void lk_client_dll_unload_stub(LK_Platform* platform) {}
static int  lk_client_win32_event_handler_stub(LK_Platform* platform, void* window, LK_U32 message, void* wparam, void* lparam, void** out_return) { return 0; }

static void lk_client_render_audio_stub(LK_Platform* platform, void* frames, LK_U32 frame_count)
{
    LK_U32 sample_size = ((platform->audio.render_sample_type == LK_AUDIO_SAMPLE_INT16) ? 2 : 4);
    LK_U32 buffer_size = frame_count * platform->audio.render_channels * sample_size;
    ZeroMemory(frames, buffer_size);
}

static void lk_client_capture_audio_stub(LK_Platform* platform, void* frames, LK_U32 frame_count) {}
static void lk_client_log_stub(LK_Platform* platform, const char* message, const char* file, int line)
{
    OutputDebugStringA(message);
}



typedef struct
{
    LK_Client_Functions functions;
    LK_Platform* platform;  // should only be used to send as argument to the client

    HMODULE library;

#ifndef LK_PLATFORM_NO_DLL
    LONG volatile reload_lock;
    bool from_dll;

    LK_B32   load_failed;
    FILETIME last_write_time;

    WCHAR*   dll_path;
    WCHAR*   temp_dll_path;
#endif
} LK_Client;


#ifndef LK_PLATFORM_NO_DLL

#ifndef LK_PLATFORM_DLL_NAME
#error "lk_platform->h implementation expects LK_PLATFORM_DLL_NAME to be defined before it is included."
#endif

#define LK_WCHAR_LITERAL(name) LK_WCHAR_LITERAL_(name)
#define LK_WCHAR_LITERAL_(name) L##name

static void lk_set_dll_paths(LK_Client* client, LK_U32 serial)
{
    WCHAR* directory = lk_get_module_directory();

    WCHAR serial_string[16];
    WCHAR* serial_write = serial_string;

    WCHAR* hex_lut = L"0123456789abcdef";
    while (serial != 0)
    {
        *(serial_write++) = hex_lut[serial & 0xF];
        serial >>= 4;
    }
    *serial_write = 0;

    client->dll_path      = lk_wchar_concatenate(directory, LK_WCHAR_LITERAL(LK_PLATFORM_DLL_NAME ".dll"), 0, 0);
    client->temp_dll_path = lk_wchar_concatenate(directory, LK_WCHAR_LITERAL(LK_PLATFORM_DLL_NAME "_temp"), serial_string, L".dll");

    if (directory)
        LocalFree(directory);
}

static void lk_load_client_dll(LK_Client* client, LK_Circular_Buffer* errors)
{
    if (!CopyFileW(client->dll_path, client->temp_dll_path, 0))
    {
        LK_Log(errors, "Failed to copy the client DLL to a temporary location.");
    }

    client->load_failed = 0;
    client->library = LoadLibraryW(client->temp_dll_path);

    if (!client->library)
    {
        LK_Log(errors, "Failed to load the client module.");
        client->load_failed = 1;
    }
}

static LK_B32 lk_check_client_reload(LK_Client* client)
{
    if (!client->dll_path)      return 0;
    if (!client->temp_dll_path) return 0;
    if (client->load_failed)    return 1;

    FILETIME file_time = { 0 };

    WIN32_FIND_DATAW find_data;
    HANDLE find_handle = FindFirstFileW(client->dll_path, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        file_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }

    if (CompareFileTime(&file_time, &client->last_write_time) != 0)
    {
        client->last_write_time = file_time;
        return 1;
    }

    return 0;
}

static void lk_lock_client(LK_Client* client)
{
    while (true)
    {
        LONG value = client->reload_lock;
        if (value < 0)
            SwitchToThread();
        else if (InterlockedCompareExchange(&client->reload_lock, value + 1, value) == value)
            break;
    }
}

static void lk_unlock_client(LK_Client* client)
{
    InterlockedDecrement(&client->reload_lock);
}


#else

#define lk_lock_client(...)
#define lk_unlock_client(...)

#endif


static void lk_load_client_from_library(LK_Client* client)
{
    LK_GetProc(client->library, client->functions.init,                "lk_client_init");
    LK_GetProc(client->library, client->functions.first_frame,         "lk_client_first_frame");
    LK_GetProc(client->library, client->functions.frame,               "lk_client_frame");
    LK_GetProc(client->library, client->functions.close,               "lk_client_close");
    LK_GetProc(client->library, client->functions.dll_load,            "lk_client_dll_load");
    LK_GetProc(client->library, client->functions.dll_unload,          "lk_client_dll_unload");
    LK_GetProc(client->library, client->functions.win32_event_handler, "lk_client_win32_event_handler");
    LK_GetProc(client->library, client->functions.render_audio,        "lk_client_render_audio");
    LK_GetProc(client->library, client->functions.capture_audio,       "lk_client_capture_audio");
    LK_GetProc(client->library, client->functions.log,                 "lk_client_log");
}

static void lk_fill_stubs(LK_Client* client)
{
    if (!client->functions.init)                client->functions.init                = lk_client_init_stub;
    if (!client->functions.first_frame)         client->functions.first_frame         = lk_client_first_frame_stub;
    if (!client->functions.frame)               client->functions.frame               = lk_client_frame_stub;
    if (!client->functions.close)               client->functions.close               = lk_client_close_stub;
    if (!client->functions.dll_load)            client->functions.dll_load            = lk_client_dll_load_stub;
    if (!client->functions.dll_unload)          client->functions.dll_unload          = lk_client_dll_unload_stub;
    if (!client->functions.win32_event_handler) client->functions.win32_event_handler = lk_client_win32_event_handler_stub;
    if (!client->functions.render_audio)        client->functions.render_audio        = lk_client_render_audio_stub;
    if (!client->functions.capture_audio)       client->functions.capture_audio       = lk_client_capture_audio_stub;
    if (!client->functions.log)                 client->functions.log                 = lk_client_log_stub;
}

static void lk_load_client(LK_Platform* platform, LK_Client* client, LK_Client_Functions* functions, LK_U32 instance_serial, LK_Circular_Buffer* errors)
{
    client->platform = platform;

    if (functions)
    {
        client->functions = *functions;
    }
    else
    {
#ifdef LK_PLATFORM_NO_DLL
        client->library = GetModuleHandle(0);
#else
        client->from_dll = 1;
        lk_set_dll_paths(client, instance_serial);
        lk_check_client_reload(client);
        lk_load_client_dll(client, errors);
#endif

        lk_load_client_from_library(client);
    }

    lk_fill_stubs(client);

    client->functions.dll_load(platform);
}

static void lk_unload_client(LK_Platform* platform, LK_Client* client)
{
    client->functions.dll_unload(platform);

    if (client->library)
    {
        FreeLibrary(client->library);
        client->library = 0;
    }

#ifndef LK_PLATFORM_NO_DLL
    if (client->from_dll)
    {
        if (client->dll_path)
        {
            LocalFree(client->dll_path);
        }
        if (client->temp_dll_path)
        {
            DeleteFileW(client->temp_dll_path);
            LocalFree(client->temp_dll_path);
        }
    }
#endif
}


static void lk_update_client(LK_Platform* platform, LK_Client* client, LK_Circular_Buffer* errors)
{
#ifndef LK_PLATFORM_NO_DLL
    if (!client->from_dll) return;
    if (!client->dll_path) return;
    if (!client->temp_dll_path) return;

    if (lk_check_client_reload(client))
    {
        // acquire exclusive lock
        InterlockedDecrement(&client->reload_lock);
        while (client->reload_lock >= 0)
            SwitchToThread();

        // reload
        client->functions.dll_unload(platform);
        if (client->library) FreeLibrary(client->library);
        DeleteFileW(client->temp_dll_path);

        lk_load_client_dll(client, errors);
        lk_load_client_from_library(client);
        lk_fill_stubs(client);
        client->functions.dll_load(platform);

        // release lock
        InterlockedIncrement(&client->reload_lock);
    }

#endif
}


////////////////////////////////////////////////////////////////////////////////
// Window thread
////////////////////////////////////////////////////////////////////////////////


typedef enum
{
    LK_MDT_EFFECTIVE_DPI,
    LK_MDT_ANGULAR_DPI,
    LK_MDT_RAW_DPI,
    LK_MDT_DEFAULT
} LK_MONITOR_DPI_TYPE;

typedef enum
{
    LK_PROCESS_DPI_UNAWARE,
    LK_PROCESS_SYSTEM_DPI_AWARE,
    LK_PROCESS_PER_MONITOR_DPI_AWARE
} LK_PROCESS_DPI_AWARENESS;

#ifndef DPI_AWARENESS_CONTEXT_UNAWARE
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#define DPI_AWARENESS_CONTEXT_UNAWARE              ((DPI_AWARENESS_CONTEXT) - 1)
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE         ((DPI_AWARENESS_CONTEXT) - 2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT) - 3)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT) - 4)
#define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT) - 5)
#endif


enum
{
    LK_RAW_INPUT_CHECK_TIMER_ID       = 1,
    LK_DEVICES_CHANGED_QUICK_TIMER_ID = 2,
    LK_DEVICES_CHANGED_SLOW_TIMER_ID  = 3,
};

typedef struct
{
    HANDLE thread;
    HANDLE opened;
    HANDLE close;
    HANDLE unpause;

    // init input
    LK_Client* client;

    LK_U32 instance_serial;
    LK_B32 disable_animations;
    LK_B32 transparent;
    LK_B32 disable_windows_keys;

    LK_U32 icon_width;
    LK_U32 icon_height;
    LK_U8* icon_pixels;

    // init output
    WCHAR class_name[128];
    HWND  hwnd;
    HDC   dc;

    HDEVNOTIFY device_notification;

    // win32 reads, client writes

    LK_B32 prevent_screen_saver;
    LK_B32 no_resize_on_dpi_change;

    // win32 writes, client reads

    LK_Circular_Buffer errors;

    LK_B32 wants_to_close;
    LK_B32 focus;
    LK_U32 dpi;
    LK_U32 monitor_refresh_rate;
    LK_U32 device_generation;  // incremented when devices are added or removed (actually incremented twice, see :DevicesChangedTimers)

    LK_B8  mouse_down[3];
    LK_S32 mouse_wheel;
    POINT  mouse_raw_cursor;

    LK_B8 keys_down[LK__KEY_COUNT];
    LK_B8 keys_repeated[LK__KEY_COUNT + 4];  // +4 is safety padding because we're doing 32-bit exchanges on these

    LK_Circular_Buffer text;

    // internal

    HCURSOR arrow_cursor;
    LK_B32 has_raw_mouse_input;
    LK_B32 has_raw_keyboard_input;

    LK_B32 client_has_win32_handler;

    HMODULE user32;
    BOOL(WINAPI* SetProcessDPIAware)();
    BOOL(WINAPI *IsProcessDPIAware)();
    UINT(WINAPI *GetDpiForWindow)(HWND hwnd);
    HRESULT(WINAPI *SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);

    HMODULE shcore;
    HRESULT(WINAPI *GetDpiForMonitor)(HMONITOR hmonitor, LK_MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
    HRESULT(WINAPI *SetProcessDpiAwareness)(LK_PROCESS_DPI_AWARENESS value);
} LK_Window_Context;


static LRESULT CALLBACK
lk_window_callback(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_NCCREATE)
    {
        HMODULE user32 = LoadLibraryA("user32.dll");
        if (user32)
        {
            BOOL(WINAPI* EnableNonClientDpiScaling)(HWND hwnd);
            LK_GetProc(user32, EnableNonClientDpiScaling, "EnableNonClientDpiScaling");
            if (EnableNonClientDpiScaling)
            {
                EnableNonClientDpiScaling(hwnd);
            }
            FreeLibrary(user32);
        }
    }


    LK_Window_Context* window;
    if (message == WM_CREATE)
    {
        CREATESTRUCT* create = (CREATESTRUCT*) lparam;
        window = (LK_Window_Context*) create->lpCreateParams;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR) window);
    }
    else
    {
        window = (LK_Window_Context*) GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    }

    if (!window)
    {
        return DefWindowProcW(hwnd, message, wparam, lparam);
    }

    SetEvent(window->unpause);

#if 0
    {
        int printf ( const char * format, ... );
        static struct { UINT code; const char* text; } message_list[] = { { 0, "WM_NULL"}, { 1, "WM_CREATE" }, { 2, "WM_DESTROY" }, { 3, "WM_MOVE" }, { 5, "WM_SIZE" }, { 6, "WM_ACTIVATE" }, { 7, "WM_SETFOCUS" }, { 8, "WM_KILLFOCUS" }, { 10, "WM_ENABLE" }, { 11, "WM_SETREDRAW" }, { 12, "WM_SETTEXT" }, { 13, "WM_GETTEXT" }, { 14, "WM_GETTEXTLENGTH" }, { 15, "WM_PAINT" }, { 16, "WM_CLOSE" }, { 17, "WM_QUERYENDSESSION" }, { 18, "WM_QUIT" }, { 19, "WM_QUERYOPEN" }, { 20, "WM_ERASEBKGND" }, { 21, "WM_SYSCOLORCHANGE" }, { 22, "WM_ENDSESSION" }, { 24, "WM_SHOWWINDOW" }, { 25, "WM_CTLCOLOR" }, { 26, "WM_WININICHANGE" }, { 27, "WM_DEVMODECHANGE" }, { 28, "WM_ACTIVATEAPP" }, { 29, "WM_FONTCHANGE" }, { 30, "WM_TIMECHANGE" }, { 31, "WM_CANCELMODE" }, { 32, "WM_SETCURSOR" }, { 33, "WM_MOUSEACTIVATE" }, { 34, "WM_CHILDACTIVATE" }, { 35, "WM_QUEUESYNC" }, { 36, "WM_GETMINMAXINFO" }, { 38, "WM_PAINTICON" }, { 39, "WM_ICONERASEBKGND" }, { 40, "WM_NEXTDLGCTL" }, { 42, "WM_SPOOLERSTATUS" }, { 43, "WM_DRAWITEM" }, { 44, "WM_MEASUREITEM" }, { 45, "WM_DELETEITEM" }, { 46, "WM_VKEYTOITEM" }, { 47, "WM_CHARTOITEM" }, { 48, "WM_SETFONT" }, { 49, "WM_GETFONT" }, { 50, "WM_SETHOTKEY" }, { 51, "WM_GETHOTKEY" }, { 55, "WM_QUERYDRAGICON" }, { 57, "WM_COMPAREITEM" }, { 61, "WM_GETOBJECT" }, { 65, "WM_COMPACTING" }, { 68, "WM_COMMNOTIFY" }, { 70, "WM_WINDOWPOSCHANGING" }, { 71, "WM_WINDOWPOSCHANGED" }, { 72, "WM_POWER" }, { 73, "WM_COPYGLOBALDATA" }, { 74, "WM_COPYDATA" }, { 75, "WM_CANCELJOURNAL" }, { 78, "WM_NOTIFY" }, { 80, "WM_INPUTLANGCHANGEREQUEST" }, { 81, "WM_INPUTLANGCHANGE" }, { 82, "WM_TCARD" }, { 83, "WM_HELP" }, { 84, "WM_USERCHANGED" }, { 85, "WM_NOTIFYFORMAT" }, { 123, "WM_CONTEXTMENU" }, { 124, "WM_STYLECHANGING" }, { 125, "WM_STYLECHANGED" }, { 126, "WM_DISPLAYCHANGE" }, { 127, "WM_GETICON" }, { 128, "WM_SETICON" }, { 129, "WM_NCCREATE" }, { 130, "WM_NCDESTROY" }, { 131, "WM_NCCALCSIZE" }, { 132, "WM_NCHITTEST" }, { 133, "WM_NCPAINT" }, { 134, "WM_NCACTIVATE" }, { 135, "WM_GETDLGCODE" }, { 136, "WM_SYNCPAINT" }, { 160, "WM_NCMOUSEMOVE" }, { 161, "WM_NCLBUTTONDOWN" }, { 162, "WM_NCLBUTTONUP" }, { 163, "WM_NCLBUTTONDBLCLK" }, { 164, "WM_NCRBUTTONDOWN" }, { 165, "WM_NCRBUTTONUP" }, { 166, "WM_NCRBUTTONDBLCLK" }, { 167, "WM_NCMBUTTONDOWN" }, { 168, "WM_NCMBUTTONUP" }, { 169, "WM_NCMBUTTONDBLCLK" }, { 171, "WM_NCXBUTTONDOWN" }, { 172, "WM_NCXBUTTONUP" }, { 173, "WM_NCXBUTTONDBLCLK" }, { 176, "EM_GETSEL" }, { 177, "EM_SETSEL" }, { 178, "EM_GETRECT" }, { 179, "EM_SETRECT" }, { 180, "EM_SETRECTNP" }, { 181, "EM_SCROLL" }, { 182, "EM_LINESCROLL" }, { 183, "EM_SCROLLCARET" }, { 185, "EM_GETMODIFY" }, { 187, "EM_SETMODIFY" }, { 188, "EM_GETLINECOUNT" }, { 189, "EM_LINEINDEX" }, { 190, "EM_SETHANDLE" }, { 191, "EM_GETHANDLE" }, { 192, "EM_GETTHUMB" }, { 193, "EM_LINELENGTH" }, { 194, "EM_REPLACESEL" }, { 195, "EM_SETFONT" }, { 196, "EM_GETLINE" }, { 197, "EM_LIMITTEXT" }, { 197, "EM_SETLIMITTEXT" }, { 198, "EM_CANUNDO" }, { 199, "EM_UNDO" }, { 200, "EM_FMTLINES" }, { 201, "EM_LINEFROMCHAR" }, { 202, "EM_SETWORDBREAK" }, { 203, "EM_SETTABSTOPS" }, { 204, "EM_SETPASSWORDCHAR" }, { 205, "EM_EMPTYUNDOBUFFER" }, { 206, "EM_GETFIRSTVISIBLELINE" }, { 207, "EM_SETREADONLY" }, { 209, "EM_SETWORDBREAKPROC" }, { 209, "EM_GETWORDBREAKPROC" }, { 210, "EM_GETPASSWORDCHAR" }, { 211, "EM_SETMARGINS" }, { 212, "EM_GETMARGINS" }, { 213, "EM_GETLIMITTEXT" }, { 214, "EM_POSFROMCHAR" }, { 215, "EM_CHARFROMPOS" }, { 216, "EM_SETIMESTATUS" }, { 217, "EM_GETIMESTATUS" }, { 224, "SBM_SETPOS" }, { 225, "SBM_GETPOS" }, { 226, "SBM_SETRANGE" }, { 227, "SBM_GETRANGE" }, { 228, "SBM_ENABLE_ARROWS" }, { 230, "SBM_SETRANGEREDRAW" }, { 233, "SBM_SETSCROLLINFO" }, { 234, "SBM_GETSCROLLINFO" }, { 235, "SBM_GETSCROLLBARINFO" }, { 240, "BM_GETCHECK" }, { 241, "BM_SETCHECK" }, { 242, "BM_GETSTATE" }, { 243, "BM_SETSTATE" }, { 244, "BM_SETSTYLE" }, { 245, "BM_CLICK" }, { 246, "BM_GETIMAGE" }, { 247, "BM_SETIMAGE" }, { 248, "BM_SETDONTCLICK" }, { 255, "WM_INPUT" }, { 256, "WM_KEYDOWN" }, { 256, "WM_KEYFIRST" }, { 257, "WM_KEYUP" }, { 258, "WM_CHAR" }, { 259, "WM_DEADCHAR" }, { 260, "WM_SYSKEYDOWN" }, { 261, "WM_SYSKEYUP" }, { 262, "WM_SYSCHAR" }, { 263, "WM_SYSDEADCHAR" }, { 264, "WM_KEYLAST" }, { 265, "WM_UNICHAR" }, { 265, "WM_WNT_CONVERTREQUESTEX" }, { 266, "WM_CONVERTREQUEST" }, { 267, "WM_CONVERTRESULT" }, { 268, "WM_INTERIM" }, { 269, "WM_IME_STARTCOMPOSITION" }, { 270, "WM_IME_ENDCOMPOSITION" }, { 271, "WM_IME_COMPOSITION" }, { 271, "WM_IME_KEYLAST" }, { 272, "WM_INITDIALOG" }, { 273, "WM_COMMAND" }, { 274, "WM_SYSCOMMAND" }, { 275, "WM_TIMER" }, { 276, "WM_HSCROLL" }, { 277, "WM_VSCROLL" }, { 278, "WM_INITMENU" }, { 279, "WM_INITMENUPOPUP" }, { 280, "WM_SYSTIMER" }, { 287, "WM_MENUSELECT" }, { 288, "WM_MENUCHAR" }, { 289, "WM_ENTERIDLE" }, { 290, "WM_MENURBUTTONUP" }, { 291, "WM_MENUDRAG" }, { 292, "WM_MENUGETOBJECT" }, { 293, "WM_UNINITMENUPOPUP" }, { 294, "WM_MENUCOMMAND" }, { 295, "WM_CHANGEUISTATE" }, { 296, "WM_UPDATEUISTATE" }, { 297, "WM_QUERYUISTATE" }, { 306, "WM_CTLCOLORMSGBOX" }, { 307, "WM_CTLCOLOREDIT" }, { 308, "WM_CTLCOLORLISTBOX" }, { 309, "WM_CTLCOLORBTN" }, { 310, "WM_CTLCOLORDLG" }, { 311, "WM_CTLCOLORSCROLLBAR" }, { 312, "WM_CTLCOLORSTATIC" }, { 512, "WM_MOUSEFIRST" }, { 512, "WM_MOUSEMOVE" }, { 513, "WM_LBUTTONDOWN" }, { 514, "WM_LBUTTONUP" }, { 515, "WM_LBUTTONDBLCLK" }, { 516, "WM_RBUTTONDOWN" }, { 517, "WM_RBUTTONUP" }, { 518, "WM_RBUTTONDBLCLK" }, { 519, "WM_MBUTTONDOWN" }, { 520, "WM_MBUTTONUP" }, { 521, "WM_MBUTTONDBLCLK" }, { 521, "WM_MOUSELAST" }, { 522, "WM_MOUSEWHEEL" }, { 523, "WM_XBUTTONDOWN" }, { 524, "WM_XBUTTONUP" }, { 525, "WM_XBUTTONDBLCLK" }, { 528, "WM_PARENTNOTIFY" }, { 529, "WM_ENTERMENULOOP" }, { 530, "WM_EXITMENULOOP" }, { 531, "WM_NEXTMENU" }, { 532, "WM_SIZING" }, { 533, "WM_CAPTURECHANGED" }, { 534, "WM_MOVING" }, { 536, "WM_POWERBROADCAST" }, { 537, "WM_DEVICECHANGE" }, { 544, "WM_MDICREATE" }, { 545, "WM_MDIDESTROY" }, { 546, "WM_MDIACTIVATE" }, { 547, "WM_MDIRESTORE" }, { 548, "WM_MDINEXT" }, { 549, "WM_MDIMAXIMIZE" }, { 550, "WM_MDITILE" }, { 551, "WM_MDICASCADE" }, { 552, "WM_MDIICONARRANGE" }, { 553, "WM_MDIGETACTIVE" }, { 560, "WM_MDISETMENU" }, { 561, "WM_ENTERSIZEMOVE" }, { 562, "WM_EXITSIZEMOVE" }, { 563, "WM_DROPFILES" }, { 564, "WM_MDIREFRESHMENU" }, { 640, "WM_IME_REPORT" }, { 641, "WM_IME_SETCONTEXT" }, { 642, "WM_IME_NOTIFY" }, { 643, "WM_IME_CONTROL" }, { 644, "WM_IME_COMPOSITIONFULL" }, { 645, "WM_IME_SELECT" }, { 646, "WM_IME_CHAR" }, { 648, "WM_IME_REQUEST" }, { 656, "WM_IMEKEYDOWN" }, { 656, "WM_IME_KEYDOWN" }, { 657, "WM_IMEKEYUP" }, { 657, "WM_IME_KEYUP" }, { 672, "WM_NCMOUSEHOVER" }, { 673, "WM_MOUSEHOVER" }, { 674, "WM_NCMOUSELEAVE" }, { 675, "WM_MOUSELEAVE" }, { 768, "WM_CUT" }, { 769, "WM_COPY" }, { 770, "WM_PASTE" }, { 771, "WM_CLEAR" }, { 772, "WM_UNDO" }, { 773, "WM_RENDERFORMAT" }, { 774, "WM_RENDERALLFORMATS" }, { 775, "WM_DESTROYCLIPBOARD" }, { 776, "WM_DRAWCLIPBOARD" }, { 777, "WM_PAINTCLIPBOARD" }, { 778, "WM_VSCROLLCLIPBOARD" }, { 779, "WM_SIZECLIPBOARD" }, { 780, "WM_ASKCBFORMATNAME" }, { 781, "WM_CHANGECBCHAIN" }, { 782, "WM_HSCROLLCLIPBOARD" }, { 783, "WM_QUERYNEWPALETTE" }, { 784, "WM_PALETTEISCHANGING" }, { 785, "WM_PALETTECHANGED" }, { 786, "WM_HOTKEY" }, { 791, "WM_PRINT" }, { 792, "WM_PRINTCLIENT" }, { 793, "WM_APPCOMMAND" }, { 856, "WM_HANDHELDFIRST" }, { 863, "WM_HANDHELDLAST" }, { 864, "WM_AFXFIRST" }, { 895, "WM_AFXLAST" }, { 896, "WM_PENWINFIRST" }, { 897, "WM_RCRESULT" }, { 898, "WM_HOOKRCRESULT" }, { 899, "WM_GLOBALRCCHANGE" }, { 899, "WM_PENMISCINFO" }, { 900, "WM_SKB" }, { 901, "WM_HEDITCTL" }, { 901, "WM_PENCTL" }, { 902, "WM_PENMISC" }, { 903, "WM_CTLINIT" }, { 904, "WM_PENEVENT" }, { 911, "WM_PENWINLAST" }, { 1024, "WM_USER" } };
        static volatile long lock;
        while (_InterlockedCompareExchange(&lock, 1, 0) != 0) Sleep(0);
        const char* message_name = "???";
        for (auto m : message_list)
            if (m.code == message)
                message_name = m.text;
        printf("%p: HWND %p, MSG %08x, %016llx, %016llx (%d: %s)\n", lk_window_callback, hwnd, message, wparam, lparam, lk_platform->window.x, message_name);
        lock = 0;
    }
#endif

    if (window->client_has_win32_handler)
    {
        LK_Client* client = window->client;
        lk_lock_client(client);
        if (client->functions.win32_event_handler == lk_client_win32_event_handler_stub)
        {
            window->client_has_win32_handler = 0;
        }
        else
        {
            void* user_return;
            if (client->functions.win32_event_handler(client->platform, hwnd, message, (void*) wparam, (void*) lparam, &user_return))
                return (LPARAM) user_return;
        }
        lk_unlock_client(client);
    }

    if (hwnd != window->hwnd)
    {
        return DefWindowProcW(hwnd, message, wparam, lparam);
    }


    switch (message)
    {


    case WM_ACTIVATE:
    {
        window->monitor_refresh_rate = lk_get_monitor_refresh_rate(window->hwnd, window->dc);
        if (LOWORD(wparam) == WA_ACTIVE || LOWORD(wparam) == WA_CLICKACTIVE)
            goto set_focus;
        else
            goto kill_focus;
    } break;

    case WM_DISPLAYCHANGE:
    {
        window->monitor_refresh_rate = lk_get_monitor_refresh_rate(window->hwnd, window->dc);
    } break;

    set_focus:
    case WM_SETFOCUS:
    {
        window->focus = 1;
    } break;

    kill_focus:
    case WM_KILLFOCUS:
    {
        ZeroMemory(&window->mouse_down,    sizeof(window->mouse_down));
        ZeroMemory(&window->keys_down,     sizeof(window->keys_down));
        ZeroMemory(&window->keys_repeated, sizeof(window->keys_repeated));
        window->focus = 0;
    } break;


    case WM_CLOSE:
    {
        window->wants_to_close = 1;
        return 0;
    } break;

    case 0x02E0:  // WM_DPICHANGED
    {
        window->dpi = (LK_U32) LOWORD(wparam);
        if (!window->no_resize_on_dpi_change)
        {
            RECT* new_rectangle = (RECT*) lparam;
            int x      = new_rectangle->left;
            int y      = new_rectangle->top;
            int width  = new_rectangle->right - x;
            int height = new_rectangle->bottom - y;
            SetWindowPos(hwnd, HWND_TOP, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
            RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
        }

        return 0;
    } break;


    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    {
        if (!window->has_raw_mouse_input)
            window->mouse_down[0] = (message == WM_LBUTTONDOWN);
        return 0;
    } break;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
        if (!window->has_raw_mouse_input)
            window->mouse_down[1] = (message == WM_LBUTTONDOWN);
        return 0;
    } break;

    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    {
        if (!window->has_raw_mouse_input)
            window->mouse_down[2] = (message == WM_LBUTTONDOWN);
        return 0;
    } break;

    case WM_MOUSEWHEEL:
    {
        if (!window->has_raw_mouse_input)
            window->mouse_wheel += GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
        return 0;
    } break;



    case WM_KEYUP:
    case WM_KEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    {
        USHORT virtual_key = (USHORT) wparam;
        if (message == WM_SYSKEYDOWN && virtual_key == VK_F4)
            PostMessage(hwnd, WM_CLOSE, 0, 0);

        if (!window->has_raw_keyboard_input)
        {
            USHORT scan_code = (USHORT)((lparam >> 16) & 0xFF);
            LK_B32 e0 = (lparam >> 24) & 1;
            LK_B32 e1 = 0;
            LK_Key key = lk_translate_key(virtual_key, scan_code, e0, e1);
            if (key)
            {
                LK_B8 is_down = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
                window->keys_down[key] = is_down;
                window->keys_repeated[key] |= is_down;
            }
        }

        return 0;
    } break;


    case WM_CHAR:
    {
        // @Incomplete - Currently doesn't support surrogate pairs.

        WCHAR utf16 = (WCHAR) wparam;
        char utf8[16];

        LK_U32 length = (LK_U32) WideCharToMultiByte(CP_UTF8, 0, &utf16, 1, utf8, sizeof(utf8), 0, 0);
        lk_write_to_circular_buffer(&window->text, utf8, length);

        return 0;
    } break;


    case WM_DEVICECHANGE:
    {
        if (wparam == DBT_DEVICEARRIVAL || wparam == DBT_DEVICEREMOVECOMPLETE)
        {
            // :DevicesChangedTimers
            // Other APIs are also looking at these events, so we give them time to update their state.
            // That's why there are timers; one after 100 ms, and one after 2 seconds (in case some API is slow).
            SetTimer(hwnd, LK_DEVICES_CHANGED_QUICK_TIMER_ID, 100, NULL);
            SetTimer(hwnd, LK_DEVICES_CHANGED_SLOW_TIMER_ID, 2000, NULL);
        }
    } break;

    case WM_TIMER:
    {
        UINT_PTR timer_id = (UINT_PTR) wparam;

        // :RawInputCheckTimer
        // Every 100 milliseconds, check if this window is still receiving RawInput.
        // If some other window in this same application requests raw input, this window will lose it.
        if (timer_id == LK_RAW_INPUT_CHECK_TIMER_ID)
        {
            window->has_raw_mouse_input    = 0;
            window->has_raw_keyboard_input = 0;

            RAWINPUTDEVICE* devices = NULL;
            UINT device_count = 4;
            while (1)
            {
                devices = (RAWINPUTDEVICE*) LocalAlloc(LMEM_FIXED, sizeof(RAWINPUTDEVICE) * device_count);
                if (!devices)
                {
                    device_count = 0;
                    break;
                }

                UINT result = GetRegisteredRawInputDevices(devices, &device_count, sizeof(RAWINPUTDEVICE));
                if ((LK_S32) result >= 0)
                {
                    device_count = result;
                    break;
                }

                LocalFree(devices);
                if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                {
                    device_count = 0;
                    break;
                }
            }

            for (UINT device_index = 0; device_index < device_count; device_index++)
            {
                RAWINPUTDEVICE* device = devices + device_index;
                if (device->hwndTarget  != hwnd) continue;
                if (device->usUsagePage != 0x01) continue;
                if (device->usUsage == 0x02) window->has_raw_mouse_input = 1;
                if (device->usUsage == 0x06) window->has_raw_keyboard_input = 1;
            }

            if (devices)
                LocalFree(devices);

            if (window->has_raw_mouse_input || window->has_raw_keyboard_input)
                SetTimer(hwnd, LK_RAW_INPUT_CHECK_TIMER_ID, 100, NULL);
        }

        // :DevicesChangedTimers
        if (timer_id == LK_DEVICES_CHANGED_QUICK_TIMER_ID || timer_id == LK_DEVICES_CHANGED_SLOW_TIMER_ID)
            window->device_generation++;
    } break;

    case WM_INPUT:
    {
        if (!window->focus)
            break;  // default proc

        UINT struct_size;
        GetRawInputData((HRAWINPUT) lparam, RID_INPUT, 0, &struct_size, sizeof(RAWINPUTHEADER));

        // @Reconsider - I never seem to get any input structures larger than 48 bytes, so 256 bytes should be fine.
        // I don't want to allocate on the heap for input, and I don't want to use alloca because that requires the CRT.
        // We could do some assembly though... But it doesn't seem to be necessary.
        char struct_memory[256];
        if (struct_size > sizeof(struct_memory))
            break;  // default proc

        RAWINPUT* input = (RAWINPUT*) struct_memory;
        GetRawInputData((HRAWINPUT) lparam, RID_INPUT, input, &struct_size, sizeof(RAWINPUTHEADER));

        if (input->header.dwType == RIM_TYPEMOUSE && input->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
        {
            window->mouse_raw_cursor.x += input->data.mouse.lLastX;
            window->mouse_raw_cursor.y += input->data.mouse.lLastY;

            USHORT button_flags = input->data.mouse.usButtonFlags;
            if (button_flags & (RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_LEFT_BUTTON_UP))
                window->mouse_down[0] = !!(button_flags & RI_MOUSE_LEFT_BUTTON_DOWN);
            if (button_flags & (RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_UP))
                window->mouse_down[1] = !!(button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN);
            if (button_flags & (RI_MOUSE_MIDDLE_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_UP))
                window->mouse_down[2] = !!(button_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN);

            if (button_flags & RI_MOUSE_WHEEL)
                window->mouse_wheel += ((SHORT) input->data.mouse.usButtonData) / WHEEL_DELTA;
        }

        if (input->header.dwType == RIM_TYPEKEYBOARD)
        {
            USHORT virtual_key = input->data.keyboard.VKey;
            USHORT scan_code   = input->data.keyboard.MakeCode;
            USHORT flags       = input->data.keyboard.Flags;
            int e0             = (flags & RI_KEY_E0) != 0;
            int e1             = (flags & RI_KEY_E1) != 0;

            LK_Key key = lk_translate_key(virtual_key, scan_code, e0, e1);
            if (key)
            {
                LK_B8 is_down = (flags & RI_KEY_BREAK) == 0;
                window->keys_down[key] = is_down;
                window->keys_repeated[key] |= is_down;
            }
        }
    } break;  // default proc

    case WM_PAINT:
    {
        PAINTSTRUCT paint;
        BeginPaint(hwnd, &paint);
        EndPaint(hwnd, &paint);
    } break;  // default proc

    case WM_SETCURSOR:
    {
        if (LOWORD(lparam) == HTCLIENT)
        {
            SetCursor(window->arrow_cursor);
            return 1;
        }
    } break;  // default proc

    case WM_SYSCOMMAND:
    {
        WPARAM command = (wparam & 0xFFF0);
        if (command == SC_KEYMENU)
            return 0;

        if (command == SC_SCREENSAVE || command == SC_MONITORPOWER)
            if (window->prevent_screen_saver)
                return 0;
    } break;  // default proc

    }

    return DefWindowProcW(hwnd, message, wparam, lparam);
}


static void lk_set_window_icon(LK_Window_Context* window)
{
    if (!window->dc)
    {
        LK_Log(&window->errors, "Failed to set the window icon; no DC.");
        return;
    }

    LK_U32 width  = window->icon_width;
    LK_U32 height = window->icon_height;

    BITMAPV5HEADER bitmap_header = { 0 };
    bitmap_header.bV5Size        = sizeof(BITMAPV5HEADER);
    bitmap_header.bV5Width       = width;
    bitmap_header.bV5Height      = height;
    bitmap_header.bV5Planes      = 1;
    bitmap_header.bV5BitCount    = 32;
    bitmap_header.bV5Compression = BI_BITFIELDS;
    bitmap_header.bV5RedMask     = 0x00FF0000;
    bitmap_header.bV5GreenMask   = 0x0000FF00;
    bitmap_header.bV5BlueMask    = 0x000000FF;
    bitmap_header.bV5AlphaMask   = 0xFF000000;

    void* bitmap_data;
    HBITMAP bitmap = CreateDIBSection(window->dc, (BITMAPINFO*) &bitmap_header, DIB_RGB_COLORS, &bitmap_data, 0, 0);
    if (bitmap)
    {
        for (LK_U32 y = 0; y < height; y++)
        {
            LK_U32* read  = (LK_U32*) window->icon_pixels + y * width;
            LK_U32* write = (LK_U32*) bitmap_data + (height - y - 1) * width;

            for (LK_U32 x = 0; x < width; x++)
            {
                LK_U32 value = *(read++);
                value = (value & 0xFF00FF00)
                     | ((value & 0x00FF0000) >> 16)
                     | ((value & 0x000000FF) << 16);
                *(write++) = value;
            }
        }

        HBITMAP mono_bitmap = CreateBitmap(width, height, 1, 1, 0);
        if (mono_bitmap)
        {
            ICONINFO info;
            info.fIcon = TRUE;
            info.xHotspot = 0;
            info.yHotspot = 0;
            info.hbmMask = mono_bitmap;
            info.hbmColor = bitmap;

            HICON icon = CreateIconIndirect(&info);
            if (icon)
            {
                SendMessage(window->hwnd, WM_SETICON, ICON_SMALL, (LPARAM) icon);
                SendMessage(window->hwnd, WM_SETICON, ICON_BIG,   (LPARAM) icon);
            }
            else
            {
                LK_Log(&window->errors, "Failed to set the window icon; couldn't create the icon.");
                return;
            }

            DeleteObject(mono_bitmap);
        }
        else
        {
            LK_Log(&window->errors, "Failed to set the window icon; couldn't create the bitmap.");
            return;
        }

        DeleteObject(bitmap);
    }
    else
    {
        LK_Log(&window->errors, "Failed to set the window icon; couldn't create the DIB section.");
    }
}


static LK_U32 lk_get_dpi_for_window(LK_Window_Context* window)
{
    // Try best solution for Windows 10 Anniversary Update (1607) or later.
    if (window->GetDpiForWindow)
    {
        LK_U32 result = window->GetDpiForWindow(window->hwnd);
        if (result)
            return result;
    }

    // Try best solution for Windows 8.1 or later.
    if (window->GetDpiForMonitor)
    {
        HMONITOR monitor = MonitorFromWindow(window->hwnd, MONITOR_DEFAULTTONEAREST);
        if (monitor)
        {
            UINT dpi_x, dpi_y;
            HRESULT status = window->GetDpiForMonitor(monitor, LK_MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
            if (status == S_OK)
                return dpi_x;
        }
    }

    // Try best solution for Windows Vista or later.
    if (window->IsProcessDPIAware)
        if (window->IsProcessDPIAware())
            if (window->dc)
                return GetDeviceCaps(window->dc, LOGPIXELSX);

    // Oh well...
    return 96;
}

static void lk_try_set_dpi_awareness(LK_Window_Context* window)
{
    // Try best solution for Windows 10 ...
    if (window->SetProcessDpiAwarenessContext)
    {
        // ... Creators Update (1703) or later.
        if (window->SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
            return;

        // ... Anniversary Update (1607) or later.
        if (window->SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
            return;
    }

    // Try best solution for Windows 8.1 or later.
    if (window->SetProcessDpiAwareness)
        if (window->SetProcessDpiAwareness(LK_PROCESS_PER_MONITOR_DPI_AWARE) == S_OK)
            return;

    // Try best solution for Windows Vista or later.
    if (window->SetProcessDPIAware)
        if (window->SetProcessDPIAware())
            return;

    // Oh well...
}


static LK_B32 lk_open_window(LK_Window_Context* window)
{
    HINSTANCE instance = GetModuleHandle(0);

    // register window class
    wsprintfW(window->class_name, L"lk_platform_class_%u", window->instance_serial);

    WNDCLASSEXW window_class = { 0 };
    window_class.cbSize = sizeof(WNDCLASSEXW);
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.hbrBackground = CreateSolidBrush(0);
    window_class.lpfnWndProc = lk_window_callback;
    window_class.hInstance = instance;
    window_class.lpszClassName = window->class_name;
    if (!RegisterClassExW(&window_class))
    {
        LK_Log(&window->errors, "Failed to open a window; RegisterClassEx() failed.");
        return 0;
    }

    // create the window
    HWND hwnd = CreateWindowExW(0, window->class_name, L"", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, window);
    if (!hwnd)
    {
        LK_Log(&window->errors, "Failed to open a window; CreateWindowEx() failed.");
        return 0;
    }

    HMODULE dwmapi = LoadLibraryA("dwmapi.dll");
    if (dwmapi)
    {
        // maybe disable animations
        if (window->disable_animations)
        {
            HRESULT(*DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
            LK_GetProc(dwmapi, DwmSetWindowAttribute, "DwmSetWindowAttribute");
            if (DwmSetWindowAttribute)
            {
                DWORD attribute = 2; // DWMWA_TRANSITIONS_FORCEDISABLED
                BOOL disable = TRUE;
                DwmSetWindowAttribute(hwnd, attribute, &disable, sizeof(disable));
            }
        }

        // maybe enable transparency
        if (window->transparent)
        {
            typedef struct
            {
                DWORD dwFlags;
                BOOL  fEnable;
                HRGN  hRgnBlur;
                BOOL  fTransitionOnMaximized;
            } DWM_BLURBEHIND;

            typedef struct
            {
                int cxLeftWidth;
                int cxRightWidth;
                int cyTopHeight;
                int cyBottomHeight;
            } MARGINS;

            HRESULT(*DwmEnableBlurBehindWindow)(HWND, DWM_BLURBEHIND*);
            HRESULT(*DwmExtendFrameIntoClientArea)(HWND, MARGINS*);
            LK_GetProc(dwmapi, DwmEnableBlurBehindWindow,    "DwmEnableBlurBehindWindow");
            LK_GetProc(dwmapi, DwmExtendFrameIntoClientArea, "DwmExtendFrameIntoClientArea");

            if (DwmEnableBlurBehindWindow && DwmExtendFrameIntoClientArea)
            {
                DWM_BLURBEHIND blur = { 0 };
                blur.dwFlags = 1; // DWM_BB_ENABLE
                blur.fEnable = TRUE;
                DwmEnableBlurBehindWindow(hwnd, &blur);

                MARGINS margins = { 0 };
                margins.cxLeftWidth = -1;
                DwmExtendFrameIntoClientArea(hwnd, &margins);
            }
            else
            {
                LK_Log(&window->errors, "Failed to enable window transparency; missing DwmApi functions.");
            }
        }

        FreeLibrary(dwmapi);
    }

    // get DC
    HDC dc = GetDC(hwnd);
    window->hwnd = hwnd;
    window->dc   = dc;
    if (!dc)
    {
        LK_Log(&window->errors, "Failed to open a window; GetDC() failed.");
    }

    // set window icon
    if (window->icon_pixels)
    {
        lk_set_window_icon(window);
    }

    window->dpi = lk_get_dpi_for_window(window);
    window->monitor_refresh_rate = lk_get_monitor_refresh_rate(hwnd, dc);

    // register raw input devices
    {
        UINT device_count = 0;
        RAWINPUTDEVICE devices[2];
        ZeroMemory(devices, sizeof(devices));

        // Mouse
        UINT mouse_device = device_count++;
        devices[mouse_device].usUsagePage = 0x01;
        devices[mouse_device].usUsage = 0x02;
        devices[mouse_device].hwndTarget = hwnd;
        // Keyboard
        UINT keyboard_device = device_count++;
        devices[keyboard_device].usUsagePage = 0x01;
        devices[keyboard_device].usUsage = 0x06;
        if (window->disable_windows_keys)
        {
            // What?? Why does this work? The documentation explicitly states this only affects application-defined hotkeys.
            // Alt-tab still works, but displays the classic alt-tab menu instead of the modern one with thumbnails.
            // ??? I guess windows keys are application-defined hotkeys?
            devices[keyboard_device].dwFlags = RIDEV_NOHOTKEYS;
        }
        devices[keyboard_device].hwndTarget = hwnd;

        if (RegisterRawInputDevices(devices, device_count, sizeof(RAWINPUTDEVICE)))
        {
            window->has_raw_mouse_input    = 1;
            window->has_raw_keyboard_input = 1;
        }
        else
        {
            LK_Log(&window->errors, "Failed to register input devices; the window won't receive input.");
        }

        SetTimer(hwnd, LK_RAW_INPUT_CHECK_TIMER_ID, 0, NULL);  // :RawInputCheckTimer
    }

    // register device notification
    {
        const GUID GUID_DEVINTERFACE_HID = { 0x4D1E55B2L, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

        DEV_BROADCAST_DEVICEINTERFACE filter = { 0 };
        filter.dbcc_size       = sizeof(filter);
        filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        filter.dbcc_classguid  = GUID_DEVINTERFACE_HID;

        window->device_notification = RegisterDeviceNotification(hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!window->device_notification)
        {
            LK_Log(&window->errors, "Failed to register a device notification; the window won't receive device change messages.");
        }
    }

    return 1;
}

static void lk_close_window(LK_Window_Context* window)
{
    if (window->device_notification)
        UnregisterDeviceNotification(window->device_notification);

    HINSTANCE instance = GetModuleHandle(0);
    HWND hwnd = window->hwnd;
    HDC dc    = window->dc;

    if (dc) ReleaseDC(hwnd, dc);
    if (hwnd) DestroyWindow(hwnd);
    UnregisterClassW(window->class_name, instance);
}


static DWORD CALLBACK lk_window_thread(LPVOID window_ptr)
{
    LK_Window_Context* window = (LK_Window_Context*) window_ptr;

    // create the thread message queue
    MSG message;
    PeekMessageA(&message, 0, 0, 0, PM_NOREMOVE);

    // load libraries
    {
        window->user32 = LoadLibraryA("user32.dll");
        LK_GetProc(window->user32, window->SetProcessDPIAware,            "SetProcessDPIAware");
        LK_GetProc(window->user32, window->IsProcessDPIAware,             "IsProcessDPIAware");
        LK_GetProc(window->user32, window->GetDpiForWindow,               "GetDpiForWindow");
        LK_GetProc(window->user32, window->SetProcessDpiAwarenessContext, "SetProcessDpiAwarenessContext");

        window->shcore = LoadLibraryA("shcore.dll");
        LK_GetProc(window->shcore, window->GetDpiForMonitor,       "GetDpiForMonitor");
        LK_GetProc(window->shcore, window->SetProcessDpiAwareness, "SetProcessDpiAwareness");
    }

    window->arrow_cursor = LoadCursor(NULL, IDC_ARROW);
    window->client_has_win32_handler = 1;  // assume there's a lk_client_win32_event_handler

    lk_try_set_dpi_awareness(window);

    // open the window
    lk_open_window(window);
    SetEvent(window->opened);

    // run message queue
    while (GetMessageA(&message, 0, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    // close the window
    window->wants_to_close = 1;
    SetEvent(window->unpause);
    WaitForSingleObject(window->close, INFINITE);
    lk_close_window(window);

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Audio
////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    LK_Circular_Buffer errors;

    LK_Client* client;
    HANDLE thread;
    LK_B32 capture;

    LK_Audio_Sample_Type sample_type;
    LK_U32 channels;
    LK_U32 frequency;

    HANDLE pause_unpause_or_shutdown_event;
    LK_B32 volatile shutdown;
    LK_B32 volatile paused;
} LK_Audio_Context;


static void lk_render_audio(LK_Audio_Context* audio, void* buffer, LK_U32 frame_count)
{
    LK_Client* client = audio->client;

    lk_lock_client(client);

    LK_Platform* platform = client->platform;
    platform->audio.render_sample_type = audio->sample_type;
    platform->audio.render_channels    = audio->channels;
    platform->audio.render_frequency   = audio->frequency;
    client->functions.render_audio(platform, buffer, frame_count);

    lk_unlock_client(client);
}


static void lk_capture_audio(LK_Audio_Context* audio, void* buffer, LK_U32 frame_count)
{
    LK_Client* client = audio->client;

    lk_lock_client(client);

    LK_Platform* platform = client->platform;
    platform->audio.capture_sample_type = audio->sample_type;
    platform->audio.capture_channels    = audio->channels;
    platform->audio.capture_frequency   = audio->frequency;
    client->functions.capture_audio(platform, buffer, frame_count);

    lk_unlock_client(client);
}


////////////////////////////////////////////////////////////////////////////////
// WASAPI

#ifdef __cplusplus
#define LK_COMREF(thing) (thing)
#define LK_COMDEREF(ref) (ref)
#else
#define LK_COMREF(thing) &(thing)
#define LK_COMDEREF(ref) *(ref)
#endif

static BOOL lk_compare_iid(const IID* a, const IID* b)
{
    return memcmp(a, b, sizeof(IID));
}

static BOOL lk_compare_guid(const GUID* a, const GUID* b)
{
    return memcmp(a, b, sizeof(GUID));
}


typedef struct
{
    const IMMNotificationClientVtbl *lpVtbl;
    IMMNotificationClientVtbl vtable;
    LONG volatile reference_count;
    LONG volatile generation;
    EDataFlow data_flow;
} LK_MMNotificationClient;

static HRESULT STDMETHODCALLTYPE LK_MMNotificationClient_QueryInterface(IMMNotificationClient* self, REFIID iid, void** object)
{
    IID IID_IMMNotificationClient = { 0x7991eec9, 0x7e89, 0x4d85, { 0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0 } };
    if (lk_compare_iid(&LK_COMDEREF(iid), &IID_IUnknown) || lk_compare_iid(&LK_COMDEREF(iid), &IID_IMMNotificationClient))
    {
        *object = self;
        self->lpVtbl->AddRef(self);
        return S_OK;
    }

    *object = 0;
    return E_NOINTERFACE;
}

static ULONG LK_MMNotificationClient_AddRef(IMMNotificationClient* self_ptr)
{
    LK_MMNotificationClient* self = (LK_MMNotificationClient*) self_ptr;
    return InterlockedIncrement(&self->reference_count);
}

static ULONG LK_MMNotificationClient_Release(IMMNotificationClient* self_ptr)
{
    // No need to ever free this object, we're just playing along with this reference counting.
    LK_MMNotificationClient* self = (LK_MMNotificationClient*) self_ptr;
    return InterlockedDecrement(&self->reference_count);
}

static HRESULT STDMETHODCALLTYPE LK_MMNotificationClient_OnDefaultDeviceChanged(IMMNotificationClient* self_ptr, EDataFlow flow, ERole role, LPCWSTR device_id)
{
    LK_MMNotificationClient* self = (LK_MMNotificationClient*) self_ptr;
    if (role == eConsole && (flow == self->data_flow || flow == eAll))
        InterlockedIncrement(&self->generation);
    return S_OK;
}

// We don't care about these.
static HRESULT STDMETHODCALLTYPE LK_MMNotificationClient_OnDeviceAdded(IMMNotificationClient* self, LPCWSTR device_id) { return S_OK; }
static HRESULT STDMETHODCALLTYPE LK_MMNotificationClient_OnDeviceRemoved(IMMNotificationClient* self, LPCWSTR device_id) { return S_OK; }
static HRESULT STDMETHODCALLTYPE LK_MMNotificationClient_OnDeviceStateChanged(IMMNotificationClient* self, LPCWSTR device_id, DWORD new_state) { return S_OK; }
static HRESULT STDMETHODCALLTYPE LK_MMNotificationClient_OnPropertyValueChanged(IMMNotificationClient* self, LPCWSTR device_id, PROPERTYKEY key) { return S_OK; }

static void lk_make_notification_client(LK_MMNotificationClient* client, EDataFlow data_flow)
{
    client->lpVtbl = &client->vtable;
    client->vtable.QueryInterface         = LK_MMNotificationClient_QueryInterface;
    client->vtable.AddRef                 = LK_MMNotificationClient_AddRef;
    client->vtable.Release                = LK_MMNotificationClient_Release;
    client->vtable.OnDeviceStateChanged   = LK_MMNotificationClient_OnDeviceStateChanged;
    client->vtable.OnDeviceAdded          = LK_MMNotificationClient_OnDeviceAdded;
    client->vtable.OnDeviceRemoved        = LK_MMNotificationClient_OnDeviceRemoved;
    client->vtable.OnDefaultDeviceChanged = LK_MMNotificationClient_OnDefaultDeviceChanged;
    client->vtable.OnPropertyValueChanged = LK_MMNotificationClient_OnPropertyValueChanged;
    client->reference_count = 1;
    client->generation = 0;
    client->data_flow = data_flow;
}


typedef struct
{
    HMODULE avrt;
    HANDLE(WINAPI *AvSetMmThreadCharacteristicsW)(LPWSTR, LPDWORD);
    BOOL(WINAPI *AvRevertMmThreadCharacteristics)(HANDLE);
    BOOL initialized_com;
    IMMDeviceEnumerator* enumerator;
    LK_MMNotificationClient notification_client;
    HANDLE pro_audio_task;

    IAudioClient* client;
    IAudioRenderClient* renderer;
    IAudioCaptureClient* capture;
    REFERENCE_TIME duration;
    WAVEFORMATEX* format;
    UINT32 buffer_size;
    HANDLE event;
} LK_WASAPI_Context;

#define LK_CheckStatus(error_message)          \
    if (FAILED(status))                        \
    {                                          \
        LK_Log(&audio->errors, error_message); \
        return FALSE;                          \
    }

static LK_B32 lk_init_wasapi(LK_Audio_Context* audio, LK_WASAPI_Context* wasapi)
{
    const CLSID CLSID_MMDeviceEnumerator = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e } };
    const IID IID_IMMDeviceEnumerator    = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6 } };

    HRESULT status;

    // load avrt.dll
    wasapi->avrt = LoadLibraryW(L"avrt.dll");
    LK_GetProc(wasapi->avrt, wasapi->AvSetMmThreadCharacteristicsW,   "AvSetMmThreadCharacteristicsW"  );
    LK_GetProc(wasapi->avrt, wasapi->AvRevertMmThreadCharacteristics, "AvRevertMmThreadCharacteristics");
    if (!wasapi->AvSetMmThreadCharacteristicsW && !wasapi->AvRevertMmThreadCharacteristics)
    {
        LK_Log(&audio->errors, "Failed to initialize WASAPI audio; Couldn't load avrt.dll");
        return FALSE;
    }

    // initialize COM
    status = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (status == RPC_E_CHANGED_MODE)
        status = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (status == S_FALSE)
        status = S_OK;
    LK_CheckStatus("Failed to initialize WASAPI audio; CoCreateInstance(MMDeviceEnumerator) failed.");
    wasapi->initialized_com = TRUE;

    // initialize enumerator
    IMMDeviceEnumerator* enumerator;
    status = CoCreateInstance(LK_COMREF(CLSID_MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, LK_COMREF(IID_IMMDeviceEnumerator), (LPVOID*) &enumerator);
    LK_CheckStatus("Failed to initialize WASAPI audio; CoCreateInstance(MMDeviceEnumerator) failed.");
    wasapi->enumerator = enumerator;

    EDataFlow data_flow = audio->capture ? eCapture : eRender;
    lk_make_notification_client(&wasapi->notification_client, data_flow);
    status = IMMDeviceEnumerator_RegisterEndpointNotificationCallback(enumerator, (IMMNotificationClient*) &wasapi->notification_client);
    if (FAILED(status))
    {
        LK_Log(&audio->errors, "Warning: Failed to register the endpoint notification callback. You might experiences issues when devices change.");
    }

    // set critical priority
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    DWORD index = 0;
    HANDLE task = wasapi->AvSetMmThreadCharacteristicsW(L"Pro Audio", &index);
    if (!task)
        LK_Log(&audio->errors, "Warning: Failed to associate the audio thread with the \"Pro Audio\" task.");
    wasapi->pro_audio_task = task;

    return TRUE;
}

static void lk_uninit_wasapi(LK_Audio_Context* audio, LK_WASAPI_Context* wasapi)
{
    if (wasapi->pro_audio_task)
        wasapi->AvRevertMmThreadCharacteristics(wasapi->pro_audio_task);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    if (wasapi->avrt)
        FreeLibrary(wasapi->avrt);

    if (wasapi->enumerator)
    {
        IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(wasapi->enumerator, (IMMNotificationClient*) &wasapi->notification_client);
        IMMDeviceEnumerator_Release(wasapi->enumerator);
    }

    if (wasapi->initialized_com)
        CoUninitialize();

    ZeroMemory(wasapi, sizeof(LK_WASAPI_Context));
}


static LK_B32 lk_init_default_device(LK_Audio_Context* audio, LK_WASAPI_Context* wasapi)
{
    const IID IID_IAudioClient        = { 0x1cb9ad4c, 0xdbfa, 0x4c32, { 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2 } };
    const IID IID_IAudioRenderClient  = { 0xf294acfc, 0x3146, 0x4483, { 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2 } };
    const IID IID_IAudioCaptureClient = { 0xc8adbd64, 0xe71e, 0x48a0, { 0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17 } };

    HRESULT status;

    // get the audio client for the default rendering device
    IMMDevice* device = 0;
    EDataFlow data_flow = audio->capture ? eCapture : eRender;
    status = IMMDeviceEnumerator_GetDefaultAudioEndpoint(wasapi->enumerator, data_flow, eConsole, &device);
    LK_CheckStatus("Failed to initialize WASAPI client; IMMDeviceEnumerator_GetDefaultAudioEndpoint failed.");

    IAudioClient* client = 0;
    status = IMMDevice_Activate(device, LK_COMREF(IID_IAudioClient), CLSCTX_ALL, 0, (LPVOID*) &client);
    IMMDevice_Release(device);

    LK_CheckStatus("Failed to initialize WASAPI client; IMMDevice_Activate failed.");
    wasapi->client = client;

    // get properties before initialization
    REFERENCE_TIME duration;
    status = IAudioClient_GetDevicePeriod(client, NULL, &duration);
    LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_GetDevicePeriod failed.");
    wasapi->duration = duration;

    WAVEFORMATEX* format = 0;
    status = IAudioClient_GetMixFormat(client, &format);
    LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_GetMixFormat failed.");
    wasapi->format = format;

    // optimal settings
    LK_U32 mix_channels = format->nChannels;
    LK_U32 mix_frequency = format->nSamplesPerSec;

    LK_Audio_Sample_Type mix_sample_type = LK_AUDIO_SAMPLE_UNSPECIFIED;
    if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        const GUID LK_KSDATAFORMAT_SUBTYPE_PCM        = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
        const GUID LK_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

        WAVEFORMATEXTENSIBLE* ext = (WAVEFORMATEXTENSIBLE*) format;
        if (lk_compare_guid(&ext->SubFormat, &LK_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) && format->wBitsPerSample == 32)
            mix_sample_type = LK_AUDIO_SAMPLE_FLOAT32;
        else if (lk_compare_guid(&ext->SubFormat, &LK_KSDATAFORMAT_SUBTYPE_PCM) && format->wBitsPerSample == 16)
            mix_sample_type = LK_AUDIO_SAMPLE_INT16;
    }
    else if ((format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) && (format->wBitsPerSample == 32))
        mix_sample_type = LK_AUDIO_SAMPLE_FLOAT32;
    else if ((format->wFormatTag == WAVE_FORMAT_PCM) && (format->wBitsPerSample == 16))
        mix_sample_type = LK_AUDIO_SAMPLE_INT16;

    DWORD stream_flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    {
        // If we don't recognize the mix sample type, use floats by default.
        if (!mix_sample_type)
        {
            mix_sample_type = LK_AUDIO_SAMPLE_FLOAT32;
            stream_flags |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
        }

        if (!audio->sample_type) audio->sample_type = mix_sample_type;
        if (!audio->channels)    audio->channels    = mix_channels;
        if (!audio->frequency)   audio->frequency   = mix_frequency;

        if (audio->sample_type != mix_sample_type || audio->channels != mix_channels || audio->frequency != mix_frequency)
            stream_flags |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
    }

    // set our wave format
    format->wFormatTag      = (audio->sample_type == LK_AUDIO_SAMPLE_INT16) ? WAVE_FORMAT_PCM : WAVE_FORMAT_IEEE_FLOAT;
    format->wBitsPerSample  = (audio->sample_type == LK_AUDIO_SAMPLE_INT16) ? 16 : 32;
    format->nChannels       = audio->channels;
    format->nSamplesPerSec  = audio->frequency;
    format->nBlockAlign     = audio->channels * (format->wBitsPerSample / 8);
    format->nAvgBytesPerSec = audio->frequency * format->nBlockAlign;
    format->cbSize          = 0;

    // initialize
    status = IAudioClient_Initialize(client, AUDCLNT_SHAREMODE_SHARED, stream_flags, duration, 0, format, NULL);
    LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_Initialize failed.");

    // get properties after initialization
    UINT32 buffer_size;
    status = IAudioClient_GetBufferSize(client, &buffer_size);
    LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_GetBufferSize failed.");
    wasapi->buffer_size = buffer_size;

    // set event
    HANDLE event = CreateEventW(0, 0, 0, 0);
    wasapi->event = event;

    status = IAudioClient_SetEventHandle(client, event);
    LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_SetEventHandle failed.");

    if (audio->capture)
    {
        // make the capture client
        IAudioCaptureClient* capture = 0;
        status = IAudioClient_GetService(client, LK_COMREF(IID_IAudioCaptureClient), (LPVOID*) &capture);
        LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_GetService(IAudioCaptureClient) failed.");
        wasapi->capture = capture;
    }
    else
    {
        // make the render client
        IAudioRenderClient* renderer = 0;
        status = IAudioClient_GetService(client, LK_COMREF(IID_IAudioRenderClient), (LPVOID*) &renderer);
        LK_CheckStatus("Failed to initialize WASAPI client; IAudioClient_GetService(IAudioRenderClient) failed.");
        wasapi->renderer = renderer;

        // fill with silence
        BYTE* buffer;
        status = IAudioRenderClient_GetBuffer(renderer, buffer_size, &buffer);
        if (SUCCEEDED(status))
        {
            IAudioRenderClient_ReleaseBuffer(renderer, buffer_size, AUDCLNT_BUFFERFLAGS_SILENT);
        }
    }

    return TRUE;
}

static void lk_uninit_device(LK_Audio_Context* audio, LK_WASAPI_Context* wasapi)
{
    if (wasapi->client)
    {
        IAudioClient_Stop(wasapi->client);
        IAudioClient_SetEventHandle(wasapi->client, 0);
        IAudioClient_Release(wasapi->client);
        wasapi->client = 0;
    }

    if (wasapi->format)
    {
        CoTaskMemFree(wasapi->format);
        wasapi->format = 0;
    }

    if (wasapi->event)
    {
        CloseHandle(wasapi->event);
        wasapi->event = 0;
    }

    if (wasapi->renderer)
    {
        IAudioRenderClient_Release(wasapi->renderer);
        wasapi->renderer = 0;
    }

    if (wasapi->capture)
    {
        IAudioCaptureClient_Release(wasapi->capture);
        wasapi->capture = 0;
    }

    wasapi->duration = 0;
    wasapi->buffer_size = 0;
}

#undef LK_CheckStatus


static LK_B32 lk_run_wasapi(LK_Audio_Context* audio)
{
    LK_B32 initialized_wasapi_device_at_least_once = 0;
    LK_B32 capture = audio->capture;

    LK_WASAPI_Context wasapi = { 0 };
    if (lk_init_wasapi(audio, &wasapi))
    {
        LONG generation = -1;
        BOOL recover = TRUE;  // on first iteration, it's not really recovery but initialization
        BOOL paused = TRUE;
        while (!audio->shutdown)
        {
            HRESULT status;

            // if the default device changed, force recover
            LONG current_generation = wasapi.notification_client.generation;
            if (generation != current_generation)
            {
                generation = current_generation;
                recover = TRUE;
            }

            // if the device got invalidated, try to recover
            if (recover)
            {
                recover = FALSE;
                lk_uninit_device(audio, &wasapi);
                if (!lk_init_default_device(audio, &wasapi))
                    break;
                paused = TRUE;
                initialized_wasapi_device_at_least_once = 1;
            }

            // pause/unpause
            if (paused != audio->paused)
            {
                paused = audio->paused;
                if (paused)
                    status = IAudioClient_Stop(wasapi.client);
                else
                    status = IAudioClient_Start(wasapi.client);

                if (FAILED(status))
                {
                    if (status == AUDCLNT_E_DEVICE_INVALIDATED)
                    {
                        recover = TRUE;
                        continue;
                    }

                    LK_Log(&audio->errors, "Audio failure; pausing/unpausing failed!");
                    break;
                }
            }

            // if paused, wait for unpause event
            if (paused)
            {
                WaitForSingleObjectEx(audio->pause_unpause_or_shutdown_event, 100, 0);

                // if capture, flush the buffer
                if (capture)
                {
                    while (TRUE)
                    {
                        BYTE* buffer = 0;
                        UINT32 frames = 0;
                        DWORD flags = 0;
                        status = IAudioCaptureClient_GetBuffer(wasapi.capture, &buffer, &frames, &flags, 0, 0);
                        if (status != S_OK) break;
                        IAudioCaptureClient_ReleaseBuffer(wasapi.capture, frames);
                    }
                }

                continue;
            }

            if (capture)
            {
                // get the sample buffer
                BYTE* buffer = 0;
                UINT32 frames = 0;
                DWORD flags = 0;
                status = IAudioCaptureClient_GetBuffer(wasapi.capture, &buffer, &frames, &flags, 0, 0);
                if (status == AUDCLNT_S_BUFFER_EMPTY)
                {
                    // nothing available, wait
                    DWORD result = WaitForSingleObjectEx(wasapi.event, 200, 0);
                    if (result == WAIT_OBJECT_0)
                        continue;
                    LK_Log(&audio->errors, "Audio capture failure; wait timeout!");
                    break;
                }

                if (SUCCEEDED(status))
                {
                    lk_capture_audio(audio, buffer, frames);

                    // release the buffer
                    status = IAudioCaptureClient_ReleaseBuffer(wasapi.capture, frames);
                    if (SUCCEEDED(status))
                        continue;
                }
            }
            else
            {
                // get how many frames are still in the buffer
                UINT32 frames_waiting;
                status = IAudioClient_GetCurrentPadding(wasapi.client, &frames_waiting);
                if (SUCCEEDED(status))
                {
                    // compute how many frames we can render
                    UINT32 frames_to_render = wasapi.buffer_size - frames_waiting;
                    if (frames_to_render > wasapi.buffer_size / 2)  // don't render more than half of the buffer at once
                        frames_to_render = wasapi.buffer_size / 2;

                    // get the sample buffer
                    BYTE* buffer = 0;
                    HRESULT status = IAudioRenderClient_GetBuffer(wasapi.renderer, frames_to_render, &buffer);
                    if (SUCCEEDED(status))
                    {
                        lk_render_audio(audio, (LK_S16*) buffer, frames_to_render);

                        // release the buffer
                        status = IAudioRenderClient_ReleaseBuffer(wasapi.renderer, frames_to_render, 0);
                        if (SUCCEEDED(status))
                        {
                            DWORD result = WaitForSingleObjectEx(wasapi.event, 200, 0);
                            if (result == WAIT_OBJECT_0)
                                continue;
                            LK_Log(&audio->errors, "Audio rendering failure; wait timeout!");
                            break;
                        }
                    }
                }
            }

            // we failed somewhere along the line; could be that the device got invalidated
            if (status == AUDCLNT_E_DEVICE_INVALIDATED)
            {
                recover = TRUE;
                continue;
            }

            LK_Log(&audio->errors, "Audio failure; buffer management failed!");
            break;
        }
    }

    lk_uninit_device(audio, &wasapi);
    lk_uninit_wasapi(audio, &wasapi);
    return initialized_wasapi_device_at_least_once;
}


////////////////////////////////////////////////////////////////////////////////
// Audio thread

static DWORD CALLBACK lk_audio_thread(LPVOID audio_ptr)
{
    LK_Audio_Context* audio = (LK_Audio_Context*) audio_ptr;

    // Try using WASAPI for audio.
    if (lk_run_wasapi(audio))
        return 0;

    // @Reconsider - try something else if WASAPI fails, like DirectSound

    return 0;
}

static void lk_start_audio_thread(LK_Platform* platform, LK_Audio_Context* audio, LK_Client* client, LK_B32 capture)
{
    if (capture)
    {
        audio->capture     = 1;
        audio->sample_type = platform->audio.capture_sample_type;
        audio->channels    = platform->audio.capture_channels;
        audio->frequency   = platform->audio.capture_frequency;
    }
    else
    {
        audio->capture     = 0;
        audio->sample_type = platform->audio.render_sample_type;
        audio->channels    = platform->audio.render_channels;
        audio->frequency   = platform->audio.render_frequency;
    }

    audio->paused = FALSE;
    audio->shutdown = FALSE;
    audio->pause_unpause_or_shutdown_event = CreateEventW(0, 0, 0, 0);

    audio->client = client;
    audio->thread = CreateThread(0, 0, lk_audio_thread, audio, 0, 0);
}

static void lk_stop_audio_thread(LK_Audio_Context* audio)
{
    audio->shutdown = TRUE;
    SetEvent(audio->pause_unpause_or_shutdown_event);
    WaitForSingleObject(audio->thread, INFINITE);

    CloseHandle(audio->thread);
    CloseHandle(audio->pause_unpause_or_shutdown_event);
}

static void lk_set_audio_paused(LK_Audio_Context* audio, LK_B32 paused)
{
    if (audio->paused != paused)
    {
        audio->paused = paused;
        SetEvent(audio->pause_unpause_or_shutdown_event);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Gamepad
////////////////////////////////////////////////////////////////////////////////


#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif

enum
{
    LK_XINPUT_GAMEPAD_GUIDE = 0x0400,
    LK_BATTERY_DEVTYPE_GAMEPAD = 0x00,
    LK_BATTERY_TYPE_WIRED   = 0x01,
    LK_BATTERY_TYPE_UNKNOWN = 0xFF,
    LK_BATTERY_LEVEL_EMPTY  = 0x00,
    LK_BATTERY_LEVEL_LOW    = 0x01,
    LK_BATTERY_LEVEL_MEDIUM = 0x02,
    LK_BATTERY_LEVEL_FULL   = 0x03,
};

typedef struct
{
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
    DWORD dwPaddingReserved;
} LK_XINPUT_GAMEPAD_EX;

typedef struct
{
    DWORD dwPacketNumber;
    LK_XINPUT_GAMEPAD_EX Gamepad;
} LK_XINPUT_STATE_EX;

typedef struct
{
    BYTE BatteryType;
    BYTE BatteryLevel;
} LK_XINPUT_BATTERY_INFORMATION;


typedef struct
{
    HMODULE xinput;
    DWORD(WINAPI* XInputGetState)(DWORD dwUserIndex, LK_XINPUT_STATE_EX* pState);
    DWORD(WINAPI* XInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
    DWORD(WINAPI* XInputGetCapabilities)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
    DWORD(WINAPI* XInputGetBatteryInformation)(DWORD dwUserIndex, BYTE devType, LK_XINPUT_BATTERY_INFORMATION* pBatteryInformation);

    BOOL available;
    LK_U32 device_generation;

    struct
    {
        BOOL connected;
        XINPUT_CAPABILITIES capabilities;
    } gamepad[XUSER_MAX_COUNT];
} LK_Gamepad_Context;

static void lk_detect_gamepads(LK_Platform* platform, LK_Gamepad_Context* context)
{
    for (DWORD index = 0; index < XUSER_MAX_COUNT; index++)
    {
        LK_B32 was_connected = context->gamepad[index].connected;
        XINPUT_CAPABILITIES* capabilities = &context->gamepad[index].capabilities;
        DWORD status = context->XInputGetCapabilities(index, XINPUT_FLAG_GAMEPAD, capabilities);
        LK_B32 connected = (status == ERROR_SUCCESS);
        context->gamepad[index].connected = connected;

        if (connected && !was_connected)
        {
            ZeroMemory(&platform->gamepads[index], sizeof(LK_Gamepad));
            platform->gamepads[index].connected = 1;
        }
    }
}

static void lk_init_gamepad_support(LK_Platform* platform, LK_Gamepad_Context* context)
{
    HMODULE xinput;
                 xinput = LoadLibraryW(L"XInput1_4.dll");
    if (!xinput) xinput = LoadLibraryW(L"XInput1_3.dll");
    if (!xinput) xinput = LoadLibraryW(L"XInput9_1_0.dll");
    context->xinput = xinput;

    LK_GetProc(xinput, context->XInputGetState, (LPCSTR) 100);  // "secret" XInputGetStateEx routine, ordinal 100
    if (!context->XInputGetState)
        LK_GetProc(xinput, context->XInputGetState, "XInputGetState");

    LK_GetProc(xinput, context->XInputSetState,              "XInputSetState");
    LK_GetProc(xinput, context->XInputGetCapabilities,       "XInputGetCapabilities");
    LK_GetProc(xinput, context->XInputGetBatteryInformation, "XInputGetBatteryInformation");

    context->available = (context->XInputGetState && context->XInputSetState && context->XInputGetCapabilities);
    if (!context->available)
        return;

    context->device_generation = 0;
    lk_detect_gamepads(platform, context);
}

static void lk_uninit_gamepad_support(LK_Gamepad_Context* context)
{
    if (context->xinput)
    {
        FreeLibrary(context->xinput);
    }
}

static void lk_update_gamepads(LK_Platform* platform, LK_Window_Context* window, LK_Gamepad_Context* context, LK_Circular_Buffer* errors)
{
    if (!context->available)
        return;

    // if devices were connected/disconnected, detect gamepads again.
    LK_U32 current_device_generation = window->device_generation;
    if (context->device_generation != current_device_generation)
    {
        context->device_generation = current_device_generation;
        lk_detect_gamepads(platform, context);
    }

    for (DWORD index = 0; index < XUSER_MAX_COUNT; index++)
    {
        LK_Gamepad* gamepad = &platform->gamepads[index];
        if (!context->gamepad[index].connected)
        {
            gamepad->connected = 0;
            continue;
        }

        LK_XINPUT_STATE_EX state;
        DWORD result = context->XInputGetState(index, &state);
        if (result == ERROR_SUCCESS)
        {
            gamepad->connected = 1;

            // Update analog values.
            if (state.Gamepad.sThumbLX == -32768) state.Gamepad.sThumbLX = -32767;
            if (state.Gamepad.sThumbLY == -32768) state.Gamepad.sThumbLY = -32767;
            if (state.Gamepad.sThumbRX == -32768) state.Gamepad.sThumbRX = -32767;
            if (state.Gamepad.sThumbRY == -32768) state.Gamepad.sThumbRY = -32767;

            SHORT ldead = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
            SHORT rdead = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
            if (state.Gamepad.sThumbLX >= -ldead && state.Gamepad.sThumbLX <= ldead) state.Gamepad.sThumbLX = 0;
            if (state.Gamepad.sThumbLY >= -ldead && state.Gamepad.sThumbLY <= ldead) state.Gamepad.sThumbLY = 0;
            if (state.Gamepad.sThumbRX >= -ldead && state.Gamepad.sThumbRX <= ldead) state.Gamepad.sThumbRX = 0;
            if (state.Gamepad.sThumbRY >= -ldead && state.Gamepad.sThumbRY <= ldead) state.Gamepad.sThumbRY = 0;

            gamepad->analog[LK_GAMEPAD_XBOX_LEFT_STICK_X ] = (LK_F64) state.Gamepad.sThumbLX / 32767.0;
            gamepad->analog[LK_GAMEPAD_XBOX_LEFT_STICK_Y ] = (LK_F64) state.Gamepad.sThumbLY / 32767.0;
            gamepad->analog[LK_GAMEPAD_XBOX_RIGHT_STICK_X] = (LK_F64) state.Gamepad.sThumbRX / 32767.0;
            gamepad->analog[LK_GAMEPAD_XBOX_RIGHT_STICK_Y] = (LK_F64) state.Gamepad.sThumbRY / 32767.0;

            gamepad->analog[LK_GAMEPAD_XBOX_LEFT_TRIGGER ] = (LK_F64) state.Gamepad.bLeftTrigger  / 255.0;
            gamepad->analog[LK_GAMEPAD_XBOX_RIGHT_TRIGGER] = (LK_F64) state.Gamepad.bRightTrigger / 255.0;

            // Update buttons.
            WORD flags = state.Gamepad.wButtons;
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_A    ], !!(flags & XINPUT_GAMEPAD_A));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_B    ], !!(flags & XINPUT_GAMEPAD_B));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_X    ], !!(flags & XINPUT_GAMEPAD_X));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_Y    ], !!(flags & XINPUT_GAMEPAD_Y));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_LB   ], !!(flags & XINPUT_GAMEPAD_LEFT_SHOULDER));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_RB   ], !!(flags & XINPUT_GAMEPAD_RIGHT_SHOULDER));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_VIEW ], !!(flags & XINPUT_GAMEPAD_BACK));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_MENU ], !!(flags & XINPUT_GAMEPAD_START));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_GUIDE], !!(flags & LK_XINPUT_GAMEPAD_GUIDE));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_LSB  ], !!(flags & XINPUT_GAMEPAD_LEFT_THUMB));
            lk_update_digital_button(&gamepad->buttons[LK_GAMEPAD_XBOX_RSB  ], !!(flags & XINPUT_GAMEPAD_RIGHT_THUMB));

            // Update hat.
            LONG dpad_vertical   = (LONG) !!(flags & XINPUT_GAMEPAD_DPAD_UP)    - (LONG) !!(flags & XINPUT_GAMEPAD_DPAD_DOWN);
            LONG dpad_horizontal = (LONG) !!(flags & XINPUT_GAMEPAD_DPAD_RIGHT) - (LONG) !!(flags & XINPUT_GAMEPAD_DPAD_LEFT);

            const LK_F64 rt2 = 0.70710678118;
                 if (dpad_vertical ==  0 && dpad_horizontal ==  0) gamepad->hat.x =  0.0, gamepad->hat.y =  0.0, gamepad->hat.angle =  -1.0;
            else if (dpad_vertical ==  0 && dpad_horizontal ==  1) gamepad->hat.x =  1.0, gamepad->hat.y =  0.0, gamepad->hat.angle =   0.0;
            else if (dpad_vertical ==  1 && dpad_horizontal ==  1) gamepad->hat.x =  rt2, gamepad->hat.y =  rt2, gamepad->hat.angle =  45.0;
            else if (dpad_vertical ==  1 && dpad_horizontal ==  0) gamepad->hat.x =  0.0, gamepad->hat.y =  1.0, gamepad->hat.angle =  90.0;
            else if (dpad_vertical ==  1 && dpad_horizontal == -1) gamepad->hat.x = -rt2, gamepad->hat.y =  rt2, gamepad->hat.angle = 135.0;
            else if (dpad_vertical ==  0 && dpad_horizontal == -1) gamepad->hat.x = -1.0, gamepad->hat.y =  0.0, gamepad->hat.angle = 180.0;
            else if (dpad_vertical == -1 && dpad_horizontal == -1) gamepad->hat.x = -rt2, gamepad->hat.y = -rt2, gamepad->hat.angle = 225.0;
            else if (dpad_vertical == -1 && dpad_horizontal ==  0) gamepad->hat.x =  0.0, gamepad->hat.y = -1.0, gamepad->hat.angle = 270.0;
            else if (dpad_vertical == -1 && dpad_horizontal ==  1) gamepad->hat.x =  rt2, gamepad->hat.y = -rt2, gamepad->hat.angle = 315.0;

            // Update battery info.
            gamepad->battery = -1.0f;
            if (context->XInputGetBatteryInformation)
            {
                LK_XINPUT_BATTERY_INFORMATION battery_info;
                DWORD result = context->XInputGetBatteryInformation(index, LK_BATTERY_DEVTYPE_GAMEPAD, &battery_info);
                if (result == ERROR_SUCCESS &&
                    battery_info.BatteryType != LK_BATTERY_TYPE_WIRED &&
                    battery_info.BatteryType != LK_BATTERY_TYPE_UNKNOWN)
                {
                         if (battery_info.BatteryLevel == LK_BATTERY_LEVEL_EMPTY ) gamepad->battery = 0.0f;
                    else if (battery_info.BatteryLevel == LK_BATTERY_LEVEL_LOW   ) gamepad->battery = 0.3f;
                    else if (battery_info.BatteryLevel == LK_BATTERY_LEVEL_MEDIUM) gamepad->battery = 0.6f;
                    else if (battery_info.BatteryLevel == LK_BATTERY_LEVEL_FULL  ) gamepad->battery = 1.0f;
                }
            }

            // Update event mask.

            if (gamepad->hat.angle >= 0.0f)
                platform->event_mask |= LK_EVENT_GAMEPAD_DOWN;

            for (int index = 0; index < LK_GAMEPAD_BUTTON_COUNT; index++)
            {
                LK_Digital_Button* button = &gamepad->buttons[index];
                if (button->down)     platform->event_mask |= LK_EVENT_GAMEPAD_DOWN;
                if (button->pressed)  platform->event_mask |= LK_EVENT_GAMEPAD_PRESS;
                if (button->released) platform->event_mask |= LK_EVENT_GAMEPAD_RELEASE;
            }

            for (int index = 0; index < LK_GAMEPAD_ANALOG_COUNT; index++)
                if (gamepad->analog[index] != 0.0f)
                {
                    platform->event_mask |= LK_EVENT_GAMEPAD_ANALOG;
                    break;
                }

            continue;
        }

        // gamepad disconnected?
        gamepad->connected = 0;
        context->gamepad[index].connected = FALSE;
        if (result != ERROR_DEVICE_NOT_CONNECTED)
            LK_Log(errors, "XInput failed while updating a gamepad.");
    }
}


////////////////////////////////////////////////////////////////////////////////
// Time
////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    LK_U64 initial_ticks;
    LK_U64 ticks_per_second;

    LK_U64 unprocessed_nanoseconds;
    LK_U64 unprocessed_microseconds;
    LK_U64 unprocessed_milliseconds;
} LK_Time_Context;

static void lk_initialize_timer(LK_Time_Context* time)
{
    LARGE_INTEGER i64;
    QueryPerformanceFrequency(&i64);
    time->ticks_per_second = i64.QuadPart;

    QueryPerformanceCounter(&i64);
    time->initial_ticks = i64.QuadPart;
}

static void lk_update_time_stamp(LK_Time_Context* time, LK_Platform* lk_platform)
{
    LARGE_INTEGER i64;
    QueryPerformanceCounter(&i64);

    LK_U64 frequency = time->ticks_per_second;
    LK_U64 new_ticks = i64.QuadPart - time->initial_ticks;
    LK_U64 delta_ticks = new_ticks - lk_platform->time.ticks;

    lk_platform->time.delta_ticks = delta_ticks;

    LK_U64 nanoseconds_ticks = 1000000000 * delta_ticks + time->unprocessed_nanoseconds;
    lk_platform->time.delta_nanoseconds = nanoseconds_ticks / frequency;
    time->unprocessed_nanoseconds = nanoseconds_ticks % frequency;

    LK_U64 microseconds_ticks = 1000000 * delta_ticks + time->unprocessed_microseconds;
    lk_platform->time.delta_microseconds = microseconds_ticks / frequency;
    time->unprocessed_microseconds = microseconds_ticks % frequency;

    LK_U64 milliseconds_ticks = 1000 * delta_ticks + time->unprocessed_milliseconds;
    lk_platform->time.delta_milliseconds = milliseconds_ticks / frequency;
    time->unprocessed_milliseconds = milliseconds_ticks % frequency;

    lk_platform->time.delta_seconds = (LK_F64) delta_ticks / (LK_F64) frequency;

    lk_platform->time.ticks        += delta_ticks;
    lk_platform->time.nanoseconds  += lk_platform->time.delta_nanoseconds;
    lk_platform->time.microseconds += lk_platform->time.delta_microseconds;
    lk_platform->time.milliseconds += lk_platform->time.delta_milliseconds;
    lk_platform->time.seconds      += lk_platform->time.delta_seconds;
}


static LK_U64 lk_get_time_stamp(LK_Time_Context* time)
{
    LARGE_INTEGER i64;
    QueryPerformanceCounter(&i64);
    return i64.QuadPart;
}

static LK_U64 lk_time_to_seconds(LK_Time_Context* time, LK_U64 time_stamp_difference)
{
    return (double) time_stamp_difference / (double) time->ticks_per_second;
}



////////////////////////////////////////////////////////////////////////////////
// OpenGL
////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    HMODULE library;

    HGLRC(*wglCreateContext )(HDC hdc);
    BOOL (*wglDeleteContext )(HGLRC hglrc);
    BOOL (*wglMakeCurrent   )(HDC hdc, HGLRC hglrc);
    PROC (*wglGetProcAddress)(LPCSTR name);

    HGLRC context;
    BOOL (*wglChoosePixelFormatARB   )(HDC hDC, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    HGLRC(*wglCreateContextAttribsARB)(HDC hDC, HGLRC hShareContext, const int* attribList);
    BOOL (*wglSwapIntervalEXT        )(int interval);
} LK_OpenGL_Context;


static int lk_load_opengl_library(LK_Platform* lk_platform, LK_OpenGL_Context* gl, LK_Circular_Buffer* errors)
{
    gl->library = LoadLibraryA("opengl32.dll");
    if (!gl->library)
    {
        LK_Log(errors, "Failed to create an OpenGL context; couldn't load OpenGL32.dll.");
        return 0;
    }

    #define LK_GetOpenGLFunction(name)                                     \
    {                                                                      \
        LK_GetProc(gl->library, gl->name, #name);                          \
        if (!gl->name)                                                     \
        {                                                                  \
            LK_Log(errors, "Failed to create an OpenGL context; "          \
                           "couldn't find " #name "() in  OpenGL32.dll."); \
            return 0;                                                      \
        }                                                                  \
    }

    LK_GetOpenGLFunction(wglCreateContext );
    LK_GetOpenGLFunction(wglDeleteContext );
    LK_GetOpenGLFunction(wglMakeCurrent   );
    LK_GetOpenGLFunction(wglGetProcAddress);

    #undef LK_GetOpenGLFunction

    return 1;
}

static void lk_unload_opengl_library(LK_OpenGL_Context* gl)
{
    if (gl->context && gl->wglMakeCurrent && gl->wglDeleteContext)
    {
        gl->wglMakeCurrent(0, 0);
        gl->wglDeleteContext(gl->context);
    }

    if (gl->library)
    {
        FreeLibrary(gl->library);
    }
}


static void lk_create_legacy_opengl_context(LK_Platform* lk_platform, LK_Window_Context* window, LK_OpenGL_Context* gl, LK_Circular_Buffer* errors)
{
    HDC dc = window->dc;

    PIXELFORMATDESCRIPTOR pixel_format_desc = { 0 };
    pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format_desc.nVersion = 1;
    pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
    pixel_format_desc.cColorBits = lk_platform->opengl.color_bits;
    pixel_format_desc.cDepthBits = lk_platform->opengl.depth_bits;
    pixel_format_desc.cStencilBits = lk_platform->opengl.stencil_bits;
    pixel_format_desc.iLayerType = PFD_MAIN_PLANE;

    int pixel_format = ChoosePixelFormat(dc, &pixel_format_desc);
    if (pixel_format == 0)
    {
        LK_Log(errors, "Failed to create a legacy OpenGL context; ChoosePixelFormat() failed.");
        return;
    }

    if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
    {
        LK_Log(errors, "Failed to create a legacy OpenGL context; SetPixelFormat() failed.");
        return;
    }

    HGLRC context = gl->wglCreateContext(dc);
    if (!context)
    {
        LK_Log(errors, "Failed to create a legacy OpenGL context; wglCreateContext() failed.");
        return;
    }

    if (!gl->wglMakeCurrent(dc, context))
    {
        LK_Log(errors, "Failed to create a legacy OpenGL context; wglMakeCurrent() failed.");
        gl->wglDeleteContext(context);
        return;
    }

    gl->context = context;
}

static int lk_create_modern_opengl_context(LK_Platform* lk_platform, LK_Window_Context* window, LK_OpenGL_Context* gl, LK_Circular_Buffer* errors)
{
    int success = 0;
    HWND fake_window;
    HDC fake_dc;
    PIXELFORMATDESCRIPTOR fake_pixel_format_desc = { 0 };
    int fake_pixel_format;
    HGLRC fake_context;
    HGLRC real_context;

    HINSTANCE instance = GetModuleHandle(0);
    fake_window = CreateWindowExW(0, window->class_name, L"fake window", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, 0);
    if (!fake_window)
    {
        LK_Log(errors, "Failed to create a modern OpenGL context; CreateWindowEx() failed.");
        return 0;
    }

    fake_dc = GetDC(fake_window);
    if (!fake_dc)
    {
        LK_Log(errors, "Failed to create a modern OpenGL context; GetDC() failed.");
        goto undo_fake_window;
    }

    fake_pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    fake_pixel_format_desc.nVersion = 1;
    fake_pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fake_pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
    fake_pixel_format_desc.cColorBits = 32;
    fake_pixel_format_desc.cDepthBits = 16;
    fake_pixel_format_desc.iLayerType = PFD_MAIN_PLANE;

    fake_pixel_format = ChoosePixelFormat(fake_dc, &fake_pixel_format_desc);
    if (fake_pixel_format == 0)
    {
        LK_Log(errors, "Failed to create a modern OpenGL context; ChoosePixelFormat() failed.");
        goto undo_fake_dc;
    }

    if (!SetPixelFormat(fake_dc, fake_pixel_format, &fake_pixel_format_desc))
    {
        LK_Log(errors, "Failed to create a modern OpenGL context; SetPixelFormat() failed.");
        goto undo_fake_dc;
    }

    fake_context = gl->wglCreateContext(fake_dc);

    if (!fake_context)
    {
        LK_Log(errors, "Failed to create a modern OpenGL context; wglCreateContext() failed.");
        goto undo_fake_dc;
    }

    if (!gl->wglMakeCurrent(fake_dc, fake_context))
    {
        LK_Log(errors, "Failed to create a modern OpenGL context; wglMakeCurrent() failed.");
        goto undo_fake_context;
    }


    #define LK_GetWGLFunction(name)                      \
    {                                                    \
        PROC proc = gl->wglGetProcAddress(#name);        \
        if ((LONG_PTR) proc < -1 || (LONG_PTR) proc > 3) \
            *(PROC*) &gl->name = proc;                   \
        else                                             \
            gl->name = 0;                                \
    }

    LK_GetWGLFunction(wglChoosePixelFormatARB)
    LK_GetWGLFunction(wglCreateContextAttribsARB)
    LK_GetWGLFunction(wglSwapIntervalEXT)

    #undef LK_GetWGLFunction

    gl->wglMakeCurrent(0, 0);


    if (gl->wglChoosePixelFormatARB && gl->wglCreateContextAttribsARB)
    {
        HDC dc = window->dc;

        const int WGL_DRAW_TO_WINDOW_ARB    = 0x2001;
        const int WGL_ACCELERATION_ARB      = 0x2003;
        const int WGL_TRANSPARENT_ARB       = 0x200A;
        const int WGL_SUPPORT_OPENGL_ARB    = 0x2010;
        const int WGL_DOUBLE_BUFFER_ARB     = 0x2011;
        const int WGL_PIXEL_TYPE_ARB        = 0x2013;
        const int WGL_COLOR_BITS_ARB        = 0x2014;
        const int WGL_ALPHA_BITS_ARB        = 0x201B;
        const int WGL_DEPTH_BITS_ARB        = 0x2022;
        const int WGL_STENCIL_BITS_ARB      = 0x2023;
        const int WGL_FULL_ACCELERATION_ARB = 0x2027;
        const int WGL_TYPE_RGBA_ARB         = 0x202B;
        const int WGL_SAMPLE_BUFFERS_ARB    = 0x2041;
        const int WGL_SAMPLES_ARB           = 0x2042;


        int pixel_format_attributes[32];
        int pixel_format_attribute_count = 0;

        #define LK_AddPixelFormatAttributes(attribute, value)                    \
            pixel_format_attributes[pixel_format_attribute_count++] = attribute; \
            pixel_format_attributes[pixel_format_attribute_count++] = value;     \
            pixel_format_attributes[pixel_format_attribute_count] = 0;

        LK_AddPixelFormatAttributes(WGL_DRAW_TO_WINDOW_ARB, 1);
        LK_AddPixelFormatAttributes(WGL_SUPPORT_OPENGL_ARB, 1);
        LK_AddPixelFormatAttributes(WGL_DOUBLE_BUFFER_ARB, 1);
        LK_AddPixelFormatAttributes(WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB);
        LK_AddPixelFormatAttributes(WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB);
        LK_AddPixelFormatAttributes(WGL_COLOR_BITS_ARB, (int) lk_platform->opengl.color_bits);
        LK_AddPixelFormatAttributes(WGL_DEPTH_BITS_ARB, (int) lk_platform->opengl.depth_bits);
        LK_AddPixelFormatAttributes(WGL_STENCIL_BITS_ARB, (int) lk_platform->opengl.stencil_bits);

        if (lk_platform->window.transparent)
        {
            LK_AddPixelFormatAttributes(WGL_TRANSPARENT_ARB, TRUE);
            LK_AddPixelFormatAttributes(WGL_ALPHA_BITS_ARB, 8);
        }

        if (lk_platform->opengl.sample_count > 1)
        {
            LK_AddPixelFormatAttributes(WGL_SAMPLE_BUFFERS_ARB, 1);
            LK_AddPixelFormatAttributes(WGL_SAMPLES_ARB, (int) lk_platform->opengl.sample_count);
        }

        #undef LK_AddPixelFormatAttributes


        int pixel_format;
        UINT pixel_format_count;
        if (!gl->wglChoosePixelFormatARB(dc, pixel_format_attributes, 0, 1, &pixel_format, &pixel_format_count))
        {
            LK_Log(errors, "Failed to create a modern OpenGL context; wglChoosePixelFormatARB() failed.");
            goto undo_fake_context;
        }

        if (pixel_format_count == 0)
        {
            LK_Log(errors, "Failed to create a modern OpenGL context; couldn't choose the appropriate pixel format.");
            goto undo_fake_context;
        }

        PIXELFORMATDESCRIPTOR pixel_format_desc;
        if (!DescribePixelFormat(dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_desc))
        {
            LK_Log(errors, "Failed to create a modern OpenGL context; DescribePixelFormat() failed.");
            goto undo_fake_context;
        }

        if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
        {
            LK_Log(errors, "Failed to create a modern OpenGL context; SetPixelFormat() failed.");
            goto undo_fake_context;
        }



        const int WGL_CONTEXT_DEBUG_BIT_ARB                 = 0x00000001;
        const int WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    = 0x00000002;
        const int WGL_CONTEXT_MAJOR_VERSION_ARB             = 0x2091;
        const int WGL_CONTEXT_MINOR_VERSION_ARB             = 0x2092;
        const int WGL_CONTEXT_FLAGS_ARB                     = 0x2094;
        const int WGL_CONTEXT_PROFILE_MASK_ARB              = 0x9126;
        const int WGL_CONTEXT_CORE_PROFILE_BIT_ARB          = 0x00000001;
        const int WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;

        int flags = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
        if (lk_platform->opengl.debug_context)
        {
            flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        }

        int profile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        if (lk_platform->opengl.compatibility_context)
        {
            profile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        }

        if (!lk_platform->opengl.major_version)
        {
            lk_platform->opengl.major_version = 3;
            lk_platform->opengl.minor_version = 3;
        }

        int attributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, (int) lk_platform->opengl.major_version,
            WGL_CONTEXT_MINOR_VERSION_ARB, (int) lk_platform->opengl.minor_version,
            WGL_CONTEXT_FLAGS_ARB, flags,
            WGL_CONTEXT_PROFILE_MASK_ARB, profile,
            0
        };

        if (lk_platform->opengl.major_version < 3)
        {
            // Don't set the profile or flags for legacy OpenGL.
            attributes[4] = 0;
        }

        real_context = gl->wglCreateContextAttribsARB(dc, 0, attributes);
        if (real_context)
        {
            if (gl->wglMakeCurrent(dc, real_context))
            {
                success = 1;
            }
            else
            {
                LK_Log(errors, "Failed to create a modern OpenGL context; wglMakeCurrent() failed.");
                gl->wglDeleteContext(real_context);
            }
        }
        else
        {
            LK_Log(errors, "Failed to create a modern OpenGL context; wglCreateContextAttribsARB() failed.");
        }
    }

undo_fake_context:
    gl->wglDeleteContext(fake_context);
undo_fake_dc:
    ReleaseDC(fake_window, fake_dc);
undo_fake_window:
    DestroyWindow(fake_window);
    return success;
}


////////////////////////////////////////////////////////////////////////////////
// Client thread
////////////////////////////////////////////////////////////////////////////////


static void lk_get_command_line_arguments(LK_Platform* lk_platform, LK_Circular_Buffer* errors)
{
    HMODULE shell32 = LoadLibraryA("shell32.dll");
    if (!shell32)
    {
        LK_Log(errors, "Couldn't get command line arguments - LoadLibraryA failed to load shell32.dll.");
        return;
    }

    LPWSTR command_line = GetCommandLineW();

    LPWSTR*(WINAPI *CommandLineToArgvW)(LPCWSTR lpCmdLine, int* pNumArgs);
    LK_GetProc(shell32, CommandLineToArgvW, "CommandLineToArgvW");
    if (!CommandLineToArgvW)
    {
        FreeLibrary(shell32);
        return;
    }

    int argc;
    LPWSTR* argv = CommandLineToArgvW(command_line, &argc);
    if (!argv)
    {
        FreeLibrary(shell32);
        return;
    }

    int allocation_size = argc * sizeof(char*);
    for (int i = 0; i < argc; i++)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, 0, 0, 0, 0);
        if (size == 0)
        {
            LocalFree(argv);
            FreeLibrary(shell32);
            return;
        }

        allocation_size += size;
    }

    LK_U8* memory = (LK_U8*) LocalAlloc(LMEM_FIXED, allocation_size);
    if (!memory)
    {
        LocalFree(argv);
        FreeLibrary(shell32);
        return;
    }

    char** result = (char**) memory;

    int available = allocation_size - argc * sizeof(char*);
    LPSTR write_cursor = (LPSTR)(memory + argc * sizeof(char*));

    for (int i = 0; i < argc; i++)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, write_cursor, available, 0, 0);
        if (size == 0)
        {
            LocalFree(memory);
            LocalFree(argv);
            FreeLibrary(shell32);
            return;
        }

        result[i] = write_cursor;
        write_cursor += size;
        available -= size;
    }

    LocalFree(argv);
    FreeLibrary(shell32);

    lk_platform->command_line.argument_count = argc;
    lk_platform->command_line.arguments = result;
}

static void lk_fill_system_info(LK_Platform* lk_platform)
{
    // @Incomplete - On x86, check if we have cpuid at all.
    void __cpuid(int[4], int);
    unsigned __int64 _xgetbv(unsigned int);

    LK_U32 id0[4];
    LK_U32 id1[4];
    LK_U32 id7[4];
    LK_U32 id80000000[4];
    LK_U32 id80000001[4];

    LK_B32 os_saves_ymm = 0;
    LK_B32 os_saves_zmm = 0;

    __cpuid((int*) id0, 0);
    LK_U32 max_function = id0[0];

    //
    // check OS feature support
    //

    if (max_function >= 1)
    {
        __cpuid((int*) id1, 1);

        if (id1[2] & 0x8000000) // supports xgetbv
        {
            int a = (int) _xgetbv(0);
            os_saves_ymm = ((a & 6) == 6) ? 1 : 0;
            os_saves_zmm = (os_saves_ymm && ((a & 0xe0) == 0xe0)) ? 1 : 0;
        }
    }

    if (max_function >= 7)
    {
        __cpuid((int*) id7, 7);
    }

    __cpuid((int*) id80000000, 0x80000000);
    if (id80000000[0] >= 0x80000001)
    {
        __cpuid((int*) id80000001, 0x80000001);
    }

    //
    // get vendor string
    //

    char vendor[13];
    *(LK_U32*) &vendor[0] = id0[1];
    *(LK_U32*) &vendor[4] = id0[3];
    *(LK_U32*) &vendor[8] = id0[2];
    vendor[12] = 0;

    lk_platform->system.vendor = vendor;

    //
    // get core/logical count
    //

    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    lk_platform->system.logical_count = system_info.dwNumberOfProcessors;

    //
    // get cache size
    //

    if (max_function >= 1)
    {
        lk_platform->system.cache_line_size = ((id1[1] >> 8) & 0xFF) * 8;
    }

    //
    // check feature support
    //

    if (max_function >= 1)
    {
        lk_platform->system.has_rdtsc = (id1[3] & 0x00000010u) ? 1 : 0;
        lk_platform->system.has_mmx   = (id1[3] & 0x00800000u) ? 1 : 0;
        lk_platform->system.has_sse   = (id1[3] & 0x02000000u) ? 1 : 0;
        lk_platform->system.has_sse2  = (id1[3] & 0x04000000u) ? 1 : 0;
        lk_platform->system.has_hyperthreading = (id1[3] & 0x10000000u) ? 1 : 0;
        lk_platform->system.has_sse3  = (id1[2] & 0x00000001u) ? 1 : 0;
        lk_platform->system.has_sse41 = (id1[2] & 0x00080000u) ? 1 : 0;
        lk_platform->system.has_sse42 = (id1[2] & 0x00100000u) ? 1 : 0;

        if (os_saves_ymm)
        {
            lk_platform->system.has_avx = (id1[2] & 0x10000000u) ? 1 : 0;

            if (max_function >= 7)
            {
                lk_platform->system.has_avx2    = (id7[1] & 0x00000020u) ? 1 : 0;
                lk_platform->system.has_avx512f = (id7[1] & 0x00010000u) && os_saves_zmm;
            }
        }
    }

    if (id80000000[0] >= 0x80000001)
    {
        if (id80000001[3] & 0x80000000)
        {
            lk_platform->system.has_3dnow = 1;
        }
    }

    //
    // get memory info
    //

    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);
    if (GlobalMemoryStatusEx(&memory_status))
    {
        lk_platform->system.ram_bytes     = memory_status.ullTotalPhys;
        lk_platform->system.ram_kilobytes = memory_status.ullTotalPhys / 1024;
        lk_platform->system.ram_megabytes = memory_status.ullTotalPhys / (1024 * 1024);
    }
}


void lk_entry(LK_Client_Functions* functions)
{
    // Get the instance serial number.
    LONG serial = 0;
    HANDLE semaphore = CreateSemaphoreA(NULL, 0, 2147483647, "lk_platform_instance_serial");
    if (semaphore)
        ReleaseSemaphore(semaphore, 1, &serial);

    // Make context structures.
    LK_Platform platform = { 0 };
    LK_Window_Context window = { 0 };
    LK_Time_Context time = { 0 };
    LK_Audio_Context audio_render = { 0 };
    LK_Audio_Context audio_capture = { 0 };
    LK_OpenGL_Context opengl = { 0 };
    LK_Gamepad_Context gamepad = { 0 };

    LK_Circular_Buffer errors = { 0 };

    // Load the client.
    LK_Client client = { 0 };
    lk_load_client(&platform, &client, functions, serial, &errors);

    // Initialize lk_platform with defaults and info.
    {
        platform.window.title = "app";
        platform.window.x = LK_DEFAULT_POSITION;
        platform.window.y = LK_DEFAULT_POSITION;

        platform.keyboard.text = "";

        platform.opengl.color_bits   = 32;
        platform.opengl.depth_bits   = 24;
        platform.opengl.stencil_bits = 8;
        platform.opengl.sample_count = 1;

        lk_initialize_timer(&time);

        lk_get_command_line_arguments(&platform, &errors);
        lk_fill_system_info(&platform);
    }

    client.functions.init(&platform);

    //
    // Open the window.
    //
    LK_Window_Backend backend = platform.window.backend;
    LK_B32 has_window = !platform.window.no_window;
    if (has_window)
    {
        window.opened  = CreateEventW(0, 0, 0, 0);
        window.close   = CreateEventW(0, 0, 0, 0);
        window.unpause = CreateEventW(0, 0, 0, 0);

        window.client               = &client;
        window.instance_serial      = serial;
        window.disable_animations   = platform.window.disable_animations;
        window.transparent          = platform.window.transparent;
        window.disable_windows_keys = platform.keyboard.disable_windows_keys;
        window.icon_width           = platform.window.icon_width;
        window.icon_height          = platform.window.icon_height;
        window.icon_pixels          = platform.window.icon_pixels;

        window.thread = CreateThread(0, 0, lk_window_thread, &window, 0, 0);
        WaitForSingleObject(window.opened, INFINITE);
    }

    BITMAPINFO canvas_bitmap = { 0 };

    //
    // Start audio threads.
    //
    LK_B32 is_rendering_audio = (client.functions.render_audio != lk_client_render_audio_stub);
    if (is_rendering_audio)
        lk_start_audio_thread(&platform, &audio_render, &client, 0 /* not capture */);

    LK_B32 is_capturing_audio = (client.functions.capture_audio != lk_client_capture_audio_stub);
    if (is_capturing_audio)
        lk_start_audio_thread(&platform, &audio_capture, &client, 1 /* not capture */);

    // Prepare for gamepad input handling.
    if (has_window)
        lk_init_gamepad_support(&platform, &gamepad);

    // Variables to keep track of previous window state.
    LK_S32 last_window_x;
    LK_S32 last_window_y;
    LK_U32 last_window_width;
    LK_U32 last_window_height;
    lk_get_client_rectangle(window.hwnd, &last_window_x, &last_window_y, &last_window_width, &last_window_height);

    // resolve default window size
    if (!platform.window.width && !platform.window.height)
    {
        platform.window.width  = last_window_width;
        platform.window.height = last_window_height;
    }

    // resolve default window position
    if (platform.window.x == LK_DEFAULT_POSITION || platform.window.y == LK_DEFAULT_POSITION)
    {
        LK_S32 monitor_x, monitor_y;
        LK_U32 monitor_width, monitor_height;
        lk_get_monitor_rectangle(0, &monitor_x, &monitor_y, &monitor_width, &monitor_height);

        RECT window_bounds = { 0, 0, (LONG) platform.window.width, (LONG) platform.window.height };
        AdjustWindowRectEx(&window_bounds, WS_OVERLAPPED, 0, 0);

        platform.window.x = monitor_x + (monitor_width  - (window_bounds.right - window_bounds.left)) / 2;
        platform.window.y = monitor_y + (monitor_height - (window_bounds.bottom - window_bounds.top)) / 2;
    }

    LK_B32 last_window_fullscreen = 0;
    LK_S32 window_x_before_fullscreen;
    LK_S32 window_y_before_fullscreen;
    LK_U32 window_width_before_fullscreen;
    LK_U32 window_height_before_fullscreen;

    LONG   last_window_style     = 0;
    LK_B32 last_window_invisible = 1;

    // Variables to keep track of previous mouse state.
    LK_U32 last_mouse_wheel = 0;
    POINT last_raw_cursor = { 0, 0 };
    POINT last_cursor;
    GetCursorPos(&last_cursor);

    // Variables to keep text input.
    char text_buffer[LK_CIRCULAR_BUFFER_SIZE + 1];

    //
    // Run frame loop.
    //
    LK_B32 first_frame = 1;
    while (!platform.break_frame_loop && !window.wants_to_close)
    {
        LK_U64 flip_time = lk_get_time_stamp(&time);

        if (platform.pause_frame_loop)
        {
            DWORD timeout = (DWORD)(platform.pause_timeout_seconds * 1000.0f + 0.5f);
            WaitForSingleObject(window.unpause, timeout);
        }

        platform.event_mask = 0;

        //
        // Apply changes to window state.
        //
        if (has_window)
        {
            // If there's a new title, set it.
            if (platform.window.title)
            {
                SetWindowTextA(window.hwnd, platform.window.title);
                platform.window.title = NULL;
            }

            // If fullscreen is toggled, remember/restore where the window was before entering fullscreen.
            LK_B32 toggle_fullscreen = (last_window_fullscreen != platform.window.fullscreen);
            if (toggle_fullscreen)
            {
                if (platform.window.fullscreen)
                {
                    window_x_before_fullscreen      = platform.window.x;
                    window_y_before_fullscreen      = platform.window.y;
                    window_width_before_fullscreen  = platform.window.width;
                    window_height_before_fullscreen = platform.window.height;
                    lk_get_monitor_rectangle(window.hwnd, &platform.window.x, &platform.window.y, &platform.window.width, &platform.window.height);
                }
                else
                {
                    platform.window.x      = window_x_before_fullscreen;
                    platform.window.y      = window_y_before_fullscreen;
                    platform.window.width  = window_width_before_fullscreen;
                    platform.window.height = window_height_before_fullscreen;
                }
            }

            // Determine desired style.
            LONG style = GetWindowLong(window.hwnd, GWL_STYLE);
            {
                const LONG RESIZABLE_FLAGS  = WS_THICKFRAME | WS_MAXIMIZEBOX;
                const LONG DECORATION_FLAGS = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
                const LONG FULLSCREEN_FLAGS = WS_POPUP | WS_MAXIMIZE;

                style &= ~(RESIZABLE_FLAGS | DECORATION_FLAGS | FULLSCREEN_FLAGS);

                if (platform.window.fullscreen)
                {
                    style |= FULLSCREEN_FLAGS;
                }
                else
                {
                    if (!platform.window.undecorated)     style |= DECORATION_FLAGS;
                    if (!platform.window.forbid_resizing) style |= RESIZABLE_FLAGS;
                }
            }

            // If anything changed, update the window position and style...
            if (style != last_window_style ||
                platform.window.invisible != last_window_invisible ||
                platform.window.x      != last_window_x ||
                platform.window.y      != last_window_y ||
                platform.window.width  != last_window_width ||
                platform.window.height != last_window_height)
            {
                LONG extended_style = GetWindowLong(window.hwnd, GWL_EXSTYLE);
                SetWindowLong(window.hwnd, GWL_STYLE, style);

                // client dimensions to window dimensions
                RECT window_bounds;
                window_bounds.left = 0;
                window_bounds.top = 0;
                window_bounds.right = (LONG) platform.window.width;
                window_bounds.bottom = (LONG) platform.window.height;
                AdjustWindowRectEx(&window_bounds, style, 0, extended_style);

                LK_S32 x      = platform.window.x + window_bounds.left;
                LK_S32 y      = platform.window.y + window_bounds.top;
                LK_U32 width  = window_bounds.right  - window_bounds.left;
                LK_U32 height = window_bounds.bottom - window_bounds.top;

                // move and resize the window
                UINT swp_flags = SWP_NOOWNERZORDER | SWP_FRAMECHANGED | (toggle_fullscreen ? 0 : SWP_NOZORDER);
                SetWindowPos(window.hwnd, HWND_TOP, x, y, width, height, swp_flags);
                ShowWindow(window.hwnd, platform.window.invisible ? SW_HIDE : SW_SHOW);

                // if shown on first frame, request focus
                if (first_frame && !platform.window.invisible)
                {
                    SetForegroundWindow(window.hwnd);
                    SetFocus(window.hwnd);
                }
            }

            last_window_style = style;
            last_window_fullscreen = platform.window.fullscreen;
            last_window_invisible = platform.window.invisible;

            // Prevent screen savers and monitor power saving when the app is in fullscreen.
            window.prevent_screen_saver = platform.window.fullscreen;
        }

        //
        // If this is our first frame, prepare the window for rendering.
        // Initialize OpenGL, or DC pixel format.
        //
        if (has_window && first_frame)
        {
            if (backend == LK_WINDOW_OPENGL)
            {
                if (lk_load_opengl_library(&platform, &opengl, &errors))
                    if (!lk_create_modern_opengl_context(&platform, &window, &opengl, &errors))
                        lk_create_legacy_opengl_context(&platform, &window, &opengl, &errors);
            }
            else
            {
                HDC dc = window.dc;

                PIXELFORMATDESCRIPTOR pixel_format_desc = { 0 };
                pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
                pixel_format_desc.nVersion = 1;
                pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
                pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
                pixel_format_desc.cColorBits = 32;
                pixel_format_desc.iLayerType = PFD_MAIN_PLANE;

                int pixel_format = ChoosePixelFormat(dc, &pixel_format_desc);
                if (!pixel_format)
                {
                    LK_Log(&errors, "Failed to initialize the window; ChoosePixelFormat() failed.");
                    break;
                }
                if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
                {
                    LK_Log(&errors, "Failed to initialize the window; SetPixelFormat() failed.");
                    break;
                }
            }
        }

        //
        // Update state.
        //
        if (has_window)
        {
            platform.window.handle    = window.hwnd;
            platform.window.has_focus = window.focus;

            // update client rectangle
            LK_S32 x, y;
            LK_U32 width, height;
            lk_get_client_rectangle(window.hwnd, &x, &y, &width, &height);

            if (x     != last_window_x     || y      != last_window_y     ) platform.event_mask |= LK_EVENT_WINDOW_MOVE;
            if (width != last_window_width || height != last_window_height) platform.event_mask |= LK_EVENT_WINDOW_SIZE;

            platform.window.x      = last_window_x      = x;
            platform.window.y      = last_window_y      = y;
            platform.window.width  = last_window_width  = width;
            platform.window.height = last_window_height = height;

            // update monitor information, compute width and height in inches
            LK_F32 dpi = (LK_F32) window.dpi;
            platform.window.dpi              = dpi;
            platform.window.width_in_inches  = (LK_F32) width  / dpi;
            platform.window.height_in_inches = (LK_F32) height / dpi;
            window.no_resize_on_dpi_change = platform.window.no_resize_on_dpi_change;

            platform.window.monitor_refresh_rate = window.monitor_refresh_rate;

            // resize canvas if necessary
            if (backend == LK_WINDOW_CANVAS)
            {
                BITMAPINFOHEADER* header = &canvas_bitmap.bmiHeader;
                if (width != header->biWidth || height != header->biHeight)
                {
                    if (platform.canvas.data)
                    {
                        VirtualFree(platform.canvas.data, 0, MEM_RELEASE);
                    }

                    header->biSize        = sizeof(BITMAPINFOHEADER);
                    header->biWidth       = width;
                    header->biHeight      = height;
                    header->biPlanes      = 1;
                    header->biBitCount    = 32;
                    header->biCompression = BI_RGB;

                    void* canvas = VirtualAlloc(0, width * height * 4, MEM_COMMIT, PAGE_READWRITE);
                    platform.canvas.data = (LK_U8*) canvas;
                }

                platform.canvas.width  = width;
                platform.canvas.height = height;
            }

            // update mouse cursor
            {
                POINT cursor;
                GetCursorPos(&cursor);

                platform.mouse.delta_x = cursor.x - last_cursor.x;
                platform.mouse.delta_y = cursor.y - last_cursor.y;
                last_cursor = cursor;

                platform.mouse.x = cursor.x - platform.window.x;
                platform.mouse.y = cursor.y - platform.window.y;

                if (window.has_raw_mouse_input)
                {
                    POINT raw_cursor = window.mouse_raw_cursor;
                    platform.mouse.delta_x_raw = raw_cursor.x - last_raw_cursor.x;
                    platform.mouse.delta_y_raw = raw_cursor.y - last_raw_cursor.y;
                    last_raw_cursor = cursor;
                }
                else
                {
                    platform.mouse.delta_x_raw = platform.mouse.delta_x;
                    platform.mouse.delta_y_raw = platform.mouse.delta_y;
                }

                if (platform.mouse.delta_x || platform.mouse.delta_x_raw ||
                    platform.mouse.delta_y || platform.mouse.delta_y_raw)
                    platform.event_mask |= LK_EVENT_MOUSE_MOVE;
            }

            // update mouse wheel
            {
                LK_U32 wheel = window.mouse_wheel;
                platform.mouse.delta_wheel = wheel - last_mouse_wheel;
                last_mouse_wheel = wheel;

                if (platform.mouse.delta_wheel)
                    platform.event_mask |= LK_EVENT_MOUSE_WHEEL;
            }

            // update mouse buttons
            {
                LK_Digital_Button* l = &platform.mouse.left_button;
                LK_Digital_Button* r = &platform.mouse.right_button;
                LK_Digital_Button* m = &platform.mouse.middle_button;

                lk_update_digital_button(l, window.mouse_down[0]);
                lk_update_digital_button(r, window.mouse_down[1]);
                lk_update_digital_button(m, window.mouse_down[2]);

                if (l->down     || r->down     || m->down    ) platform.event_mask |= LK_EVENT_MOUSE_DOWN;
                if (l->pressed  || r->pressed  || m->pressed ) platform.event_mask |= LK_EVENT_MOUSE_PRESS;
                if (l->released || r->released || m->released) platform.event_mask |= LK_EVENT_MOUSE_RELEASE;
            }

            // update keyboard
            {
                LK_B8 repeated[LK__KEY_COUNT + 4];

                // exchange repeated array with zeroes
                LONG volatile* read_cursor = (LONG volatile*) &window.keys_repeated[0];
                LONG* write_cursor = (LONG*) &repeated[0];
                for (int key_index = 0; key_index < LK__KEY_COUNT; key_index += 4)
                    *(write_cursor++) = InterlockedExchange(read_cursor++, 0);

                for (int key_index = 0; key_index < LK__KEY_COUNT; key_index++)
                {
                    LK_Digital_Button* button = &platform.keyboard.state[key_index];
                    lk_update_digital_button(button, window.keys_down[key_index]);
                    button->repeated |= button->down && repeated[key_index];

                    if (button->down)     platform.event_mask |= LK_EVENT_KEYBOARD_DOWN;
                    if (button->pressed)  platform.event_mask |= LK_EVENT_KEYBOARD_PRESS;
                    if (button->released) platform.event_mask |= LK_EVENT_KEYBOARD_RELEASE;
                }
            }

            // update text input
            {
                LK_U32 length = lk_available_in_circular_buffer(&window.text);
                lk_read_from_circular_buffer(&window.text, text_buffer, length);
                text_buffer[length] = 0;

                platform.keyboard.text = text_buffer;
                if (length)
                    platform.event_mask |= LK_EVENT_KEYBOARD_TEXT;
            }

            lk_update_gamepads(&platform, &window, &gamepad, &errors);
        }

        //
        // Call the client.
        //
        {
            lk_update_client(&platform, &client, &errors);

            // lk_client_log
            LK_Circular_Buffer* buffers[] = { &window.errors, &audio_render.errors, &audio_capture.errors, &errors };
            LK_U32 buffer_count = sizeof(buffers) / sizeof(buffers[0]);
            for (LK_U32 buffer_index = 0; buffer_index < buffer_count; buffer_index++)
            {
                LK_Circular_Buffer* buffer = buffers[buffer_index];
                while (lk_available_in_circular_buffer(buffer))
                {
                    LK_Log_Line line;
                    lk_read_from_circular_buffer(buffer, &line, sizeof(line));
                    client.functions.log(&platform, line.message, __FILE__, line.line);
                }
            }

            platform.pause_frame_loop = 0;
            platform.pause_timeout_seconds = 0.1f;
            platform.window.swap_buffers = 1;

            // lk_client_first_frame
            if (first_frame)
            {
                lk_update_time_stamp(&time, &platform);
                client.functions.first_frame(&platform);
            }

            // lk_client_frame
            if (!platform.break_frame_loop)
            {
                lk_update_time_stamp(&time, &platform);
                client.functions.frame(&platform);
            }
        }

        lk_set_audio_paused(&audio_render,  platform.audio.pause_render);
        lk_set_audio_paused(&audio_capture, platform.audio.pause_capture);

        //
        // Swap buffers.
        //
        if (window.dc && platform.window.swap_buffers)
        {
            if (backend == LK_WINDOW_CANVAS)
            {
                void* pixels = platform.canvas.data;
                if (pixels)
                {
                    StretchDIBits(window.dc,
                        0, 0, platform.window.width, platform.window.height,
                        0, 0, platform.canvas.width, platform.canvas.height,
                        pixels, &canvas_bitmap,
                        DIB_RGB_COLORS, SRCCOPY);
                }
            }
            else if (backend == LK_WINDOW_OPENGL)
            {
                // update v-sync
                if (opengl.wglSwapIntervalEXT)
                {
                    int interval = platform.opengl.swap_interval;
                    opengl.wglSwapIntervalEXT(interval);
                }
            }

            SwapBuffers(window.dc);
        }

        first_frame = 0;
    }

    // Inform the client that we're closing.
    lk_update_time_stamp(&time, &platform);
    client.functions.close(&platform);

    // Destroy canvas.
    if (backend == LK_WINDOW_CANVAS)
        if (platform.canvas.data)
            VirtualFree(platform.canvas.data, 0, MEM_RELEASE);

    // Uninitialize OpenGL.
    if (backend == LK_WINDOW_OPENGL)
        lk_unload_opengl_library(&opengl);

    // Close audio threads.
    if (is_rendering_audio)
        lk_stop_audio_thread(&audio_render);
    if (is_capturing_audio)
        lk_stop_audio_thread(&audio_capture);

    // Close gamepads.
    lk_uninit_gamepad_support(&gamepad);

    // Close the window.
    if (has_window)
    {
        PostMessageA(window.hwnd, WM_QUIT, 0, 0);
        SetEvent(window.close);
        WaitForSingleObject(window.thread, INFINITE);

        CloseHandle(window.thread);
        CloseHandle(window.opened);
        CloseHandle(window.close);
        CloseHandle(window.unpause);
    }

    lk_unload_client(&platform, &client);
}

#ifndef LK_PLATFORM_NO_MAIN
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    lk_entry(NULL);
    return 0;
}

int main(int argc, char** argv)
{
    lk_entry(NULL);
    return 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif


/*********************************************************************************************

THE UNLICENCE (http://unlicense.org)

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
    software, either in source code form or as a compiled binary, for any purpose,
    commercial or non-commercial, and by any means.

    In jurisdictions that recognize copyright laws, the author or authors of this
    software dedicate any and all copyright interest in the software to the public
    domain. We make this dedication for the benefit of the public at large and to
    the detriment of our heirs and successors. We intend this dedication to be an
    overt act of relinquishment in perpetuity of all present and future rights to
    this software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
    ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 *********************************************************************************************/
