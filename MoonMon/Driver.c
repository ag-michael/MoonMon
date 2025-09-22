#include "Device.h"



void UnloadMoonMon(_In_ PDRIVER_OBJECT DriverObject)
{

	UNICODE_STRING symbolicLinkName;
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;

	KdPrint(("MoonMon: DriverUnload - Unloading\n"));

	if (UnregisterMoonMon(DriverObject) != STATUS_SUCCESS) {
		KdPrint(("MoonMon: UnregisterMoonMon failure!!! expect bug check\n"));
	}
	KdPrint(("Unregistered MoonMon\n"));
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\MoonMon");

	IoDeleteSymbolicLink(&symbolicLinkName);

	if (deviceObject != NULL) {
		KdPrint(("Deleting deviceobject\n"));
		IoDeleteDevice(deviceObject);
	}
	else {
		KdPrint(("MoonMon: No device object found to delete during unload.\n"));
	}

	

	KdPrint(("MoonMon: DriverUnload - Unloaded Successfully\n"));
}

NTSTATUS
DriverEntry(
	_In_  PDRIVER_OBJECT   DriverObject,
	_In_ PUNICODE_STRING      RegistryPath
)
{

	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING deviceName;
	UNICODE_STRING symbolicLinkName;
	PDEVICE_OBJECT deviceObject = NULL;
	DriverObject->DriverUnload = UnloadMoonMon;


	KdPrint(("MoonMon: DriverEntry - Loading, RegistryPath:%wZ\n",RegistryPath));

	RtlInitUnicodeString(&deviceName, L"\\Device\\MoonMon");

	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\MoonMon");

	status = IoCreateDevice(
		DriverObject,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&deviceObject);

	if (!NT_SUCCESS(status)) {
		KdPrint(("M0onMon Driver: Failed to create device object (0x%X)\n", status));
		return status;
	}


	status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("MoonMon: Failed to create symbolic link (0x%X)\n", status));
		IoDeleteDevice(deviceObject);
		return status;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = MoonMonDispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = MoonMonDispatchClose;
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadLogEntry;

	deviceObject->Flags |= DO_DIRECT_IO ;


	KdPrint(("MoonMon: DriverEntry - Loaded Successfully\n"));

	status |= InitMoonMon(DriverObject,RegistryPath,deviceObject);

	return status;
}

