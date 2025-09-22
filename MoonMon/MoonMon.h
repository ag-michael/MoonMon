#pragma once

//#include <ntddk.h>
//#include <wdf.h>

#include <ntifs.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntdef.h>
#include <wdm.h>
#include <fltKernel.h>
#include <ip2string.h>
#include "DataTypes.h"
#include "WFP.h"
#include "Utils.h"
#include "Config.h"
#include "Logging.h"
#include "Process.h"
#include "FS.h"
#include "Registry.h"
#include "Object.h"
#include "Imageload.h"
#include "Thread.h"

PFLT_FILTER FsFilter;


// constants
const char* POSTAMBLE = "NOMNOOM";


NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
    IN INT64,
    OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL);










