//  lk_nocrt.c - public domain minimal CRT implementation
//  no warranty is offered or implied

/*********************************************************************************************

THIS IS NOT A CRT REPLACEMENT. This library exists to help you compile programs without the CRT.
This sound easy and this library shouldn't really exist. But the authors of modern C/C++ compilers are
doing a great job at making your life as difficult as possible when doing this.


WHY WOULD YOU WANT TO REMOVE THE CRT?
  * It runs code before and after yours (main or WinMain). That code usually does some CRT prepwork,
    but recent MSVC CRT implementations also do telemetry, it's abysmal.
  * The compiler sprinkles security checks and other overhead everywhere, even in release builds.
  * Implementation characteristics vary between different compilers, and even between different versions.
  * It adds ~80kB of code to your EXE for no particular reason.
  * It links dynamically by default, so if you're not careful you'll need to ship with VC runtime installers.


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

    lk_nocrt.c will add this entry function for you. If you're using a different name, define:
        #define LK_NOCRT_ENTRY_POINT my_entry_name

    If you don't want the entry point to be added to your program, define:
        #define LK_NOCRT_NO_ENTRY_POINT


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


  64-BIT MATH OPERATORS FOR 32-BIT SYSTEMS
    For 32-bit code, all registers are only 32 bits, so working with 64-bit integers creates a bit of a problem.
    The compiler uses a bunch of functions to do 64-bit math operations with 32-bit registers.

    If you don't want these functions to be added to your program, define:
        #define LK_NOCRT_NO_64BIT_MATH


  FLOATING POINT SUPPORT
    If your code contains floating point ops, the linker will want to see the "_fltused" symbol.
    As far as I know, it doens't actually care about its value, it just wants the symbol to exist.
    Additionally, on 32-bit architectures, the compiler generates calls to several functions when casting
    between floating point and integer.

    If you don't want _fltused and these functions to be added to your program, define:
        #define LK_NOCRT_NO_FLOATING_POINT

 *********************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif



#ifndef LK_NOCRT_NO_64BIT_MATH
#ifdef _M_IX86

__declspec(naked) void _alldiv()
{
    __asm
    {
    push    edi
    push    esi
    push    ebx
    xor     edi,edi
    mov     eax,dword ptr [esp+20]
    or      eax,eax
    jge     short L1
    inc     edi
    mov     edx,dword ptr [esp+16]
    neg     eax
    neg     edx
    sbb     eax,0
    mov     dword ptr [esp+20],eax
    mov     dword ptr [esp+16],edx
L1:
    mov     eax,dword ptr [esp+28]
    or      eax,eax
    jge     short L2
    inc     edi
    mov     edx,dword ptr [esp+24]
    neg     eax
    neg     edx
    sbb     eax,0
    mov     dword ptr [esp+28],eax
    mov     dword ptr [esp+24],edx
L2:

    or      eax,eax
    jnz     short L3
    mov     ecx,dword ptr [esp+24]
    mov     eax,dword ptr [esp+20]
    xor     edx,edx
    div     ecx
    mov     ebx,eax
    mov     eax,dword ptr [esp+16]
    div     ecx
    mov     edx,ebx
    jmp     short L4
L3:
    mov     ebx,eax
    mov     ecx,dword ptr [esp+24]
    mov     edx,dword ptr [esp+20]
    mov     eax,dword ptr [esp+16]
L5:
    shr     ebx,1
    rcr     ecx,1
    shr     edx,1
    rcr     eax,1
    or      ebx,ebx
    jnz     short L5
    div     ecx
    mov     esi,eax
    mul     dword ptr [esp+28]
    mov     ecx,eax
    mov     eax,dword ptr [esp+24]
    mul     esi
    add     edx,ecx
    jc      short L6
    cmp     edx,dword ptr [esp+20]
    ja      short L6
    jb      short L7
    cmp     eax,dword ptr [esp+16]
    jbe     short L7
L6:
    dec     esi
L7:
    xor     edx,edx
    mov     eax,esi

L4:
    dec     edi
    jnz     short L8
    neg     edx
    neg     eax
    sbb     edx,0
L8:
    pop     ebx
    pop     esi
    pop     edi
    ret     16
    }
}

__declspec(naked) void _alldvrm()
{
    __asm
    {
    push    edi
    push    esi
    push    ebp
    xor     edi,edi
    xor     ebp,ebp
    mov     eax,dword ptr [esp+20]
    or      eax,eax
    jge     short L1
    inc     edi
    inc     ebp
    mov     edx,dword ptr [esp+16]
    neg     eax
    neg     edx
    sbb     eax,0
    mov     dword ptr [esp+20],eax
    mov     dword ptr [esp+16],edx
L1:
    mov     eax,dword ptr [esp+28]
    or      eax,eax
    jge     short L2
    inc     edi
    mov     edx,dword ptr [esp+24]
    neg     eax
    neg     edx
    sbb     eax,0
    mov     dword ptr [esp+28],eax
    mov     dword ptr [esp+24],edx
L2:

    or      eax,eax
    jnz     short L3
    mov     ecx,dword ptr [esp+24]
    mov     eax,dword ptr [esp+20]
    xor     edx,edx
    div     ecx
    mov     ebx,eax
    mov     eax,dword ptr [esp+16]
    div     ecx
    mov     esi,eax
    mov     eax,ebx
    mul     dword ptr [esp+24]
    mov     ecx,eax
    mov     eax,esi
    mul     dword ptr [esp+24]
    add     edx,ecx
    jmp     short L4
L3:
    mov     ebx,eax
    mov     ecx,dword ptr [esp+24]
    mov     edx,dword ptr [esp+20]
    mov     eax,dword ptr [esp+16]
L5:
    shr     ebx,1
    rcr     ecx,1
    shr     edx,1
    rcr     eax,1
    or      ebx,ebx
    jnz     short L5
    div     ecx
    mov     esi,eax
    mul     dword ptr [esp+28]
    mov     ecx,eax
    mov     eax,dword ptr [esp+24]
    mul     esi
    add     edx,ecx
    jc      short L6
    cmp     edx,dword ptr [esp+20]
    ja      short L6
    jb      short L7
    cmp     eax,dword ptr [esp+16]
    jbe     short L7
L6:
    dec     esi
    sub     eax,dword ptr [esp+24]
    sbb     edx,dword ptr [esp+28]
L7:
    xor     ebx,ebx
L4:

    sub     eax,dword ptr [esp+16]
    sbb     edx,dword ptr [esp+20]
    dec     ebp
    jns     short L9
    neg     edx
    neg     eax
    sbb     edx,0
L9:
    mov     ecx,edx
    mov     edx,ebx
    mov     ebx,ecx
    mov     ecx,eax
    mov     eax,esi

    dec     edi
    jnz     short L8
    neg     edx
    neg     eax
    sbb     edx,0
L8:
    pop     ebp
    pop     esi
    pop     edi
    ret     16
    }
}

__declspec(naked) void _allmul()
{
    __asm
    {
    push    ebx
    mov     eax,dword ptr [esp + 12]
    mov     ecx,dword ptr [esp + 16]
    mul     ecx
    mov     ebx,eax
    mov     eax,dword ptr [esp + 8]
    mul     dword ptr [esp + 20]
    add     ebx,eax
    mov     eax,dword ptr [esp + 8]
    mul     ecx
    add     edx,ebx
    pop     ebx
    ret     16
    }
}

__declspec(naked) void _allrem()
{
    __asm
    {
    push    ebx
    push    edi
    xor     edi,edi
    mov     eax,dword ptr [esp+16]
    or      eax,eax
    jge     short L1
    inc     edi
    mov     edx,dword ptr [esp+12]
    neg     eax
    neg     edx
    sbb     eax,0
    mov     dword ptr [esp+16],eax
    mov     dword ptr [esp+12],edx
L1:
    mov     eax,dword ptr [esp+24]
    or      eax,eax
    jge     short L2
    mov     edx,dword ptr [esp+20]
    neg     eax
    neg     edx
    sbb     eax,0
    mov     dword ptr [esp+24],eax
    mov     dword ptr [esp+20],edx
L2:

    or      eax,eax
    jnz     short L3
    mov     ecx,dword ptr [esp+20]
    mov     eax,dword ptr [esp+16]
    xor     edx,edx
    div     ecx
    mov     eax,dword ptr [esp+12]
    div     ecx
    mov     eax,edx
    xor     edx,edx
    dec     edi
    jns     short L4
    jmp     short L8
L3:
    mov     ebx,eax
    mov     ecx,dword ptr [esp+20]
    mov     edx,dword ptr [esp+16]
    mov     eax,dword ptr [esp+12]
L5:
    shr     ebx,1
    rcr     ecx,1
    shr     edx,1
    rcr     eax,1
    or      ebx,ebx
    jnz     short L5
    div     ecx
    mov     ecx,eax
    mul     dword ptr [esp+24]
    xchg    ecx,eax
    mul     dword ptr [esp+20]
    add     edx,ecx
    jc      short L6
    cmp     edx,dword ptr [esp+16]
    ja      short L6
    jb      short L7
    cmp     eax,dword ptr [esp+12]
    jbe     short L7
L6:
    sub     eax,dword ptr [esp+20]
    sbb     edx,dword ptr [esp+24]
L7:
    sub     eax,dword ptr [esp+12]
    sbb     edx,dword ptr [esp+16]
    dec     edi
    jns     short L8
L4:
    neg     edx
    neg     eax
    sbb     edx,0

L8:
    pop     edi
    pop     ebx
    ret     16
    }
}

__declspec(naked) void _allshl()
{
    __asm
    {
    cmp     cl, 64
    jae     short RETZERO
    cmp     cl, 32
    jae     short MORE32
    shld    edx,eax,cl
    shl     eax,cl
    ret
MORE32:
    mov     edx,eax
    xor     eax,eax
    and     cl,31
    shl     edx,cl
    ret
RETZERO:
    xor     eax,eax
    xor     edx,edx
    ret
    }
}

__declspec(naked) void _allshr()
{
    __asm
    {
    cmp     cl,64
    jae     short RETSIGN
    cmp     cl, 32
    jae     short MORE32
    shrd    eax,edx,cl
    sar     edx,cl
    ret
MORE32:
    mov     eax,edx
    sar     edx,31
    and     cl,31
    sar     eax,cl
    ret
RETSIGN:
    sar     edx,31
    mov     eax,edx
    ret
    }
}

__declspec(naked) void _aulldiv()
{
    __asm
    {
    push    ebx
    push    esi
    mov     eax,dword ptr [esp+24]
    or      eax,eax
    jnz     short L1
    mov     ecx,dword ptr [esp+20]
    mov     eax,dword ptr [esp+16]
    xor     edx,edx
    div     ecx
    mov     ebx,eax
    mov     eax,dword ptr [esp+12]
    div     ecx
    mov     edx,ebx
    jmp     short L2
L1:
    mov     ecx,eax
    mov     ebx,dword ptr [esp+20]
    mov     edx,dword ptr [esp+16]
    mov     eax,dword ptr [esp+12]
L3:
    shr     ecx,1
    rcr     ebx,1
    shr     edx,1
    rcr     eax,1
    or      ecx,ecx
    jnz     short L3
    div     ebx
    mov     esi,eax
    mul     dword ptr [esp+24]
    mov     ecx,eax
    mov     eax,dword ptr [esp+20]
    mul     esi
    add     edx,ecx
    jc      short L4
    cmp     edx,dword ptr [esp+16]
    ja      short L4
    jb      short L5
    cmp     eax,dword ptr [esp+12]
    jbe     short L5
L4:
    dec     esi
L5:
    xor     edx,edx
    mov     eax,esi

L2:
    pop     esi
    pop     ebx
    ret     16
    }
}

__declspec(naked) void _aulldvrm()
{
    __asm
    {
    push    esi
    mov     eax,dword ptr [esp+20]
    or      eax,eax
    jnz     short L1
    mov     ecx,dword ptr [esp+16]
    mov     eax,dword ptr [esp+12]
    xor     edx,edx
    div     ecx
    mov     ebx,eax
    mov     eax,dword ptr [esp+8]
    div     ecx
    mov     esi,eax
    mov     eax,ebx
    mul     dword ptr [esp+16]
    mov     ecx,eax
    mov     eax,esi
    mul     dword ptr [esp+16]
    add     edx,ecx
    jmp     short L2
L1:
    mov     ecx,eax
    mov     ebx,dword ptr [esp+16]
    mov     edx,dword ptr [esp+12]
    mov     eax,dword ptr [esp+8]
L3:
    shr     ecx,1
    rcr     ebx,1
    shr     edx,1
    rcr     eax,1
    or      ecx,ecx
    jnz     short L3
    div     ebx
    mov     esi,eax
    mul     dword ptr [esp+20]
    mov     ecx,eax
    mov     eax,dword ptr [esp+16]
    mul     esi
    add     edx,ecx
    jc      short L4
    cmp     edx,dword ptr [esp+12]
    ja      short L4
    jb      short L5
    cmp     eax,dword ptr [esp+8]
    jbe     short L5
L4:
    dec     esi
    sub     eax,dword ptr [esp+16]
    sbb     edx,dword ptr [esp+20]
L5:
    xor     ebx,ebx
L2:

    sub     eax,dword ptr [esp+8]
    sbb     edx,dword ptr [esp+12]
    neg     edx
    neg     eax
    sbb     edx,0
    mov     ecx,edx
    mov     edx,ebx
    mov     ebx,ecx
    mov     ecx,eax
    mov     eax,esi
    pop     esi
    ret     16
    }
}

__declspec(naked) void _aullrem()
{
    __asm
    {
    push    ebx

    mov     eax,dword ptr [esp+20]
    or      eax,eax
    jnz     short L1
    mov     ecx,dword ptr [esp+16]
    mov     eax,dword ptr [esp+12]
    xor     edx,edx
    div     ecx
    mov     eax,dword ptr [esp+8]
    div     ecx
    mov     eax,edx
    xor     edx,edx
    jmp     short L2
L1:
    mov     ecx,eax
    mov     ebx,dword ptr [esp+16]
    mov     edx,dword ptr [esp+12]
    mov     eax,dword ptr [esp+8]
L3:
    shr     ecx,1
    rcr     ebx,1
    shr     edx,1
    rcr     eax,1
    or      ecx,ecx
    jnz     short L3
    div     ebx
    mov     ecx,eax
    mul     dword ptr [esp+20]
    xchg    ecx,eax
    mul     dword ptr [esp+16]
    add     edx,ecx
    jc      short L4

    cmp     edx,dword ptr [esp+12]
    ja      short L4
    jb      short L5
    cmp     eax,dword ptr [esp+8]
    jbe     short L5
L4:
    sub     eax,dword ptr [esp+16]
    sbb     edx,dword ptr [esp+20]
L5:
    sub     eax,dword ptr [esp+8]
    sbb     edx,dword ptr [esp+12]
    neg     edx
    neg     eax
    sbb     edx,0
L2:
    pop     ebx
    ret     16
    }
}

__declspec(naked) void _aullshr()
{
    __asm
    {
    cmp     cl,64
    jae     short RETZERO
    cmp     cl, 32
    jae     short MORE32
    shrd    eax,edx,cl
    shr     edx,cl
    ret
MORE32:
    mov     eax,edx
    xor     edx,edx
    and     cl,31
    shr     eax,cl
    ret
RETZERO:
    xor     eax,eax
    xor     edx,edx
    ret
    }
}

#endif
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
__declspec(dllimport) void* __stdcall GetCommandLineA();
__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned int uExitCode);

extern int __stdcall WinMain(void* hInstance, void* hPrevInstance, void* lpCmdLine, int nCmdShow);

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