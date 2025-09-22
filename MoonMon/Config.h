#pragma once
#include <ntdef.h>
#include "DataTypes.h"
#include "Utils.h"


void UnloadConfig(_In_ PGC G);

NTSTATUS initConfigLists(_In_ PGC globals);
NTSTATUS loadConfigItem(_In_ PGC G,_In_ UINT16 listType,_In_  PCFG cfg,_In_  PCFI newitem);
NTSTATUS LoadConfig(_In_ PGC G,_In_  PUNICODE_STRING RegistryPath);
NTSTATUS purgeConfigList(_In_ PGC G, _In_ UINT16 listType);