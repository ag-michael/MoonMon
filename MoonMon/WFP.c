
#include "WFP.h"

/*
Unregisters WFP callouts
*/
void UnregisterCallouts() {
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE hEngine = NULL;
	status = FwpmEngineOpen(NULL, RPC_C_AUTHN_DEFAULT, NULL, NULL, &hEngine);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to open WFP engine:%#08x\n", status));
		return;
	}
	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_V4);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_V4);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_V6);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_V6);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_UDP_V4);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_UDP_V4);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_UDP_V6);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_UDP_V6);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_ACCEPT_V4);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_ACCEPT_V4);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_ACCEPT_V6);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_ACCEPT_V6);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_LISTEN_V4);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_LISTEN_V4);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_LISTEN_V6);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_LISTEN_V6);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_RESOURCE_ASSIGNMENT_V4);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_RESOURCE_ASSIGNMENT_V4);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_RESOURCE_ASSIGNMENT_V6);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_RESOURCE_ASSIGNMENT_V6);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_DNS_CACHE_V4);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_DNS_CACHE_V4);

	FwpmCalloutDeleteByKey(hEngine, &MM_CALLOUT_DNS_CACHE_V6);
	FwpsCalloutUnregisterByKey(&MM_CALLOUT_DNS_CACHE_V6);

	FwpmEngineClose(hEngine);
}

/*
Unregisters WFP filters
*/
void UnregisterFilters() {
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE hEngine = NULL;
	status = FwpmEngineOpen(NULL, RPC_C_AUTHN_DEFAULT, NULL, NULL, &hEngine);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to open WFP engine:%#08x\n", status));
		return;
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_V4);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_V6);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #0\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_UDP_V4);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #1\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_UDP_V6);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #2\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_ACCEPT_V4);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #3\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_ACCEPT_V6);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #4\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_LISTEN_V4);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #5\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_LISTEN_V6);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #6\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_RESOURCE_ASSIGNMENT_V4);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #6\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_RESOURCE_ASSIGNMENT_V6);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #6\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_DNS_CACHE_V4);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #6\n"));
	}
	status = FwpmFilterDeleteByKey(hEngine, &MM_FILTER_DNS_CACHE_V6);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete filter by key #6\n"));
	}
	FwpmEngineClose(hEngine);
}
/*
Unregisters the WFP provider.
*/
void UnregisterProvider() {
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE hEngine = NULL;
	status = FwpmEngineOpen(NULL, RPC_C_AUTHN_DEFAULT, NULL, NULL, &hEngine);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to open WFP engine:%#08x\n", status));
		return;
	}
	status = FwpmProviderDeleteByKey(hEngine, &MM_WFP_PROVIDER);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to delete WFP Provider:%#08x\n", status));
	}
	else {
		KdPrint(("Deleted WFP provider\n"));
		FwpmFreeMemory((void**)&provider);
	}
	FwpmEngineClose(hEngine);
}

/*
Calls the various unregisration functions when tearing down the 
WFP registration.
*/
void UnregisterWFP(_In_ PGC globals) {
	UNREFERENCED_PARAMETER(globals);
	MM_ASSERT(globals, GANYMEDE);
	UnregisterFilters();
	KdPrint(("Deleted WFP Filters.\n"));
	UnregisterCallouts();
	KdPrint(("Deleted WFP callouts\n"));
	UnregisterProvider();
	KdPrint(("Unregistered WFP provider\n"));


}

