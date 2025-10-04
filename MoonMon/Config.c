#include "DataTypes.h"
#include "Config.h"




/*
Resolves the listType parameter's value and returns the correct CONFIG_GROUP list.
It sets the 'c' parameter to the current count of entries in the list and the
list_max parameter's value is set to the maximum number of entries allowed for the list.
*/
CONFIG_GROUP** resolveList(_In_ PGC G, UINT16 listType, PULONG* c, PULONG list_max) {
    CONFIG_GROUP** List=NULL;
    *c = 0;
    *list_max = 0;

    switch (listType) {
    case(PROCESS_BLOCK_LIST):
        List = G->PBL;
        *c = &G->PBL_COUNT;
        *list_max = PBL_ENTRIES_MAX;
        break;
    case(PROCESS_INCLUDE_LIST):
        List = G->PINC;
        *c = &G->PINC_COUNT;
        *list_max = PINC_ENTRIES_MAX;
        break;
    case(PROCESS_EXCLUDE_LIST):
        List = G->PEXC;
        *c = &G->PEXC_COUNT;
        *list_max = PEXC_ENTRIES_MAX;
        break;
    case(PROCESS_TERMINATED_INCLUDE_LIST):
        List = G->PTINC;
        *c = &G->PTINC_COUNT;
        *list_max = PTINC_ENTRIES_MAX;
        break;
    case(PROCESS_TERMINATED_EXCLUDE_LIST):
        List = G->PTEXC;
        *c = &G->PTEXC_COUNT;
        *list_max = PTEXC_ENTRIES_MAX;
        break;
    case(THREAD_BLOCK_LIST):
        List = G->TBL;
        *c = &G->TBL_COUNT;
        *list_max = TBL_ENTRIES_MAX;
        break;
    case(THREAD_INCLUDE_LIST):
        List = G->TINC;
        *c = &G->TINC_COUNT;
        *list_max = TINC_ENTRIES_MAX;
        break;
    case(THREAD_EXCLUDE_LIST):
        List = G->TEXC;
        *c = &G->TEXC_COUNT;
        *list_max = TEXC_ENTRIES_MAX;
        break;
    case(MODULE_BLOCK_LIST):
        List = G->MBL;
        *c = &G->MBL_COUNT;
        *list_max = MBL_ENTRIES_MAX;
        break;
    case(MODULE_INCLUDE_LIST):
        List = G->MINC;
        *c = &G->MINC_COUNT;
        *list_max = MINC_ENTRIES_MAX;
        break;
    case(MODULE_EXCLUDE_LIST):
        List = G->MEXC;
        *c = &G->MEXC_COUNT;
        *list_max = MEXC_ENTRIES_MAX;
        break;
    case(OBJECT_BLOCK_LIST):
        List = G->OBL;
        *c = &G->OBL_COUNT;
        *list_max = OBL_ENTRIES_MAX;
        break;
    case(OBJECT_INCLUDE_LIST):
        List = G->OINC;
        *c = &G->OINC_COUNT;
        *list_max = OINC_ENTRIES_MAX;
        break;
    case(OBJECT_EXCLUDE_LIST):
        List = G->OEXC;
        *c = &G->OEXC_COUNT;
        *list_max = OEXC_ENTRIES_MAX;
        break;
    case(REGISTRY_BLOCK_LIST):
        List = G->RBL;
        *c = &G->RBL_COUNT;
        *list_max = RBL_ENTRIES_MAX;
        break;
    case(REGISTRY_INCLUDE_LIST):
        List = G->RINC;
        *c = &G->RINC_COUNT;
        *list_max = RINC_ENTRIES_MAX;
        break;
    case(REGISTRY_EXCLUDE_LIST):
        List = G->REXC;
        *c = &G->REXC_COUNT;
        *list_max = REXC_ENTRIES_MAX;
        break;
    case(FILE_CREATE_BLOCK_LIST):
        List = G->FCBL;
        *c = &G->FCBL_COUNT;
        *list_max = FCBL_ENTRIES_MAX;
        break;
    case(FILE_CREATE_INCLUDE_LIST):
        List = G->FCINC;
        *c = &G->FCINC_COUNT;
        *list_max = FCINC_ENTRIES_MAX;
        break;
    case(FILE_CREATE_EXCLUDE_LIST):
        List = G->FCEXC;
        *c = &G->FCEXC_COUNT;
        *list_max = FCEXC_ENTRIES_MAX;
        break;
    case(FILE_SET_INFO_BLOCK_LIST):
        List = G->FSBL;
        *c = &G->FSBL_COUNT;
        *list_max = FSBL_ENTRIES_MAX;
        break;
    case(FILE_SET_INFO_INCLUDE_LIST):
        List = G->FSINC;
        *c = &G->FSINC_COUNT;
        *list_max = FSINC_ENTRIES_MAX;
        break;
    case(FILE_SET_INFO_EXCLUDE_LIST):
        List = G->FSEXC;
        *c = &G->FSEXC_COUNT;
        *list_max = FSEXC_ENTRIES_MAX;
        break;
    case(WFP_BLOCK_LIST):
        List = G->WBL;
        *c = &G->WBL_COUNT;
        *list_max = WBL_ENTRIES_MAX;
        break;
    case(WFP_INCLUDE_LIST):
        List = G->WINC;
        *c = &G->WINC_COUNT;
        *list_max = WINC_ENTRIES_MAX;
        break;
    case(WFP_EXCLUDE_LIST):
        List = G->WEXC;
        *c = &G->WEXC_COUNT;
        *list_max = WEXC_ENTRIES_MAX;
        break;
    default:
        return NULL;

    }
    return List;
}
/*
Allocates memory for all supported config lists.
*/
NTSTATUS initConfigLists(_In_ PGC G) {
    /*
    I would want to simplify this a lot and use a loop to initialize all these.
    Gave it a shot and it introduced bugs, since this approach works really well, leaving it this way for now.
    */
    G->PBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * PBL_ENTRIES_MAX, MAGIC);
    if (G->PBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->PBL, sizeof(CONFIG_GROUP*) * PBL_ENTRIES_MAX);
    G->PINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * PINC_ENTRIES_MAX, MAGIC);
    if (G->PINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->PINC, sizeof(CONFIG_GROUP*) * PINC_ENTRIES_MAX);
    G->PEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * PEXC_ENTRIES_MAX, MAGIC);
    if (G->PEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->PEXC, sizeof(CONFIG_GROUP*) * PEXC_ENTRIES_MAX);
    G->PTINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * PTINC_ENTRIES_MAX, MAGIC);
    if (G->PTINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->PTINC, sizeof(CONFIG_GROUP*) * PTINC_ENTRIES_MAX);
    G->PTEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * PTEXC_ENTRIES_MAX, MAGIC);
    if (G->PTEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->PTEXC, sizeof(CONFIG_GROUP*) * PTEXC_ENTRIES_MAX);
    G->TBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * TBL_ENTRIES_MAX, MAGIC);
    if (G->TBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->TBL, sizeof(CONFIG_GROUP*) * TBL_ENTRIES_MAX);
    G->TINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * TINC_ENTRIES_MAX, MAGIC);
    if (G->TINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->TINC, sizeof(CONFIG_GROUP*) * TINC_ENTRIES_MAX);
    G->TEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * TEXC_ENTRIES_MAX, MAGIC);
    if (G->TEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->TEXC, sizeof(CONFIG_GROUP*) * TEXC_ENTRIES_MAX);
    G->MBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * MBL_ENTRIES_MAX, MAGIC);
    if (G->MBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->MBL, sizeof(CONFIG_GROUP*) * MBL_ENTRIES_MAX);
    G->MINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * MINC_ENTRIES_MAX, MAGIC);
    if (G->MINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->MINC, sizeof(CONFIG_GROUP*) * MINC_ENTRIES_MAX);
    G->MEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * MEXC_ENTRIES_MAX, MAGIC);
    if (G->MEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->MEXC, sizeof(CONFIG_GROUP*) * MEXC_ENTRIES_MAX);
    G->OBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * OBL_ENTRIES_MAX, MAGIC);
    if (G->OBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->OBL, sizeof(CONFIG_GROUP*) * OBL_ENTRIES_MAX);
    G->OINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * OINC_ENTRIES_MAX, MAGIC);
    if (G->OINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->OINC, sizeof(CONFIG_GROUP*) * OINC_ENTRIES_MAX);
    G->OEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * OEXC_ENTRIES_MAX, MAGIC);
    if (G->OEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->OEXC, sizeof(CONFIG_GROUP*) * OEXC_ENTRIES_MAX);
    G->RBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * RBL_ENTRIES_MAX, MAGIC);
    if (G->RBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->RBL, sizeof(CONFIG_GROUP*) * RBL_ENTRIES_MAX);
    G->RINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * RINC_ENTRIES_MAX, MAGIC);
    if (G->RINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->RINC, sizeof(CONFIG_GROUP*) * RINC_ENTRIES_MAX);
    G->REXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * REXC_ENTRIES_MAX, MAGIC);
    if (G->REXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->REXC, sizeof(CONFIG_GROUP*) * REXC_ENTRIES_MAX);
    G->FCBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * FCBL_ENTRIES_MAX, MAGIC);
    if (G->FCBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->FCBL, sizeof(CONFIG_GROUP*) * FCBL_ENTRIES_MAX);
    G->FCINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * FCINC_ENTRIES_MAX, MAGIC);
    if (G->FCINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->FCINC, sizeof(CONFIG_GROUP*) * FCINC_ENTRIES_MAX);
    G->FCEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * FCEXC_ENTRIES_MAX, MAGIC);
    if (G->FCEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->FCEXC, sizeof(CONFIG_GROUP*) * FCEXC_ENTRIES_MAX);
    G->FSBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * FSBL_ENTRIES_MAX, MAGIC);
    if (G->FSBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->FSBL, sizeof(CONFIG_GROUP*) * FSBL_ENTRIES_MAX);
    G->FSINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * FSINC_ENTRIES_MAX, MAGIC);
    if (G->FSINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->FSINC, sizeof(CONFIG_GROUP*) * FSINC_ENTRIES_MAX);
    G->FSEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * FSEXC_ENTRIES_MAX, MAGIC);
    if (G->FSEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->FSEXC, sizeof(CONFIG_GROUP*) * FSEXC_ENTRIES_MAX);
    G->WBL = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * WBL_ENTRIES_MAX, MAGIC);
    if (G->WBL == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->WBL, sizeof(CONFIG_GROUP*) * WBL_ENTRIES_MAX);
    G->WINC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * WINC_ENTRIES_MAX, MAGIC);
    if (G->WINC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->WINC, sizeof(CONFIG_GROUP*) * WINC_ENTRIES_MAX);

    G->WEXC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP*) * WEXC_ENTRIES_MAX, MAGIC);
    if (G->WEXC == NULL) {
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(G->WEXC, sizeof(CONFIG_GROUP*) * WEXC_ENTRIES_MAX);
    
   
    return STATUS_SUCCESS;

}

