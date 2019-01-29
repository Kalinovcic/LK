//  lk_platform.h - public domain platform abstraction layer with live code editing support
//  no warranty is offered or implied

/*********************************************************************************************

Include this file in all places you need to refer to it. In one of your compilation units, write:
    #define LK_PLATFORM_IMPLEMENTATION
before including lk_platform.h, in order to paste in the source code.

This library provides a platform layer with support for live code editing. That enables you to
modify your source code, recompile it, and see the changes happen live (on the next frame),
without restarting the application. In order for this to work, you should compile this
platform layer to an EXE, and compile the rest of your application code to a DLL. The platform
layer will then load the DLL dynamically at runtime, and reload it if it detects a change.

If you want live code editing, you must specify the DLL name (not including the extension) by defining:
    #define LK_PLATFORM_DLL_NAME "my_application_dll"
before including the implementation of lk_platform.h.

If you don't want live code editing, you must disable it by defining:
    #define LK_PLATFORM_NO_DLL
before including the implementation of lk_platform.h.

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

If you're doing audio output using the LK_AUDIO_CALLBACK strategy, you should expose:

    LK_CLIENT_EXPORT void lk_client_audio(LK_Platform* platform, LK_S16* samples);  // called every so often on a separate thread,
                                                                                       you should fill out the samples array

If you want lk_platform to use your logging API, you should expose:

    LK_CLIENT_EXPORT void lk_client_log(LK_Platform* platform, const char* message, const char* file, int line);

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
    LK_B8 pressed;
    LK_B8 released;
    LK_B8 down;
    LK_B8 was_down; // The state of "down" for the previous frame.
    LK_B8 repeated; // true when "pressed" is true, or when a repeat event happened
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
    LK_MAX_GAMEPADS = 8,
};

enum
{
    // Xbox controller buttons
    LK_GAMEPAD_XBOX_A = 0,
    LK_GAMEPAD_XBOX_B,
    LK_GAMEPAD_XBOX_X,
    LK_GAMEPAD_XBOX_Y,
    LK_GAMEPAD_XBOX_LB,
    LK_GAMEPAD_XBOX_RB,
    LK_GAMEPAD_XBOX_VIEW,
    LK_GAMEPAD_XBOX_MENU,
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
    LK_GAMEPAD_PS_L3,
    LK_GAMEPAD_PS_R3,

    LK_GAMEPAD_BUTTON_COUNT
};

enum
{
    // Xbox controller analogs
    LK_GAMEPAD_XBOX_LEFT_STICK_X = 0,
    LK_GAMEPAD_XBOX_LEFT_STICK_Y,
    LK_GAMEPAD_XBOX_RIGHT_STICK_X,
    LK_GAMEPAD_XBOX_RIGHT_STICK_Y,
    LK_GAMEPAD_XBOX_TRIGGER,

    // PlayStation controller analogs
    LK_GAMEPAD_PS_LEFT_STICK_X = 0,
    LK_GAMEPAD_PS_LEFT_STICK_Y,
    LK_GAMEPAD_PS_RIGHT_STICK_X,
    LK_GAMEPAD_PS_RIGHT_STICK_Y,
    LK_GAMEPAD_PS_LR2,

    LK_GAMEPAD_ANALOG_COUNT
};

struct LK_Analog_Stick
{
    LK_F64 x;
    LK_F64 y;
};

struct LK_Gamepad
{
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
};

enum
{
    LK_DEFAULT_POSITION = 0x80000000,
};

typedef enum
{
    LK_NO_AUDIO,
    LK_AUDIO_CALLBACK,
    LK_AUDIO_MIXER,
} LK_Audio_Strategy;

typedef struct
{
    LK_S16* samples;
    LK_U32 count;
    LK_U32 channels;
    LK_U32 frequency;
} LK_Wave;

typedef struct
{
    LK_B32 playing;
    LK_Wave wave;
    LK_B32 loop;
    LK_F32 volume;
} LK_Sound;

enum
{
    LK_MIXER_SLOT_COUNT = 32,
};

typedef struct LK_Platform_Structure
{
    LK_B32 break_frame_loop;
    void* client_data;

    struct
    {
        LK_B32 no_window;
        LK_Window_Backend backend;

        void* handle; // HWND on Windows

        char* title;

        // During initialization, x and y will be set to LK_DEFAULT_POSITION, width and height will be set to zero.
        // You may modify these to specify initial window coordinates or dimensions. Otherwise, the window will be
        // centered on the primary monitor and get some default dimensions. During execution, changing these will
        // move or resize the window.
        // All of these are relative to the client space, not the actual window.
        LK_S32 x;
        LK_S32 y;
        LK_U32 width;
        LK_U32 height;

        LK_B32 fullscreen;
        LK_B32 forbid_resizing;
        LK_B32 undecorated;
        LK_B32 invisible;
        LK_B32 disable_animations;

        // Changing 'transparent' during execution (after initialization) doesn't have an effect.
        // Transparency on Windows is only available for undecorated windows.
        LK_B32 transparent;

        // The icon is set once, when the window is created, if icon_pixels is set.
        LK_U32 icon_width;
        LK_U32 icon_height;
        LK_U8* icon_pixels; // must be RGBA, left to right, top to bottom
    } window;

    struct
    {
        LK_S32 x;
        LK_S32 y;
        LK_S32 delta_x;
        LK_S32 delta_y;
        LK_S32 delta_wheel;

        LK_Digital_Button left_button;
        LK_Digital_Button right_button;
    } mouse;

    struct
    {
        LK_Digital_Button state[LK__KEY_COUNT];
        char* text; // UTF-8 formatted string.
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
        LK_Audio_Strategy strategy;
        LK_B32 silent_when_not_focused;

        LK_U32 channels;
        LK_U32 frequency;
        LK_U32 sample_count;

        LK_Sound mixer_slots[LK_MIXER_SLOT_COUNT];
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

#ifdef LK_PLATFORM_USER_CONTEXT
    LK_PLATFORM_USER_CONTEXT
#endif
} LK_Platform;

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

#ifndef LK_WINDOW_CLASS_NAME
#define LK_WINDOW_CLASS_NAME L"lk_platform_window_class"
#endif

#if !defined(_WIN32_WINNT) && !defined(WINVER)
// Targeting Windows XP by default.
#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#endif

#include <windows.h> // @Incomplete - get rid of this include
#include <dsound.h> // @Incomplete - get rid of this include
#include <intrin.h> // @Incomplete - get rid of this include
#include <hidclass.h> // @Incomplete - get rid of this include
#include <hidsdi.h> // @Incomplete - get rid of this include
#include <hidpi.h> // @Incomplete - get rid of this include


typedef void LK_Client_Init_Function(LK_Platform* platform);
typedef void LK_Client_Close_Function(LK_Platform* platform);
typedef void LK_Client_Frame_Function(LK_Platform* platform);
typedef void LK_Client_Audio_Function(LK_Platform* platform, LK_S16* samples);
typedef void LK_Client_Log(LK_Platform* platform, const char* message, const char* file, int line);
typedef void LK_Client_Dll_Load_Function(LK_Platform* platform);
typedef void LK_Client_Dll_Unload_Function(LK_Platform* platform);

enum
{
    LK_MAX_TEXT_SIZE = 256,
};

typedef enum
{
    LK_NOT_PLAYING,
    LK_PLAYING,
    LK_FINISHED,
} LK_Playing_State;

typedef struct
{
    LK_Playing_State state;

    LK_Wave wave;
    LK_B32 loop;
    LK_F32 volume;

    LK_F64 cursor;
    LK_F64 cursor_step;
} LK_Playing_Sound;

typedef struct
{
    struct
    {
        FILETIME last_dll_write_time;
        CHAR dll_path[16384];      // @Incomplete - this is dumb and unsafe
        CHAR temp_dll_path[16384]; // @Incomplete - this is dumb and unsafe

        HMODULE library;
        LK_Client_Init_Function* init;
        LK_Client_Close_Function* close;
        LK_Client_Frame_Function* frame;
        LK_Client_Audio_Function* audio;
        LK_Client_Log* log;
        LK_Client_Dll_Load_Function* dll_load;
        LK_Client_Dll_Unload_Function* dll_unload;

        LK_B32 load_failed;
    } client;

    struct
    {
        HWND handle;
        HDC dc;

        HANDLE main_fiber;
        HANDLE message_fiber;

        LK_Window_Backend backend;
        BITMAPINFO bitmap_info;

        char* title;

        LK_S32 x;
        LK_S32 y;
        LK_U32 width;
        LK_U32 height;

        LK_S32 x_before_fullscreen;
        LK_S32 y_before_fullscreen;
        LK_U32 width_before_fullscreen;
        LK_U32 height_before_fullscreen;

        LK_B32 fullscreen;
        LK_B32 forbid_resizing;
        LK_B32 undecorated;
        LK_B32 invisible;
    } window;

    struct
    {
        int text_size;
        char text_buffer[LK_MAX_TEXT_SIZE];
    } keyboard;

    struct
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
    } opengl;

    struct
    {
        LPDIRECTSOUNDBUFFER secondary_buffer;
        LK_U32 secondary_buffer_size;
        LK_U32 sample_buffer_size;
        LK_U32 sample_buffer_count;

        HANDLE mixer_mutex;
        LK_Playing_Sound mixer_slots[LK_MIXER_SLOT_COUNT];
    } audio;

    struct
    {
        LK_U64 initial_ticks;
        LK_U64 ticks_per_second;

        LK_U64 unprocessed_nanoseconds;
        LK_U64 unprocessed_microseconds;
        LK_U64 unprocessed_milliseconds;
    } time;
} LK_Platform_Private;

static LK_Platform lk_platform;
static LK_Platform_Private lk_private;


#define LK_Log(message)  lk_private.client.log(&lk_platform, message, __FILE__, __LINE__)

#define LK_GetProc(module, destination, name)    \
{                                                \
    FARPROC proc = GetProcAddress(module, name); \
    if (proc)                                    \
        *(FARPROC*) &destination = proc;         \
}


static void lk_client_init_stub(LK_Platform* platform) {}
static void lk_client_frame_stub(LK_Platform* platform) {}
static void lk_client_close_stub(LK_Platform* platform) {}
static void lk_client_dll_load_stub(LK_Platform* platform) {}
static void lk_client_dll_unload_stub(LK_Platform* platform) {}

static void lk_client_audio_stub(LK_Platform* platform, LK_S16* samples)
{
    ZeroMemory(samples, platform->audio.sample_count * platform->audio.channels * 2);
}

static void lk_client_log_stub(LK_Platform* platform, const char* message, const char* file, int line)
{
    OutputDebugStringA(message);
}

static void lk_load_client_functions_from_module(HMODULE module)
{
    lk_private.client.init       = lk_client_init_stub;
    lk_private.client.close      = lk_client_close_stub;
    lk_private.client.frame      = lk_client_frame_stub;
    lk_private.client.audio      = lk_client_audio_stub;
    lk_private.client.log        = lk_client_log_stub;
    lk_private.client.dll_load   = lk_client_dll_load_stub;
    lk_private.client.dll_unload = lk_client_dll_unload_stub;

    lk_private.client.library = module;
    if (module)
    {
        lk_private.client.load_failed = 0;

        #define LK_GetClientFunction(name) LK_GetProc(module, lk_private.client.name, "lk_client_" #name)
        LK_GetClientFunction(init);
        LK_GetClientFunction(close);
        LK_GetClientFunction(frame);
        LK_GetClientFunction(audio);
        LK_GetClientFunction(log);
        LK_GetClientFunction(dll_load);
        LK_GetClientFunction(dll_unload);
        #undef LK_GetClientFunction

        lk_private.client.dll_load(&lk_platform);
    }
    else
    {
        lk_private.client.load_failed = 1;
        LK_Log("Failed to load the client module.");
    }
}

#ifdef LK_PLATFORM_NO_DLL

static void lk_set_client_functions()
{
    HMODULE module = GetModuleHandle(NULL);
    lk_load_client_functions_from_module(module);
}

#else

#ifndef LK_PLATFORM_DLL_NAME
#error "lk_platform.h implementation expects LK_PLATFORM_DLL_NAME to be defined before it is included."
#endif

static void lk_get_dll_path()
{
    const char dll_name[] = LK_PLATFORM_DLL_NAME ".dll";

    LPSTR dll_path = lk_private.client.dll_path;

    DWORD directory_length = GetModuleFileNameA(0, dll_path, sizeof(lk_private.client.dll_path)); // @Incomplete - the buffer could be too small!
    while (dll_path[directory_length] != '\\' && dll_path[directory_length] != '/')
        directory_length--;
    directory_length++;

    CopyMemory(dll_path + directory_length, dll_name, sizeof(dll_name) + 1);
}

static LK_B32 lk_check_client_reload()
{
    if (lk_private.client.load_failed)
    {
        return 1;
    }

    FILETIME file_time;
    ZeroMemory(&file_time, sizeof(FILETIME));

    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFile(lk_private.client.dll_path, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        file_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }

    if (CompareFileTime(&file_time, &lk_private.client.last_dll_write_time) != 0)
    {
        lk_private.client.last_dll_write_time = file_time;
        return 1;
    }

    return 0;
}

static void lk_get_temp_dll_path()
{
    // Get process serial number
    LONG serial = 0;

    const char* semaphore_name = "lk_platform_" LK_PLATFORM_DLL_NAME "_count";
    HANDLE semaphore = CreateSemaphoreA(NULL, 0, 2147483647, semaphore_name);
    if (semaphore)
    {
        ReleaseSemaphore(semaphore, 1, &serial);
    }

    // Copy dll_path over to temp_dll_path
    char* temp_dll_path = lk_private.client.temp_dll_path;

    int dll_path_length = 0;
    for (char* dll_path = lk_private.client.dll_path; *dll_path; dll_path++, temp_dll_path++)
    {
        *temp_dll_path = *dll_path;
        dll_path_length++;
    }

    temp_dll_path -= 4; // Remove trailing ".dll"

    // Append "_temp"
    CopyMemory(temp_dll_path, "_temp", 5);
    temp_dll_path += 5;

    // Append hexadecimal serial number
    const char* hex_lookup = "0123456789abcdef";
    while (serial != 0)
    {
        *(temp_dll_path++) = hex_lookup[serial & 0xF];
        serial /= 16;
    }

    // Append ".dll" and null terminator
    CopyMemory(temp_dll_path, ".dll", 5);
}

static void lk_delete_temp_dll()
{
    LPSTR temp_dll_path = lk_private.client.temp_dll_path;
    if (temp_dll_path[0])
    {
        DeleteFileA(temp_dll_path);
    }
}

static void lk_load_client()
{
    LPSTR dll_path = lk_private.client.dll_path;
    LPSTR temp_dll_path = lk_private.client.temp_dll_path;

    if (!CopyFileA(dll_path, temp_dll_path, 0))
    {
        LK_Log("Failed to copy the client DLL to a temporary location.");
    }

    HMODULE library = LoadLibraryA(temp_dll_path);
    lk_load_client_functions_from_module(library);
}

static void lk_unload_client()
{
    if (lk_private.client.library)
    {
        lk_private.client.dll_unload(&lk_platform);

        if (!FreeLibrary(lk_private.client.library))
        {
            LK_Log("Failed to release the client DLL.");
        }

        lk_private.client.library = 0;
        lk_private.client.init = 0;
        lk_private.client.close = 0;
        lk_private.client.frame = 0;
        lk_private.client.audio = 0;
        lk_private.client.dll_load = 0;
        lk_private.client.dll_unload = 0;
    }

    lk_delete_temp_dll();
}

#endif

static LONG lk_apply_window_style(LONG old_style, LK_B32 ignore_fullscreen)
{
    LONG resizable_flags = WS_THICKFRAME | WS_MAXIMIZEBOX;
    LONG decoration_flags = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    LONG style = old_style & ~(resizable_flags | decoration_flags | WS_POPUP);

    LK_B32 fullscreen = lk_platform.window.fullscreen && !ignore_fullscreen;
    if (!lk_platform.window.undecorated && !fullscreen)
    {
        style |= decoration_flags;
        if (!lk_platform.window.forbid_resizing)
        {
            style |= resizable_flags;
        }
    }
    else
    {
        style |= WS_POPUP;
    }

    return style;
}

static void lk_push_window_data()
{
    HWND window = lk_private.window.handle;

    LK_B32 toggle_fullscreen = (lk_private.window.fullscreen != lk_platform.window.fullscreen);
    if (toggle_fullscreen)
    {
        // remember/restore where the window was before it was toggled to fullscreen
        if (lk_platform.window.fullscreen)
        {
            lk_private.window.x_before_fullscreen = lk_platform.window.x;
            lk_private.window.y_before_fullscreen = lk_platform.window.y;
            lk_private.window.width_before_fullscreen = lk_platform.window.width;
            lk_private.window.height_before_fullscreen = lk_platform.window.height;

            HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO monitor_info;
            monitor_info.cbSize = sizeof(monitor_info);
            GetMonitorInfo(monitor, &monitor_info);

            lk_platform.window.x = monitor_info.rcMonitor.left;
            lk_platform.window.y = monitor_info.rcMonitor.top;
            lk_platform.window.width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
            lk_platform.window.height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
        }
        else
        {
            lk_platform.window.x = lk_private.window.x_before_fullscreen;
            lk_platform.window.y = lk_private.window.y_before_fullscreen;
            lk_platform.window.width = lk_private.window.width_before_fullscreen;
            lk_platform.window.height = lk_private.window.height_before_fullscreen;
        }
    }

    if (toggle_fullscreen ||
        (lk_private.window.x               != lk_platform.window.x              ) ||
        (lk_private.window.y               != lk_platform.window.y              ) ||
        (lk_private.window.width           != lk_platform.window.width          ) ||
        (lk_private.window.height          != lk_platform.window.height         ) ||
        (lk_private.window.forbid_resizing != lk_platform.window.forbid_resizing) ||
        (lk_private.window.undecorated     != lk_platform.window.undecorated    ) ||
        (lk_private.window.invisible       != lk_platform.window.invisible      ))
    {
        LONG style = GetWindowLong(window, GWL_STYLE);
        LONG extended_style = GetWindowLong(window, GWL_EXSTYLE);

        style = lk_apply_window_style(style, 0);
        SetWindowLong(window, GWL_STYLE, style);

        // client dimensions to window dimensions
        LK_U32 width = lk_platform.window.width;
        LK_U32 height = lk_platform.window.height;

        RECT window_bounds;
        window_bounds.left = 0;
        window_bounds.top = 0;
        window_bounds.right = width;
        window_bounds.bottom = height;
        AdjustWindowRectEx(&window_bounds, style, 0, extended_style);

        width = window_bounds.right - window_bounds.left;
        height = window_bounds.bottom - window_bounds.top;

        // move and resize the window
        LK_S32 x = lk_platform.window.x + window_bounds.left;
        LK_S32 y = lk_platform.window.y + window_bounds.top;


        UINT swp_flags = SWP_NOOWNERZORDER | SWP_FRAMECHANGED;
        if (!toggle_fullscreen) swp_flags |= SWP_NOZORDER;
        SetWindowPos(window, HWND_TOP, x, y, width, height, swp_flags);
        ShowWindow(window, lk_platform.window.invisible ? SW_HIDE : SW_SHOW);

        lk_private.window.fullscreen = lk_platform.window.fullscreen;
        lk_private.window.forbid_resizing = lk_platform.window.forbid_resizing;
        lk_private.window.undecorated = lk_platform.window.undecorated;
        lk_private.window.invisible = lk_platform.window.invisible;
    }
}

static void lk_window_update_title()
{
    char* new_title = lk_platform.window.title;
    if (new_title)
    {
        HWND window = lk_private.window.handle;
        SetWindowTextA(window, new_title);

        lk_platform.window.title = NULL;
    }
}

static void lk_push()
{
    HWND window = lk_private.window.handle;
    if (!window)
    {
        return;
    }

    lk_platform.mouse.delta_x = 0;
    lk_platform.mouse.delta_y = 0;
    lk_platform.mouse.delta_wheel = 0;

    lk_private.keyboard.text_size = 0;
    lk_private.keyboard.text_buffer[0] = 0;
    lk_platform.keyboard.text = lk_private.keyboard.text_buffer;

    lk_push_window_data();
    lk_window_update_title();

    for (int key_index = 0; key_index < LK__KEY_COUNT; key_index++)
    {
        lk_platform.keyboard.state[key_index].repeated = 0;
    }
}


static void lk_pull_window_data()
{
    HWND window = lk_private.window.handle;

    // get width and height
    RECT client_rect;
    GetClientRect(window, &client_rect);

    LK_U32 width = (LK_U32)(client_rect.right - client_rect.left);
    LK_U32 height = (LK_U32)(client_rect.bottom - client_rect.top);

    lk_platform.window.width = width;
    lk_platform.window.height = height;
    lk_private.window.width = width;
    lk_private.window.height = height;

    // get X and Y
    POINT window_position = { client_rect.left, client_rect.top };
    ClientToScreen(window, &window_position);

    lk_platform.window.x = window_position.x;
    lk_platform.window.y = window_position.y;
    lk_private.window.x = window_position.x;
    lk_private.window.y = window_position.y;
}

static void lk_pull_mouse_data()
{
    POINT mouse_position;
    GetCursorPos(&mouse_position);

    mouse_position.x -= lk_platform.window.x;
    mouse_position.y -= lk_platform.window.y;

    lk_platform.mouse.x = mouse_position.x;
    lk_platform.mouse.y = mouse_position.y;
}

static void lk_update_digital_button(LK_Digital_Button* button)
{
    button->pressed = (button->down && !button->was_down);
    button->released = (!button->down && button->was_down);
    button->was_down = button->down;
}

static void lk_update_canvas()
{
    LK_U32 width = lk_platform.window.width;
    LK_U32 height = lk_platform.window.height;
    LK_U32 canvas_width = lk_platform.canvas.width;
    LK_U32 canvas_height = lk_platform.canvas.height;

    if (width != canvas_width || height != canvas_height)
    {
        if (lk_platform.canvas.data)
        {
            VirtualFree(lk_platform.canvas.data, 0, MEM_RELEASE);
        }

        BITMAPINFOHEADER* header = &lk_private.window.bitmap_info.bmiHeader;
        header->biSize = sizeof(BITMAPINFOHEADER);
        header->biWidth = width;
        header->biHeight = height;
        header->biPlanes = 1;
        header->biBitCount = 32;
        header->biCompression = BI_RGB;

        LK_U32 bytes_per_pixel = 4;
        LK_U32 canvas_size = width * height * bytes_per_pixel;

        lk_platform.canvas.data = (LK_U8*) VirtualAlloc(0, canvas_size, MEM_COMMIT, PAGE_READWRITE);
        lk_platform.canvas.width = width;
        lk_platform.canvas.height = height;
    }
}

static void lk_pull()
{
    HWND window = lk_private.window.handle;
    if (!window)
    {
        return;
    }

    lk_pull_window_data();
    lk_pull_mouse_data();

    lk_update_digital_button(&lk_platform.mouse.left_button);
    lk_update_digital_button(&lk_platform.mouse.right_button);

    for (int key_index = 0; key_index < LK__KEY_COUNT; key_index++)
    {
        LK_Digital_Button* button = lk_platform.keyboard.state + key_index;
        lk_update_digital_button(button);
    }

    for (int gamepad_index = 0; gamepad_index < LK_MAX_GAMEPADS; gamepad_index++)
    {
        LK_Gamepad* gamepad = &lk_platform.gamepads[gamepad_index];
        for (int button_index = 0; button_index < LK_GAMEPAD_BUTTON_COUNT; button_index++)
        {
            LK_Digital_Button* button = &gamepad->buttons[button_index];
            lk_update_digital_button(button);
        }
    }

    if (lk_private.window.backend == LK_WINDOW_CANVAS)
    {
        lk_update_canvas();
    }
}


static void lk_update_swap_interval()
{
    if (lk_private.opengl.wglSwapIntervalEXT)
    {
        int interval = lk_platform.opengl.swap_interval;
        lk_private.opengl.wglSwapIntervalEXT(interval);
    }
}

static void lk_repaint_canvas_rectangle(HDC device_context, int x, int y, int width, int height)
{
    if (!lk_platform.canvas.data)
    {
        return;
    }

    void* pixels = lk_platform.canvas.data;
    BITMAPINFO* info = &lk_private.window.bitmap_info;

    // @Optimization - actually use the given dirty rectangle
    LK_U32 window_width = lk_platform.window.width;
    LK_U32 window_height = lk_platform.window.height;
    LK_U32 bitmap_width = lk_platform.canvas.width;
    LK_U32 bitmap_height = lk_platform.canvas.height;

    StretchDIBits(device_context,
        0, 0, window_width, window_height,
        0, 0, bitmap_width, bitmap_height,
        pixels, info,
        DIB_RGB_COLORS, SRCCOPY);
}


static LRESULT CALLBACK
lk_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (window != lk_private.window.handle)
    {
        return DefWindowProcW(window, message, wparam, lparam);
    }

    switch (message)
    {

    case WM_CLOSE:
    {
        lk_platform.break_frame_loop = 1;
    } break;

    case WM_CHAR:
    {
        // @Incomplete - Currently doesn't support surrogate pairs.

        WCHAR utf16 = (WCHAR) wparam;
        char utf8[16];

        int length = WideCharToMultiByte(CP_UTF8, 0, &utf16, 1, utf8, sizeof(utf8), 0, 0);

        int old_text_size = lk_private.keyboard.text_size;
        int new_text_size = old_text_size + length;
        if (length && new_text_size < LK_MAX_TEXT_SIZE)
        {
            lk_private.keyboard.text_size = new_text_size;

            char* read = &utf8[0];
            char* write = lk_private.keyboard.text_buffer + old_text_size;
            while (length--)
            {
                *(write++) = *(read++);
            }

            *write = 0;
        }
    } break;

    case WM_INPUT:
    {
        UINT struct_size;
        GetRawInputData((HRAWINPUT) lparam, RID_INPUT, 0, &struct_size, sizeof(RAWINPUTHEADER));

        static char struct_memory[256];
        if (struct_size > sizeof(struct_memory))
        {
            // @Reconsider - I never seem to get any input structures larger than 48 bytes, so 256 bytes should be fine.
            // I don't want to allocate on the heap for input, and I don't want to use alloca because that requires the CRT.
            // We could do some assembly though... But it doesn't seem to be necessary.
            goto run_default_proc;
        }

        if (GetRawInputData((HRAWINPUT) lparam, RID_INPUT, struct_memory, &struct_size, sizeof(RAWINPUTHEADER)) != struct_size)
        {
            goto run_default_proc;
        }

        RAWINPUT* input = (RAWINPUT*) struct_memory;

        if (input->header.dwType == RIM_TYPEMOUSE && input->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
        {
            USHORT button_flags = input->data.mouse.usButtonFlags;

            lk_platform.mouse.delta_x += input->data.mouse.lLastX;
            lk_platform.mouse.delta_y += input->data.mouse.lLastY;

            if (button_flags & RI_MOUSE_LEFT_BUTTON_DOWN)
            {
                lk_platform.mouse.left_button.down = 1;
            }
            if (button_flags & RI_MOUSE_LEFT_BUTTON_UP)
            {
                lk_platform.mouse.left_button.down = 0;
            }

            if (button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
            {
                lk_platform.mouse.right_button.down = 1;
            }
            if (button_flags & RI_MOUSE_RIGHT_BUTTON_UP)
            {
                lk_platform.mouse.right_button.down = 0;
            }

            if (button_flags & RI_MOUSE_WHEEL)
            {
                lk_platform.mouse.delta_wheel += ((SHORT) input->data.mouse.usButtonData) / WHEEL_DELTA;
            }
        }

        if (input->header.dwType == RIM_TYPEKEYBOARD)
        {
            USHORT virtual_key = input->data.keyboard.VKey;
            USHORT scan_code   = input->data.keyboard.MakeCode;
            USHORT flags       = input->data.keyboard.Flags;

            if (virtual_key == 0xFF)
            {
                goto run_default_proc;
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

            int is_e0 = (flags & RI_KEY_E0) != 0;
            int is_e1 = (flags & RI_KEY_E1) != 0;

            if (is_e1)
            {
                // MapVirtualKey is buggy, so we need to set VK_PAUSE manually
                scan_code = (virtual_key == VK_PAUSE) ? 0x45 : MapVirtualKey(virtual_key, MAPVK_VK_TO_VSC);
            }

            switch (virtual_key)
            {
            case VK_CONTROL: virtual_key = is_e0 ? VK_RCONTROL : VK_LCONTROL; break;
            case VK_MENU:    virtual_key = is_e0 ? VK_RMENU    : VK_LMENU;    break;
            case VK_RETURN:  if ( is_e0) virtual_key = VK_RETURN;  break;
            case VK_INSERT:  if (!is_e0) virtual_key = VK_NUMPAD0; break;
            case VK_DELETE:  if (!is_e0) virtual_key = VK_DECIMAL; break;
            case VK_HOME:    if (!is_e0) virtual_key = VK_NUMPAD7; break;
            case VK_END:     if (!is_e0) virtual_key = VK_NUMPAD1; break;
            case VK_PRIOR:   if (!is_e0) virtual_key = VK_NUMPAD9; break;
            case VK_NEXT:    if (!is_e0) virtual_key = VK_NUMPAD3; break;
            case VK_LEFT:    if (!is_e0) virtual_key = VK_NUMPAD4; break;
            case VK_RIGHT:   if (!is_e0) virtual_key = VK_NUMPAD6; break;
            case VK_UP:      if (!is_e0) virtual_key = VK_NUMPAD8; break;
            case VK_DOWN:    if (!is_e0) virtual_key = VK_NUMPAD2; break;
            case VK_CLEAR:   if (!is_e0) virtual_key = VK_NUMPAD5; break;
            }


            LK_Key key = (LK_Key) 0;

            if (virtual_key >= '0' && virtual_key <= '9') key = (LK_Key)((int) LK_KEY_0 + (virtual_key - '0'));
            if (virtual_key >= 'A' && virtual_key <= 'Z') key = (LK_Key)((int) LK_KEY_A + (virtual_key - 'A'));
            if (virtual_key >=  96 && virtual_key <= 105) key = (LK_Key)((int) LK_KEY_NUMPAD_0 + (virtual_key - 96));
            if (virtual_key >= 112 && virtual_key <= 123) key = (LK_Key)((int) LK_KEY_F1 + (virtual_key - 112));

            switch (virtual_key)
            {
            case 27:  key = LK_KEY_ESCAPE;          break;
            case 192: key = LK_KEY_GRAVE;           break;
            case 9:   key = LK_KEY_TAB;             break;
            case 20:  key = LK_KEY_CAPS_LOCK;       break;
            case 160: key = LK_KEY_LEFT_SHIFT;      break;
            case 162: key = LK_KEY_LEFT_CONTROL;    break;
            case 91:  key = LK_KEY_LEFT_WINDOWS;    break;
            case 164: key = LK_KEY_LEFT_ALT;        break;
            case 161: key = LK_KEY_RIGHT_SHIFT;     break;
            case 163: key = LK_KEY_RIGHT_CONTROL;   break;
            case 92:  key = LK_KEY_RIGHT_WINDOWS;   break;
            case 165: key = LK_KEY_RIGHT_ALT;       break;
            case 32:  key = LK_KEY_SPACE;           break;
            case 8:   key = LK_KEY_BACKSPACE;       break;
            case 13:  key = LK_KEY_ENTER;           break;
            case 44:  key = LK_KEY_PRINT_SCREEN;    break;
            case 145: key = LK_KEY_SCREEN_LOCK;     break;
            case 19:  key = LK_KEY_PAUSE;           break;
            case 45:  key = LK_KEY_INSERT;          break;
            case 46:  key = LK_KEY_DELETE;          break;
            case 36:  key = LK_KEY_HOME;            break;
            case 35:  key = LK_KEY_END;             break;
            case 33:  key = LK_KEY_PAGE_UP;         break;
            case 34:  key = LK_KEY_PAGE_DOWN;       break;
            case 37:  key = LK_KEY_ARROW_LEFT;      break;
            case 39:  key = LK_KEY_ARROW_RIGHT;     break;
            case 38:  key = LK_KEY_ARROW_UP;        break;
            case 40:  key = LK_KEY_ARROW_DOWN;      break;
            case 190: key = LK_KEY_PERIOD;          break;
            case 188: key = LK_KEY_COMMA;           break;
            case 144: key = LK_KEY_NUMLOCK;         break;
            case 107: key = LK_KEY_NUMPAD_PLUS;     break;
            case 109: key = LK_KEY_NUMPAD_MINUS;    break;
            case 106: key = LK_KEY_NUMPAD_MULTIPLY; break;
            case 111: key = LK_KEY_NUMPAD_DIVIDE;   break;
            case 110: key = LK_KEY_NUMPAD_PERIOD;   break;
            case 96:  key = LK_KEY_NUMPAD_ENTER;    break;
            }

            if (key)
            {
                int is_down = (flags & RI_KEY_BREAK) == 0;
                if (is_down)
                {
                    lk_platform.keyboard.state[key].repeated = 1;
                }
                lk_platform.keyboard.state[key].down = is_down;
            }
        }

        if (input->header.dwType == RIM_TYPEHID)
        {
            HANDLE device = input->header.hDevice;

            UINT preparsed_data_size;
            if (GetRawInputDeviceInfo(device, RIDI_PREPARSEDDATA, NULL, &preparsed_data_size))
            {
                LK_Log("Failed to handle raw HID input; GetRawInputDeviceInfo() failed.");
                goto run_default_proc;
            }

            PHIDP_PREPARSED_DATA preparsed_data = (PHIDP_PREPARSED_DATA) LocalAlloc(LMEM_FIXED, preparsed_data_size);

            if (GetRawInputDeviceInfo(device, RIDI_PREPARSEDDATA, preparsed_data, &preparsed_data_size) <= 0)
            {
                LocalFree(preparsed_data);
                LK_Log("Failed to handle raw HID input; GetRawInputDeviceInfo() failed.");
                goto run_default_proc;
            }

            HIDP_CAPS capabilities;
            if (HidP_GetCaps(preparsed_data, &capabilities) != HIDP_STATUS_SUCCESS)
            {
                LocalFree(preparsed_data);
                LK_Log("Failed to get HID capabilities; HidP_GetCaps() failed.");
                goto run_default_proc;
            }

            // printf("EVENT FROM DEVICE, USAGE %d\n", capabilities.Usage);

            if (capabilities.Usage == 4 || // joystick input
                capabilities.Usage == 5)   // gamepad input
            {
                LK_U32 gamepad_index = 0;  // @Incomplete - handle multiple gamepads
                LK_Gamepad* gamepad = &lk_platform.gamepads[gamepad_index];

                CHAR* raw_data = (CHAR*)(input->data.hid.bRawData);
                ULONG raw_size = input->data.hid.dwSizeHid;

                // Read button states.
                {
                    USHORT button_capability_count = capabilities.NumberInputButtonCaps;
                    HIDP_BUTTON_CAPS* button_capability_array = (HIDP_BUTTON_CAPS*) LocalAlloc(LMEM_FIXED, sizeof(HIDP_BUTTON_CAPS) * button_capability_count);

                    if (HidP_GetButtonCaps(HidP_Input, button_capability_array, &button_capability_count, preparsed_data) != HIDP_STATUS_SUCCESS)
                    {
                        LocalFree(preparsed_data);
                        LocalFree(button_capability_array);
                        LK_Log("Failed to get gamepad button capabilities; HidP_GetButtonCaps() failed.");
                        goto run_default_proc;
                    }

                    if (button_capability_count)
                    {
                        HIDP_BUTTON_CAPS* button_capabilities = button_capability_array;
                        USAGE number_of_buttons = button_capabilities->Range.UsageMax - button_capabilities->Range.UsageMin + 1;

                        USAGE usage_page = button_capabilities->UsagePage;
                        USAGE* usages = (USAGE*) LocalAlloc(LMEM_FIXED, sizeof(USAGE) * number_of_buttons);
                        ULONG usage_count = number_of_buttons;

                        if (HidP_GetUsages(HidP_Input, usage_page, 0, usages, &usage_count, preparsed_data, raw_data, raw_size) != HIDP_STATUS_SUCCESS)
                        {
                            LocalFree(usages);
                            LocalFree(preparsed_data);
                            LocalFree(button_capability_array);
                            LK_Log("Failed to get gamepad button state; HidP_GetUsages() failed.");
                            goto run_default_proc;
                        }

                        // printf("%d caps, %d buttons, %d pressed:", button_capability_count, number_of_buttons, usage_count);

                        for (int i = 0; i < LK_GAMEPAD_BUTTON_COUNT; i++)
                        {
                            gamepad->buttons[i].down = 0;
                        }

                        for (int i = 0; i < usage_count; i++)
                        {
                            USAGE button_index = usages[i] - button_capabilities->Range.UsageMin;
                            // printf(" %d", button_index);
                            if (button_index >= 0 && button_index < LK_GAMEPAD_BUTTON_COUNT)
                            {
                                gamepad->buttons[button_index].down = 1;
                            }
                        }

                        // printf("\n");

                        LocalFree(usages);
                    }

                    LocalFree(button_capability_array);
                }

                // Read value states.
                {
                    USHORT value_capability_count = capabilities.NumberInputValueCaps;
                    HIDP_VALUE_CAPS* value_capability_array = (HIDP_VALUE_CAPS*) LocalAlloc(LMEM_FIXED, sizeof(HIDP_VALUE_CAPS) * value_capability_count);

                    if (HidP_GetValueCaps(HidP_Input, value_capability_array, &value_capability_count, preparsed_data) != HIDP_STATUS_SUCCESS)
                    {
                        LocalFree(preparsed_data);
                        LocalFree(value_capability_array);
                        LK_Log("Failed to get gamepad value capabilities; HidP_GetValueCaps() failed.");
                        goto run_default_proc;
                    }

                    for (USHORT value_index = 0; value_index < value_capability_count; value_index++)
                    {
                        HIDP_VALUE_CAPS* value_capabilities = &value_capability_array[value_index];
                        USAGE usage_page = value_capabilities->UsagePage;
                        USAGE usage = value_capabilities->Range.UsageMin;

                        ULONG value;
                        if (HidP_GetUsageValue(HidP_Input, usage_page, 0, usage, &value, preparsed_data, raw_data, raw_size) != HIDP_STATUS_SUCCESS)
                        {
                            LocalFree(preparsed_data);
                            LocalFree(value_capability_array);
                            LK_Log("Failed to get gamepad usage value; HidP_GetUsageValue() failed.");
                            goto run_default_proc;
                        }

                        USHORT bits = value_capabilities->BitSize;
                        // printf("value #%d: usage %02x, value %d\n", value_index, usage, value);

                        switch (usage)
                        {
                        case 0x30: gamepad->analog[0] =  ((LK_F64) value / (LK_F64)(1ul << (bits - 1)) - 1.0); break; // usage X,  left stick x
                        case 0x31: gamepad->analog[1] = -((LK_F64) value / (LK_F64)(1ul << (bits - 1)) - 1.0); break; // usage Y,  left stick y
                        case 0x33: gamepad->analog[2] =  ((LK_F64) value / (LK_F64)(1ul << (bits - 1)) - 1.0); break; // usage Rx, right stick x
                        case 0x34: gamepad->analog[3] = -((LK_F64) value / (LK_F64)(1ul << (bits - 1)) - 1.0); break; // usage Ry, right stick y
                        case 0x32: gamepad->analog[4] = -((LK_F64) value / (LK_F64)(1ul << (bits - 1)) - 1.0); break; // usage Z,  trigger left/right
                        case 0x39: // usage hat switch
                        {
                            const double HSQRT2 = 0.70710678118;

                            switch (value)
                            {
                            case 0: gamepad->hat.x =     0.0; gamepad->hat.y =     0.0; gamepad->hat.angle =  -1.0; break;
                            case 1: gamepad->hat.x =     0.0; gamepad->hat.y =     1.0; gamepad->hat.angle =  90.0; break;
                            case 2: gamepad->hat.x =  HSQRT2; gamepad->hat.y =  HSQRT2; gamepad->hat.angle =  45.0; break;
                            case 3: gamepad->hat.x =     1.0; gamepad->hat.y =     0.0; gamepad->hat.angle =   0.0; break;
                            case 4: gamepad->hat.x =  HSQRT2; gamepad->hat.y = -HSQRT2; gamepad->hat.angle = 315.0; break;
                            case 5: gamepad->hat.x =     0.0; gamepad->hat.y =    -1.0; gamepad->hat.angle = 270.0; break;
                            case 6: gamepad->hat.x = -HSQRT2; gamepad->hat.y = -HSQRT2; gamepad->hat.angle = 225.0; break;
                            case 7: gamepad->hat.x =    -1.0; gamepad->hat.y =     0.0; gamepad->hat.angle = 180.0; break;
                            case 8: gamepad->hat.x = -HSQRT2; gamepad->hat.y =  HSQRT2; gamepad->hat.angle = 135.0; break;
                            }
                        } break;
                        }
                    }

                    LocalFree(value_capability_array);
                }
            }

            LocalFree(preparsed_data);
        }

        goto run_default_proc;
    } break;

    case WM_PAINT:
    {
        if (lk_private.window.backend == LK_WINDOW_CANVAS)
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window, &paint);

            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - x;
            int height = paint.rcPaint.bottom - y;

            lk_repaint_canvas_rectangle(device_context, x, y, width, height);

            EndPaint(window, &paint);
        }

        goto run_default_proc;
    } break;

    case WM_ERASEBKGND:
    {
        return 1;
    } break;

    case WM_TIMER:
    {
        SwitchToFiber(lk_private.window.main_fiber);
    } break;

    case WM_ENTERMENULOOP:
    case WM_ENTERSIZEMOVE:
    {
        SetTimer(window, 0, 1, 0);
    } break;

    case WM_EXITMENULOOP:
    case WM_EXITSIZEMOVE:
    {
        KillTimer(window, 0);
    } break;

    case WM_SETCURSOR:
    {
        if (LOWORD(lparam) == HTCLIENT)
        {
            static HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
            SetCursor(cursor);
            return 1;
        }

        goto run_default_proc;
    } break;

    case WM_SYSCOMMAND:
    {
        WPARAM command = (wparam & 0xFFF0);
        if (command == SC_KEYMENU)
            return 0;

        if (command == SC_SCREENSAVE || command == SC_MONITORPOWER)
            if (lk_private.window.fullscreen)
                return 0;

        goto run_default_proc;
    } break;

    default:
    {
        goto run_default_proc;
    } break;

    }

    return 0;
run_default_proc:
    return DefWindowProcW(window, message, wparam, lparam);
}

static void lk_fill_default_window_settings(HINSTANCE instance)
{
    if (!lk_platform.window.title)
    {
        lk_platform.window.title = "app";
    }


    LK_B32 default_x      = (lk_platform.window.x == LK_DEFAULT_POSITION);
    LK_B32 default_y      = (lk_platform.window.y == LK_DEFAULT_POSITION);
    LK_B32 default_width  = !lk_platform.window.width;
    LK_B32 default_height = !lk_platform.window.height;

    LK_B32 default_position = default_x || default_y;
    LK_B32 default_size = default_width || default_height;

    if (!default_position && !default_size)
    {
        return;
    }

    if (default_size)
    {
        RECT client_rect;
        client_rect.left = 0;
        client_rect.top = 0;
        client_rect.right = 600;
        client_rect.bottom = 600;

        HWND temp_window = CreateWindowExW(0, LK_WINDOW_CLASS_NAME, L"", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, 0);
        if (temp_window)
        {
            GetClientRect(temp_window, &client_rect);
            DestroyWindow(temp_window);
        }
        else
        {
            LK_Log("Failed to set the default window size; couldn't create a temporary window.");
        }

        LK_U32 width = client_rect.right - client_rect.left;
        LK_U32 height = client_rect.bottom - client_rect.top;

        if (default_width) lk_platform.window.width = width;
        if (default_height) lk_platform.window.height = height;
    }

    if (default_position)
    {
        int x = 100;
        int y = 100;

        HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(monitor_info);
        if (GetMonitorInfo(monitor, &monitor_info))
        {
            int width = lk_platform.window.width;
            int height = lk_platform.window.height;

            RECT window_bounds;
            window_bounds.left = 0;
            window_bounds.top = 0;
            window_bounds.right = width;
            window_bounds.bottom = height;
            AdjustWindowRectEx(&window_bounds, WS_OVERLAPPED, 0, 0);

            width = window_bounds.right - window_bounds.left;
            height = window_bounds.bottom - window_bounds.top;

            x = (monitor_info.rcWork.left + monitor_info.rcWork.right - width) / 2;
            y = (monitor_info.rcWork.top + monitor_info.rcWork.bottom - height) / 2;
        }
        else
        {
            LK_Log("Failed to set the default window position; couldn't get the screen size.");
        }

        if (default_x) lk_platform.window.x = x;
        if (default_y) lk_platform.window.y = y;
    }
}

static void lk_disable_window_animations(HMODULE dwmapi, HWND window)
{
    const DWORD DWMWA_TRANSITIONS_FORCEDISABLED = 2;

    HRESULT(*DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
    LK_GetProc(dwmapi, DwmSetWindowAttribute, "DwmSetWindowAttribute");
    if (DwmSetWindowAttribute)
    {
        BOOL disable = TRUE;

        DwmSetWindowAttribute(window, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
    }
}

static void lk_enable_window_transparency(HMODULE dwmapi, HWND window)
{
    const DWORD LK_DWM_BB_ENABLE = 1;

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
    LK_GetProc(dwmapi, DwmEnableBlurBehindWindow, "DwmEnableBlurBehindWindow");
    LK_GetProc(dwmapi, DwmExtendFrameIntoClientArea, "DwmExtendFrameIntoClientArea");

    if (!DwmEnableBlurBehindWindow || !DwmExtendFrameIntoClientArea)
    {
        LK_Log("Failed to enable window transparency; missing DwmApi functions.");
        return;
    }


    DWM_BLURBEHIND blur;
    ZeroMemory(&blur, sizeof(DWM_BLURBEHIND));
    blur.dwFlags = LK_DWM_BB_ENABLE;
    blur.fEnable = TRUE;
    DwmEnableBlurBehindWindow(window, &blur);

    MARGINS margins;
    ZeroMemory(&margins, sizeof(MARGINS));
    margins.cxLeftWidth = -1;
    DwmExtendFrameIntoClientArea(window, &margins);
}


static void lk_set_window_icon(HWND window, HDC dc)
{
    LK_U32 width = lk_platform.window.icon_width;
    LK_U32 height = lk_platform.window.icon_height;

    BITMAPV5HEADER bitmap_header;
    ZeroMemory(&bitmap_header, sizeof(BITMAPV5HEADER));
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
    HBITMAP bitmap = CreateDIBSection(dc, (BITMAPINFO*) &bitmap_header, DIB_RGB_COLORS, &bitmap_data, 0, 0);
    if (bitmap)
    {
        for (LK_U32 y = 0; y < height; y++)
        {
            LK_U32* read  = (LK_U32*) lk_platform.window.icon_pixels + y * width;
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
                SendMessage(window, WM_SETICON, ICON_SMALL, (LPARAM) icon);
                SendMessage(window, WM_SETICON, ICON_BIG,   (LPARAM) icon);
            }
            else
            {
                LK_Log("Failed to set the window icon; couldn't create the icon.");
                return;
            }

            DeleteObject(mono_bitmap);
        }
        else
        {
            LK_Log("Failed to set the window icon; couldn't create the bitmap.");
            return;
        }

        DeleteObject(bitmap);
    }
}

#if 0
#define DIDFT_OPTIONAL      0x80000000

static const LK_U8 LK_GUID_IDirectInput8A[16] = { 0x30,0x80,0x79,0xBF,0x3A,0x48,0xA2,0x4D,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00 }; // BF798030-483A-4DA2-AA99-5D64ED369700
static const LK_U8 LK_GUID_IDirectInput8W[16] = { 0x31,0x80,0x79,0xBF,0x3A,0x48,0xA2,0x4D,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00 }; // BF798031-483A-4DA2-AA99-5D64ED369700
static const LK_U8 LK_GUID_XAxis[16]          = { 0xE0,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_YAxis[16]          = { 0xE1,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_ZAxis[16]          = { 0xE2,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_RxAxis[16]         = { 0xF4,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_RyAxis[16]         = { 0xF5,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_RzAxis[16]         = { 0xE3,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_Slider[16]         = { 0xE4,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_Button[16]         = { 0xF0,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_Key[16]            = { 0x20,0x82,0x72,0x55,0x3C,0xD3,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };
static const LK_U8 LK_GUID_POV[16]            = { 0xF2,0x02,0x6D,0xA3,0xF3,0xC9,0xCF,0x11,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };

static const DIOBJECTDATAFORMAT lk_joystick_data_format_array[] =
{
    { (GUID*) &LK_GUID_XAxis,  DIJOFS_X,         DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_YAxis,  DIJOFS_Y,         DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_ZAxis,  DIJOFS_Z,         DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_RxAxis, DIJOFS_RX,        DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_RyAxis, DIJOFS_RY,        DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_RzAxis, DIJOFS_RZ,        DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_Slider, DIJOFS_SLIDER(0), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_Slider, DIJOFS_SLIDER(1), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_POV,    DIJOFS_POV(0),    DIDFT_OPTIONAL | DIDFT_POV  | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_POV,    DIJOFS_POV(1),    DIDFT_OPTIONAL | DIDFT_POV  | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_POV,    DIJOFS_POV(2),    DIDFT_OPTIONAL | DIDFT_POV  | DIDFT_ANYINSTANCE, 0},
    { (GUID*) &LK_GUID_POV,    DIJOFS_POV(3),    DIDFT_OPTIONAL | DIDFT_POV  | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(0),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(1),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(2),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(3),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(4),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(5),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(6),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(7),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(8),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(9),  DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(10), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(11), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(12), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(13), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(14), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(15), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(16), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(17), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(18), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(19), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(20), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(21), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(22), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(23), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(24), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(25), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(26), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(27), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(28), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(29), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(30), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
    { NULL, DIJOFS_BUTTON(31), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
};

static const DIDATAFORMAT lk_joystick_data_format =
{
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    DIDF_ABSAXIS,
    sizeof(DIJOYSTATE),
    sizeof(lk_joystick_data_format_array) / sizeof(lk_joystick_data_format_array[0]),
    (LPDIOBJECTDATAFORMAT) lk_joystick_data_format_array
};

static BOOL lk_directinput_enum_devices(LPCDIDEVICEINSTANCEA device, LPVOID userdata)
{
    IDirectInput8A* direct_input = lk_private.directinput.direct_input;

    LPDIRECTINPUTDEVICE8 controller;
    HRESULT create_success = direct_input->CreateDevice(device->guidInstance, &controller, NULL);
    if (FAILED(create_success))
        return DIENUM_CONTINUE;

    HWND window = lk_private.window.handle;
    HRESULT cooperate_success = controller->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(cooperate_success))
    {
        LK_Log("Failed to set cooperative level for controller device.");
    }

    if (FAILED(controller->SetDataFormat(&lk_joystick_data_format)))
    {
        LK_Log("Failed to set the data format for controller device.");
    }

    // printf("found %s!\n", device->tszInstanceName);
    return DIENUM_CONTINUE;
}

static int lk_load_directinput_library()
{
    HMODULE library = LoadLibraryA("dinput8.dll");
    if (library == 0)
    {
        LK_Log("Failed to load DirectInput; couldn't load dinput8.dll.");
        return 0;
    }


    HINSTANCE instance = GetModuleHandle(NULL);
    HRESULT(*DirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, IID* riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
    LK_GetProc(library, DirectInput8Create, "DirectInput8Create");
    if (!DirectInput8Create)
    {
        LK_Log("Failed to load DirectInput; couldn't find DirectInput8Create in dinput8.dll.");
        return 0;
    }

    IDirectInput8A* direct_input;
    if (FAILED(DirectInput8Create(instance, DIRECTINPUT_VERSION, (IID*) &LK_GUID_IDirectInput8A, (void**) &direct_input, NULL)))
    {
        LK_Log("Failed to load DirectInput; couldn't to create an IDirectInput8 instance.");
        return 0;
    }

    lk_private.directinput.direct_input = direct_input;
    direct_input->EnumDevices(DI8DEVCLASS_GAMECTRL, lk_directinput_enum_devices, NULL, DIEDFL_ATTACHEDONLY);

    // printf("loaded!\n");

    return 1;
}
#endif

static int lk_load_opengl_library()
{
    HMODULE library = LoadLibraryA("opengl32.dll");
    if (library == 0)
    {
        LK_Log("Failed to create an OpenGL context; couldn't load OpenGL32.dll.");
        return 0;
    }

    lk_private.opengl.library = library;

    #define LK_GetOpenGLFunction(name)                      \
    {                                                       \
        LK_GetProc(library, lk_private.opengl.name, #name); \
        if (!lk_private.opengl.name)                        \
        {                                                   \
            LK_Log("Failed to create an OpenGL context;"    \
                  " couldn't find " #name "() in"           \
                  " OpenGL32.dll.");                        \
            return 0;                                       \
        }                                                   \
    }

    LK_GetOpenGLFunction(wglCreateContext );
    LK_GetOpenGLFunction(wglDeleteContext );
    LK_GetOpenGLFunction(wglMakeCurrent   );
    LK_GetOpenGLFunction(wglGetProcAddress);

    #undef LK_GetOpenGLFunction

    return 1;
}

static void lk_create_legacy_opengl_context(HINSTANCE instance)
{
    HWND window = lk_private.window.handle;
    HDC dc = lk_private.window.dc;

    PIXELFORMATDESCRIPTOR pixel_format_desc;
    ZeroMemory(&pixel_format_desc, sizeof(PIXELFORMATDESCRIPTOR));
    pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format_desc.nVersion = 1;
    pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
    pixel_format_desc.cColorBits = lk_platform.opengl.color_bits;
    pixel_format_desc.cDepthBits = lk_platform.opengl.depth_bits;
    pixel_format_desc.cStencilBits = lk_platform.opengl.stencil_bits;
    pixel_format_desc.iLayerType = PFD_MAIN_PLANE;

    int pixel_format = ChoosePixelFormat(dc, &pixel_format_desc);
    if (pixel_format == 0)
    {
        LK_Log("Failed to create a legacy OpenGL context; ChoosePixelFormat() failed.");
        return;
    }

    if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
    {
        LK_Log("Failed to create a legacy OpenGL context; SetPixelFormat() failed.");
        return;
    }

    HGLRC context = lk_private.opengl.wglCreateContext(dc);

    if (!context)
    {
        LK_Log("Failed to create a legacy OpenGL context; wglCreateContext() failed.");
        return;
    }

    if (!lk_private.opengl.wglMakeCurrent(dc, context))
    {
        LK_Log("Failed to create a legacy OpenGL context; wglMakeCurrent() failed.");
        lk_private.opengl.wglDeleteContext(context);
        return;
    }

    lk_private.opengl.context = context;
}

static int lk_create_modern_opengl_context(HINSTANCE instance)
{
    HWND fake_window;
    HDC fake_dc;
    PIXELFORMATDESCRIPTOR fake_pixel_format_desc;
    int fake_pixel_format;
    HGLRC fake_context;
    HGLRC real_context;

    fake_window = CreateWindowExW(0, LK_WINDOW_CLASS_NAME, L"fake window", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, 0);
    if (!fake_window)
    {
        LK_Log("Failed to create a modern OpenGL context; CreateWindowEx() failed.");
        return 0;
    }

    fake_dc = GetDC(fake_window);
    if (!fake_dc)
    {
        LK_Log("Failed to create a modern OpenGL context; GetDC() failed.");
        goto undo_fake_window;
    }

    ZeroMemory(&fake_pixel_format_desc, sizeof(PIXELFORMATDESCRIPTOR));
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
        LK_Log("Failed to create a modern OpenGL context; ChoosePixelFormat() failed.");
        goto undo_fake_dc;
    }

    if (!SetPixelFormat(fake_dc, fake_pixel_format, &fake_pixel_format_desc))
    {
        LK_Log("Failed to create a modern OpenGL context; SetPixelFormat() failed.");
        goto undo_fake_dc;
    }

    fake_context = lk_private.opengl.wglCreateContext(fake_dc);

    if (!fake_context)
    {
        LK_Log("Failed to create a modern OpenGL context; wglCreateContext() failed.");
        goto undo_fake_dc;
    }

    if (!lk_private.opengl.wglMakeCurrent(fake_dc, fake_context))
    {
        LK_Log("Failed to create a modern OpenGL context; wglMakeCurrent() failed.");
        goto undo_fake_context;
    }


    #define LK_GetWGLFunction(name)                                                                        \
    {                                                                                                      \
        PROC proc = lk_private.opengl.wglGetProcAddress(#name);                                            \
        if (proc && (proc != (PROC) 1) && (proc != (PROC) 2) && (proc != (PROC) 3) && (proc != (PROC) -1)) \
            *(PROC*) &lk_private.opengl.name = proc;                                                       \
        else                                                                                               \
            lk_private.opengl.name = 0;                                                                    \
    }

    LK_GetWGLFunction(wglChoosePixelFormatARB)
    LK_GetWGLFunction(wglCreateContextAttribsARB)
    LK_GetWGLFunction(wglSwapIntervalEXT)

    #undef LK_GetWGLFunction


    if (lk_private.opengl.wglChoosePixelFormatARB && lk_private.opengl.wglCreateContextAttribsARB)
    {
        HWND window = lk_private.window.handle;
        HDC dc = lk_private.window.dc;

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
        LK_AddPixelFormatAttributes(WGL_COLOR_BITS_ARB, (int) lk_platform.opengl.color_bits);
        LK_AddPixelFormatAttributes(WGL_DEPTH_BITS_ARB, (int) lk_platform.opengl.depth_bits);
        LK_AddPixelFormatAttributes(WGL_STENCIL_BITS_ARB, (int) lk_platform.opengl.stencil_bits);

        if (lk_platform.window.transparent)
        {
            LK_AddPixelFormatAttributes(WGL_TRANSPARENT_ARB, TRUE);
            LK_AddPixelFormatAttributes(WGL_ALPHA_BITS_ARB, 8);
        }

        if (lk_platform.opengl.sample_count > 1)
        {
            LK_AddPixelFormatAttributes(WGL_SAMPLE_BUFFERS_ARB, 1);
            LK_AddPixelFormatAttributes(WGL_SAMPLES_ARB, (int) lk_platform.opengl.sample_count);
        }

        #undef LK_AddPixelFormatAttributes


        int pixel_format;
        UINT pixel_format_count;
        if (!lk_private.opengl.wglChoosePixelFormatARB(dc, pixel_format_attributes, 0, 1, &pixel_format, &pixel_format_count))
        {
            LK_Log("Failed to create a modern OpenGL context; wglChoosePixelFormatARB() failed.");
            goto undo_fake_context;
        }

        if (pixel_format_count == 0)
        {
            LK_Log("Failed to create a modern OpenGL context; couldn't choose the appropriate pixel format.");
            goto undo_fake_context;
        }

        PIXELFORMATDESCRIPTOR pixel_format_desc;
        if (!DescribePixelFormat(dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_desc))
        {
            LK_Log("Failed to create a modern OpenGL context; DescribePixelFormat() failed.");
            goto undo_fake_context;
        }

        if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
        {
            LK_Log("Failed to create a modern OpenGL context; SetPixelFormat() failed.");
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
        if (lk_platform.opengl.debug_context)
        {
            flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        }

        int profile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        if (lk_platform.opengl.compatibility_context)
        {
            profile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        }

        if (!lk_platform.opengl.major_version)
        {
            lk_platform.opengl.major_version = 3;
            lk_platform.opengl.minor_version = 3;
        }

        int attributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, (int) lk_platform.opengl.major_version,
            WGL_CONTEXT_MINOR_VERSION_ARB, (int) lk_platform.opengl.minor_version,
            WGL_CONTEXT_FLAGS_ARB, flags,
            WGL_CONTEXT_PROFILE_MASK_ARB, profile,
            0
        };

        if (lk_platform.opengl.major_version < 3)
        {
            // Don't set the profile or flags for legacy OpenGL.
            attributes[4] = 0;
        }

        real_context = lk_private.opengl.wglCreateContextAttribsARB(dc, 0, attributes);

        if (!real_context)
        {
            LK_Log("Failed to create a modern OpenGL context; wglCreateContextAttribsARB() failed.");
            goto undo_fake_context;
        }

        lk_private.opengl.wglMakeCurrent(0, 0);
        lk_private.opengl.wglDeleteContext(fake_context);
        ReleaseDC(fake_window, fake_dc);
        DestroyWindow(fake_window);

        if (!lk_private.opengl.wglMakeCurrent(dc, real_context))
        {
            LK_Log("Failed to create a modern OpenGL context; wglMakeCurrent() failed.");
            goto undo_real_context;
        }

        lk_private.opengl.context = real_context;
        return 1; // success!
    }
    else
    {
        goto undo_fake_context;
    }


undo_real_context:
    lk_private.opengl.wglDeleteContext(real_context);
undo_fake_context:
    lk_private.opengl.wglMakeCurrent(0, 0);
    lk_private.opengl.wglDeleteContext(fake_context);
undo_fake_dc:
    ReleaseDC(fake_window, fake_dc);
undo_fake_window:
    DestroyWindow(fake_window);
    return 0;
}

static void lk_open_window()
{
    WNDCLASSEXW window_class;
    ZeroMemory(&window_class, sizeof(WNDCLASSEXW));

    HINSTANCE instance = GetModuleHandle(0);

    window_class.cbSize = sizeof(WNDCLASSEXW);
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = lk_window_callback;
    window_class.hInstance = instance;
    window_class.lpszClassName = LK_WINDOW_CLASS_NAME;

    if (!RegisterClassExW(&window_class))
    {
        LK_Log("Failed to open a window; RegisterClassEx() failed.");
        return;
    }

    lk_fill_default_window_settings(instance);

    HWND window_handle = CreateWindowExW(0, LK_WINDOW_CLASS_NAME, L"", 0,
                                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                         0, 0, instance, 0);

    if (!window_handle)
    {
        LK_Log("Failed to open a window; CreateWindowEx() failed.");
        return;
    }


    HMODULE dwmapi = LoadLibraryA("dwmapi.dll");
    if (dwmapi)
    {
        if (lk_platform.window.disable_animations)
        {
            lk_disable_window_animations(dwmapi, window_handle);
        }

        if (lk_platform.window.transparent)
        {
            lk_enable_window_transparency(dwmapi, window_handle);
        }

        FreeLibrary(dwmapi);
    }


    lk_platform.window.handle = window_handle;
    lk_private.window.handle = window_handle;
    lk_push();

    SetForegroundWindow(window_handle);
    SetFocus(window_handle);


    // display a black window as soon as possible
    PAINTSTRUCT paint;
    HDC paint_dc = BeginPaint(window_handle, &paint);
    PatBlt(paint_dc, paint.rcPaint.left, paint.rcPaint.top, paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top, BLACKNESS);
    EndPaint(window_handle, &paint);


    HDC dc = GetDC(window_handle);
    lk_private.window.dc = dc;

    if (!dc)
    {
        LK_Log("Failed to open a window; GetDC() failed.");
        return;
    }


    if (lk_platform.window.icon_pixels)
    {
        lk_set_window_icon(window_handle, dc);
    }


    LK_B32 use_opengl = (lk_private.window.backend == LK_WINDOW_OPENGL);
    if (use_opengl)
    {
        if (lk_load_opengl_library())
        {
            if (!lk_create_modern_opengl_context(instance))
            {
                lk_create_legacy_opengl_context(instance);
            }
        }
    }
    else
    {
        PIXELFORMATDESCRIPTOR pixel_format_desc;
        ZeroMemory(&pixel_format_desc, sizeof(PIXELFORMATDESCRIPTOR));
        pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pixel_format_desc.nVersion = 1;
        pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
        pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
        pixel_format_desc.cColorBits = 32;
        pixel_format_desc.iLayerType = PFD_MAIN_PLANE;

        int pixel_format = ChoosePixelFormat(dc, &pixel_format_desc);
        if (!pixel_format)
        {
            LK_Log("Failed to open a window; ChoosePixelFormat() failed.");
            return;
        }

        if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
        {
            LK_Log("Failed to open a window; SetPixelFormat() failed.");
            return;
        }
    }

#if 0
    lk_load_directinput_library();
#endif

    lk_pull();
    lk_update_canvas();
    lk_update_swap_interval();

#if 0
    UINT count_raw_devices = 0;
    GetRawInputDeviceList(NULL, &count_raw_devices, sizeof(RAWINPUTDEVICELIST));
    printf("Detected %d raw input devices:\n", count_raw_devices);
    RAWINPUTDEVICELIST* devices = (RAWINPUTDEVICELIST*) malloc(sizeof(RAWINPUTDEVICELIST) * count_raw_devices);
    GetRawInputDeviceList(devices, &count_raw_devices, sizeof(RAWINPUTDEVICELIST));
    for (int i = 0; i < count_raw_devices; i++)
    {
        HANDLE device = devices[i].hDevice;
        RID_DEVICE_INFO device_info;
        device_info.cbSize = sizeof(RID_DEVICE_INFO);
        UINT size = sizeof(RID_DEVICE_INFO);
        GetRawInputDeviceInfoA(device, RIDI_DEVICEINFO, &device_info, &size);

        const char* type[] = { "mouse", "keyboard", "other" };
        printf("  %s", type[device_info.dwType]);
        if (device_info.dwType == 2)
        {
            printf(" - usage %d, page %d", device_info.hid.usUsage, device_info.hid.usUsagePage);
        }
        printf("\n");
    }
    free(devices);
#endif

    RAWINPUTDEVICE raw_input_mouse;
    ZeroMemory(&raw_input_mouse, sizeof(raw_input_mouse));
    raw_input_mouse.usUsagePage = 0x01;
    raw_input_mouse.usUsage = 0x02;
    raw_input_mouse.hwndTarget = window_handle;
    if (!RegisterRawInputDevices(&raw_input_mouse, 1, sizeof(raw_input_mouse)))
    {
        LK_Log("Failed to register the mouse input device; the window won't receive mouse events.");
        return;
    }

    RAWINPUTDEVICE raw_input_keyboard;
    ZeroMemory(&raw_input_keyboard, sizeof(raw_input_keyboard));
    raw_input_keyboard.usUsagePage = 0x01;
    raw_input_keyboard.usUsage = 0x06;
    raw_input_keyboard.hwndTarget = window_handle;
    if (!RegisterRawInputDevices(&raw_input_keyboard, 1, sizeof(raw_input_keyboard)))
    {
        LK_Log("Failed to register the keyboard input device; the window won't receive keyboard events.");
        return;
    }

    RAWINPUTDEVICE raw_input_joystick;
    ZeroMemory(&raw_input_joystick, sizeof(raw_input_joystick));
    raw_input_joystick.usUsagePage = 0x01;
    raw_input_joystick.usUsage = 0x04;
    raw_input_joystick.hwndTarget = window_handle;
    if (!RegisterRawInputDevices(&raw_input_joystick, 1, sizeof(raw_input_joystick)))
    {
        LK_Log("Failed to register the joystick input device; the window won't receive joystick events.");
        return;
    }

    RAWINPUTDEVICE raw_input_gamepad;
    ZeroMemory(&raw_input_gamepad, sizeof(raw_input_gamepad));
    raw_input_gamepad.usUsagePage = 0x01;
    raw_input_gamepad.usUsage = 0x05;
    raw_input_gamepad.hwndTarget = window_handle;
    if (!RegisterRawInputDevices(&raw_input_gamepad, 1, sizeof(raw_input_gamepad)))
    {
        LK_Log("Failed to register the gamepad input device; the window won't receive gamepad events.");
        return;
    }
}

static void lk_close_window()
{
    HINSTANCE instance = GetModuleHandle(0);
    HWND window = lk_private.window.handle;
    HDC dc = lk_private.window.dc;
    HGLRC context = lk_private.opengl.context;

    if (context)
    {
        lk_private.opengl.wglMakeCurrent(0, 0);
        lk_private.opengl.wglDeleteContext(context);
    }

    if (dc)
    {
        ReleaseDC(window, dc);
    }

    if (window)
    {
        DestroyWindow(window);
    }

    UnregisterClassW(LK_WINDOW_CLASS_NAME, instance);

    lk_private.window.handle = 0;
    lk_private.window.dc = 0;
    lk_private.opengl.context = 0;
}

static void CALLBACK lk_message_fiber_proc(void* unused)
{
    HANDLE main_fiber = lk_private.window.main_fiber;

    while (1)
    {
        MSG message;
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        SwitchToFiber(main_fiber);
    }
}

static void lk_window_message_loop()
{
    HWND window = lk_private.window.handle;
    if (!window)
    {
        return;
    }

    HANDLE message_fiber = lk_private.window.message_fiber;
    SwitchToFiber(message_fiber);
}

static void lk_window_swap_buffers()
{
    HDC dc = lk_private.window.dc;
    if (!dc)
    {
        return;
    }

    if (lk_private.window.backend == LK_WINDOW_CANVAS)
    {
        LK_U32 width = lk_platform.window.width;
        LK_U32 height = lk_platform.window.height;
        lk_repaint_canvas_rectangle(dc, 0, 0, width, height);
    }

    SwapBuffers(dc);
}

static void lk_initialize_timer()
{
    LARGE_INTEGER i64;
    QueryPerformanceFrequency(&i64);
    lk_private.time.ticks_per_second = i64.QuadPart;

    QueryPerformanceCounter(&i64);
    lk_private.time.initial_ticks = i64.QuadPart;
}

static void lk_update_time_stamp()
{
    LARGE_INTEGER i64;
    QueryPerformanceCounter(&i64);

    LK_U64 frequency = lk_private.time.ticks_per_second;
    LK_U64 new_ticks = i64.QuadPart - lk_private.time.initial_ticks;
    LK_U64 delta_ticks = new_ticks - lk_platform.time.ticks;

    lk_platform.time.delta_ticks = delta_ticks;

    LK_U64 nanoseconds_ticks = 1000000000 * delta_ticks + lk_private.time.unprocessed_nanoseconds;
    lk_platform.time.delta_nanoseconds = nanoseconds_ticks / frequency;
    lk_private.time.unprocessed_nanoseconds = nanoseconds_ticks % frequency;

    LK_U64 microseconds_ticks = 1000000 * delta_ticks + lk_private.time.unprocessed_microseconds;
    lk_platform.time.delta_microseconds = microseconds_ticks / frequency;
    lk_private.time.unprocessed_microseconds = microseconds_ticks % frequency;

    LK_U64 milliseconds_ticks = 1000 * delta_ticks + lk_private.time.unprocessed_milliseconds;
    lk_platform.time.delta_milliseconds = milliseconds_ticks / frequency;
    lk_private.time.unprocessed_milliseconds = milliseconds_ticks % frequency;

    lk_platform.time.delta_seconds = (LK_F64) delta_ticks / (LK_F64) frequency;

    lk_platform.time.ticks        += delta_ticks;
    lk_platform.time.nanoseconds  += lk_platform.time.delta_nanoseconds;
    lk_platform.time.microseconds += lk_platform.time.delta_microseconds;
    lk_platform.time.milliseconds += lk_platform.time.delta_milliseconds;
    lk_platform.time.seconds      += lk_platform.time.delta_seconds;
}

static void lk_mixer_synchronize()
{
    if (lk_platform.audio.strategy != LK_AUDIO_MIXER)
    {
        return;
    }

    LK_F64 playing_frequency = (LK_F64) lk_platform.audio.frequency;

    HANDLE mutex = lk_private.audio.mixer_mutex;
    WaitForSingleObject(mutex, INFINITE);
    {
        for (int sound_index = 0; sound_index < LK_MIXER_SLOT_COUNT; sound_index++)
        {
            LK_Sound* user = lk_platform.audio.mixer_slots + sound_index;
            LK_Playing_Sound* live = lk_private.audio.mixer_slots + sound_index;

            LK_B32 playing = user->playing;
            switch (live->state)
            {
            case LK_NOT_PLAYING:
            {
                if (playing)
                {
                    goto begin_playing;
                }
            } break;
            case LK_PLAYING:
            {
                if (!playing)
                {
                    live->state = LK_NOT_PLAYING;
                }
                else
                {
                    live->volume = user->volume;
                }
            } break;
            case LK_FINISHED:
            {
                live->state = LK_NOT_PLAYING;
                user->playing = 0;
            } break;
            }

            continue;
            begin_playing:
            {
                live->state = LK_PLAYING;
                live->wave = user->wave;
                live->loop = user->loop;
                live->volume = user->volume;
                live->cursor = 0;
                live->cursor_step = user->wave.frequency / playing_frequency;
            } continue;
        }
    }
    ReleaseMutex(mutex);
}

static void lk_mix(LK_Platform* unused, LK_S16* output)
{
    HANDLE mutex = lk_private.audio.mixer_mutex;
    WaitForSingleObject(mutex, INFINITE);


    LK_U32 output_channels = lk_platform.audio.channels;
    LK_U32 output_count = lk_platform.audio.sample_count;
    LK_Playing_Sound* slots = lk_private.audio.mixer_slots;

    LK_S32 sound_count;
    LK_F32 samples_sum[8];
    while (output_count--)
    {
        sound_count = 0;
        ZeroMemory(samples_sum, output_channels * sizeof(LK_F32));

        for (int sound_index = 0; sound_index < LK_MIXER_SLOT_COUNT; sound_index++)
        {
            LK_Playing_Sound* sound = slots + sound_index;
            if (sound->state != LK_PLAYING) continue;
            sound_count++;

            LK_U32 count = sound->wave.count;
            LK_U32 channels = sound->wave.channels;
            LK_F32 volume = sound->volume;

            LK_U32 cursor = (LK_U32) sound->cursor;
            sound->cursor += sound->cursor_step;
            if ((LK_U32) sound->cursor >= count)
            {
                if (sound->loop)
                {
                    sound->cursor = 0;
                }
                else
                {
                    sound->state = LK_FINISHED;
                }
            }

            LK_S16* source = sound->wave.samples + (cursor * channels);
            if (channels == output_channels)
            {
                for (int channel = 0; channel < channels; channel++)
                    samples_sum[channel] += (LK_F32)(*(source++)) * volume;
            }
            else if (output_channels == 1)
            {
                LK_S32 single = 0;
                for (int channel = 0; channel < channels; channel++)
                    single += *(source++);
                samples_sum[0] += ((LK_F32) single / (LK_F32) channels) * volume;
            }
            else
            {
                for (int channel = 0; channel < output_channels; channel++)
                    samples_sum[channel] += (LK_F32)(*source) * volume;
            }
        }

        if (!sound_count)
        {
            // Fill the remainder of the output buffer with silence, there are no more sounds to play.
            ZeroMemory(output, (output_count + 1) * output_channels * sizeof(LK_S16));
            break;
        }

        for (int channel_index = 0; channel_index < output_channels; channel_index++)
        {
            LK_S32 clamped = samples_sum[channel_index];
            if (clamped < -32767) clamped = -32767;
            if (clamped >  32767) clamped =  32767;

            *(output++) = (LK_S16) clamped;
        }
    }


    ReleaseMutex(mutex);
}

static DWORD CALLBACK lk_audio_thread(LPVOID parameter)
{
    LPDIRECTSOUNDBUFFER secondary_buffer = lk_private.audio.secondary_buffer;

    LK_Audio_Strategy strategy = lk_platform.audio.strategy;
    LK_U32 channels = lk_platform.audio.channels;
    LK_U32 sample_buffer_size = lk_private.audio.sample_buffer_size;
    LK_U32 sample_buffer_count = lk_private.audio.sample_buffer_count;

    int playing = 0;

    int last_buffer_index = -1;
    while (1)
    {
        DWORD play_cursor;
        DWORD write_cursor;
        if (!SUCCEEDED(IDirectSoundBuffer_GetCurrentPosition(secondary_buffer, &play_cursor, &write_cursor)))
        {
            LK_Log("Audio thread: DirectSoundBuffer.GetCurrentPosition() failed.");
        }

        LK_U32 buffer_index = (write_cursor / sample_buffer_size) + 1;
        buffer_index %= sample_buffer_count;

        if (buffer_index == last_buffer_index)
        {
            Sleep(1);
            continue;
        }

        LK_U32 expected_buffer_index = (last_buffer_index + 1) % sample_buffer_count;
        if (buffer_index != expected_buffer_index)
        {
            // @Reconsider - Commented this warning out. This happens frequently on app startup and it's annoying.
            // LK_Log("Warning: The audio thread is too slow! Missed a buffer...");
        }

        last_buffer_index = buffer_index;


        write_cursor = buffer_index * sample_buffer_size;

        LK_S16* buffer;
        DWORD buffer_size;
        if (SUCCEEDED(IDirectSoundBuffer_Lock(secondary_buffer, write_cursor, sample_buffer_size, (LPVOID*) &buffer, &buffer_size, 0, 0, 0)))
        {
            if (strategy == LK_AUDIO_CALLBACK)
            {
                lk_private.client.audio(&lk_platform, buffer);
            }
            else
            {
                lk_mix(&lk_platform, buffer);
            }

            IDirectSoundBuffer_Unlock(secondary_buffer, buffer, buffer_size, 0, 0);
        }

        if (!playing)
        {
            if (!SUCCEEDED(IDirectSoundBuffer_Play(secondary_buffer, 0, 0, DSBPLAY_LOOPING)))
            {
                LK_Log("Audio thread: DirectSoundBuffer.Play() failed.");
            }

            playing = 1;
        }
    }
}

static void lk_initialize_audio()
{
    if (lk_platform.window.no_window)
    {
        LK_Log("The audio system requires an open window, but the client specified 'no_window'. The client won't get audio.");
        lk_platform.audio.strategy = LK_NO_AUDIO;
        return;
    }


    HMODULE dsound = LoadLibraryA("dsound.dll");
    if (!dsound)
    {
        LK_Log("Failed to initialize audio; couldn't load dsound.dll.");
        lk_platform.audio.strategy = LK_NO_AUDIO;
        return;
    }

    HRESULT(WINAPI *DirectSoundCreate)(LPGUID, LPDIRECTSOUND*, LPUNKNOWN);
    LK_GetProc(dsound, DirectSoundCreate, "DirectSoundCreate");
    if (!DirectSoundCreate)
    {
        LK_Log("Failed to initialize audio; couldn't find DirectSoundCreate() in dsound.dll.");
        lk_platform.audio.strategy = LK_NO_AUDIO;
        return;
    }

    LPDIRECTSOUND direct_sound;
    if (!SUCCEEDED(DirectSoundCreate(0, &direct_sound, 0)))
    {
        LK_Log("Failed to initialize audio; DirectSoundCreate() failed.");
        lk_platform.audio.strategy = LK_NO_AUDIO;
        return;
    }

    HWND window = lk_private.window.handle;
    if (!SUCCEEDED(IDirectSound_SetCooperativeLevel(direct_sound, window, DSSCL_PRIORITY)))
    {
        LK_Log("Failed to initialize audio; DirectSound.SetCooperativeLevel() failed.");
        lk_platform.audio.strategy = LK_NO_AUDIO;
        return;
    }


    LK_U32 frequency = lk_platform.audio.frequency;
    if (!frequency)
    {
        frequency = 44100;
        lk_platform.audio.frequency = frequency;
    }

    LK_U32 channels = lk_platform.audio.channels;
    if (!channels)
    {
        channels = 2;
        lk_platform.audio.channels = channels;
    }

    LK_U32 sample_count = lk_platform.audio.sample_count;
    if (!sample_count)
    {
        sample_count = 2048;
        lk_platform.audio.sample_count = sample_count;
    }

    LK_U32 sample_buffer_size = sample_count * channels * 2;
    lk_private.audio.sample_buffer_size = sample_buffer_size;


    WAVEFORMATEX format;
    ZeroMemory(&format, sizeof(format));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = channels;
    format.nSamplesPerSec = frequency;
    format.wBitsPerSample = 16;
    format.nBlockAlign = (channels * format.wBitsPerSample) / 8;
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize = 0;

    LPDIRECTSOUNDBUFFER primary_buffer;
    {
        DSBUFFERDESC description;
        ZeroMemory(&description, sizeof(description));
        description.dwSize = sizeof(description);
        description.dwFlags = DSBCAPS_PRIMARYBUFFER;

        if (!SUCCEEDED(IDirectSound_CreateSoundBuffer(direct_sound, &description, &primary_buffer, 0)))
        {
            LK_Log("Failed to initialize audio; DirectSound.CreateSoundBuffer() failed for the primary buffer.");
            lk_platform.audio.strategy = LK_NO_AUDIO;
            return;
        }

        if (!SUCCEEDED(IDirectSoundBuffer_SetFormat(primary_buffer, &format)))
        {
            LK_Log("Failed to initialize audio; DirectSound.SetFormat() failed for the primary buffer.");
            lk_platform.audio.strategy = LK_NO_AUDIO;
            return;
        }
    }

    LPDIRECTSOUNDBUFFER secondary_buffer;
    {
        LK_U32 bytes_per_second = frequency * channels * 2;
        LK_U32 sample_buffer_count = (bytes_per_second + sample_buffer_size - 1) / sample_buffer_size;
        lk_private.audio.sample_buffer_count = sample_buffer_count;

        LK_U32 secondary_buffer_size = sample_buffer_count * sample_buffer_size;
        lk_private.audio.secondary_buffer_size = secondary_buffer_size;

        DSBUFFERDESC description;
        ZeroMemory(&description, sizeof(description));
        description.dwSize = sizeof(description);
        description.dwFlags = lk_platform.audio.silent_when_not_focused ? 0 : DSBCAPS_GLOBALFOCUS;
        description.dwBufferBytes = secondary_buffer_size;
        description.lpwfxFormat = &format;

        if (!SUCCEEDED(IDirectSound_CreateSoundBuffer(direct_sound, &description, &secondary_buffer, 0)))
        {
            LK_Log("Failed to initialize audio; DirectSound.CreateSoundBuffer() failed for the secondary buffer.");
            lk_platform.audio.strategy = LK_NO_AUDIO;
            return;
        }

        void* buffer1;
        void* buffer2;
        DWORD buffer1_size;
        DWORD buffer2_size;
        if (SUCCEEDED(IDirectSoundBuffer_Lock(secondary_buffer, 0, secondary_buffer_size, &buffer1, &buffer1_size, &buffer2, &buffer2_size, 0)))
        {
            // fill the buffer with silence
            ZeroMemory(buffer1, buffer1_size);
            ZeroMemory(buffer2, buffer2_size);

            if (!SUCCEEDED(IDirectSoundBuffer_Unlock(secondary_buffer, buffer1, buffer1_size, buffer2, buffer2_size)))
            {
                LK_Log("Warning: DirectSoundBuffer.Unlock() failed while initializing audio.");
            }
        }
        else
        {
            LK_Log("Warning: DirectSoundBuffer.Lock() failed while initializing audio.");
        }
    }

    lk_private.audio.secondary_buffer = secondary_buffer;

    if (lk_platform.audio.strategy == LK_AUDIO_MIXER)
    {
        HANDLE mutex = CreateMutex(0, 0, 0);
        lk_private.audio.mixer_mutex = mutex;

        if (!mutex)
        {
            LK_Log("Failed to initialize the audio mixer; couldn't create a mutex object.");
            lk_platform.audio.strategy = LK_NO_AUDIO;
            return;
        }
    }

    CreateThread(0, 0, lk_audio_thread, 0, 0, 0);
}

static void lk_fill_system_info()
{
    // @Incomplete - On x86, check if we have cpuid at all.

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

    static char vendor[13];
    *(LK_U32*) &vendor[0] = id0[1];
    *(LK_U32*) &vendor[4] = id0[3];
    *(LK_U32*) &vendor[8] = id0[2];
    vendor[12] = 0;

    lk_platform.system.vendor = vendor;

    //
    // get core/logical count
    //

    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    lk_platform.system.logical_count = system_info.dwNumberOfProcessors;

    //
    // get cache size
    //

    if (max_function >= 1)
    {
        lk_platform.system.cache_line_size = ((id1[1] >> 8) & 0xFF) * 8;
    }

    //
    // check feature support
    //

    if (max_function >= 1)
    {
        lk_platform.system.has_rdtsc = (id1[3] & 0x00000010u) ? 1 : 0;
        lk_platform.system.has_mmx   = (id1[3] & 0x00800000u) ? 1 : 0;
        lk_platform.system.has_sse   = (id1[3] & 0x02000000u) ? 1 : 0;
        lk_platform.system.has_sse2  = (id1[3] & 0x04000000u) ? 1 : 0;
        lk_platform.system.has_hyperthreading = (id1[3] & 0x10000000u) ? 1 : 0;
        lk_platform.system.has_sse3  = (id1[2] & 0x00000001u) ? 1 : 0;
        lk_platform.system.has_sse41 = (id1[2] & 0x00080000u) ? 1 : 0;
        lk_platform.system.has_sse42 = (id1[2] & 0x00100000u) ? 1 : 0;

        if (os_saves_ymm)
        {
            lk_platform.system.has_avx = (id1[2] & 0x10000000u) ? 1 : 0;

            if (max_function >= 7)
            {
                lk_platform.system.has_avx2    = (id7[1] & 0x00000020u) ? 1 : 0;
                lk_platform.system.has_avx512f = (id7[1] & 0x00010000u) && os_saves_zmm;
            }
        }
    }

    if (id80000000[0] >= 0x80000001)
    {
        if (id80000001[3] & 0x80000000)
        {
            lk_platform.system.has_3dnow = 1;
        }
    }

    //
    // get memory info
    //

    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);
    if (GlobalMemoryStatusEx(&memory_status))
    {
        lk_platform.system.ram_bytes     = memory_status.ullTotalPhys;
        lk_platform.system.ram_kilobytes = memory_status.ullTotalPhys / 1024;
        lk_platform.system.ram_megabytes = memory_status.ullTotalPhys / (1024 * 1024);
    }

/*
    printf("vendor: %s\n", vendor);
    printf("logical count: %d\n", lk_platform.system.logical_count);
    printf("cache line size: %d\n", lk_platform.system.cache_line_size);
    if (lk_platform.system.has_rdtsc) printf("rdtsc\n");
    if (lk_platform.system.has_mmx) printf("mmx\n");
    if (lk_platform.system.has_sse) printf("sse\n");
    if (lk_platform.system.has_sse2) printf("sse2\n");
    if (lk_platform.system.has_sse3) printf("sse3\n");
    if (lk_platform.system.has_sse41) printf("sse41\n");
    if (lk_platform.system.has_sse42) printf("sse42\n");
    if (lk_platform.system.has_avx) printf("avx\n");
    if (lk_platform.system.has_avx2) printf("avx2\n");
    if (lk_platform.system.has_avx512f) printf("avx\n");
    if (lk_platform.system.has_3dnow) printf("3dnow\n");
    if (lk_platform.system.has_hyperthreading) printf("hyperthreading\n");
    printf("RAM: %llu bytes (%llu MB)\n", lk_platform.system.ram_bytes, lk_platform.system.ram_megabytes);
*/
}

