#pragma once
#include <ntifs.h>

#include <ntdef.h>
#include <wdm.h>

#include "DataTypes.h"
#include "Utils.h"

void InspectImageloadEvent(_In_ PGC globals, _In_ PMODULE_INFO MI, _Out_ PBOOLEAN skip_logging,_Out_ PULONG match_id);