/*
Calls resolveList to determine the correct list corresponding to listType.
It then frees all the entries in that list as well as the list itself.
*/
NTSTATUS purgeConfigList(_In_ PGC G,_In_ UINT16 listType) {
    MM_ASSERT(G, GANYMEDE);
    PULONG C = 0;
    ULONG c = 0, list_max = 0;
    CONFIG_GROUP** List = resolveList(G, listType, &C, &list_max);
    if (List == NULL || C == NULL) {
        KdPrint(("purgeConfigList: Unable to resolve list of type %hu\n", listType));
        return STATUS_INVALID_MESSAGE;
    }
    c = *C;
    ULONG i = 0, j = 0;

    for (i; i < c && i<list_max; i++) {
        if (List[i] == NULL) break;
        if (!MM_TYPE_CHECK(List[i],CALIBAN)) {
            continue;
        }
        for (j = 0; j < List[i]->count; j++) {
            ExFreePool(List[i]->entries[j]);
        }

        List[i]->count = 0;
        ExFreePool(List[i]);
    }
    KdPrint(("purgeConfigList: Purged list of type %hu\n", listType));
    return STATUS_SUCCESS;
}

/*
Processes a config item. Returns STATUS_SUCCESS upon succesful processing of an entry.
Each config entry newitem could be a stand-alone entry or part of a group (such as for AND logic matching).
It determines the right config group and associates the entry accordingly.
If there is no group, it allocates memory for a new entry and adds it to the config list.
The actions value for each config group entry is also updated to indicate to
matchers later on what action they should take upon a succesful match.
*/
NTSTATUS loadConfigItem(_In_ PGC G, _In_ UINT16 listType, _In_ PCFG cfg, _In_ PCFI newitem) {
    MM_ASSERT(G, GANYMEDE);
    
    PULONG C = 0;
    ULONG list_max = 0;
    CONFIG_GROUP** List = resolveList(G, listType, &C, &list_max);
    if (List == NULL || C == NULL) {
        KdPrint(("loadConfigItem: Unable to resolve list of type %hu\n", listType));
        return STATUS_INVALID_MESSAGE;
    }
    ULONG c = *C;
    BOOLEAN group_found = FALSE;
    // Search if there is an existing config group with a matching id
    // if there is a match, then add the new item to that group
    // all items in a group must match during inspection
    // actions from the new item are AND'd (&) 
    // this means the last action can clear previous actions
    // it is important to use at most one action per group,
    // this is ideally enforced by the client app parsing
    // the user-friendly config.
    for (ULONG i = 0; i < c; i++) {

        if (!MM_TYPE_CHECK(List[i],CALIBAN)) break;

        if (List[i]->id == cfg->id) {

            List[i]->entries[List[i]->count] = newitem;
            List[i]->actions &= cfg->actions;

            group_found = TRUE;
            ++List[i]->count;
            KdPrint(("Existing group found under newcount:%u, id:%u for %wZ\n", List[i]->count,List[i]->id, newitem->item));

            break;
        }
    }
    // if there is no existing group with a matching id, 
    // then allocate a new config group and 
    // assign the new item as it's first entry
    if (group_found == FALSE) {

        List[c] = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CONFIG_GROUP), CLBN);
        if (List[c] == NULL) {
            KdPrint(("Failure to allocate config_group memory!\n"));
            return STATUS_NO_MEMORY;
        }
        List[c]->id = cfg->id;
        List[c]->actions = cfg->actions;
        List[c]->entries[List[c]->count] = newitem;
        KdPrint(("New item added, listType %hu: id:%u , %p\n",listType, List[c]->id, List[c]->entries[List[c]->count]->item));

        _(List[c], CALIBAN);
        if (List[c]->count < CONFIG_GROUP_MAX) {
            ++List[c]->count;
            KdPrint(("New count:%i\n", List[c]->count));
        }

        if (c < list_max)
            ++(*C);
        
    }

    return STATUS_SUCCESS;
}

