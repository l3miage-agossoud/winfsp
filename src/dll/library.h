/**
 * @file dll/library.h
 *
 * @copyright 2015-2016 Bill Zissimopoulos
 */
/*
 * This file is part of WinFsp.
 *
 * You can redistribute it and/or modify it under the terms of the
 * GNU Affero General Public License version 3 as published by the
 * Free Software Foundation.
 *
 * Licensees holding a valid commercial license may use this file in
 * accordance with the commercial license agreement provided with the
 * software.
 */

#ifndef WINFSP_DLL_LIBRARY_H_INCLUDED
#define WINFSP_DLL_LIBRARY_H_INCLUDED

#define WINFSP_DLL_INTERNAL
#define WINFSP_DLL_NODEFAULTLIB
#include <winfsp/winfsp.h>
#include <strsafe.h>

#define LIBRARY_NAME                    "WinFsp"

/* DEBUGLOG */
#if !defined(NDEBUG)
#define DEBUGLOG(fmt, ...)              \
    FspDebugLog("[U] " LIBRARY_NAME "!" __FUNCTION__ ": " fmt "\n", __VA_ARGS__)
#define DEBUGLOGSD(fmt, SD)             \
    FspDebugLogSD("[U] " LIBRARY_NAME "!" __FUNCTION__ ": " fmt "\n", SD)
#else
#define DEBUGLOG(fmt, ...)              ((void)0)
#define DEBUGLOGSD(fmt, SD)             ((void)0)
#endif

static inline PVOID MemAlloc(SIZE_T Size)
{
    extern HANDLE ProcessHeap;
    return HeapAlloc(ProcessHeap, 0, Size);
}
static inline VOID MemFree(PVOID Pointer)
{
    extern HANDLE ProcessHeap;
    if (0 != Pointer)
        HeapFree(ProcessHeap, 0, Pointer);
}

/*
 * Define WINFSP_DLL_NODEFAULTLIB to eliminate dependency on the MSVCRT libraries.
 *
 * For this to work the following project settings must be set:
 * - "C/C++ > General > SDL checks" must be empty (not "Yes" or "No").
 * - "C/C++ > Code Generation > Basic Runtime Checks" must be set to "Default"
 * - "C/C++ > Code Generation > Security Check" must be disabled (/GS-).
 * - "Linker > Input > Ignore All Default Libraries" must be "Yes".
 */
#if defined(WINFSP_DLL_NODEFAULTLIB)
#undef RtlFillMemory
#undef RtlMoveMemory
NTSYSAPI VOID NTAPI RtlFillMemory(VOID *Destination, DWORD Length, BYTE Fill);
NTSYSAPI VOID NTAPI RtlMoveMemory(VOID *Destination, CONST VOID *Source, DWORD Length);

#pragma function(memcpy)
#pragma function(memset)
static inline
void *memcpy(void *dst, const void *src, size_t siz)
{
    RtlMoveMemory(dst, src, (DWORD)siz);
    return dst;
}
static inline
void *memset(void *dst, int val, size_t siz)
{
    RtlFillMemory(dst, (DWORD)siz, val);
    return dst;
}
#endif

static FORCEINLINE
VOID InsertTailList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}
static FORCEINLINE
BOOLEAN RemoveEntryList(PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return Flink == Blink;
}

VOID FspNtStatusInitialize(VOID);

VOID FspFileSystemInitialize(VOID);
VOID FspFileSystemFinalize(VOID);

NTSTATUS FspFsctlRegister(VOID);
NTSTATUS FspFsctlUnregister(VOID);

NTSTATUS FspNpRegister(VOID);
NTSTATUS FspNpUnregister(VOID);

NTSTATUS FspEventLogRegister(VOID);
NTSTATUS FspEventLogUnregister(VOID);

#endif
