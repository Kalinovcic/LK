/*  lk_region.h - public domain stack-based allocator */
/*  no warranty is offered or implied */

/*********************************************************************************************

Include this file in all places you need to refer to it. In one of your compilation units, write:
    #define LK_REGION_IMPLEMENTATION
before including lk_region.h, in order to paste in the source code.

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

#ifndef LK_REGION_HEADER
#define LK_REGION_HEADER

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* This entire preprocessor mess is here just because
   C and C++ don't really provide a simple way to set alignment,
   or inquire about alignment. We need alignment info to allocate,
   and we would also like LK_Region to be cache-aligned. */

#define LK__REGION_CACHE_SIZE 32
#if defined(__cplusplus) && (__cplusplus>=201103L)
/* Do it with C++11 features, if available. */
#define LK__REGION_CACHE_ALIGN alignas(LK__REGION_CACHE_SIZE)
#define LK__REGION_CACHE_ALIGN_POST
#define LK__REGION_ALIGNOF(type) alignof(type)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__>=201112L)
/* Do it with C11 features, if available. */
#define LK__REGION_CACHE_ALIGN _Alignas(LK__REGION_CACHE_SIZE)
#define LK__REGION_CACHE_ALIGN_POST
#define LK__REGION_ALIGNOF(type) _Alignof(type)
#elif defined(_MSC_VER)
/* Do it with MSVC features, if available. */
#define LK__REGION_CACHE_ALIGN __declspec(align(LK__REGION_CACHE_SIZE))
#define LK__REGION_CACHE_ALIGN_POST
#define LK__REGION_ALIGNOF(type) __alignof(type)
#elif defined(__GNUC__) || defined(__GNUG__)
/* Do it with GCC features, if available. */
#define LK__REGION_CACHE_ALIGN
#define LK__REGION_CACHE_ALIGN_POST __attribute__((aligned(LK__REGION_CACHE_SIZE)))
#define LK__REGION_ALIGNOF(type) __alignof__(type)
#else
/* Oh well, best of luck to you! */
#define LK__REGION_CACHE_ALIGN
#define LK__REGION_CACHE_ALIGN_POST
#define LK__REGION_ALIGNOF(type) (sizeof(type) > 4 ? 8 : (sizeof(type) > 2 ? 4 : (sizeof(type) == 2 ? 2 : 1)))
#endif

/* LK_Region struct.
   You shouldn't need to care about the members of this struct,
   it is only in the header so that you can allocate it. */
typedef LK__REGION_CACHE_ALIGN struct
{
    uintptr_t page_size;
    void*     page_end;
    void*     cursor;
    void*     alloc_head;
    uintptr_t alloc_count;
    void*     next_page;
    uintptr_t next_page_size;
} LK__REGION_CACHE_ALIGN_POST LK_Region;

/* Use this macro to initialize region variables. Like this:
       LK_Region region = LK_RegionInit;
   If you're using C++, you can also do:
       LK_Region region = { 0 };
       LK_Region region = {}; // C++11 */
#define LK_RegionInit { 0, 0, 0, 0, 0, 0, 0 }

#ifdef LK_REGION_COLLECT_CALLER_INFO
#define lk_region_alloc(...) (lk_region_alloc_(__VA_ARGS__, __FUNCTION__))
void* lk_region_alloc_(LK_Region* region, size_t size, size_t alignment, const char* caller_name);
#else
void* lk_region_alloc(LK_Region* region, size_t size, size_t alignment);
#endif

void lk_region_free(LK_Region* region);

/* Helper macros. */
#define LK_RegionValue(region_ptr, type)                          ((type*) lk_region_alloc((region_ptr), sizeof(type),           LK__REGION_ALIGNOF(type)))
#define LK_RegionArray(region_ptr, type, count)                   ((type*) lk_region_alloc((region_ptr), sizeof(type) * (count), LK__REGION_ALIGNOF(type)))
#define LK_RegionValueAligned(region_ptr, type, alignment)        ((type*) lk_region_alloc((region_ptr), sizeof(type),           (alignment)))
#define LK_RegionArrayAligned(region_ptr, type, count, alignment) ((type*) lk_region_alloc((region_ptr), sizeof(type) * (count), (alignment)))

/* LK_Region_Cursor struct.
   You shouldn't need to care about the members of this struct,
   it is only in the header so that you can allocate it. */
typedef struct
{
    void* page_end;
    void* cursor;
    void* alloc_head;
} LK_Region_Cursor;

void lk_region_cursor(LK_Region* region, LK_Region_Cursor* cursor);
void lk_region_rewind(LK_Region* region, LK_Region_Cursor* cursor);

#ifdef __cplusplus
}
#endif

#endif /* LK_REGION_HEADER */

/*********************************************************************************************

  END OF HEADER - BEGINNING OF IMPLEMENTATION

 *********************************************************************************************/

#ifdef LK_REGION_IMPLEMENTATION
#ifndef LK_REGION_IMPLEMENTED
#define LK_REGION_IMPLEMENTED

