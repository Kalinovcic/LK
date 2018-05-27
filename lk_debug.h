//  lk_platform.h - public domain utility for runtime profiling and debug event gathering
//  no warranty is offered or implied

/*********************************************************************************************

QUICK NOTES
    Call lkdbg_start() at the beginning and lkdbg_end() at the end of your program, or part of the program you're profiling.
    Also remember to call lkdbg_register_thread() once for each thread.

    The following macros are only defined for C++ (or for C using GCC-specific extensions):
        LKDBG_FUNCTION          Place this at the very beginning of a function to make the entire function a block.
        LKDBG_BLOCK(name)       Place this at the very beginning of a block.

    If you can't use those, use the following:
        LKDBG_BEGIN_BLOCK(name)
        LKDBG_END_BLOCK()
    Be careful not to forget to end your blocks! Also note that you can only open one block per scope.
    If you want more than one, do something like this:

        {
            LKDBG_BEGIN_BLOCK("Block A")
            ...
            LKDBG_END_BLOCK()
        }
        {
            LKDBG_BEGIN_BLOCK("Block B")
            ...
            LKDBG_END_BLOCK()
        }

    If for some reason you don't want to use these macros, you can use:
        lkdbg_push_block_event(name, begin)
    However, **BE VERY CAREFUL** when using this function. The name **POINTER** must be equal when beginning
    and ending a block. This is not guaranteed for identical string literals by the C/C++ standards.
    For example, the following code may be INCORRECT:

        lkdbg_push_block_event("My Block", 1);
        ...
        lkdbg_push_block_event("My Block", 0);  // may be a different address!

LICENSE
    This software is in the public domain. Anyone can use it, modify it,
    roll'n'smoke hardcopies of the source code, sell it to the terrorists, etc.
    No warranty is offered or implied; use this code at your own risk!

    See end of file for license information.

DOCUMENTATION
    @Incomplete


 *********************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

////////////////////////////////////////////////////////////////////////////////
// Header
////////////////////////////////////////////////////////////////////////////////

void lkdbg_register_thread(const char* name);
void lkdbg_push_block_event(const char* name, int begin);
void lkdbg_start(int do_etw);
void lkdbg_end(const char* profile_path);

#define LKDBG_BEGIN_BLOCK(name) \
    const char* lkdbg_block_name = name; \
    lkdbg_push_block_event(lkdbg_block_name, 1);

#define LKDBG_END_BLOCK() \
    lkdbg_push_block_event(lkdbg_block_name, 0);

#if defined(__cplusplus)

struct LKDBG_Debug_Block
{
    const char* name;
    LKDBG_Debug_Block(const char* name): name(name) { lkdbg_push_block_event(name, 1); }
    ~LKDBG_Debug_Block() { lkdbg_push_block_event(name, 0); }
};

#define LKDBG_FUNCTION LKDBG_Debug_Block lkdbg_debug_function(__FUNCTION__);
#define LKDBG_BLOCK(name) LKDBG_Debug_Block lkdbg_debug_block(name);

#elif defined(__GNUC__)

#define LKDBG_BLOCK(name) \
    void lkdbg_debug_block_cleanup(); \
    const char* lkdbg_debug_block_name __attribute__((cleanup(lkdbg_debug_block_cleanup))) = name; \
    lkdbg_push_block_event(name, 1); \
    void lkdbg_debug_block_cleanup(const char** name) \
    { \
        lkdbg_push_block_event(*name, 0); \
    }

#define LKDBG_FUNCTION LKDBG_BLOCK(__FUNCTION__)

#endif

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

#ifdef LKDBG_IMPLEMENTATION

#ifndef LKDBG_MALLOC
 #define LKDBG_MALLOC(size) malloc(size)
#endif

#ifndef LKDBG_FREE
 #define LKDBG_FREE(size) free(size)
#endif

#ifndef LKDBG_MEMCPY
  #define LKDBG_MEMCPY(dest, src, size) memcpy(dest, src, size)
#endif

#ifndef LKDBG_ASSERT
  #include <assert.h>
  #define LKDBG_ASSERT(test, message) assert((test) && (message))
#endif

#ifndef LKDBG_THREAD_LOCAL
  #if defined(_MSC_VER)
    #define LKDBG_THREAD_LOCAL __declspec(thread)
  #elif defined(__GNUC__)
    #define LKDBG_THREAD_LOCAL __thread
  #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define LKDBG_THREAD_LOCAL _Thread_local
  #elif defined(__cplusplus) && (__cplusplus > 199711L)
    #define LKDBG_THREAD_LOCAL thread_local
  #else
    #define LKDBG_THREAD_LOCAL
  #endif
#endif


#include <windows.h>
#define INITGUID
#include <evntrace.h>
#include <evntcons.h>


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


typedef struct
{
    LK_U64 time_frequency;
    LK_U64 string_count;
    LK_U64 thread_count;
    LK_U64 event_count;
} LKDBG_File_Header;

typedef struct
{
    LK_U32 thread_id;
    const char* name;
} LKDBG_File_Thread;

typedef struct
{
    const void* ptr;
    char string[128];
} LKDBG_File_String;



typedef struct
{
    LK_U8  kind;
    LK_U8  begin; // 1 if begin, 0 if end
    LK_U32 thread_id;
    const char* name;
    LK_U64 time;
} LKDBG_Block;

typedef struct
{
    LK_U8  kind;
    LK_U8  processor;
    LK_U32 thread_id;
    LK_U64 time;
} LKDBG_Context_Switch;

typedef enum
{
    LKDBG_BLOCK,
    LKDBG_CONTEXT_SWITCH,
} LKDBG_Event_Kind;

typedef union
{
    LK_U8 kind;
    LKDBG_Block block;
    LKDBG_Context_Switch context_switch;
} LKDBG_Event;

static LK_U64 lkdbg_get_event_time(const LKDBG_Event* a)
{
    switch (a->kind)
    {
    case LKDBG_BLOCK:          return a->block.time;
    case LKDBG_CONTEXT_SWITCH: return a->context_switch.time;
    default:                   return 0;
    }
}


typedef struct
{
    LK_U32 thread_id;
    const char* name;

    LKDBG_Event* events;
    LK_U64 event_count;
    LK_U64 event_capacity;
} LKDBG_Thread;

typedef struct
{
    CRITICAL_SECTION lock;

    LKDBG_Thread** threads;
    LK_U64 thread_count;
    LK_U64 thread_capacity;

    TRACEHANDLE etw_consumer_handle;
    HANDLE etw_thread;
} LKDBG_Context;

static void lkdbg_array_push(void** address, LK_U64* count, LK_U64* capacity, void* data, LK_U64 size)
{
    if (*count == *capacity)
    {
        LK_U64 old_capacity = *capacity;
        LK_U64 new_capacity = old_capacity * 2;
        if (new_capacity == 0)
        {
            new_capacity = 2048;
        }

        void* new_address = LKDBG_MALLOC(size * new_capacity);

        if (*address)
        {
            LKDBG_MEMCPY(new_address, *address, old_capacity * size);
            LKDBG_FREE(*address);
        }

        *address = new_address;
        *capacity = new_capacity;
    }

    LKDBG_MEMCPY((LK_U8*) *address + ((*count)++ * size), data, size);
}


LKDBG_Context lkdbg_context;
LKDBG_THREAD_LOCAL LKDBG_Thread* lkdbg_thread;

void lkdbg_register_thread(const char* name)
{
    LKDBG_ASSERT(!lkdbg_thread, "same thread registered more than once");

    LKDBG_Thread* thread = (LKDBG_Thread*) LKDBG_MALLOC(sizeof(LKDBG_Thread));
    lkdbg_thread = thread;

    thread->thread_id = GetCurrentThreadId();
    thread->name = name;
    thread->events = 0;
    thread->event_count = 0;
    thread->event_capacity = 0;

    EnterCriticalSection(&lkdbg_context.lock);
    lkdbg_array_push((void**) &lkdbg_context.threads, &lkdbg_context.thread_count, &lkdbg_context.thread_capacity, &thread, sizeof(LKDBG_Thread*));
    LeaveCriticalSection(&lkdbg_context.lock);
}

void lkdbg_push_block_event(const char* name, int begin)
{
    LKDBG_ASSERT(lkdbg_thread, "pushed events on thread before it was registered");

    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);

    LKDBG_Event event;
    event.kind = LKDBG_BLOCK;
    event.block.begin = begin ? 1 : 0;
    event.block.thread_id = GetCurrentThreadId();
    event.block.name = name;
    event.block.time = qpc.QuadPart;

    lkdbg_array_push((void**) &lkdbg_thread->events, &lkdbg_thread->event_count, &lkdbg_thread->event_capacity, &event, sizeof(LKDBG_Event));
}

static void lkdbg_etw_start();
static void lkdbg_etw_end();

void lkdbg_start(int do_etw)
{
    InitializeCriticalSection(&lkdbg_context.lock);

    lkdbg_context.etw_consumer_handle = INVALID_PROCESSTRACE_HANDLE;

    if (do_etw)
    {
        lkdbg_etw_start();
    }
}

static void lkdbg_add_file_string(const char* str, LKDBG_File_String** strings, LK_U64* count, LK_U64* capacity)
{
    for (LK_U64 i = 0; i < *count; i++)
        if ((*strings)[i].ptr == str)
            return;

    LKDBG_File_String string;
    string.ptr = str;

    int i = 0;
    while (str[i] && i < (sizeof(string.string) - 1))
    {
        string.string[i] = str[i];
        i++;
    }
    string.string[i] = 0;

    lkdbg_array_push((void**) strings, count, capacity, &string, sizeof(LKDBG_File_String));
}

void lkdbg_end(const char* profile_path)
{
    if (lkdbg_context.etw_consumer_handle != INVALID_PROCESSTRACE_HANDLE)
    {
        lkdbg_etw_end();
    }

    if (profile_path)
    {
        LK_U64 total_event_capacity = 0;
        for (LK_U64 i = 0; i < lkdbg_context.thread_count; i++)
        {
            LKDBG_Thread* thread = lkdbg_context.threads[i];
            total_event_capacity += thread->event_count;
        }

        LKDBG_Event* temp_events = (LKDBG_Event*) LKDBG_MALLOC(sizeof(LKDBG_Event) * total_event_capacity);
        LKDBG_Event* all_events = (LKDBG_Event*) LKDBG_MALLOC(sizeof(LKDBG_Event) * total_event_capacity);
        LK_U64 all_count = 0;

        LKDBG_File_String* strings = 0;
        LK_U64 string_count = 0;
        LK_U64 string_capacity = 0;

        for (LK_U64 i = 0; i < lkdbg_context.thread_count; i++)
        {
            LKDBG_MEMCPY(temp_events, all_events, sizeof(LKDBG_Event) * all_count);
            LKDBG_Thread* thread = lkdbg_context.threads[i];

            lkdbg_add_file_string(thread->name, &strings, &string_count, &string_capacity);
            for (LK_U64 j = 0; j < thread->event_count; j++)
            {
                if (thread->events[j].kind != LKDBG_BLOCK) continue;
                lkdbg_add_file_string(thread->events[j].block.name, &strings, &string_count, &string_capacity);
            }

            LK_U64 ai = 0;
            LK_U64 ti = 0;
            LK_U64 ei = 0;
            while (ti < all_count && ei < thread->event_count)
            {
                LK_U64 t_qpc = lkdbg_get_event_time(&temp_events[ti]);
                LK_U64 e_qpc = lkdbg_get_event_time(&thread->events[ei]);
                if (t_qpc < e_qpc)
                    all_events[ai++] = temp_events[ti++];
                else
                    all_events[ai++] = thread->events[ei++];
            }

            while (ti < all_count)
                all_events[ai++] = temp_events[ti++];

            while (ei < thread->event_count)
                all_events[ai++] = thread->events[ei++];

            all_count += thread->event_count;
        }

        LKDBG_FREE(temp_events);

        FILE* out = fopen(profile_path, "wb");
        if (out)
        {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);

            LKDBG_File_Header header;
            header.time_frequency = frequency.QuadPart;
            header.string_count = string_count;
            header.thread_count = lkdbg_context.thread_count;
            header.event_count = all_count;
            fwrite(&header, sizeof(header), 1, out);

            fwrite(strings, sizeof(LKDBG_File_String), string_count, out);

            for (LK_U64 i = 0; i < lkdbg_context.thread_count; i++)
            {
                LKDBG_Thread* thread = lkdbg_context.threads[i];

                LKDBG_File_Thread thread_data;
                thread_data.thread_id = thread->thread_id;
                thread_data.name = thread->name;
                fwrite(&thread_data, sizeof(thread_data), 1, out);
            }

            fwrite(all_events, sizeof(LKDBG_Event), all_count, out);
        }
        fclose(out);

        LKDBG_FREE(all_events);
        LKDBG_FREE(strings);
    }

    for (LK_U64 i = 0; i < lkdbg_context.thread_count; i++)
    {
        LKDBG_Thread* thread = lkdbg_context.threads[i];
        if (thread->events)
        {
            LKDBG_FREE(thread->events);
        }
        LKDBG_FREE(thread);
    }

    if (lkdbg_context.threads)
    {
        LKDBG_FREE(lkdbg_context.threads);
    }

    lkdbg_context.threads = 0;
    lkdbg_context.thread_count = 0;
    lkdbg_context.thread_capacity = 0;

    DeleteCriticalSection(&lkdbg_context.lock);
}

static void lkdbg_etw_print_error(const char* what, LK_U32 error_code)
{
    printf("%s %u", what, error_code);
    switch (error_code)
    {
    case ERROR_ACCESS_DENIED:          printf(" (ERROR_ACCESS_DENIED)");          break;
    case ERROR_ALREADY_EXISTS:         printf(" (ERROR_ALREADY_EXISTS)");         break;
    case ERROR_BAD_LENGTH:             printf(" (ERROR_BAD_LENGTH)");             break;
    case ERROR_BAD_PATHNAME:           printf(" (ERROR_BAD_PATHNAME)");           break;
    case ERROR_CANCELLED:              printf(" (ERROR_CANCELLED)");              break;
    case ERROR_DISK_FULL:              printf(" (ERROR_DISK_FULL)");              break;
    case ERROR_INVALID_HANDLE:         printf(" (ERROR_INVALID_HANDLE)");         break;
    case ERROR_INVALID_PARAMETER:      printf(" (ERROR_INVALID_PARAMETER)");      break;
    case ERROR_INVALID_TIME:           printf(" (ERROR_INVALID_TIME)");           break;
    case ERROR_MORE_DATA:              printf(" (ERROR_MORE_DATA)");              break;
    case ERROR_NO_SYSTEM_RESOURCES:    printf(" (ERROR_NO_SYSTEM_RESOURCES)");    break;
    case ERROR_NOACCESS:               printf(" (ERROR_NOACCESS)");               break;
    case ERROR_WMI_ALREADY_ENABLED:    printf(" (ERROR_WMI_ALREADY_ENABLED)");    break;
    case ERROR_WMI_INSTANCE_NOT_FOUND: printf(" (ERROR_WMI_INSTANCE_NOT_FOUND)"); break;
    }
    printf("\n");
}

static LKDBG_Event** lkdbg_etw_events;
static LK_U64* lkdbg_etw_event_count;
static LK_U64* lkdbg_etw_event_capacity;

static void WINAPI lkdbg_etw_callback(PEVENT_RECORD event)
{
    static GUID ThreadGuid = { 0x3d6fa8d1, 0xfe05, 0x11d0, { 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c } };

    typedef struct
    {
        LK_U32 NewThreadId;
        LK_U32 OldThreadId;
        LK_S8  NewThreadPriority;
        LK_S8  OldThreadPriority;
        LK_U8  PreviousCState;
        LK_S8  SpareByte;
        LK_S8  OldThreadWaitReason;
        LK_S8  OldThreadWaitMode;
        LK_S8  OldThreadState;
        LK_S8  OldThreadWaitIdealProcessor;
        LK_U32 NewThreadWaitTime;
        LK_U32 Reserved;
    } CSwitch;

    if (((LK_U64*) &event->EventHeader.ProviderId)[0] != ((LK_U64*) &ThreadGuid)[0]) return;
    if (((LK_U64*) &event->EventHeader.ProviderId)[1] != ((LK_U64*) &ThreadGuid)[1]) return;
    if (event->EventHeader.EventDescriptor.Opcode != 36) return;
    if (event->UserDataLength != sizeof(CSwitch)) return;

    CSwitch* cswitch = (CSwitch*) event->UserData;
    LK_U32 thread_id = cswitch->NewThreadId;

    LKDBG_Event debug_event;
    debug_event.kind = LKDBG_CONTEXT_SWITCH;
    debug_event.context_switch.processor = event->BufferContext.ProcessorNumber;
    debug_event.context_switch.thread_id = thread_id;
    debug_event.context_switch.time = event->EventHeader.TimeStamp.QuadPart;

    lkdbg_array_push((void**) lkdbg_etw_events, lkdbg_etw_event_count, lkdbg_etw_event_capacity, &debug_event, sizeof(LKDBG_Event));
}

static DWORD WINAPI lkdbg_etw_processing_thread(LPVOID userdata)
{
    lkdbg_register_thread("ETW processing thread");

    lkdbg_etw_events         = &lkdbg_thread->events;
    lkdbg_etw_event_count    = &lkdbg_thread->event_count;
    lkdbg_etw_event_capacity = &lkdbg_thread->event_capacity;

    TRACEHANDLE consumer_handle = (TRACEHANDLE) userdata;
    ULONG status = ProcessTrace(&consumer_handle, 1, 0, 0);
    if (status != ERROR_SUCCESS)
    {
        lkdbg_etw_print_error("ProcessTrace failure", status);
    }

    return 0;
}

static void lkdbg_etw_start()
{
    ULONG status;
    ULONG buffer_size = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(KERNEL_LOGGER_NAME);
    EVENT_TRACE_PROPERTIES* session_properties = (EVENT_TRACE_PROPERTIES*) LKDBG_MALLOC(buffer_size);

    // ControlTrace

    ZeroMemory(session_properties, sizeof(EVENT_TRACE_PROPERTIES));
    session_properties->Wnode.BufferSize = buffer_size;
    session_properties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
    session_properties->Wnode.ClientContext = 1; // magic constant, means that timestamps should be QueryPerformanceCounter
    session_properties->Wnode.Guid = SystemTraceControlGuid;
    session_properties->EnableFlags = EVENT_TRACE_FLAG_CSWITCH;
    session_properties->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
    session_properties->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
    LKDBG_MEMCPY((LPSTR)(session_properties + 1), KERNEL_LOGGER_NAME, sizeof(KERNEL_LOGGER_NAME));

    status = ControlTrace(0, KERNEL_LOGGER_NAME, session_properties, EVENT_TRACE_CONTROL_STOP);
    if (status != ERROR_SUCCESS && status != ERROR_WMI_INSTANCE_NOT_FOUND)
    {
        lkdbg_etw_print_error("ControlTrace failure", status);
        return;
    }

    // StartTrace

    // set this struct again, because ControlTrace seems to do something to it that StartTrace doesn't like
    ZeroMemory(session_properties, sizeof(EVENT_TRACE_PROPERTIES));
    session_properties->Wnode.BufferSize = buffer_size;
    session_properties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
    session_properties->Wnode.ClientContext = 1; // magic constant, means that timestamps should be QueryPerformanceCounter
    session_properties->Wnode.Guid = SystemTraceControlGuid;
    session_properties->EnableFlags = EVENT_TRACE_FLAG_CSWITCH;
    session_properties->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
    session_properties->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
    LKDBG_MEMCPY((LPSTR)(session_properties + 1), KERNEL_LOGGER_NAME, sizeof(KERNEL_LOGGER_NAME));

    TRACEHANDLE session_handle;
    status = StartTrace(&session_handle, KERNEL_LOGGER_NAME, session_properties);
    if (status != ERROR_SUCCESS)
    {
        printf("StartTrace failure %d", status);
        lkdbg_etw_print_error("StartTrace failure", status);
        return;
    }

    // OpenTrace

    EVENT_TRACE_LOGFILE log_file = {0};
    log_file.LoggerName = KERNEL_LOGGER_NAME;
    log_file.ProcessTraceMode = (PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD | PROCESS_TRACE_MODE_RAW_TIMESTAMP);
    log_file.EventRecordCallback = lkdbg_etw_callback;

    TRACEHANDLE consumer_handle = OpenTrace(&log_file);
    if (consumer_handle == INVALID_PROCESSTRACE_HANDLE)
    {
        lkdbg_etw_print_error("OpenTrace failure", GetLastError());
        return;
    }

    // processing thread

    lkdbg_context.etw_thread = CreateThread(0, 0, lkdbg_etw_processing_thread, (LPVOID) consumer_handle, 0, 0);
    lkdbg_context.etw_consumer_handle = consumer_handle;
}

static void lkdbg_etw_end()
{
    if (lkdbg_context.etw_consumer_handle)
    {
        CloseTrace(lkdbg_context.etw_consumer_handle);

        printf("Waiting for ETW processing thread...\n");
        WaitForSingleObject(lkdbg_context.etw_thread, INFINITE);
    }
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