/*
Takes the Name parameter and determines if matches a
pre-set prefix that corresponds to a supported config list.
If there is a match it returns the numeric value corresponding to the list, 
else it returns 0.
*/
UINT16 resolveValueNameToList(_In_ PWCHAR Name) {
    if (Name == NULL) return 0;
    UINT16 found_list = 0;

     KV map[26] = {
          {PROCESS_BLOCK_LIST,L"CFG_PBL_"},
        {PROCESS_INCLUDE_LIST,L"CFG_PINC_"},
        {PROCESS_EXCLUDE_LIST,L"CFG_PEXC_"},
        {PROCESS_TERMINATED_INCLUDE_LIST,L"CFG_PTINC_"},
        {PROCESS_TERMINATED_EXCLUDE_LIST,L"CFG_PTEXC_"},
        {THREAD_BLOCK_LIST,L"CFG_TBL_"},
        {THREAD_INCLUDE_LIST,L"CFG_TINC_"},
        {THREAD_EXCLUDE_LIST,L"CFG_TEXC_"},
        {MODULE_BLOCK_LIST,L"CFG_MBL_"},
        {MODULE_INCLUDE_LIST,L"CFG_MINC_"},
        {MODULE_EXCLUDE_LIST,L"CFG_MEXC"},
        {OBJECT_BLOCK_LIST,L"CFG_OBL_"},
        {OBJECT_INCLUDE_LIST,L"CFG_OINC_"},
        {OBJECT_EXCLUDE_LIST,L"CFG_OEXC_"},
        {REGISTRY_BLOCK_LIST,L"CFG_RBL_"},
        {REGISTRY_INCLUDE_LIST,L"CFG_RINC_"},
        {REGISTRY_EXCLUDE_LIST,L"CFG_REXC_"},
        {FILE_CREATE_BLOCK_LIST,L"CFG_FCBL_"},
        {FILE_CREATE_INCLUDE_LIST,L"CFG_FCINC_"},
        {FILE_CREATE_EXCLUDE_LIST,L"CFG_FCEXC_"},
        {FILE_SET_INFO_BLOCK_LIST,L"CFG_FSBL_"},
        {FILE_SET_INFO_INCLUDE_LIST,L"CFG_FSINC_"},
        {FILE_SET_INFO_EXCLUDE_LIST,L"CFG_FSEXC_"},
        {WFP_BLOCK_LIST,L"CFG_WBL_"},
        {WFP_INCLUDE_LIST,L"CFG_WINC_"},
        {WFP_EXCLUDE_LIST,L"CFG_WEXC_"}
    };


    for (int i = 0; i< 26; i++) {
        if (map[i].value == NULL) break;
        size_t namelen = wcslen(Name);
        size_t prefixlen = wcslen(map[i].value);
        if (namelen < prefixlen) continue;
     //   KdPrint((">> %ls vs %ls, %u vs %u\n", Name, map[i].value, namelen, prefixlen));
        if (_wcsnicmp((PVOID)map[i].value, (PVOID)Name, prefixlen) == 0) {
          //  KdPrint((">>matched:%hu\n", map[i].key));
            return map[i].key;

        }
      //  KdPrint((">>not matched\n"));
    }
    
    return found_list;
 }

