#include <ntddk.h>
#include <wdf.h>
#include <wdftypes.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <wdmsec.h>
#include <wdm.h>
#include <wdfobject.h>


__drv_dispatchType(IRP_MJ_CREATE) DRIVER_DISPATCH MoonMonDispatchCreate;
__drv_dispatchType(IRP_MJ_CLOSE) DRIVER_DISPATCH MoonMonDispatchClose;
__drv_dispatchType(IRP_MJ_READ) DRIVER_DISPATCH ReadLogEntry;


DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD UnloadMoonMon;


NTSTATUS  UnregisterMoonMon(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS InitMoonMon(PDRIVER_OBJECT   DriverObject, PUNICODE_STRING RegistryPath, PDEVICE_OBJECT deviceObject);
