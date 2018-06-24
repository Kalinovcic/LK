//  lk_platform.h - public domain platform abstraction layer with live code editing support
//  no warranty is offered or implied

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

typedef struct LK_Region_Structure
{
    void* alloc_head;
    void* page_end;
    void* cursor;
    uintptr_t page_size;
} LK_Region;

void* lk_region_alloc(LK_Region* region, size_t size, size_t alignment);
void lk_region_free(LK_Region* region);

#define LK__AlignmentGuess(type) (sizeof(type) > 4 ? 8 : (sizeof(type) > 2 ? 4 : (sizeof(type) == 2 ? 2 : 1)))

#define LK_RegionValue(region_ptr, type)                          ((type*) lk_region_alloc((region_ptr), sizeof(type),           LK__AlignmentGuess(type)))
#define LK_RegionArray(region_ptr, type, count)                   ((type*) lk_region_alloc((region_ptr), sizeof(type) * (count), LK__AlignmentGuess(type)))
#define LK_RegionValueAligned(region_ptr, type, alignment)        ((type*) lk_region_alloc((region_ptr), sizeof(type),           (alignment)))
#define LK_RegionArrayAligned(region_ptr, type, count, alignment) ((type*) lk_region_alloc((region_ptr), sizeof(type) * (count), (alignment)))

#ifdef __cplusplus
}
#endif

#endif // LK_REGION_HEADER

/*********************************************************************************************

  END OF HEADER - BEGINNING OF IMPLEMENTATION

 *********************************************************************************************/

#ifdef LK_REGION_IMPLEMENTATION
#ifndef LK_REGION_IMPLEMENTED
#define LK_REGION_IMPLEMENTED

#ifdef _WIN32

#include <WinBase.h>

void* lk_region_alloc(LK_Region* region, size_t size, size_t alignment)
{
    typedef uintptr_t umm;

    umm page_size = region->page_size;
    if (!page_size)
    {
        page_size = 0x10000; // 64 kB
        region->page_size = page_size;
    }

    // big allocation check
    umm minimum_alignment = (alignment > sizeof(void*)) ? alignment : sizeof(void*);
    umm big_allocation_threshold = page_size - minimum_alignment;
    if (size > big_allocation_threshold)
    {
        char* page = (char*) VirtualAlloc(0, size + minimum_alignment, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        *(void**) page = region->alloc_head;
        region->alloc_head = page;
        return page + minimum_alignment;
    }

    // align cursor
    umm cursor_address = (umm) region->cursor;
    umm remainder = cursor_address & (umm)(alignment - 1);
    if (remainder)
    {
        cursor_address += alignment - remainder;
    }

    // resize check
    umm end_address = cursor_address + size;
    if (end_address > (umm) region->page_end)
    {
        char* page = (char*) VirtualAlloc(0, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        char* page_end = page + page_size;

        void** cursor = (void**) page;
        *(cursor++) = region->alloc_head;

        region->alloc_head = page;
        region->page_end = page_end;
        region->cursor = cursor;

        return lk_region_alloc(region, size, alignment);
    }

    // success
    void* cursor = (void*) cursor_address;
    region->cursor = (void*) end_address;
    return cursor;
}

void lk_region_free(LK_Region* region)
{
    void* page = region->alloc_head;
    while (page)
    {
        void* next_page = *(void**) page;
        VirtualFree(page, 0, MEM_RELEASE);
        page = next_page;
    }
}

#else // !defined(_WIN32)
#error Unrecognized operating system
#endif

#endif // LK_REGION_IMPLEMENTED
#endif // LK_REGION_IMPLEMENTATION


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