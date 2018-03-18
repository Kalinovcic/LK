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
    LK_B32 break_frame_loop;
    void* client_data;

    struct
    {
        LK_B32 no_window;
        LK_Window_Backend backend;

        char*  title;
        LK_S32 x;
        LK_S32 y;
        LK_U32 width;
        LK_U32 height;
        LK_B32 forbid_resizing;
    } window;

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
#define LK_WINDOW_CLASS_NAME "lk_platform_window_class"
#endif


#include <windows.h> // @Incomplete - get rid of this include


typedef LK_CLIENT_INIT(LK_Client_Init_Function);
typedef LK_CLIENT_CLOSE(LK_Client_Close_Function);
typedef LK_CLIENT_FRAME(LK_Client_Frame_Function);

typedef HGLRC WGLCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL WGLSwapIntervalEXT(int interval);

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
    } window;

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

static void lk_update_window_size()
{
    HWND window = lk_private.window.handle;

    RECT client_rect;
    GetClientRect(window, &client_rect);

    LK_U32 width = (LK_U32)(client_rect.right - client_rect.left);
    LK_U32 height = (LK_U32)(client_rect.bottom - client_rect.top);

    lk_platform.window.width = width;
    lk_platform.window.height = height;
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
        return DefWindowProc(window, message, wparam, lparam);
    }


    LRESULT result = 0;

    switch (message)
    {

    case WM_CLOSE:
    {
        lk_platform.break_frame_loop = 1;
    } break;

    case WM_SIZE:
    {
        lk_update_window_size();
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
    } break;

    default:
    {
        result = DefWindowProc(window, message, wparam, lparam);
    } break;

    }

    return result;
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
    WNDCLASSEXA window_class;
    ZeroMemory(&window_class, sizeof(WNDCLASSEXA));

    HINSTANCE instance = GetModuleHandle(0);

    window_class.cbSize = sizeof(WNDCLASSEXA);
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = lk_window_callback;
    window_class.hInstance = instance;
    window_class.lpszClassName = LK_WINDOW_CLASS_NAME;

    if (!RegisterClassExA(&window_class))
    {
        /* @Incomplete - logging */
        return;
    }

    DWORD extended_style = 0;
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    if (lk_platform.window.width != 0 && lk_platform.window.height != 0)
    {
        width = lk_platform.window.width;
        height = lk_platform.window.height;

        RECT window_bounds;
        window_bounds.left = 0;
        window_bounds.top = 0;
        window_bounds.right = width;
        window_bounds.bottom = height;
        AdjustWindowRectEx(&window_bounds, style, 0, extended_style);

        width = window_bounds.right - window_bounds.left;
        height = window_bounds.bottom - window_bounds.top;
    }

    LPCSTR title = lk_platform.window.title;
    if (!title)
    {
        title = "lk_platform.h";
        lk_platform.window.title = (char*) title;
    }

    HWND window_handle = CreateWindowExA(extended_style, window_class.lpszClassName, title, style,
                                         CW_USEDEFAULT, CW_USEDEFAULT,
                                         width, height,
                                         0, 0, instance, 0);

    if (!window_handle)
    {
        /* @Incomplete - logging */
        return;
    }

    lk_private.window.handle = window_handle;

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
                WGL_CONTEXT_MAJOR_VERSION_ARB, lk_platform.opengl.major_version,
                WGL_CONTEXT_MINOR_VERSION_ARB, lk_platform.opengl.minor_version,
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

    lk_update_window_size();
    lk_update_canvas();
    lk_update_swap_interval();
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

    UnregisterClass(LK_WINDOW_CLASS_NAME, instance);

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