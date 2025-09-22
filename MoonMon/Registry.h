#pragma once
#include <ntifs.h>

#include <ntdef.h>
#include <wdm.h>

#include "DataTypes.h"
#include "Utils.h"

NTSTATUS InspectRegistryEvent(_In_ PGC globals, _In_ PREG_INFO RI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);