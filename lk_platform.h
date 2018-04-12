//  lk_platform.h - public domain platform abstraction layer
//  no warranty is offered or implied

/*********************************************************************************************

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

#define LK_CLIENT_INIT(name) void __cdecl name(LK_Platform* platform)
#define LK_CLIENT_CLOSE(name) void __cdecl name(LK_Platform* platform)
#define LK_CLIENT_FRAME(name) void __cdecl name(LK_Platform* platform)

#ifdef __cplusplus
extern "C"
{
#endif

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

typedef enum
{
    LK_WINDOW_CANVAS,
    LK_WINDOW_OPENGL,
} LK_Window_Backend;

typedef struct
{
    LK_B8 pressed;
    LK_B8 released;
    LK_B8 down;
    LK_B8 was_down; // The state of "down" for the previous frame.
} LK_Digital_Button;

typedef enum
{
    LK_KEY_ESCAPE = 1,
    LK_KEY_GRAVE,
    LK_KEY_TAB,
    LK_KEY_CAPS_LOCK,
    LK_KEY_LEFT_SHIFT,
    LK_KEY_LEFT_CONTROL,
    LK_KEY_LEFT_WINDOWS,
    LK_KEY_LEFT_ALT,
    LK_KEY_RIGHT_SHIFT,
    LK_KEY_RIGHT_CONTROL,
    LK_KEY_RIGHT_WINDOWS,
    LK_KEY_RIGHT_ALT,
    LK_KEY_SPACE,
    LK_KEY_BACKSPACE,
    LK_KEY_ENTER,

    LK_KEY_PRINT_SCREEN,
    LK_KEY_SCREEN_LOCK,
    LK_KEY_PAUSE,
    LK_KEY_INSERT,
    LK_KEY_DELETE,
    LK_KEY_HOME,
    LK_KEY_END,
    LK_KEY_PAGE_UP,
    LK_KEY_PAGE_DOWN,

    LK_KEY_ARROW_LEFT,
    LK_KEY_ARROW_RIGHT,
    LK_KEY_ARROW_UP,
    LK_KEY_ARROW_DOWN,

    LK_KEY_0 = 48, // '0'
    LK_KEY_1,
    LK_KEY_2,
    LK_KEY_3,
    LK_KEY_4,
    LK_KEY_5,
    LK_KEY_6,
    LK_KEY_7,
    LK_KEY_8,
    LK_KEY_9,

    LK_KEY_A = 65, // 'A'
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

    LK_KEY_PERIOD,
    LK_KEY_COMMA,

    LK_KEY_NUMLOCK = 128,
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
    LK_DEFAULT_POSITION = 0x80000000,
};

typedef struct
{
    LK_B32 break_frame_loop;
    void* client_data;

    struct
    {
        LK_B32 no_window;
        LK_Window_Backend backend;

        char*  title;

        // During initialization, x and y will be set to LK_DEFAULT_POSITION, width and height will be set to zero.
        // You may modify these to specify initial window coordinates or dimensions. Otherwise, the window will be
        // centered on the primary monitor and get some default dimensions. During execution, changing these will
        // move or resize the window.
        // All of these are relative to the client space, not the actual window.
        LK_S32 x;
        LK_S32 y;
        LK_U32 width;
        LK_U32 height;

        LK_B32 forbid_resizing;
        LK_B32 undecorated;
        LK_B32 invisible;
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
    } opengl;

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

#ifndef LK_PLATFORM_DLL_NAME
#error "lk_platform.h implementation expects LK_PLATFORM_DLL_NAME to be defined before it is included."
#endif

#ifndef LK_PLATFORM_TEMP_DLL_NAME
#define LK_PLATFORM_TEMP_DLL_NAME LK_PLATFORM_DLL_NAME "_temp"
#endif

#ifndef LK_WINDOW_CLASS_NAME
#define LK_WINDOW_CLASS_NAME L"lk_platform_window_class"
#endif


#include <windows.h> // @Incomplete - get rid of this include


typedef LK_CLIENT_INIT(LK_Client_Init_Function);
typedef LK_CLIENT_CLOSE(LK_Client_Close_Function);
typedef LK_CLIENT_FRAME(LK_Client_Frame_Function);

typedef HGLRC WGLCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL WGLSwapIntervalEXT(int interval);

enum
{
    LK_MAX_TEXT_SIZE = 256,
};

typedef struct
{
    struct
    {
        FILETIME last_dll_write_time;
        CHAR dll_path[MAX_PATH];      // @Incomplete - Never use MAX_PATH, because it's not correct for long file names!
        CHAR temp_dll_path[MAX_PATH]; // @Incomplete - Never use MAX_PATH, because it's not correct for long file names!

        HMODULE library;
        LK_Client_Init_Function* init;
        LK_Client_Close_Function* close;
        LK_Client_Frame_Function* frame;
    } client;

    struct
    {
        HWND handle;
        HDC dc;

        LK_Window_Backend backend;
        BITMAPINFO bitmap_info;

        LK_S32 x;
        LK_S32 y;
        LK_U32 width;
        LK_U32 height;

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
        HGLRC context;
        WGLCreateContextAttribsARB* wglCreateContextAttribsARB;
        WGLSwapIntervalEXT* wglSwapIntervalEXT;
    } opengl;

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


static LK_CLIENT_INIT(lk_client_init_stub) {}
static LK_CLIENT_FRAME(lk_client_frame_stub) {}
static LK_CLIENT_CLOSE(lk_client_close_stub) {}

static void lk_get_dll_paths()
{
    const char dll_name[] = LK_PLATFORM_DLL_NAME ".dll";
    const char temp_dll_name[] = LK_PLATFORM_TEMP_DLL_NAME ".dll";

    LPSTR dll_path = lk_private.client.dll_path;
    LPSTR temp_dll_path = lk_private.client.temp_dll_path;

    DWORD directory_length = GetModuleFileNameA(0, dll_path, sizeof(lk_private.client.dll_path)); // @Incomplete - the buffer could be too small!
    while (dll_path[directory_length] != '\\' && dll_path[directory_length] != '/')
        directory_length--;
    directory_length++;

    CopyMemory(dll_path + directory_length, dll_name, sizeof(dll_name) + 1);
    CopyMemory(temp_dll_path, dll_path, directory_length);
    CopyMemory(temp_dll_path + directory_length, temp_dll_name, sizeof(temp_dll_name) + 1);
}

static LK_B32 lk_check_client_reload()
{
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

static void lk_load_client()
{
    LPSTR dll_path = lk_private.client.dll_path;
    LPSTR temp_dll_path = lk_private.client.temp_dll_path;

    if (!CopyFileA(dll_path, temp_dll_path, 0))
    {
        /* @Incomplete - logging */
    }

    HMODULE library = LoadLibraryA(temp_dll_path);
    lk_private.client.library = library;

    if (library)
    {
        #define LK_GetClientFunction(ptr, type, name)     \
            ptr = (type*) GetProcAddress(library, #name); \
            if (!ptr)                                     \
            {                                             \
                /* @Incomplete - logging */               \
                ptr = name##_stub;                        \
            }

        LK_GetClientFunction(lk_private.client.init,  LK_Client_Init_Function,  lk_client_init);
        LK_GetClientFunction(lk_private.client.close, LK_Client_Close_Function, lk_client_close);
        LK_GetClientFunction(lk_private.client.frame, LK_Client_Frame_Function, lk_client_frame);

        #undef LK_GetClientFunction
    }
    else
    {
        /* @Incomplete - logging */
    }
}