/*
Registers NetClassifyCallback and NetNotifyCallback functions as the callouts
for the desired WFP layers.

Returns the number of succesfully registered callouts.
*/
ULONG registerCallouts(_In_ PGC globals, _In_ HANDLE hEngine, _In_ FWPS_CALLOUT_NOTIFY_FN NetNotifyCallback, _In_ FWPS_CALLOUT_CLASSIFY_FN NetClassifyCallback) {
	MM_ASSERT(globals, GANYMEDE);
	NTSTATUS status = STATUS_SUCCESS;

	const MM_CALLOUTS callouts[] = {
		{
	FWPM_LAYER_ALE_AUTH_CONNECT_V4,
	MM_CALLOUT_V4,
	 L"Collect IPV4 traffic information",
	  L"Collect IPV4 traffic information",
	  MM_WFP_PROVIDER
	},
		{
	FWPM_LAYER_ALE_AUTH_CONNECT_V6,
	MM_CALLOUT_V6,
	 L"Collect IPV6 traffic information",
	  L"Collect IPV6 traffic information",
	  MM_WFP_PROVIDER
	},
		{
	FWPM_LAYER_DATAGRAM_DATA_V4 ,
	MM_CALLOUT_UDP_V4,
	 L"Collect IPV4/UDP traffic information",
	  L"Collect IPV4/UDP traffic information",
	  MM_WFP_PROVIDER
	}
		,
		{
	FWPM_LAYER_DATAGRAM_DATA_V6,
	MM_CALLOUT_UDP_V6,
	 L"Collect IPV6/UDP traffic information",
	  L"Collect IPV6/UDP traffic information",
	  MM_WFP_PROVIDER
	},
		{
	FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4 ,
	MM_CALLOUT_ACCEPT_V4,
	 L"Collect IPV4/ACCEPT traffic information",
	  L"Collect IPV4/ACCEPT traffic information",
	  MM_WFP_PROVIDER
	},
		{
	FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6,
	MM_CALLOUT_ACCEPT_V6,
	 L"Collect IPV6/ACCEPT traffic information",
	  L"Collect IPV6/ACCEPT traffic information",
	  MM_WFP_PROVIDER
	},
		{
	FWPM_LAYER_ALE_AUTH_LISTEN_V4  ,
	MM_CALLOUT_LISTEN_V4,
	 L"Collect IPV4/LISTEN traffic information",
	  L"Collect IPV4/LISTEN traffic information",
	  MM_WFP_PROVIDER
	},
		{
	FWPM_LAYER_ALE_AUTH_LISTEN_V6,
	MM_CALLOUT_LISTEN_V6,
	 L"Collect IPV6/UDP traffic information",
	  L"Collect IPV6/UDP traffic information",
	  MM_WFP_PROVIDER
	},
	{
	 FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4  ,
	MM_CALLOUT_RESOURCE_ASSIGNMENT_V4,
	 L"Collect resource assignment information for IPV4",
	  L"Collect resource assignment information for IPV4",
	  MM_WFP_PROVIDER
	},
	{
	FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6,
	MM_CALLOUT_RESOURCE_ASSIGNMENT_V6,
	 L"Collect resource assignment information for IPV6",
	  L"Collect resource assignment information for IPV6",
	  MM_WFP_PROVIDER
	},
	{
	FWPM_LAYER_NAME_RESOLUTION_CACHE_V4 ,
	MM_CALLOUT_DNS_CACHE_V4,
	 L"Collect IPV4 DNS Cache activity information",
	  L"Collect IPV4 DNS Cache activity information",
	  MM_WFP_PROVIDER
	},
	{
	FWPM_LAYER_NAME_RESOLUTION_CACHE_V6,
	MM_CALLOUT_DNS_CACHE_V6,
	 L"Collect IPV6 DNS Cache activity information",
	  L"Collect IPV6 DNS Cache activity information",
	  MM_WFP_PROVIDER
	}

	};

	const ULONG callout_count = 10;
	ULONG i = 0;

	for (; i < callout_count; i++) {
		FWPS_CALLOUT callout = { 0 };
		callout.calloutKey = callouts[i].calloutKey;
		callout.notifyFn = NetNotifyCallback;
		callout.classifyFn = NetClassifyCallback;
		callout.flags = 0; // FWP_CALLOUT_FLAG_ALLOW_OFFLOAD;
		status = FwpsCalloutRegister(globals->Device, &callout, NULL);
		if (status != STATUS_SUCCESS) {
			KdPrint(("Failed to register WFP Callout index %u: %x\n", i, status));
			// We don't want driver to fail to load when callout regs fail
			// This is because some systems may have tcp/ip disabled entirely,
			// or conflicts/bugs with some other callouts. For now this is best effort.
			// but if no callouts are registerd, we should fail registration.
			--i;
		}
		FWPM_CALLOUT co = { 0 };
			co.applicableLayer = callouts[i].applicableLayer;
			co.calloutKey = callouts[i].calloutKey;

			co.displayData.name = callouts[i].name;
			co.displayData.description = callouts[i].description;
			co.providerKey = (GUID*)&callouts[i].providerKey;

			status = FwpmCalloutAdd(hEngine, &co, NULL, NULL);
			if (!NT_SUCCESS(status)) {
				KdPrint(("Failed to add callout %ls\n", callouts[i].name));
				--i;
			}
			else {
				KdPrint(("Added callout [%i][%ls]\n", i, callouts[i].name));
			}
		
	}

	return i;
}


