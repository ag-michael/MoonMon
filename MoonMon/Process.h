#pragma once
#include <ntifs.h>

#include <ntdef.h>
#include <wdm.h>
#include "DataTypes.h"
#include "Utils.h"
#include "Logging.h"

NTKERNELAPI PPEB PsGetProcessPeb(
    IN PEPROCESS Process);
void AddNewProcess(_In_ GC* globals, _In_ HANDLE p, _In_ PPROC_INFO PI);
void RemoveNewProcessStatus(_In_ GC* globals, _In_ HANDLE p);
BOOLEAN IsNewProcess(_In_ GC* globals, _In_ HANDLE p);
PPCACHE GetCachedProcess(_In_ GC* globals, _In_ HANDLE p);
void PurgeCachedProcess(_In_ GC* globals, _In_ HANDLE p);
BOOLEAN ProcessListMatch(_In_ GC* globals, _In_ PPROC_INFO PI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions);
BOOLEAN processInfoInit(_In_ PEPROCESS p, _In_ PPROC_INFO PI, _In_opt_  PPS_CREATE_NOTIFY_INFO pInfo, BOOLEAN parent);
void PCacheLogEntries(_In_ PGC globals, _In_ LONG64 id, _In_ LONGLONG Timestamp, _In_ PPROC_INFO PI, _In_ BOOLEAN target);
void processInfoDelete(PPROC_INFO PI);
NTSTATUS InspectProcessCreation(_In_ PGC globals, _In_ PPROC_INFO PI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);
void InspectProcessTermination(_In_ PGC globals, _In_ PPROC_INFO PI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);

BOOLEAN TerminateProcess(_In_ PEPROCESS p);
void NapTime();

/*
PVOID newProcCache(
	__in struct _RTL_AVL_TABLE* Table,
	__in CLONG  ByteSize
);

VOID freeProcCache(
	__in struct _RTL_AVL_TABLE* Table,
	__in PVOID  Buffer
);

RTL_GENERIC_COMPARE_RESULTS
compareProcCache(
	__in struct _RTL_AVL_TABLE* Table,
	__in PVOID  FirstStruct,
	__in PVOID  SecondStruct
);
*/