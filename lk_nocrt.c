//  lk_nocrt.h - public domain minimal CRT implementation
//  no warranty is offered or implied

/*********************************************************************************************

THIS IS NOT A CRT REPLACEMENT. This library exists to help you compile programs without the CRT.
This sound easy and this library shouldn't really exist. But the authors of modern C/C++ compilers are
doing a great job at making your life as difficult as possible when doing this.

QUICK NOTES
    Compile your programs with the following compiler and linker switches:

        set CL_NOCRT= lk_nocrt.c /GS- /Gs10000000 /GR- /EHa-
        set LINK_NOCRT= /nodefaultlib /stack:0x100000,0x100000

        cl %CL_NOCRT% <your cl input> -link %LINK_NOCRT% <your link input>

    Remember that once you remove the CRT, you're not allowed to use:
        C++ RTTI (turned off by /GR-)
        C++ exceptions (turned off by /EHa-)
        SEH exceptions (you can if you implement _C_specific_handler for 64-bit and _except_handler3 for 32-bit architectures)
        globals with C++ constructors/destructors (you can implement it yourself)
        pure virtual functions (you can if you implement __purecall)
        new/delete C++ operators (you can if you implement them)

    Most of these you don't care about if you're working in C,
    and you probably shouldn't care about if you're working in C++.

    Of course, you can't use anything from the C standard library, except for:
        stddef.h  for size_t and NULL
        stdint.h  for int##_t and uint##_t
        stdarg.h  for varargs
        intrin.h  for intrinsics


LICENSE
    This software is in the public domain. Anyone can use it, modify it,
    roll'n'smoke hardcopies of the source code, sell it to the terrorists, etc.
    No warranty is offered or implied; use this code at your own risk!

    See end of file for license information.


DOCUMENTATION

  ENTRY POINT
    The entry name for executable programs is WinMainCRTStartup,
    for DLLs it is _DllMainCRTStartup.

    You can change this name with a linker switch:
        /ENTRY:my_entry_name

    lk_nocrt.h will add this entry function for you. If you're using a different name, define:
        #define LK_NOCRT_ENTRY_POINT my_entry_name
    before including lk_nocrt.h

    If you don't want the entry point to be added to your program, define:
        #define LK_NOCRT_NO_ENTRY_POINT
    before including lk_nocrt.h


  FUNCTIONS WITH LARGE STACK FOOTPRINTS
    If you allocate large arrays or structures on the stack (larger than ~4kB),
    the compiler generates calls to __chkstk and some security junk.

    Microsoft's idea of security is a joke, so just turn that off with a compiler switch:
        /GS-
    This gets rid of calls to:
        ___security_cookie
        @__security_check_cookie@4
        ___report_rangecheckfailure
    Maybe these functions can help you with debugging, so you might want to compile with the CRT in debug builds.
    But in release builds, these just create additional overhead.

    As for __chkstk, that function does 4kB page probing. Windows usually reserves 1MB of memory for the stack,
    but only commits a few pages. This function would commit more pages. However, computers have infinite memory
    these days, so you can just turn this off with another compiler switch:
        /Gs10000000
    and a linker switch:
        /STACK:0x100000,0x100000
    Alternatively, you could implement __chkstk yourself.


  CLEARING AND COPYING LARGE STRUCTURES
    When zeroing big arrays or structures, the compiler assumes it can call memset to clear that memory.
    Also, when copying large structures, the optimizer generates calls to memcpy.

    If you don't want memset and memcpy to be added to your program, define:
        #define LK_NOCRT_NO_LARGE_STRUCTURES
    before including lk_nocrt.h


  FLOATING POINT SUPPORT
    If your code contains floating point ops, the linker will want to see the "_fltused" symbol.
    As far as I know, it doens't actually care about its value, it just wants the symbol to exist.
    Additionally, on 32-bit architectures, the compiler generates calls to several functions when casting
    between floating point and integer.

    If you don't want _fltused and these functions to be added to your program, define:
        #define LK_NOCRT_NO_FLOATING_POINT
    before including lk_nocrt.h

 *********************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif



#ifndef LK_NOCRT_NO_FLOATING_POINT

int _fltused;

#ifdef _M_IX86
__declspec(naked) void _ftol2()
{
    __asm
    {
        fistp qword ptr [esp-8]
        mov   edx,[esp-4]
        mov   eax,[esp-8]
        ret
    }
}

__declspec(naked) void _ftol2_sse()
{
    __asm
    {
        fistp dword ptr [esp-4]
        mov   eax,[esp-4]
        ret
    }
}
#endif

#endif



#ifndef LK_NOCRT_NO_LARGE_STRUCTURES

#ifdef _WIN64
#define LK_NOCRT_SIZE_T unsigned __int64
#else
#define LK_NOCRT_SIZE_T unsigned int
#endif

#pragma function(memset)
void* memset(void* dest, int c, LK_NOCRT_SIZE_T count)
{
    char* bytes = (char*) dest;
    while (count--)
    {
        *(bytes++) = (char) c;
    }
    return dest;
}

#pragma function(memcpy)
void* memcpy(void* dest, const void* src, LK_NOCRT_SIZE_T count)
{
    char* dest_bytes = (char*) dest;
    const char* src_bytes = (const char*) src;
    while (count--)
    {
        *(dest_bytes++) = *(src_bytes++);
    }
    return dest;
}

#undef LK_NOCRT_SIZE_T

#endif



#ifndef LK_NOCRT_NO_ENTRY_POINT

#ifndef LK_NOCRT_ENTRY_POINT
#define LK_NOCRT_ENTRY_POINT WinMainCRTStartup
#endif

__declspec(dllimport) void* __stdcall GetModuleHandleA(const char* lpModuleName);

void __stdcall LK_NOCRT_ENTRY_POINT()
{
    int exit_code = WinMain(GetModuleHandleA(0), 0, GetCommandLineA(), 0);
    ExitProcess(exit_code);
}

#endif



#ifdef __cplusplus
}
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