#pragma once
#include <ntdef.h>
#include "DataTypes.h"
#include "Utils.h"
_IRQL_raises_(APC_LEVEL)
void AddLogEntry(_In_ PGC G, _In_ LONG64 id, _In_ LONGLONG Timestamp, _In_ ULONG size, _In_ USHORT type, _In_ PVOID Data);
void DeleteLogEntry(_In_ PGC G, _Inout_ LOG_ENTRY* entry);