/*
This is called if there is no config list match for a registry value.
If it the registry valueName matches a supported global setting name,
the corresponding global setting is set.
*/
VOID loadGlobalSetting(PGC G, HANDLE keyHandle, PUNICODE_STRING valueName) {
    if (!VALID_PAGE(valueName)) {
        return;
    }
    ULONG requiredSize = 0;
    KEY_VALUE_PARTIAL_INFORMATION* regInfo = NULL;
    const PWCHAR validGlobalSettings[] = {
    L"KEEP_EXCLUDES",
    L"RESIST_TAMPERING",
    L"PROCESS_CALLBACK",
    L"THREAD_CALLBACK",
    L"MODULE_CALLBACK",
    L"OBJECT_CALLBACK",
    L"REGISTRY_CALLBACK",
    L"FILE_CALLBACK",
    L"WFP_CALLBACK",
    };
    int validGlobalSettingsCount = 9;
    for (int i = 0; i < validGlobalSettingsCount; i++) {
        
        if (valueName->Length < wcslen(validGlobalSettings[i])) continue; 

        if (_wcsnicmp(valueName->Buffer,validGlobalSettings[i],valueName->Length) != 0) {
            KdPrint(("GlobalSetting:%p [%wZ] != %p [%wZ]\n", &validGlobalSettings[i], &validGlobalSettings[i], valueName, valueName));
            continue;
        }
        else {
            KdPrint(("\nProcessing global setting:%wZ\n", valueName));
        }
        NTSTATUS status = ZwQueryValueKey(
            keyHandle,
            valueName,
            KeyValuePartialInformation,
            NULL,
            0,
            &requiredSize
        );

        if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL) {
            KdPrint(("GlobalSetting:ZwQueryValueKey: Required size:%u\n", requiredSize));
        }
        else if (!NT_SUCCESS(status)) {
            KdPrint(("GlobalSetting:ZwQueryValueKey (size) failed (0x%08X)\n", status));
            return;
        }
        regInfo = (KEY_VALUE_PARTIAL_INFORMATION*)ExAllocatePool2(POOL_FLAG_PAGED, requiredSize * 2, IO);
        if (regInfo == NULL) {
            KdPrint(("GlobalSetting:regInfo allocation failure\n"));
            return;
        }
        RtlZeroMemory(regInfo, requiredSize * 2);

        try {
            status = ZwQueryValueKey(
                keyHandle,
                valueName,
                KeyValuePartialInformation,
                regInfo,
                requiredSize,
                &requiredSize
            );


            if (NT_SUCCESS(status)) {
                switch (i) {
                case 0:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->KEEP_EXCLUDES = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 1:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->RESIST_TAMPERING = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 2:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->PROCESS_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 3:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->THREAD_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 4:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->MODULE_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 5:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->OBJECT_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 6:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->REGISTRY_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 7:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->FILE_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                case 8:
                    if (regInfo->Type == REG_BINARY && regInfo->DataLength >= 1) {
                        if (regInfo->Data[0] == 1) {
                            G->WFP_CALLBACK = TRUE;
                        }
                        KdPrint(("GlobalSetting:%i=%i\n", i, regInfo->Data[0]));
                    }
                    else {
                        KdPrint(("GlobalSetting:Invalid value at index:%i\n", i));
                    }
                    break;
                default:
                    KdPrint(("Critical: Found a match for a global setting which will not be processed!\n"));
                }
            }
        }
        finally {
            ExFreePoolWithTag(regInfo, IO);
        }
    }

}

