#pragma once

#include <ntifs.h>

#include <ntdef.h>
#include <wdm.h>

#include "DataTypes.h"
#include "Utils.h"

void InspectThreadCreation(_In_ PGC globals, _In_ PTC_INFO TI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id);