static void lk_unload_client()
{
    if (lk_private.client.library)
    {
        if (!FreeLibrary(lk_private.client.library))
        {
            /* @Incomplete - logging */
        }

        lk_private.client.library = 0;
    }
}

static void lk_window_update_title()
{
    HWND window = lk_private.window.handle;
    SetWindowTextA(window, lk_platform.window.title);
}

static LONG lk_apply_window_style(LONG old_style)
{
    LONG resizable_flags = WS_THICKFRAME | WS_MAXIMIZEBOX;
    LONG decoration_flags = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    LONG style = old_style & ~(resizable_flags | decoration_flags);

    if (!lk_platform.window.undecorated)
    {
        style |= decoration_flags;
        style &= ~WS_POPUP;

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

    int force_resize = 0;

    if (lk_private.window.undecorated != lk_platform.window.undecorated ||
        lk_private.window.forbid_resizing != lk_platform.window.forbid_resizing)
    {
        LONG style = GetWindowLong(window, GWL_STYLE);
        style = lk_apply_window_style(style);
        SetWindowLong(window, GWL_STYLE, style);

        lk_private.window.undecorated = lk_platform.window.undecorated;
        lk_private.window.forbid_resizing = lk_platform.window.forbid_resizing;
        force_resize = 1;
    }

    if ((lk_private.window.width  != lk_platform.window.width ) ||
        (lk_private.window.height != lk_platform.window.height) ||
        (lk_private.window.x      != lk_platform.window.x     ) ||
        (lk_private.window.y      != lk_platform.window.y     ) ||
        force_resize)
    {
        LONG style = GetWindowLong(window, GWL_STYLE);
        LONG extended_style = GetWindowLong(window, GWL_EXSTYLE);

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

        MoveWindow(window, x, y, width, height, 0);
    }

    if (lk_private.window.invisible != lk_platform.window.invisible)
    {
        ShowWindow(window, lk_platform.window.invisible ? SW_HIDE : SW_SHOW);
        lk_private.window.invisible = lk_platform.window.invisible;
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


    LRESULT result = 0;

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
        break;
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
                lk_platform.keyboard.state[key].down = is_down;
            }
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

    default:
    {
        goto run_default_proc;
    } break;

    }

    return result;
run_default_proc:
    return DefWindowProcW(window, message, wparam, lparam);
}