/*
Determines the data type for a field.
data types determine what sort of match logic, operators and validation is done.
*/
MM_TYPE getFieldDataType(USHORT field_type) {
    switch (field_type) {
    case(MM_FILE_DELETION):
        return MM_BOOL;
    case(MM_NET_DIRECTION):
    case(MM_NET_IP_PROTOCOL):
    case(MM_NET_ADDRESS_TYPE):
    case(MM_NET_PROMISCUOUS):
        return MM_UINT8;
    case(MM_WFP_LAYER):
    case(MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR):
    case(MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR):
    case(MM_NET_LOCAL_PORT):
    case(MM_NET_REMOTE_PORT):
        return MM_UINT16;
    case(REG_OP):
    case(REG_DATA_TYPE):
    case(MM_FILE_OPERATION):
    case(MM_FILE_ACCESS_FLAGS):
    case(MM_FILE_FLAGS):
    case(MODULE_PROPERTIES):
    case(CREATION_STATUS):
    case(PHOPERATION):
    case(PHDESIRED_ACCESS):
    case(PHORIGINAL_DESIRED_ACCESS):
    case(MM_FILE_NETWORK_PROTOCOL):
    case(MM_NAMED_PIPE_TYPE):
    case(MM_NAMED_PIPE_CONFIG):
    case(MM_NAMED_PIPE_STATE):
    case(MM_NAMED_PIPE_END):
    case(MM_NET_INTERFACE_TYPE):
        return MM_UINT32;
    case(MM_FILE_CREATION_TIME):
    case(MM_FILE_CHANGE_TIME):
        return MM_UINT64;
    case(MM_NET_LOCAL_IPV4_ADDR):
    case(MM_NET_REMOTE_IPV4_ADDR):
        return MM_IPV4;
    case(MM_NET_LOCAL_IPV6_ADDR):
    case(MM_NET_REMOTE_IPV6_ADDR):
        return MM_IPV6;
    case(TARGET_PARENT_IMAGE_FILE_NAME):
    case(TARGET_PARENT_COMMAND_LINE):
    case(TARGET_PWINDOW_TITLE):
    case(TARGET_PCWD):
    case(IMAGE_FILE_NAME):
    case(COMMAND_LINE):
    case(PARENT_IMAGE_FILE_NAME):
    case(PARENT_COMMAND_LINE):
    case(PCWD):
    case(PWINDOW_TITLE):
    case(LOADED_MODULE):
    case(TARGET_IMAGE_FILE_NAME):
    case(TARGET_COMMAND_LINE):
    case(REG_KEY):
    case(REG_KEY_NEW):
    case(REG_VALUE_NAME):
    case(REG_OLD_FILE_NAME):
    case(REG_NEW_FILE_NAME):
    case(MM_FILE_NAME):
    case(MM_FILE_VOLUME):
    case(MM_FILE_EXTENSION):
    case(MM_FILE_SHARE):
    case(MM_FILE_STREAM):
    case(MM_FILE_FINAL_COMPONENT):
    case(MM_FILE_PARENT_DIR):
    case(MM_FILE_NEW_NAME):
        return MM_UNICODE;

    case(MATCH_ID):
    case(LOG_FLAGS):
    case(EVENT_TYPE):
    case(EVENT_END):
        return MM_META;
    case(SID_DATA):
        return MM_ENUM;

    default:
        return MM_BAD_TYPE;
    }
}