/*
Setup the various WFP filters after associating them with the right
layers and callouts using the provided hEngine WFP engine handle.
*/
NTSTATUS setupFilters(_In_ HANDLE hEngine) {
	NTSTATUS status = STATUS_SUCCESS;
	 
	MM_FILTERS filters[] = {
		{MM_FILTER_V4,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter V4",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_AUTH_CONNECT_V4,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_V4,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,
		MM_FILTER_V4_ID
		},

		{MM_FILTER_V6,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter V6",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_AUTH_CONNECT_V6,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_V6,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,
		MM_FILTER_V6_ID
		},

		{MM_FILTER_UDP_V4,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter UDP V4",
		8,
		FWP_UINT8,
		FWPM_LAYER_DATAGRAM_DATA_V4,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_UDP_V4,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0, MM_FILTER_UDP_V4_ID},

		{MM_FILTER_UDP_V6,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter UDP V6",
		8,
		FWP_UINT8,
		FWPM_LAYER_DATAGRAM_DATA_V6,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_UDP_V6,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_UDP_V6_ID },

		{MM_FILTER_ACCEPT_V4,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter ACCEPT V4",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_ACCEPT_V4,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_ACCEPT_V4_ID },

		{MM_FILTER_ACCEPT_V6,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter ACCEPT V6",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_ACCEPT_V6,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_ACCEPT_V6_ID },

		{MM_FILTER_LISTEN_V4,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter LISTEN V4",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_AUTH_LISTEN_V4,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_LISTEN_V4,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_LISTEN_V4_ID },

		{MM_FILTER_LISTEN_V6,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter LISTEN V6",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_AUTH_LISTEN_V6,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_LISTEN_V6,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_LISTEN_V6_ID },


		{ MM_FILTER_RESOURCE_ASSIGNMENT_V4,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter for resource assignment IPV4",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4 ,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_RESOURCE_ASSIGNMENT_V4,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_RESOURCE_ASSIGNMENT_V4_ID },


		{ MM_FILTER_RESOURCE_ASSIGNMENT_V6,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter for resource assignment IPV6",
		8,
		FWP_UINT8,
		FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6 ,
		FWP_ACTION_CALLOUT_INSPECTION,
		MM_CALLOUT_RESOURCE_ASSIGNMENT_V6,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_RESOURCE_ASSIGNMENT_V6_ID },


		{ MM_FILTER_DNS_CACHE_V4,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter DNS Cache IPV4",
		1,
		FWP_UINT8,
		FWPM_LAYER_NAME_RESOLUTION_CACHE_V4 ,
		FWP_ACTION_CALLOUT_TERMINATING,
		MM_CALLOUT_DNS_CACHE_V4,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_DNS_CACHE_V4_ID },


		{ MM_FILTER_DNS_CACHE_V6,
		MM_WFP_PROVIDER,
		L"MoonMon WFP Filter DNS Cache IPV6",
		1,
		FWP_UINT8,
		FWPM_LAYER_NAME_RESOLUTION_CACHE_V6 ,
		FWP_ACTION_CALLOUT_TERMINATING,
		MM_CALLOUT_DNS_CACHE_V6,
		MM_INSPECTION_SUBLAYER,
		L"Moon inspection sublayer for transport",
		0,
		0,MM_FILTER_DNS_CACHE_V6_ID }

	};
	const ULONG filter_count = 10;

	for (ULONG i = 0; i < filter_count; i++) {
		FWPM_FILTER filter = { 0 };
		//FWPM_SUBLAYER sublayer = { 0 };

		filter.filterKey = filters[i].filterKey;
		filter.providerKey = (GUID*)&filters[i].providerKey;
		filter.displayData.name = filters[i].filterName;
		filter.weight.uint8 = filters[i].weight;
		filter.weight.type = filters[i].weight_type;
		filter.layerKey = filters[i].layerKey;
		filter.action.type = filters[i].action_type;
		filter.action.calloutKey = filters[i].callout_key;
		//filter.rawContext = filters[i].id;

		filter.flags = FWPM_FILTER_FLAG_NONE;
		//filter.subLayerKey = filters[i].sublayer_key;

		/*sublayer.subLayerKey = filters[i].sublayer_key;
		sublayer.displayData.name = filters[i].sublayerName;
		sublayer.flags = filters[i].sublayer_flags;
		sublayer.weight = filters[i].sublayer_weight;*/

		//status |= FwpmSubLayerAdd(hEngine, &sublayer, NULL);

		status |= FwpmFilterAdd(hEngine, &filter, NULL, &filters[i].id);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to add layer/sublayer %ls/%ls:%#08x\n", filters[i].filterName, filters[i].sublayerName, status));

		}
	}
	return status;
}

/*
Registers callouts and filters using the provided WFP engine handle hEngine and the notify and classify callout
functions.
*/
NTSTATUS setupCalloutsAndFilters(_In_ PGC globals, _In_ HANDLE hEngine, _In_ FWPS_CALLOUT_NOTIFY_FN NetNotifyCallback, _In_ FWPS_CALLOUT_CLASSIFY_FN NetClassifyCallback) {
	NTSTATUS status;
	status = FwpmTransactionBegin(hEngine, 0);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to start a WFP transaction:%#08x\n", status));
		FwpmFreeMemory((void**)provider);
	}
	else {
		if (registerCallouts(globals, hEngine, NetNotifyCallback, NetClassifyCallback) < 1) {
			KdPrint(("WFP setup failed because no callouts could be registered!\n"));
			return STATUS_INTERNAL_ERROR;
		}
		else {
			status = FwpmTransactionCommit(hEngine);
			if (!NT_SUCCESS(status)) {
				KdPrint(("Failed to commit a WFP transaction:%#08x\n", status));
				return STATUS_INTERNAL_ERROR;
			}
			else {
				status = FwpmTransactionBegin(hEngine, 0);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to start a WFP transaction:%#08x\n", status));
				}
				else {
					KdPrint(("Finished setting up WFP callouts.\n"));
					status = setupFilters(hEngine);

					if (!NT_SUCCESS(status)) {
						KdPrint(("Warning, failed to add WFP Filters:%#08x\n", status));
					}

					status = FwpmTransactionCommit(hEngine);
					if (!NT_SUCCESS(status)) {
						KdPrint(("Failed to commit a WFP transaction:%#08x\n", status));

					}

				}
			}
		}
	}
	return status;

}

/*
This is the core WFP setup function.
It opens the WFP engine, adds a provider and calls 
setupCalloutsAndFilters to set up the configured callouts and their filters.
*/
NTSTATUS SetupWFP(_In_ PGC globals, _In_ FWPS_CALLOUT_NOTIFY_FN NetNotifyCallback, _In_ FWPS_CALLOUT_CLASSIFY_FN NetClassifyCallback) {
	MM_ASSERT(globals, GANYMEDE);
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE hEngine = NULL;
	LARGE_INTEGER waitInterval;
	waitInterval.QuadPart = -10 * (10 * 1000 * 1000);

	status = FwpmEngineOpen(NULL, RPC_C_AUTHN_DEFAULT, NULL, NULL, &hEngine);
	if(!NT_SUCCESS(status)) {
		FwpmEngineClose(hEngine);
		return status;
	}
	else {
		provider = NULL;
		status = FwpmProviderGetByKey(hEngine, &MM_WFP_PROVIDER, &provider);
		/*if (NT_SUCCESS(status)) {
			KdPrint(("Existing WFP provider found by key:%#08x\n", status));
			FwpmFreeMemory((void**)&provider);
		}
		else {*/
			FWPM_PROVIDER reg = { 0 };
			WCHAR name[] = L"MoonMon WFP Provider";
			reg.displayData.name = name;
			reg.providerKey = MM_WFP_PROVIDER;
			//reg.flags = FWPM_PROVIDER_FLAG_PERSISTENT;
			status = FwpmProviderAdd0(hEngine, &reg, NULL);
			if (!NT_SUCCESS(status)) {
				KdPrint(("Failed to get WFP provider added:%#08x\n", status));
			}
			else {

				status |= setupCalloutsAndFilters(globals,hEngine,NetNotifyCallback,NetClassifyCallback);
			}
		//}
		FwpmEngineClose(hEngine);
	}
	return status;
}

/*
Uses the LIST_TYPE parameter to determine which thred creation list to use,
and searches the config group list found for entries that match the corresponding
values of the PWFP_INFO value (NI)'s members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
The littleEndian parameter dictates the endianess for IPv4 field evaluations.
*/
BOOLEAN NetworkEventListMatch(_In_ GC* globals, _In_ PWFP_INFO NI,_In_ BOOLEAN littleEndian, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(NI, NEREID);
	BOOLEAN matched = FALSE;
	ULONG matches = 0,_id=0;
	CONFIG_GROUP** PL;

	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case WFP_BLOCK_LIST:
		PL = globals->WBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("NetworkEventListMatch: Invalid list WBL\n"));
			return FALSE;
		}
		break;
	case WFP_INCLUDE_LIST:
		PL = globals->WINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("NetworkEventListMatch: Invalid list WINC\n"));
			return FALSE;
		}
		break;
	case WFP_EXCLUDE_LIST:
		PL = globals->WEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("NetworkEventListMatch: Invalid list WEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("NetworkEventListMatch: list type!\n"));
		return FALSE;
	}

	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("NetworkEventListMatch: Invalid list entry\n"));
			break;
		}
		matches = 0;
		
		//  KdPrint(("PL %u / %p \n", i,PL[i]));
	   //   KdPrint(("%u entries\n", PL[i]->count));
		try {
			_id = PL[i]->id;
			ULONG j = 0;
			do {
				CFI* entry = PL[i]->entries[j];
				//  KdPrint(("[i:%u|j:%u] PL %u has %u entries. entry: %p\n", i,j, i, PL[i]->count,entry));
				switch (entry->field_type) {
				case MM_WFP_LAYER:
					CheckUshortEntry(entry, NI->layer, &matches);
				//	KdPrint(("+MM_WFP_LAYER:[%u] %u\n", _id, matches));
					break;
				case MM_NET_DIRECTION:
					CheckInt8Entry(entry, NI->Direction, &matches);
				//	KdPrint(("+MM_NET_DIRECTION:[%u] %u\n", _id, matches));
					break;
				case MM_NET_IP_PROTOCOL:
					CheckUint8Entry(entry, NI->ipProtocol, &matches);
				//	KdPrint(("+MM_NET_IP_PROTOCOL:[%u] %u\n", _id, matches));
					break;
				case MM_NET_ADDRESS_TYPE:
					CheckUint8Entry(entry, NI->addrType, &matches);
				//	KdPrint(("+MM_NET_ADDRESS_TYPE:[%u] %u\n", _id, matches));
					break;
				case MM_NET_PROMISCUOUS:
					CheckUint8Entry(entry, NI->promiscuous, &matches);
				//	KdPrint(("+MM_NET_PROMISCUOUS:[%u] %u\n", _id, matches));
					break;
				case MM_NET_INTERFACE_TYPE:
					CheckUlongEntry(entry, NI->interfaceType, &matches);
				//	KdPrint(("+MM_NET_INTERFACE_TYPE:[%u] %u\n", _id, matches));
					break;
				case MM_NET_INTERFACE:
					CheckLonglongEntry(entry,(LONGLONG) NI->interface, &matches);
				//	KdPrint(("+MM_NET_INTERFACE:[%u] %u\n", _id, matches));
					break;
				case MM_NET_LOCAL_PORT:
					CheckUshortEntry(entry, NI->localPort, &matches);
				//	KdPrint(("+MM_NET_LOCAL_PORT:[%u] %u\n", _id, matches));
					break;
				case MM_NET_REMOTE_PORT:
				//	KdPrint(("-REMOTE_PORT:[%u] %u\n", _id, matches));
					CheckUshortEntry(entry, NI->remotePort, &matches);
		//		KdPrint(("+REMOTE_PORT:[%u] %u\n", _id,matches));
					break;
				case MM_NET_LOCAL_IPV4_ADDR:
					CheckIpv4(entry, NI->localIpv4Addr, littleEndian, &matches);
				//	KdPrint(("+MM_NET_LOCAL_IPV4_ADDR:[%u] %u\n", _id, matches));
					break;
				case MM_NET_REMOTE_IPV4_ADDR:
					CheckIpv4(entry, NI->remoteIpv4Addr, littleEndian, &matches);
				//	KdPrint(("+REMOTE_IPV4:[%u] %u, v4:%#08x\n", _id, matches,entry->ipv4Subnet));
					break;
				case MM_NET_LOCAL_IPV6_ADDR:
					CheckIpv6(entry, NI->localIpv6Addr, &matches);
				//	KdPrint(("+MM_NET_LOCAL_IPV6_ADDR:[%u] %u\n", _id, matches));
					break;
				case MM_NET_REMOTE_IPV6_ADDR:
					CheckIpv6(entry, NI->remoteIpv6Addr, &matches);
				//	KdPrint(("+REMOTE_IPV6:[%u] %u, v6:%p\n", _id, matches,entry->ipv6Subnet));
					break;
				case IMAGE_FILE_NAME:
					if(MM_TYPE_CHECK(NI->PI,PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->ImageFileName, &matches);
				//	KdPrint(("+IMAGE_FILE_NAME:[%u] %u\n", _id, matches));
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(NI->PI, PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->ParentImageFileName, &matches);
				//	KdPrint(("+PARENT_IMAGE_FILE_NAME:[%u] %u\n", _id, matches));
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(NI->PI, PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->CommandLine, &matches);
				//KdPrint(("+CMD:[%u] %u\n", _id, matches));
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(NI->PI, PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->ParentCommandLine, &matches);
					//KdPrint(("+PARENT_COMMAND_LINE:[%u] %u\n", _id, matches));
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(NI->PI, PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->WindowTitle, &matches);
					//KdPrint(("+PWINDOW_TITLE:[%u] %u\n", _id, matches));
					break;
				case PCWD:
					if (MM_TYPE_CHECK(NI->PI, PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->CWD, &matches);
					//KdPrint(("+PCWD:[%u] %u\n", _id, matches));
					break;
				case TARGET_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(NI->PI, PHOEBE))
					CheckUnicodeEntry(entry, NI->PI->ImageFileName, &matches);
				//	KdPrint(("+TARGET_IMAGE_FILE_NAME:[%u] %u\n", _id, matches));
					break;

				default:
					KdPrint(("NetworkEventListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
				KdPrint(("NetworkEventListMatch:Matches %u , %u vs %u\n", _id, PL[i]->count, matches));
				
				break;
			}
			else {
		//	KdPrint(("NetworkEventListMatch:No matches %u,  %u vs %u\n", _id, PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("NetworkEventListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Inspects the network information (NI) struct's members.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
The littleEndian parameter dictates the endianess for IPv4 field evaluations.
*/
void InspectNetworkEvent(_In_ PGC globals, _In_ PWFP_INFO NI,_In_ BOOLEAN littleEndian, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;

	if (globals->WINC_COUNT > 0) {
		if (NetworkEventListMatch(globals, NI, littleEndian,WFP_INCLUDE_LIST, globals->WINC_COUNT, &id, &actions) == TRUE) {
				*match_id = id;
			*skip_logging = FALSE;
			return; // if there is an include list, exclude lists won't be processed
		}
		
	}
	if (*skip_logging == FALSE && globals->WEXC_COUNT > 0) {
		if (NetworkEventListMatch(globals, NI, littleEndian,WFP_EXCLUDE_LIST, globals->WEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}
	}
}