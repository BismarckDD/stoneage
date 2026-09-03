/*-----------------------------------------------------------------------------
    caryMemory_lib.h -- cary's Virtual memory 恨瞶
    Waei(c) All rights reserved
    author: 砛κ秤Cary Hsu, 2000.10.8
-----------------------------------------------------------------------------*/
#ifndef __MEMDLL
#define __MEMDLL

#define _MEM_LIB

#ifdef _MEM_LIB
    #define MEMDLL_API
#else
    #ifdef MEMDLL_EXPORTS
        #define MEMDLL_API __declspec(dllexport)
    #else
        #define MEMDLL_API __declspec(dllimport)
    #endif
#endif

//肚岿粇***************************************************************************************
#define ERROR_MEMORY_OK                0x00000000
#define ERROR_MEMORY_INITFAILED        0x00000001    //皌竚Handle皚岿粇

#define ERROR_HEAP_NOT_ENOUGH        0x00000010
#define ERROR_HEAP_LIST_NOT_ENOUGH    0x00000020

//*************************************************************************************************
/*    ﹍て癘拘砰ㄧΑ畐
return:    ERROR_MEMORY_OK:        Θ
        ERROR_MEMORY_INITFAILED:﹍てア毖    */
DWORD MemoryInit();

/*    睦癘拘砰ㄧΑ畐癘拘砰    */
void MemoryRelease();

//*************************************************************************************************
/*    玂痙絛瞅Virtual Memory肚Handle某page虫(皌竚1commit page癘拘砰)
parameter:    lpdwHandle:    肚Handle
            iSize:        璶皌竚ぶ癘拘砰肚ミ
return:        獶NULL:        肚皌竚癘拘砰
            NULL:        ア毖                    */
MEMDLL_API LPVOID VirtualReserve( LPDWORD lpdwHandle, int &iSize);

/*    睦Handle皌竚癘拘砰
parameter:    lpdwHandle:    Handle    */
MEMDLL_API void VirtualRelease( LPDWORD lpdwHandle);

/*    祇ネException矪瞶ㄧΑ
parameter:    dwHandle:                        Handle
            lpEP:                            EXCEPTION_POINTERSstructure
return:        EXCEPTION_CONTINUE_EXECUTION:    膥尿磅︽
            EXCEPTION_CONTINUE_SEARCH:        膥尿碝т矪瞶exception handle    */
MEMDLL_API int ExpFilter( DWORD dwHandle, LPEXCEPTION_POINTERS lpEP);

//*************************************************************************************************
//*************************************************************************************************
/*    皌竚iSize癘拘砰讽Heap
parameter:    lpdwHandle:        肚Handle
            iSize:            璶皌竚ぶ癘拘砰
return:        TRUE:            皌竚Θ
            FALSE:            ア毖                */
MEMDLL_API BOOL HeapCreate( LPDWORD lpdwHandle, int &iSize);

/*    睦Heap┮Τ癘拘砰
parameter:    lpdwHandle:    Handle    */
MEMDLL_API void HeapRelease( LPDWORD lpdwHandle);

/*    睲埃HeapListㄏノ
parameter:    dwHandle:    Handle    */
MEMDLL_API void HeapReset( DWORD dwHandle);

/*    眖lpheapい皌竚dwSize癘拘砰
parameter:    dwHandle:    Handle
            dwSize:        璶皌竚ぶ癘拘砰
return:        獶NULL:        肚皌竚癘拘砰
            NULL:        ア毖                */
MEMDLL_API LPVOID HeapAlloc( DWORD dwHandle, DWORD dwSize);

/*    眖lpheapい睦奔Heapmemlt
parameter:    dwHandle:    Handle
            lpAddress:    璶睦癘拘砰                */
MEMDLL_API void HeapFree( DWORD dwHandle, LPVOID *lpAddress);

//*************************************************************************************************
//*************************************************************************************************
typedef struct USEDMEMORY{
    LPBYTE            lpbyStart;
    DWORD            dwSize;
}*LPUSEDMEMORY;

/*    lpheap材Heap list
parameter:    dwHandle:    Handle
return:        肚ㄏノいaddressNULL程    */
MEMDLL_API LPUSEDMEMORY GetMemoryListHead( DWORD dwHandle);

/*    lpheapHeap list
parameter:    dwHandle:    Handle
return:        肚ㄏノいaddressNULL程    */
MEMDLL_API LPUSEDMEMORY GetMemoryListNext( DWORD dwHandle);

/*    岿粇
parameter:    dwHandle:    Handle
return:        肚岿粇            */
MEMDLL_API DWORD GetCaryError( DWORD dwHandle);

//*************************************************************************************************
//*************************************************************************************************
/*    穐簿癘拘砰ㄏノMMX暗皐癸pentum II程ㄎて矪瞶
parameter:    dest:    ヘ
            src:    ㄓ方
            cnt:    穐簿byte计
return:        TRUE:    Θ磅︽
            FALSE:    ぃ猭                        */
MEMDLL_API BOOL mmx_CopyMemory( LPVOID dest, LPVOID src, int cnt);

/*    穐簿癘拘砰ㄏノMMX暗皐癸pentum II程ㄎて矪瞶
parameter:    dest:    ヘ
            src:    ㄓ方
            cnt:    穐簿byte计
return:        TRUE:    Θ磅︽
            FALSE:    ぃ猭                        */
MEMDLL_API BOOL mmx_CopyMemory_test( LPVOID dest, LPVOID src, int cnt);

#endif