/*
Determines the data type of the field to allocate memory as needed and copy data into the 
Config item (CFI) newitem's members.
*/
NTSTATUS makeNewItem(_In_ CFI* newitem,_In_ USHORT field_type,_In_ ULONG copy_size,_In_ PVOID Data) {
    UNREFERENCED_PARAMETER(field_type);
    NTSTATUS status = STATUS_SUCCESS;
    MM_TYPE data_type = getFieldDataType(field_type);
    
    newitem->item = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(UNICODE_STRING), IO);
    if (newitem->item == NULL) {
        KdPrint(("makeNewItem allocation failure!\n"));
        ExFreePoolWithTag(newitem, IO);
        

        return STATUS_NO_MEMORY;
    }
    
    newitem->item->Length = 0;
    newitem->item->MaximumLength = 0;
    if (data_type == MM_UNICODE) {
        // +2 in case null terminator isn't there later on, unicode_string isn't always null terminated and some things might assume it is
        newitem->item->Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, copy_size+1, IO);
        if (newitem->item->Buffer == NULL) {
            KdPrint(("makeNewItem allocation failure!\n"));
            ExFreePoolWithTag(newitem->item, IO);
            return STATUS_NO_MEMORY;
        }
        RtlZeroMemory(newitem->item->Buffer, copy_size + 1);
        RtlCopyMemory(newitem->item->Buffer, Data, copy_size);
        newitem->item->Length = (USHORT)copy_size;
        newitem->item->MaximumLength = newitem->item->Length;
    }
    else if (data_type == MM_IPV4) {
        if (copy_size < 8)
            return STATUS_BAD_DATA;
        RtlCopyMemory(&newitem->ipv4SubnetMask, Data, sizeof(ULONG));
        RtlCopyMemory(&newitem->ipv4Subnet, ((PUCHAR)Data)+4, sizeof(ULONG));
        KdPrint(("IPV4 Subnet[%p]: %#08x , Mask: %#08x\n", Data,newitem->ipv4Subnet, newitem->ipv4SubnetMask));
        newitem->ipv6Subnet = NULL;
    }
    else if (data_type == MM_IPV6) {
        if (copy_size < 16)
            return STATUS_BAD_DATA;
        USHORT in6_size = (sizeof(UINT8) * 16);
        RtlCopyMemory(&newitem->ipv6Prefix, &((UINT8*)Data) [16], 1);
        newitem->ipv6Subnet = ExAllocatePool2(POOL_FLAG_NON_PAGED, in6_size, IO);
        if (newitem->ipv6Subnet == NULL) {
            KdPrint(("makeNewItem allocation failure!\n"));
            ExFreePoolWithTag(newitem->item, IO);
            return STATUS_NO_MEMORY;
        }
        RtlCopyMemory(newitem->ipv6Subnet, Data, in6_size);
        KdPrint(("IPV6:%p, Data:%p , prefix:%u\n", newitem->ipv6Subnet,Data, newitem->ipv6Prefix));
    }
    else if (data_type == MM_META) {
        KdPrint(("makeNewItem:Warning, got a metadata field type:%u\n", field_type));
        return STATUS_BAD_DATA;
    }
    else if (data_type == MM_BAD_TYPE) {
        KdPrint(("makeNewItem:Warning, got bad field type:%u\n", field_type));
        return STATUS_BAD_DATA;
    }
    else {
        switch (data_type) {
        case MM_BOOL:
            if (*((PUINT8)Data))
                newitem->itemBoolean = TRUE;
            else newitem->itemBoolean = FALSE;
            break;
        case MM_INT8:
            RtlCopyMemory(&newitem->itemInt8, Data, sizeof(INT8));
            break;
        case MM_UINT8:
            RtlCopyMemory(&newitem->itemUint8, Data, sizeof(UINT8));
            break;
        case MM_UINT16:
            RtlCopyMemory(&newitem->itemUshort, Data, sizeof(USHORT));
            break;
        case MM_UINT32:
            RtlCopyMemory(&newitem->itemUlong, Data, sizeof(ULONG));
            break;
        case MM_UINT64:
            RtlCopyMemory(&newitem->itemLonglong, Data, sizeof(LONGLONG));
            break;
            
        }
    }
    return status;
}

