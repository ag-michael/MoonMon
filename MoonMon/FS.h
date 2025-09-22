#pragma once
#include <ntifs.h>

#include <ntdef.h>
#include <wdm.h>
#include <fltKernel.h>

#include "DataTypes.h"
#include "Utils.h"

NTSTATUS InspectFsSetEvent(_In_ PGC globals, _In_ PFILE_SET_INFO FI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);
NTSTATUS InspectFsCreateEvent(_In_ PGC globals, _In_ PFILE_INFO FI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);
