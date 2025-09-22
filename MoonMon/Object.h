#pragma once

#include <ntifs.h>

#include <ntdef.h>
#include <wdm.h>

#include "DataTypes.h"
#include "Utils.h"

ULONG InspectObjectAccessEvent(_In_ PGC globals, _In_ POBPROC OB, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);