/*
Discovers registry values under the 'Settings' sub-key of the driver's registrypath.
It then iterates through them, attempting to parse config list items as well as global settings.
*/
NTSTATUS LoadConfig(_In_ PGC G, _In_ PUNICODE_STRING RegistryPath)  {
MM_ASSERT(G,GANYMEDE);

    NTSTATUS status = STATUS_SUCCESS;
    HANDLE _keyHandle,keyHandle;
    OBJECT_ATTRIBUTES attrO,attrS;
    KEY_VALUE_PARTIAL_INFORMATION* regInfo = NULL;
    ULONG index = 0;
    ULONG sz_kv = 4096; // We don't need this much data but to avoid reallocation later on, a full page makes sense
    UNICODE_STRING Settings = RTL_CONSTANT_STRING(L"Settings");
    ULONG resultLength;
    ULONG requiredSize;
    PUNICODE_STRING valueName = NULL;

    InitializeObjectAttributes(&attrO, RegistryPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);


    status = ZwOpenKey(&_keyHandle, KEY_ALL_ACCESS, &attrO);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to open registry key (0x%08X)\n", status));
        return status;
    }
    InitializeObjectAttributes(&attrS, &Settings, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, _keyHandle, NULL);

    status = ZwOpenKey(&keyHandle, KEY_ALL_ACCESS, &attrS);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to open registry settings key (0x%08X)\n", status));
        return status;
    }
    PKEY_VALUE_BASIC_INFORMATION pValueInfo = NULL;

    while (TRUE) { // process entries under the settings key


        pValueInfo = ExAllocatePool2(POOL_FLAG_PAGED, sz_kv, IO); // o = &valueInfoBuffer;
        if (pValueInfo == NULL) {
            KdPrint(("Failed pValueInfo memory allocation!\n"));
            return STATUS_NO_MEMORY;
        }
        RtlZeroMemory(pValueInfo, sz_kv);
        KdPrint(("index:%u ; keyHandle: %x ; pValueInfo: %p ; sz_valueinfobuffer: %u, resultLength: %p\n", index, HandleToULong(keyHandle), pValueInfo, sz_kv, &resultLength));
        status = ZwEnumerateValueKey(
            keyHandle,
            index,
            KeyValueBasicInformation,
            pValueInfo,
            sz_kv,
            &resultLength
        );

        if (status == STATUS_NO_MORE_ENTRIES) {// No more values to enumerate
            status = STATUS_SUCCESS;
            ExFreePoolWithTag(pValueInfo, IO);
            break;
        }
        if (status == STATUS_BUFFER_OVERFLOW) { // The value requires more memory, reallocate and re-attempt enumeration
            KdPrint(("ZwEnumerateValueKey failed [index:%u] with Buffer overflow, resultlength:%u\n", index, resultLength));
            sz_kv = resultLength + 1;
            ExFreePoolWithTag(pValueInfo, IO);
            /*pValueInfo = ExAllocatePool2(POOL_FLAG_PAGED, sz_kv, IO);
            if (pValueInfo == NULL) {
                KdPrint(("Failed pValueInfo memory allocation 2!\n"));
                return STATUS_NO_MEMORY;
            }
            RtlZeroMemory(pValueInfo, sz_kv);*/
            continue;

        }

       /* if (status == STATUS_BUFFER_OVERFLOW) {
            KdPrint(("ZwEnumerateValueKey failed [index:%u] with Buffer too small, resultlength:%u\n", index, resultLength));
            sz_kv = resultLength + 1;
            ExFreePoolWithTag(pValueInfo, IO);
            pValueInfo = ExAllocatePool2(POOL_FLAG_PAGED, sz_kv, IO);
            if (pValueInfo == NULL) {
                KdPrint(("Failed pValueInfo memory allocation!\n"));
                return STATUS_NO_MEMORY;
            }
            RtlZeroMemory(pValueInfo, sz_kv);
            continue;

        }*/

        if (!NT_SUCCESS(status)) {
            KdPrint(("ZwEnumerateValueKey failed for index %lu (0x%x)\n", index, status));
            ExFreePoolWithTag(pValueInfo, IO);
            break;
        }


        pValueInfo->Name[pValueInfo->NameLength] = '\0';

        valueName = ExAllocatePool2(POOL_FLAG_PAGED, sizeof(UNICODE_STRING), IO);
        if (valueName == NULL) {
            KdPrint(("Failed to allocate memory for valueName\n"));
            return STATUS_NO_MEMORY;
        }
        RtlZeroMemory(valueName, sizeof(UNICODE_STRING));
        RtlInitUnicodeString(valueName, pValueInfo->Name);
        KdPrint(("Value Name: '%wZ', Type: 0x%x\n", valueName, pValueInfo->Type));
        if (resultLength > 0) {

            UINT16 found_list = resolveValueNameToList((PWCHAR)&pValueInfo->Name);

            if (found_list > 0) {
             //   KdPrint(("found_list %hu\n", found_list));
                status = ZwQueryValueKey(
                    keyHandle,
                    valueName,
                    KeyValuePartialInformation,
                    NULL,
                    0,
                    &requiredSize
                );

                if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL) {
                    KdPrint(("ZwQueryValueKey: Required size:%u\n", requiredSize));
                }
                else if (!NT_SUCCESS(status)) {
                    KdPrint(("ZwQueryValueKey (size) failed (0x%08X)\n", status));
                    index++;
                  //  ExFreePoolWithTag(valueName, IO);
                    goto LOOP_END;
                    //return status;
                }
                regInfo = (KEY_VALUE_PARTIAL_INFORMATION*)ExAllocatePool2(POOL_FLAG_PAGED, requiredSize * 2, IO);
                if (regInfo == NULL) {
                    KdPrint(("regInfo allocation failure\n"));
                    return STATUS_NO_MEMORY;
                }
                RtlZeroMemory(regInfo, requiredSize * 2);


                status = ZwQueryValueKey(
                    keyHandle,
                    valueName,
                    KeyValuePartialInformation,
                    regInfo,
                    requiredSize,
                    &requiredSize
                );


                if (NT_SUCCESS(status)) {
                    if (regInfo->Type == REG_BINARY) {
                        KdPrint(("Found reg_binary\n"));
                        CFI* newitem = (CFI*)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(CFI), CSTO);
                        if (newitem == NULL) {
                            KdPrint(("regData allocation failure!\n"));
                            ExFreePoolWithTag(regInfo, IO);
                            return STATUS_NO_MEMORY;
                        }
                        
                        CFG cfg;
                        int offset = 0;

                        RtlCopyMemory(&cfg.size, regInfo->Data + offset, sizeof(ULONG));
                        offset += sizeof(ULONG);
                        RtlCopyMemory(&cfg.type, regInfo->Data + offset, sizeof(USHORT));
                        offset += sizeof(USHORT);
                        RtlCopyMemory(&cfg.field_type, regInfo->Data + offset, sizeof(USHORT));
                        offset += sizeof(USHORT);
                        RtlCopyMemory(&cfg.match_type, regInfo->Data + offset, sizeof(char));
                        offset += sizeof(char);
                        RtlCopyMemory(&cfg.id, regInfo->Data + offset, sizeof(ULONG));
                        offset += sizeof(ULONG);
                        RtlCopyMemory(&cfg.actions, regInfo->Data + offset, sizeof(ULONG));
                        offset += sizeof(ULONG);

                       
                        /*
                        cfg.size should always be smaller.
                        But if there are bugs or vulns, always make sure the copy_size is 
                        lesser than the data in the registry, minus the standard config header.
                        This should prevent overrunning the heap allocated memory.
                        */
                        ULONG copy_size = regInfo->DataLength > cfg.size ? cfg.size : regInfo->DataLength - ((sizeof(ULONG) * 3) + (sizeof(SHORT) * 2) + sizeof(char));
                        if (copy_size <= 0) {
                            KdPrint(("reg data copy size failure!\n"));
                            ExFreePoolWithTag(newitem, IO);
                            ExFreePoolWithTag(regInfo, IO);
                            break;
                        }
                        
                       
                        
                        status = makeNewItem(newitem,cfg.field_type,copy_size, regInfo->Data + offset);
                        if (status == STATUS_BAD_DATA) {
                            ExFreePoolWithTag(newitem, IO);
                            ExFreePoolWithTag(regInfo, IO);
                            goto LOOP_END;
                        }
                        else if (!NT_SUCCESS(status)) {
                            KdPrint(("makeNewItem Failed: %#08x\n", status));
                            ExFreePoolWithTag(newitem, IO);
                            ExFreePoolWithTag(regInfo, IO);
                            goto LOOP_END;
                        }
                        
                        newitem->match_type = cfg.match_type;
                        newitem->field_type = cfg.field_type;
                        
                        _(newitem, CALLISTO);
                        status = loadConfigItem(G, found_list, &cfg, newitem);
                        if (!NT_SUCCESS(status)) {
                            KdPrint(("Failed to load config item!\n"));
                        }
                        KdPrint(("<%p> ValueName:%wZ == ValueData:[%ls] {%wZ}\n", newitem->item, valueName, newitem->item->Buffer,newitem->item));
                    }
                    else {
                        KdPrint(("LoadConfig:Not reg binary\n"));
                    }
                }
                else {
                    KdPrint(("LoadConfig:Failed querying value key %wZ\n", valueName));
                }
                ExFreePoolWithTag(regInfo, IO);
            }
            else {
                KdPrint(("Value length is %u and doesn't start with a valid list prefix, checking if it is a global setting...\n", resultLength));
                loadGlobalSetting(G,keyHandle,valueName);
            }
        }


    LOOP_END:
        index++;
        if (valueName != NULL) {
            ExFreePoolWithTag(valueName, IO);
        }
        if (index >= BLOCKLIST_MAX) break;


    }
    ZwClose(keyHandle);
    return status;
}