#ifdef __cplusplus
extern "C"
{
#endif

void* lk_region_os_alloc(size_t size, const char* caller_name);
void lk_region_os_free(void* memory, size_t size);

#ifdef _WIN32
/*********************************************************************************************
  Windows-specific
 *********************************************************************************************/
#ifndef LK_REGION_DEFAULT_PAGE_SIZE
#define LK_REGION_DEFAULT_PAGE_SIZE 0x10000 /* 64 kB */
#endif

#include <windows.h>

#ifndef LK_REGION_CUSTOM_PAGE_ALLOCATOR

void* lk_region_os_alloc(size_t size, const char* caller_name)
{
    return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void lk_region_os_free(void* memory, size_t size)
{
    VirtualFree(memory, 0, MEM_RELEASE);
}

#endif

#else
#error Unrecognized operating system
#endif

/*********************************************************************************************
  Cross-platform
 *********************************************************************************************/

typedef struct
{
    void*     next;
    uintptr_t size;
} LK_Page_Header;

#ifdef LK_REGION_COLLECT_CALLER_INFO
void* lk_region_alloc_(LK_Region* region, size_t size, size_t alignment, const char* caller_name)
{
#else
void* lk_region_alloc(LK_Region* region, size_t size, size_t alignment)
{
    const char* caller_name = 0;
#endif

    typedef   uint8_t byte;
    typedef  intptr_t smm;
    typedef uintptr_t umm;

    /* set default page size */
    umm page_size = region->page_size;
    if (!page_size)
    {
        page_size = LK_REGION_DEFAULT_PAGE_SIZE;
        region->page_size = page_size;
    }

    /* check if this is a big allocation */
    umm big_allocation_threshold = (page_size >> 2);
    if (size > big_allocation_threshold)
    {
        if (alignment < sizeof(LK_Page_Header))
            alignment = sizeof(LK_Page_Header);

        page_size = size + alignment;
        byte* page = (byte*) lk_region_os_alloc(page_size, caller_name);

        LK_Page_Header* header = (LK_Page_Header*) page;
        header->next = region->alloc_head;
        header->size = page_size;

        region->alloc_head = header;
        region->alloc_count++;

        return page + alignment;
    }

    /* align cursor */
    umm cursor_address = (umm) region->cursor;
    cursor_address += -cursor_address & (umm)(alignment - 1);

    /* end of page check */
    umm end_address = cursor_address + size;
    if (end_address > (umm) region->page_end)
    {
        /* allocate another page */
        byte* page;
        if (region->next_page)
        {
            page_size = region->next_page_size;
            page = (byte*) region->next_page;
            region->next_page      = 0;
            region->next_page_size = 0;
        }
        else
        {
            page = (byte*) lk_region_os_alloc(page_size, caller_name);
        }

        LK_Page_Header* header = (LK_Page_Header*) page;
        header->next = region->alloc_head;
        header->size = page_size;

        region->page_end = page + page_size;
        region->alloc_head = header;
        region->alloc_count++;

        cursor_address = (umm)(header + 1);
        cursor_address += -cursor_address & (umm)(alignment - 1);  /* realign */
        end_address = cursor_address + size;
    }

    /* success */
    void* result = (void*) cursor_address;
    region->cursor = (void*) end_address;
    return result;
}

void lk_region_free(LK_Region* region)
{
    void* memory = region->alloc_head;
    while (memory)
    {
        LK_Page_Header* header = (LK_Page_Header*) memory;
        void* next_memory = header->next;

        lk_region_os_free(memory, header->size);
        memory = next_memory;
    }

    if (region->next_page)
    {
        lk_region_os_free(region->next_page, region->next_page_size);
        region->next_page      = 0;
        region->next_page_size = 0;
    }

    region->page_end    = 0;
    region->cursor      = 0;
    region->alloc_head  = 0;
    region->alloc_count = 0;
}

void lk_region_cursor(LK_Region* region, LK_Region_Cursor* cursor)
{
    cursor->page_end   = region->page_end;
    cursor->cursor     = region->cursor;
    cursor->alloc_head = region->alloc_head;
}

void lk_region_rewind(LK_Region* region, LK_Region_Cursor* cursor)
{
    void* new_page_end = cursor->page_end;
    void* new_cursor = cursor->cursor;
    void* new_alloc_head = cursor->alloc_head;

    void* memory = region->alloc_head;
    while (memory != new_alloc_head)
    {
        LK_Page_Header* header = (LK_Page_Header*) memory;
        void* next_memory = header->next;

        if (header->size > region->next_page_size)
        {
            if (region->next_page)
            {
                region->alloc_count--;
                lk_region_os_free(region->next_page, region->next_page_size);
            }
            region->next_page      = memory;
            region->next_page_size = header->size;
        }
        else
        {
            region->alloc_count--;
            lk_region_os_free(memory, header->size);
        }

        memory = next_memory;
    }

    SIZE_T size;
    if (cursor->page_end == region->page_end)
    {
        size = (char*) region->cursor - (char*) new_cursor;
    }
    else
    {
        size = (char*) new_page_end - (char*) new_cursor;
    }
    ZeroMemory(new_cursor, size);

    region->page_end   = new_page_end;
    region->cursor     = new_cursor;
    region->alloc_head = new_alloc_head;
}

#ifdef __cplusplus
}
#endif

#endif /* LK_REGION_IMPLEMENTED */
#endif /* LK_REGION_IMPLEMENTATION */


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
