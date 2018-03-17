#ifndef LK_PLATFORM_HEADER
#define LK_PLATFORM_HEADER

#ifdef __cplusplus
#define LK_CLIENT_EXPORT extern "C" __declspec(dllexport)
#else
#define LK_CLIENT_EXPORT __declspec(dllexport)
#endif

#define LK_CLIENT_INIT(name) void __cdecl name(LK_Platform* platform)
#define LK_CLIENT_FRAME(name) void __cdecl name(LK_Platform* platform)

#ifdef __cplusplus
extern "C"
{
#endif

typedef signed char  LK_S8;
typedef signed short LK_S16;
typedef signed long  LK_S32;

typedef unsigned char  LK_U8;
typedef unsigned short LK_U16;
typedef unsigned long  LK_U32;

typedef LK_U8  LK_B8;
typedef LK_U16 LK_B16;
typedef LK_U32 LK_B32;

typedef struct
{
    struct
    {
        LK_S32 x;
        LK_S32 y;
        LK_S32 width;
        LK_S32 height;
    } window;
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

#include <windows.h> // @Incomplete - get rid of this include


typedef LK_CLIENT_INIT(LK_Client_Init_Function);
typedef LK_CLIENT_FRAME(LK_Client_Frame_Function);

typedef struct
{
    FILETIME last_dll_write_time;
    CHAR dll_path[MAX_PATH];      // @Incomplete - Never use MAX_PATH, because it's not correct for long file names!
    CHAR temp_dll_path[MAX_PATH]; // @Incomplete - Never use MAX_PATH, because it's not correct for long file names!

    HMODULE library;
    LK_Client_Init_Function* init;
    LK_Client_Frame_Function* frame;
} LK_Platform_Private;


static LK_CLIENT_INIT(lk_client_init_stub) {}
static LK_CLIENT_FRAME(lk_client_frame_stub) {}

static void lk_get_dll_paths(LK_Platform_Private* private)
{
    const char dll_name[] = LK_PLATFORM_DLL_NAME ".dll";
    const char temp_dll_name[] = LK_PLATFORM_TEMP_DLL_NAME ".dll";

    LPSTR dll_path = private->dll_path;
    LPSTR temp_dll_path = private->temp_dll_path;

    DWORD directory_length = GetModuleFileNameA(0, dll_path, sizeof(private->dll_path)); // @Incomplete - the buffer could be too small!
    while (dll_path[directory_length] != '\\' && dll_path[directory_length] != '/')
        directory_length--;
    directory_length++;

    CopyMemory(dll_path + directory_length, dll_name, sizeof(dll_name) + 1);
    CopyMemory(temp_dll_path, dll_path, directory_length);
    CopyMemory(temp_dll_path + directory_length, temp_dll_name, sizeof(temp_dll_name) + 1);
}

static LK_B32 lk_check_client_reload(LK_Platform_Private* private)
{
    FILETIME file_time;
    ZeroMemory(&file_time, sizeof(FILETIME));

    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFile(private->dll_path, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        file_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }

    if (CompareFileTime(&file_time, &private->last_dll_write_time) != 0)
    {
        private->last_dll_write_time = file_time;
        return 1;
    }

    return 0;
}

static void lk_load_client(LK_Platform_Private* private)
{
    LPSTR dll_path = private->dll_path;
    LPSTR temp_dll_path = private->temp_dll_path;

    if (!CopyFileA(dll_path, temp_dll_path, 0))
    {
        /* @Incomplete - logging */
    }

    private->library = LoadLibraryA(temp_dll_path);
    if (private->library)
    {
        #define LK_GetClientFunction(ptr, type, name)               \
            ptr = (type*) GetProcAddress(private->library, #name);  \
            if (!ptr)                                               \
            {                                                       \
                /* @Incomplete - logging */                         \
                ptr = name##_stub;                                  \
            }

        LK_GetClientFunction(private->init,  LK_Client_Init_Function,  lk_client_init);
        LK_GetClientFunction(private->frame, LK_Client_Frame_Function, lk_client_frame);

        #undef LK_GetClientFunction
    }
    else
    {
        /* @Incomplete - logging */
    }
}

static void lk_unload_client(LK_Platform_Private* private)
{
    if (private->library)
    {
        if (!FreeLibrary(private->library))
        {
            /* @Incomplete - logging */
        }

        private->library = 0;
    }
}

static void lk_entry()
{
    static LK_Platform platform;
    static LK_Platform_Private private;

    lk_get_dll_paths(&private);
    lk_check_client_reload(&private);

    lk_load_client(&private);
    private.init(&platform);

    while (1)
    {
        if (lk_check_client_reload(&private))
        {
            lk_unload_client(&private);
            lk_load_client(&private);
        }

        private.frame(&platform);
    }

    lk_unload_client(&private);
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