static void lk_update_swap_interval()
{
    if (lk_private.opengl.wglSwapIntervalEXT)
    {
        int interval = lk_platform.opengl.swap_interval;
        lk_private.opengl.wglSwapIntervalEXT(interval);
    }
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
        /* @Incomplete - logging */
        return;
    }

    DWORD extended_style = 0;
    DWORD style = lk_apply_window_style(0);

    // adjust window size if specified (client size to window size)
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    if (lk_platform.window.width || lk_platform.window.height)
    {
        width = lk_platform.window.width;
        height = lk_platform.window.height;

        RECT window_bounds;
        window_bounds.left = 0;
        window_bounds.top = 0;
        window_bounds.right = width;
        window_bounds.bottom = height;
        AdjustWindowRectEx(&window_bounds, style, 0, extended_style);

        width = (window_bounds.right - window_bounds.left);
        height = (window_bounds.bottom - window_bounds.top);
    }

    int x = lk_platform.window.x;
    int y = lk_platform.window.y;

    if (x == LK_DEFAULT_POSITION || y == LK_DEFAULT_POSITION)
    {
        // try to position the window on the center of the primary monitor
        x = CW_USEDEFAULT;
        y = CW_USEDEFAULT;

        HMONITOR primary_monitor = MonitorFromWindow(0, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO primary_monitor_info;
        primary_monitor_info.cbSize = sizeof(MONITORINFO);

        if (GetMonitorInfo(primary_monitor, &primary_monitor_info))
        {
            x = (primary_monitor_info.rcWork.left + primary_monitor_info.rcWork.right  - width ) / 2;
            y = (primary_monitor_info.rcWork.top  + primary_monitor_info.rcWork.bottom - height) / 2;
        }
    }
    else
    {
        RECT window_bounds;
        window_bounds.left = 0;
        window_bounds.top = 0;
        window_bounds.right = 0;
        window_bounds.bottom = 0;
        AdjustWindowRectEx(&window_bounds, style, 0, extended_style);

        x += window_bounds.left;
        y += window_bounds.top;
    }



    HWND window_handle = CreateWindowExW(extended_style, window_class.lpszClassName, L"", style,
                                         x, y, width, height, 0, 0, instance, 0);

    if (!window_handle)
    {
        /* @Incomplete - logging */
        return;
    }


    lk_private.window.handle = window_handle;


    if (!lk_platform.window.title)
    {
        lk_platform.window.title = "app";
    }

    SetWindowTextA(window_handle, lk_platform.window.title);

    if (!lk_platform.window.invisible)
    {
        ShowWindow(window_handle, SW_SHOW);
    }


    lk_private.window.undecorated = lk_platform.window.undecorated;
    lk_private.window.forbid_resizing = lk_platform.window.forbid_resizing;
    lk_private.window.invisible = lk_platform.window.invisible;

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
        /* @Incomplete - logging */
        return;
    }

    PIXELFORMATDESCRIPTOR pixel_format_desc;
    ZeroMemory(&pixel_format_desc, sizeof(PIXELFORMATDESCRIPTOR));
    pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format_desc.nVersion = 1;
    pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
    pixel_format_desc.cColorBits = 32;
    pixel_format_desc.cDepthBits = 32;
    pixel_format_desc.cStencilBits = 32;
    pixel_format_desc.iLayerType = PFD_MAIN_PLANE;

    LK_B32 use_opengl = (lk_private.window.backend == LK_WINDOW_OPENGL);
    if (use_opengl)
    {
        pixel_format_desc.dwFlags |= PFD_SUPPORT_OPENGL;
    }

    int pixel_format = ChoosePixelFormat(dc, &pixel_format_desc);
    if (!pixel_format)
    {
        /* @Incomplete - logging */
        return;
    }

    if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc))
    {
        /* @Incomplete - logging */
        return;
    }

    if (use_opengl)
    {
        HGLRC context = wglCreateContext(dc);
        lk_private.opengl.context = context;

        if (!context)
        {
            /* @Incomplete - logging */
            return;
        }

        if (!wglMakeCurrent(dc, context))
        {
            /* @Incomplete - logging */
            return;
        }


        #define LK_GetWGLFunction(type, name)                                                                       \
        {                                                                                                           \
            void* proc = wglGetProcAddress(#name);                                                                  \
            if (proc && (proc != (void*) 1) && (proc != (void*) 2) && (proc != (void*) 3) && (proc != (void*) -1))  \
                lk_private.opengl.name = (type*) proc;                                                              \
            else                                                                                                    \
                lk_private.opengl.name = 0;                                                                         \
        }

        LK_GetWGLFunction(WGLCreateContextAttribsARB, wglCreateContextAttribsARB)
        LK_GetWGLFunction(WGLSwapIntervalEXT, wglSwapIntervalEXT)

        #undef LK_GetWGLFunction


        if (lk_private.opengl.wglCreateContextAttribsARB)
        {
            wglMakeCurrent(0, 0);
            wglDeleteContext(context);

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

            context = lk_private.opengl.wglCreateContextAttribsARB(dc, 0, attributes);

            if (!context)
            {
                /* @Incomplete - logging */
                return;
            }

            if (!wglMakeCurrent(dc, context))
            {
                /* @Incomplete - logging */
                return;
            }
        }
    }

    lk_pull_window_data();
    lk_pull_mouse_data();
    lk_update_canvas();
    lk_update_swap_interval();


    RAWINPUTDEVICE raw_input_mouse;
    ZeroMemory(&raw_input_mouse, sizeof(raw_input_mouse));
    raw_input_mouse.usUsagePage = 0x01;
    raw_input_mouse.usUsage = 0x02;
    raw_input_mouse.hwndTarget = window_handle;
    if (!RegisterRawInputDevices(&raw_input_mouse, 1, sizeof(raw_input_mouse)))
    {
        /* @Incomplete - logging */
        return;
    }

    RAWINPUTDEVICE raw_input_keyboard;
    ZeroMemory(&raw_input_keyboard, sizeof(raw_input_keyboard));
    raw_input_keyboard.usUsagePage = 0x01;
    raw_input_keyboard.usUsage = 0x06;
    raw_input_keyboard.hwndTarget = window_handle;
    if (!RegisterRawInputDevices(&raw_input_keyboard, 1, sizeof(raw_input_keyboard)))
    {
        /* @Incomplete - logging */
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
        wglMakeCurrent(0, 0);
        wglDeleteContext(context);
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

static void lk_window_message_loop()
{
    if (!lk_private.window.handle)
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

    MSG message;
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    if (lk_private.window.backend == LK_WINDOW_CANVAS)
    {
        lk_update_canvas();
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

    lk_platform.time.delta_seconds = (double) delta_ticks / (double) frequency;

    lk_platform.time.ticks        += delta_ticks;
    lk_platform.time.nanoseconds  += lk_platform.time.delta_nanoseconds;
    lk_platform.time.microseconds += lk_platform.time.delta_microseconds;
    lk_platform.time.milliseconds += lk_platform.time.delta_milliseconds;
    lk_platform.time.seconds      += lk_platform.time.delta_seconds;
}

static void lk_entry()
{
    lk_get_dll_paths();
    lk_check_client_reload();

    lk_platform.window.x = LK_DEFAULT_POSITION;
    lk_platform.window.y = LK_DEFAULT_POSITION;

    lk_load_client();

    lk_initialize_timer();
    lk_private.client.init(&lk_platform);

    lk_private.window.backend = lk_platform.window.backend;

    if (!lk_platform.window.no_window)
    {
        lk_open_window();
    }

    while (!lk_platform.break_frame_loop)
    {
        if (lk_check_client_reload())
        {
            lk_unload_client();
            lk_load_client();
        }

        lk_window_message_loop();

        lk_update_time_stamp();
        lk_private.client.frame(&lk_platform);

        lk_window_update_title();

        lk_window_swap_buffers();
    }

    lk_update_time_stamp();
    lk_private.client.close(&lk_platform);

    lk_close_window();
    lk_unload_client();
}

#ifndef LK_PLATFORM_NO_MAIN
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    lk_entry();
    return 0;
}

int main(int argc, char** argv)
{
    lk_entry();
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