/*
Calls purgeConfigList to deallocate and cleanup all the 
previously configured lists and their entries.
*/
void UnloadConfig(_In_ PGC G)  {
MM_ASSERT(G,GANYMEDE);

    KdPrint(("Unloading Config\n"));
    purgeConfigList(G,PROCESS_BLOCK_LIST);
    purgeConfigList(G,PROCESS_INCLUDE_LIST);
    purgeConfigList(G,PROCESS_EXCLUDE_LIST);
    purgeConfigList(G,PROCESS_TERMINATED_INCLUDE_LIST);
    purgeConfigList(G,PROCESS_TERMINATED_EXCLUDE_LIST);
    purgeConfigList(G,THREAD_BLOCK_LIST);
    purgeConfigList(G,THREAD_INCLUDE_LIST);
    purgeConfigList(G,THREAD_EXCLUDE_LIST);
    purgeConfigList(G,MODULE_BLOCK_LIST);
    purgeConfigList(G,MODULE_INCLUDE_LIST);
    purgeConfigList(G,MODULE_EXCLUDE_LIST);
    purgeConfigList(G,OBJECT_BLOCK_LIST);
    purgeConfigList(G,OBJECT_INCLUDE_LIST);
    purgeConfigList(G,OBJECT_EXCLUDE_LIST);
    purgeConfigList(G,REGISTRY_BLOCK_LIST);
    purgeConfigList(G,REGISTRY_INCLUDE_LIST);
    purgeConfigList(G,REGISTRY_EXCLUDE_LIST);
    purgeConfigList(G,FILE_CREATE_BLOCK_LIST);
    purgeConfigList(G,FILE_CREATE_INCLUDE_LIST);
    purgeConfigList(G,FILE_CREATE_EXCLUDE_LIST);
    purgeConfigList(G,FILE_SET_INFO_BLOCK_LIST);
    purgeConfigList(G,FILE_SET_INFO_INCLUDE_LIST);
    purgeConfigList(G,FILE_SET_INFO_EXCLUDE_LIST);
    purgeConfigList(G,WFP_BLOCK_LIST);
    purgeConfigList(G,WFP_INCLUDE_LIST);
    purgeConfigList(G, WFP_EXCLUDE_LIST);

    ExFreePool(G);
    KdPrint(("Unloaded Config\n"));
}