static void lk_get_command_line_arguments()
{
    LPWSTR command_line = GetCommandLineW();

    int argc;
    LPWSTR* argv = CommandLineToArgvW(command_line, &argc);
    if (!argv)
    {
        return;
    }

    int allocation_size = argc * sizeof(char*);
    for (int i = 0; i < argc; i++)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, 0, 0, 0, 0);
        if (size == 0)
        {
            LocalFree(argv);
            return;
        }

        allocation_size += size;
    }

    LK_U8* memory = (LK_U8*) LocalAlloc(LMEM_FIXED, allocation_size);
    if (!memory)
    {
        LocalFree(argv);
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
            return;
        }

        result[i] = write_cursor;
        write_cursor += size;
        available -= size;
    }

    LocalFree(argv);

    lk_platform.command_line.argument_count = argc;
    lk_platform.command_line.arguments = result;
}

void lk_entry()
{
    lk_fill_system_info();
    lk_get_command_line_arguments();

#ifndef LK_PLATFORM_NO_DLL
    lk_get_dll_path();
    lk_check_client_reload();
    lk_get_temp_dll_path();
#endif

    lk_platform.window.x = LK_DEFAULT_POSITION;
    lk_platform.window.y = LK_DEFAULT_POSITION;

    lk_platform.opengl.color_bits   = 32;
    lk_platform.opengl.depth_bits   = 24;
    lk_platform.opengl.stencil_bits = 8;
    lk_platform.opengl.sample_count = 1;

    lk_initialize_timer();

#ifdef LK_PLATFORM_NO_DLL
    lk_set_client_functions();
#else
    lk_load_client();
#endif

    lk_private.client.init(&lk_platform);

    lk_private.window.backend = lk_platform.window.backend;

    if (!lk_platform.window.no_window)
    {
        lk_private.window.main_fiber = ConvertThreadToFiber(0);
        lk_private.window.message_fiber = CreateFiber(0, lk_message_fiber_proc, 0);

        lk_open_window();
    }

    if (lk_platform.audio.strategy != LK_NO_AUDIO)
    {
        lk_initialize_audio();
    }

    while (!lk_platform.break_frame_loop)
    {
        lk_push();
        lk_window_message_loop();
        lk_pull();

#ifndef LK_PLATFORM_NO_DLL
        if (lk_check_client_reload())
        {
            lk_unload_client();
            lk_load_client();
        }
#endif

        lk_update_time_stamp();
        lk_private.client.frame(&lk_platform);

        lk_mixer_synchronize();
        lk_window_swap_buffers();
    }

    lk_update_time_stamp();
    lk_private.client.close(&lk_platform);

    lk_close_window();

#ifdef LK_PLATFORM_NO_DLL
    lk_private.client.dll_unload(&lk_platform);
#else
    lk_unload_client();
#endif
}

#ifndef LK_PLATFORM_NO_DLL
static BOOL WINAPI lk_console_ctrl_handler(DWORD ctrl_type)
{
    if (ctrl_type == CTRL_C_EVENT)
    {
        // We want to kill the client quickly, without calling dll_unload.
        // That's why we set dll_unload to the stub function here.
        lk_private.client.dll_unload = lk_client_dll_unload_stub;
        lk_unload_client();
    }

    return FALSE;
}
#endif

void lk_console_entry()
{
#ifndef LK_PLATFORM_NO_DLL
    SetConsoleCtrlHandler(lk_console_ctrl_handler, TRUE);
#endif
    lk_entry();
}

#ifndef LK_PLATFORM_NO_MAIN
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    lk_entry();
    return 0;
}

int main(int argc, char** argv)
{
    lk_console_entry();
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
