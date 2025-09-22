
#include "Process.h"

/*
Terminates a process and returns success status
*/
BOOLEAN TerminateProcess(_In_ PEPROCESS p) {
	HANDLE pid;
	BOOLEAN success = FALSE;
	//KIRQL oldirql = KeRaiseIrqlToDpcLevel();
	//try {
		NTSTATUS pstatus = ObOpenObjectByPointer(p,
			OBJ_KERNEL_HANDLE,
			NULL,
			PROCESS_TERMINATE,
			*PsProcessType,
			KernelMode,
			&pid);
		if (NT_SUCCESS(pstatus)) {
			try {
				ZwTerminateProcess(pid, 0);
				KdPrint(("\nKilled: %u\n", HandleToULong(pid)));
			}
			finally {
				ZwClose(pid);
				ObDereferenceObject(p);
			}
			success = TRUE;
		}
	/* }
	finally {
		KeLowerIrql(oldirql);
	}*/
	return success;
}
/*
Sleep forever
*/
void NapTime() {
	for (;;) {
		LARGE_INTEGER delay = { 0 };

		delay.QuadPart = -10000000000;
		KeDelayExecutionThread(KernelMode, FALSE, &delay);
		
	}
}

/*
Adds data from the process information (PI) structure to the global process cache.
*/
void AddNewProcess(_In_ GC* globals, _In_ HANDLE p, _In_ PPROC_INFO PI) {
	MM_ASSERT(globals, GANYMEDE);
	if (!MM_TYPE_CHECK(PI, PHOEBE)) {
		KdPrint(("Critical:PROC_INFO type check failure at AddNewProcess\n"));
		return;
	}
	KIRQL oldirql = { 0 };
	try {
		do {
			PPCACHE PC = NULL;
			BOOLEAN cachehit = FALSE;
			oldirql = KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)&globals->PCACHE_LOCK);
			try {

				for (int i = 0; i < NEW_PROCS_MAX; i++) {
					if (globals->NEW_PROCS[i].p == p) {
						cachehit = TRUE;
						break;
					}
				}
				if (cachehit == FALSE) {
					do {
						if (globals->NEW_PROCS_INDEX >= NEW_PROCS_MAX) {
							globals->NEW_PROCS_INDEX = 0;
						}
						else {
							++globals->NEW_PROCS_INDEX;

						}
						PC = &globals->NEW_PROCS[globals->NEW_PROCS_INDEX];
						if (PC->InUse == TRUE) {
							continue;
						}
						PC->_ = 0;
					} while (PC == NULL);
				}
			}
			__finally {
				KeReleaseSpinLock((PKSPIN_LOCK)&globals->PCACHE_LOCK, oldirql);
			}
			if (cachehit == TRUE || PC == NULL) break;
			if(PC->PI != NULL)
			processInfoDelete(PC->PI);

			PC->PI = NULL;
			PC->PI = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(PROC_INFO), PHBE);
			if (PC->PI == NULL) {
				KdPrint(("AddNewProcess PI allocation failure!\n"));
				break;
			}
			RtlCopyMemory(PC->PI, PI,sizeof(PROC_INFO));

			if (MM_TYPE_CHECK(PI->UI, MNEME)) {
				PC->PI->UI = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(UINF), MNME);
				if (PC->PI->UI == NULL) {
					KdPrint(("AddNewProcess PI->UI allocation failure!\n"));
					break;
				}
				RtlCopyMemory(PC->PI->UI, PI->UI, sizeof(UINF));
			}
			PC->p = p;
			PC->New = TRUE;
			PC->InUse = FALSE;
			_(PC, CHARON);
			KdPrint(("Added new process %u\n", HandleToULong(PC->p)));
			break;
		} while (TRUE);
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled AddNewProcess exception!\n"));
	}
}

/*
If the handle 'p' is found matching for a process in the global process cache,
the cache entry is removed.
*/
void PurgeCachedProcess(_In_ GC* globals, _In_ HANDLE p) {
	MM_ASSERT(globals, GANYMEDE);
	KIRQL oldirql = { 0 };
	try {
		do {
			oldirql = KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)&globals->PCACHE_LOCK);
			try {

				for (int i = 0; i < NEW_PROCS_MAX; i++) {
					PPCACHE PC = &globals->NEW_PROCS[i];
					if (!MM_TYPE_CHECK(PC, CHARON)) continue;
					if (p == PC->p && PC->InUse == FALSE) {
						PC->New = FALSE;
						if (PC->PI != NULL) {
							processInfoDelete(PC->PI);
							ExFreePoolWithTag(PC->PI, PHBE);
							PC->PI = NULL;
						}
						PC->p = NULL;
						PC->_ = 0;
						KdPrint(("Removed new process %u\n", HandleToULong(p)));
						break;
					}
				}
			}
			__finally {
				KeReleaseSpinLock((PKSPIN_LOCK)&globals->PCACHE_LOCK, oldirql);
			}
			break;
		} while (TRUE);
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled PurgeCachedProcess exception!\n"));
	}
}

/*
This is called by the remote thread callback.
Once the first thread is created, this function updates the associated entry
in the global process chache to indicate that it is no longer a "new" process.
This allows the remote thread callback to better detect remote thread creations.
*/
void RemoveNewProcessStatus(_In_ GC* globals, _In_ HANDLE p) {
	MM_ASSERT(globals, GANYMEDE);
	KIRQL oldirql = { 0 };
	try {
		do {
			oldirql = KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)&globals->PCACHE_LOCK);
			try {

				for (int i = 0; i < NEW_PROCS_MAX; i++) {
					PPCACHE PC = &globals->NEW_PROCS[i];
					if (MM_TYPE_CHECK(PC, CHARON) && p == PC->p) {
						PC->New = FALSE;
						break;
					}
				}
			}
			__finally {
				KeReleaseSpinLock((PKSPIN_LOCK)&globals->PCACHE_LOCK, oldirql);
			}
			break;
		} while (TRUE);
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled RemoveNewProcess exception!\n"));
	}
}

/*
Find a matching entry for the process handle 'p' in the
global process cache and return that matching entry
*/
PPCACHE GetCachedProcess(_In_ GC* globals, _In_ HANDLE p) {
	MM_ASSERT(globals, GANYMEDE);
	PPCACHE cached = NULL;
	KIRQL oldirql = { 0 };
	try {
		do {

			oldirql = KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)&globals->PCACHE_LOCK);
			try {
				for (int i = 0; i < NEW_PROCS_MAX; i++) {
					PPCACHE PC = &globals->NEW_PROCS[i];
					if (MM_TYPE_CHECK(PC, CHARON) && p == PC->p) {
						cached = PC;
						//   KdPrint(("Cache Hit:%u/%p\n", p,cached));
						break;
					}
				}
			}
			__finally {

				KeReleaseSpinLock((PKSPIN_LOCK)&globals->PCACHE_LOCK, oldirql);
			}
			break;
		} while (TRUE);
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled GetCachedProcess exception!\n"));
	}
	return cached;
}

/*
Checks the global process cache and returns TRUE if it is
a "new" process, which means it is still in the creation stage.
*/
BOOLEAN IsNewProcess(_In_ GC* globals, _In_ HANDLE p) {
	MM_ASSERT(globals, GANYMEDE);
	if (p == NULL) return FALSE;

	for (int i = 0; i < NEW_PROCS_MAX; i++) {
		PPCACHE PC = &globals->NEW_PROCS[i];
		if (MM_TYPE_CHECK(PC, CHARON) && PC->p == p && PC->New == TRUE) {
			return TRUE;
		}
	}

	return FALSE;
}


/*
Cleans up and deallocates the members of the process information (PI) structure.
*/
void processInfoDelete(PPROC_INFO PI) {
	if (PI == NULL) return;
	PI->_ = 0;
	DeallocateAndDestroyUnicodeString(&PI->ImageFileName,PHBE);
	DeallocateAndDestroyUnicodeString(&PI->ParentImageFileName, PHBE);
	DeallocateAndDestroyUnicodeString(&PI->CommandLine, PHBE);
	DeallocateAndDestroyUnicodeString(&PI->ParentCommandLine, PHBE);
	DeallocateAndDestroyUnicodeString(&PI->WindowTitle, PHBE);
	DeallocateAndDestroyUnicodeString(&PI->CWD, PHBE);

	if (MM_TYPE_CHECK(PI->UI, MNEME)) {
		destroySidInfo(PI->UI);
	}
	
}
/*
Looks up the PEB values of the process associated with the parameter 'p'.
Copies over relevant values into the PI parameter (process info) struct's members.
The PI parameter should already be initialized when this function is called.

Attaching to the process is required to read its PEB. 

The return value indicates if the initalization succeded or not.
The pInfo parameter should be passed from the process notify's callback's corresponding parameter as-is.
The parent parameter indicates if the PEPROCESS parameter 'p' is the parent process in this context, and thus
fields like ParentImageFileName should be set in lieu of ImageFileName.

*/
BOOLEAN processInfoInit( _In_ PEPROCESS p,_In_ PPROC_INFO PI, _In_opt_  PPS_CREATE_NOTIFY_INFO pInfo,  BOOLEAN parent) {
	BOOLEAN success=FALSE;

	if (p == NULL) {
		KdPrint(("Warning, processInfoInit received a null PEPROCESS pointer!\n"));
		return FALSE;
	}
	RtlZeroMemory(PI, sizeof(PROC_INFO));
	try {
		if (VALID_PAGE(pInfo)) {
			{
				/*
				   It would be faster to just point PI's members to pInfo
				   But this is less risky and uncomplicates freeing the struct later on.
				   processInfoInit is called by callbacks that need to return to the caller as quickly as possible.
				   Copying around things is slow, but blocking until the process data isn't needed is much slower.
				   The process creation could be blocked, or the process could terminate by the time the data
				   for that process is needed.
				  */
				AllocateAndCopyUnicodeString((PUNICODE_STRING) pInfo->ImageFileName, &PI->ImageFileName, PHBE);
				AllocateAndCopyUnicodeString((PUNICODE_STRING) pInfo->CommandLine, &PI->CommandLine, PHBE);
			}
		}
		KAPC_STATE apc_state;
		KeStackAttachProcess(p, &apc_state);
		try {
			do {
				PPEB Peb = PsGetProcessPeb(p);
				UINT_PTR processParams = 0;
				if (Peb != NULL) {
					try{
					 processParams = *((UINT_PTR*)((char*)Peb + OFFSET_PROCESS_PARAMS));
					}
					__except (EXCEPTION_EXECUTE_HANDLER) {
						
						//KdPrint(("processInfoInit exception! bad PEB address:(%#08X) %u\n",
						//	PsGetProcessExitStatus(p),
						//	HandleToULong(PsGetProcessId(p))));
						//printStackTrace();
						
						break;
					}
					if (parent == FALSE) {
						if (pInfo == NULL) {
							AllocateAndCopyUnicodeString(
								((UNICODE_STRING*)(processParams + OFFSET_IMAGE_FILE_NAME)),
								&PI->ImageFileName, PHBE);
							//KdPrint(("New ImageFileName:%wZ/%wZ\n", PI->ImageFileName, ((UNICODE_STRING*)(processParams + OFFSET_IMAGE_FILE_NAME))));
							AllocateAndCopyUnicodeString(
								((UNICODE_STRING*)(processParams + OFFSET_COMMAND_LINE)),
								&PI->CommandLine, PHBE);
						}
						 
						AllocateAndCopyUnicodeString(
							((UNICODE_STRING*)(processParams + OFFSET_WINDOW_TITLE)),
							&PI->WindowTitle, PHBE);
						AllocateAndCopyUnicodeString(
							((UNICODE_STRING*)(processParams + OFFSET_CWD)),
							&PI->CWD, PHBE);
					}
					else {
						AllocateAndCopyUnicodeString(
							((UNICODE_STRING*)(processParams + OFFSET_IMAGE_FILE_NAME)),
							&PI->ParentImageFileName,PHBE);

						AllocateAndCopyUnicodeString(
							((UNICODE_STRING*)(processParams + OFFSET_COMMAND_LINE)),
							&PI->ParentCommandLine, PHBE);

						AllocateAndCopyUnicodeString(
							((UNICODE_STRING*)(processParams + OFFSET_WINDOW_TITLE)),
						   &PI->WindowTitle,PHBE);
						AllocateAndCopyUnicodeString(
							((UNICODE_STRING*)(processParams + OFFSET_CWD)),
							&PI->CWD,PHBE);
					}
				}
				success = TRUE;
				break;
			} while (TRUE);
		}
		__finally {
			KeUnstackDetachProcess(&apc_state);
		}
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled processInfoInit exception!\n"));
	}
	return success;
}

/*
Takes a process info parameter (PI) along with id and Timestamp, 
it then calls AddLogEntry after validating each member to add the entries to
the global log cache.

Callbacks that lookup the global log cache call this after a cache hit to
enrich and add context to other event types' logs.

The target parameter will set TARGET_ prefixed equivalent fields for when the event is 
looking up a target process, such as with process acces or remote thread creation.
*/
void PCacheLogEntries(_In_ PGC globals, _In_ LONG64 id, _In_ LONGLONG Timestamp,_In_ PPROC_INFO PI,_In_ BOOLEAN target) {
	if (!MM_TYPE_CHECK(PI, PHOEBE)) return;
	if (target == FALSE) {
		if (VALID_PAGE(PI->ImageFileName) && VALID_PAGE(PI->ImageFileName->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->ImageFileName->Length, IMAGE_FILE_NAME, PI->ImageFileName->Buffer);
		}

		if (VALID_PAGE(PI->CommandLine) && VALID_PAGE(PI->CommandLine->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->CommandLine->Length, COMMAND_LINE, PI->CommandLine->Buffer);
		}
		if (VALID_PAGE(PI->ParentImageFileName) && VALID_PAGE(PI->ParentImageFileName->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->ParentImageFileName->Length, PARENT_IMAGE_FILE_NAME, PI->ParentImageFileName->Buffer);
		}
		if (VALID_PAGE(PI->ParentCommandLine) && VALID_PAGE(PI->ParentCommandLine->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->ParentCommandLine->Length, PARENT_COMMAND_LINE, PI->ParentCommandLine->Buffer);
		}
		if (VALID_PAGE(PI->WindowTitle) && VALID_PAGE(PI->WindowTitle->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->WindowTitle->Length, PWINDOW_TITLE, PI->WindowTitle->Buffer);
		}
		if (VALID_PAGE(PI->CWD) && VALID_PAGE(PI->CWD->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->CWD->Length, PCWD, PI->CWD->Buffer);
		}
	}
	else {
		if (VALID_PAGE(PI->ImageFileName) && VALID_PAGE(PI->ImageFileName->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->ImageFileName->Length, TARGET_IMAGE_FILE_NAME, PI->ImageFileName->Buffer);
		}

		if (VALID_PAGE(PI->CommandLine) && VALID_PAGE(PI->CommandLine->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->CommandLine->Length, TARGET_COMMAND_LINE, PI->CommandLine->Buffer);
		}
		if (VALID_PAGE(PI->ParentImageFileName) && VALID_PAGE(PI->ParentImageFileName->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->ParentImageFileName->Length, TARGET_PARENT_IMAGE_FILE_NAME, PI->ParentImageFileName->Buffer);
		}
		if (VALID_PAGE(PI->ParentCommandLine) && VALID_PAGE(PI->ParentCommandLine->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->ParentCommandLine->Length, TARGET_PARENT_COMMAND_LINE, PI->ParentCommandLine->Buffer);
		}
		if (VALID_PAGE(PI->WindowTitle) && VALID_PAGE(PI->WindowTitle->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->WindowTitle->Length, TARGET_PWINDOW_TITLE, PI->WindowTitle->Buffer);
		}
		if (VALID_PAGE(PI->CWD) && VALID_PAGE(PI->CWD->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->CWD->Length, TARGET_PCWD, PI->CWD->Buffer);
		}
	}

	if (MM_TYPE_CHECK(PI->UI, MNEME)) {
		if (VALID_PAGE(PI->UI->sid_data) && PI->UI->sid_data_length > 0) {
			AddLogEntry(globals, id, Timestamp, (SIZE_T) PI->UI->sid_data_length, SID_DATA, (PVOID)PI->UI->sid_data);
		}
		if (VALID_PAGE(PI->UI->domain) && VALID_PAGE(PI->UI->domain->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->UI->domain->Length, ACCOUNT_DOMAIN, PI->UI->domain->Buffer);
		}
		if (VALID_PAGE(PI->UI->name) && VALID_PAGE(PI->UI->name->Buffer)) {
			AddLogEntry(globals, id, Timestamp, PI->UI->name->Length, ACCOUNT_NAME, PI->UI->name->Buffer);
		}
	}
}

/*
Uses the LIST_TYPE parameter to determine which process list to use,
and searches the config group list found for entries that match the corresponding
values of the PPROC_INFO (PI) parameter's members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
*/
BOOLEAN ProcessListMatch(_In_ GC* globals, _In_ PPROC_INFO PI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(PI, PHOEBE);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case PROCESS_BLOCK_LIST:
		PL = globals->PBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ProcessListMatch: Invalid list PBL\n"));
			return FALSE;
		}
		break;
	case PROCESS_INCLUDE_LIST:
		PL = globals->PINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ProcessListMatch: Invalid list PINC\n"));
			return FALSE;
		}
		break;
	case PROCESS_EXCLUDE_LIST:
		PL = globals->PEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ProcessListMatch: Invalid list PEXC\n"));
			return FALSE;
		}
		break;
	case PROCESS_TERMINATED_INCLUDE_LIST:
		PL = globals->PTINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ProcessListMatch: Invalid list PTINC\n"));
			return FALSE;
		}
		break;
	case PROCESS_TERMINATED_EXCLUDE_LIST:
		PL = globals->PTEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ProcessListMatch: Invalid list PTEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("ProcessListMatch:invalid list type!\n"));
		return FALSE;
	}
	//   KdPrint(("[P] %u items in PL\n", PL_COUNT));
	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("ProcessListMatch: Invalid list entry\n"));
			break;
		}
		matches = 0;
		//  KdPrint(("PL %u / %p \n", i,PL[i]));
	   //   KdPrint(("%u entries\n", PL[i]->count));
		try {
			ULONG j = 0;
			do {
				CFI* entry = PL[i]->entries[j];
				//  KdPrint(("[i:%u|j:%u] PL %u has %u entries. entry: %p\n", i,j, i, PL[i]->count,entry));
				switch (entry->field_type) {
				case IMAGE_FILE_NAME:

					//KdPrint(("I:%wZ\n", PI->ImageFileName));
					//KdPrint(("=>%i\n", matches));
					CheckUnicodeEntry(entry, PI->ImageFileName, &matches);
					//KdPrint(("->%i\n", matches));
					break;
				case PARENT_IMAGE_FILE_NAME:
					CheckUnicodeEntry(entry, PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					CheckUnicodeEntry(entry, PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					//KdPrint(("II:%wZ\n", PI->ParentCommandLine));
				//	KdPrint(("=>%i\n", matches));
					CheckUnicodeEntry(entry, PI->ParentCommandLine, &matches);
					//KdPrint(("->%i\n", matches));
					break;
				case PWINDOW_TITLE:
					CheckUnicodeEntry(entry, PI->WindowTitle, &matches);
					break;
				case PCWD:
					//KdPrint(("III:%wZ\n", PI->CWD));
					//KdPrint(("=>%i\n", matches));
					CheckUnicodeEntry(entry, PI->CWD, &matches);
					//KdPrint(("->%i\n", matches));
					break;
					/*case ACCOUNT_NAME:
						if (MM_TYPE_CHECK(PI->UI, MNEME) && VALID_PAGE(PI->UI->name)) {
							checkUnicodeEntry(entry, PI->UI->name, &matches);
						}
						break;
					case ACCOUNT_DOMAIN:
						if (MM_TYPE_CHECK(PI->UI, MNEME) && VALID_PAGE(PI->UI->domain)) {
							checkUnicodeEntry(entry, PI->UI->domain, &matches);
						}
						break;*/
				default:
					KdPrint(("ProcessListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
				//KdPrint(("Matched %i, %#08x\n", matches, *actions));
				break;
			}
			else {
				// KdPrint(("No matches %u vs %u\n", PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("ProcessListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}
/*
Inspects the process information (PI) struct's members.
If the corresponding block list has a hit, an access denied status is returned, unless
the matching block rule has an action value set, in which case that value is returned as the block status code.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
*/
NTSTATUS InspectProcessCreation(_In_ PGC globals, _In_ PPROC_INFO PI, _Out_ PBOOLEAN skip_logging,_Out_ PULONG match_id) {
	NT_ASSERT(match_id != NULL);

	NTSTATUS status = STATUS_SUCCESS;
	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;
	if (globals->PBL_COUNT > 0) {
		if (ProcessListMatch(globals, PI, PROCESS_BLOCK_LIST, globals->PBL_COUNT, &id, &actions) == TRUE) {
			*match_id = id;
			if (actions == 0) {
				status = STATUS_ACCESS_DENIED;
			}
			else {
				status = (NTSTATUS)actions;
			}
			KdPrint(("\nProcess Creation Blocked [%#08x]: %wZ \n", status, PI->ImageFileName));
		}
		else {
		//	KdPrint(("PBL NOT matched\n"));
		}
	}
	// Check the Include list
	if (globals->PINC_COUNT > 0) {
		if (ProcessListMatch(globals, PI, PROCESS_INCLUDE_LIST, globals->PINC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = FALSE;
			if(*match_id == 0)
			*match_id = id;
			
		}
		else {
			*skip_logging = FALSE;
			if (*match_id == 0)
			*match_id = id;
		}
	}
	// Check the exclude list but only if the include list check didn't match
	if (*skip_logging == FALSE && globals->PEXC_COUNT > 0) {
		if (ProcessListMatch(globals, PI, PROCESS_EXCLUDE_LIST, globals->PEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}
	}

	return status;
}
/*
Inspects the process information (PI) struct's members.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
*/
void InspectProcessTermination(_In_ PGC globals, _In_ PPROC_INFO PI, _Out_ PBOOLEAN skip_logging,_Out_ PULONG match_id) {
	NT_ASSERT(match_id != NULL);
	
	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;
	
	if (globals->PTINC_COUNT > 0) {
		if (ProcessListMatch(globals, PI, PROCESS_TERMINATED_INCLUDE_LIST, globals->PTINC_COUNT, &id, &actions) == TRUE) {
			if (*match_id == 0)
				*match_id = id;
			*skip_logging = FALSE;
			return; // if there is an include list, exclude lists won't be processed
		}
		
	}
	if (*skip_logging == FALSE && globals->PTEXC_COUNT > 0) {
		if (ProcessListMatch(globals, PI, PROCESS_TERMINATED_EXCLUDE_LIST, globals->PTEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}

	}
}
/*

PVOID newProcCache(
	__in struct _RTL_AVL_TABLE* Table,
	__in CLONG  ByteSize
) {
	PPCACHE PC = NULL;
	//KdPrint(("newProcCache\n"));

	PC = ExAllocatePool2(POOL_FLAG_NON_PAGED, ByteSize, CHRN);
	if (PC == NULL) {
		KdPrint(("newProcCache PC allocation failure!\n"));
		return NULL;
	}


	PC->PI = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(PROC_INFO), PHBE);
	if (PC->PI == NULL) {
		KdPrint(("newProcCache PI allocation failure!\n"));
		return NULL;
	}

	PC->PI->UI = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(UINF), MNME);
	if (PC->PI->UI == NULL) {
		KdPrint(("newProcCache PI->UI allocation failure!\n"));
		return NULL;
	}
		return (PVOID)PC;
}

VOID freeProcCache(
	__in struct _RTL_AVL_TABLE* Table,
	__in PVOID  Buffer
) {
	//KdPrint(("freeProcCache\n"));
	PPCACHE PC = (PPCACHE)Buffer;

	if (MM_TYPE_CHECK(PC, CHARON)) {
		if (MM_TYPE_CHECK(PC->PI, PHOEBE)) {
			if (MM_TYPE_CHECK(PC->PI->UI, MNEME)) {
				ExFreePoolWithTag(PC->PI->UI, MNME);
				PC->PI->UI = NULL;
			}
			ExFreePoolWithTag(PC->PI, PHBE);
			PC->PI = NULL;
		}
		ExFreePoolWithTag(PC,CHRN);
		PC = NULL;
	}
}

RTL_GENERIC_COMPARE_RESULTS
compareProcCache(
	__in struct _RTL_AVL_TABLE* Table,
	__in PVOID  FirstStruct,
	__in PVOID  SecondStruct
) {
	PPCACHE first = (PPCACHE)FirstStruct;
	PPCACHE second = (PPCACHE)SecondStruct;
	if (!MM_TYPE_CHECK(first, CHARON) || !MM_TYPE_CHECK(second, CHARON)) {
		return GenericLessThan; // There is no option to return an error/invalid value, just picking this at random
	}
	ULONG f = HandleToULong(first->p);
	ULONG s = HandleToULong(second->p);

	if (f < s) {
		return GenericLessThan;
	}
	if (f > s) {
		return GenericGreaterThan;
	}
	if (f == s) {
		return GenericEqual;
	}

	return GenericLessThan;
}

void AddNewProcess(_In_ GC* globals, _In_ HANDLE p, _In_ PPROC_INFO PI) {
	//KdPrint(("AddNewProcess\n"));
	MM_ASSERT(globals, GANYMEDE);
	NT_ASSERT(VALID_PAGE(globals->T_PROC));
	if (!MM_TYPE_CHECK(PI, PHOEBE)) {
		KdPrint(("Critical:PROC_INFO type check failure at AddNewProcess\n"));
		return;
	}
	KIRQL oldirql = { 0 };
	PPCACHE PC = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(PCACHE), CHRN);
	if (PC == NULL) {
		KdPrint(("AddNewProcess PC allocation failure!\n"));
		return;
	}
	memset(PC, 0x43, sizeof(PCACHE));
	try {

		
		PC->p = p;
		PC->New = TRUE;
		PC->InUse = FALSE;
		PC->PI = PI;
		_(PC, CHARON);
		AcquireLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, &oldirql);
		try {
			BOOLEAN inserted = FALSE;
			PPCACHE NEW = (PPCACHE)RtlInsertElementGenericTableAvl(
				globals->T_PROC,
				(PVOID)PC,
				sizeof(PCACHE),
				&inserted
			);
			if (!inserted) {
				KdPrint(("%u not added to cache.\n"));
			}
			else if (!MM_TYPE_CHECK(NEW, CHARON)) {
				KdPrint(("AddNewProcess, RtlInsertElementGenericTableAvl insertion failure\n"));

			}
			else {
				KdPrint(("Added new process %u\n", HandleToULong(PC->p)));
			}
		}
		__finally {
			ReleaseLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, oldirql);
		}

	}
	except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled AddNewProcess exception!\n"));
	}
}

void PurgeCachedProcess(_In_ GC* globals, _In_ HANDLE p) {
	//KdPrint(("PurgeCachedProcess\n"));
	MM_ASSERT(globals, GANYMEDE);
	KIRQL oldirql = { 0 };
	try {
		
			AcquireLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, &oldirql);
			try {
				do {
					PCACHE compare = { 0 };
					compare.p = p;
					PPCACHE COMPARE = &compare;
					COMPARE->InUse = TRUE;
					_(COMPARE, CHARON);
					PPCACHE HIT = RtlLookupElementGenericTableAvl(globals->T_PROC, (PVOID)COMPARE);
					if (!MM_TYPE_CHECK(HIT, CHARON)) {
						KdPrint(("Can't purge %u, not found in table.\n", HandleToULong(p)));
						break;
					}
					if (HIT->InUse) {
						KdPrint(("Cache item in-use,not deleting it:%u\n", HandleToULong(p)));
						break;
					}
					KdPrint(("PurgeCachedProcess PI:%u %p\n", HandleToULong(p), HIT->PI));
					BOOLEAN deleted = RtlDeleteElementGenericTableAvl(globals->T_PROC, (PVOID)COMPARE);
					if (deleted) {
						KdPrint(("Deleted %u.\n", HandleToULong(p)));
					}
					else {
						KdPrint(("Failed to delete %u\n", HandleToULong(p)));
					}
					break;
				} while (TRUE);
			}
			__finally {
				ReleaseLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, oldirql);
			}
			
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled PurgeCachedProcess exception!\n"));
	}
}

void RemoveNewProcessStatus(_In_ GC* globals, _In_ HANDLE p) {
	//KdPrint(("RemoveNewProcessStatus\n"));
	MM_ASSERT(globals, GANYMEDE);
	KIRQL oldirql = { 0 };
	try {
	
			AcquireLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, &oldirql);
			try {
				do {
					PCACHE compare = { 0 };
					compare.p = p;
					PPCACHE COMPARE = &compare;
					_(COMPARE, CHARON);
					PPCACHE HIT = RtlLookupElementGenericTableAvl(globals->T_PROC, (PVOID)COMPARE);
					if (!MM_TYPE_CHECK(HIT, CHARON)) {
						KdPrint(("Can't remove new status for %u, not found in table.\n", HandleToULong(p)));
						break;
					}
					HIT->New = FALSE;
					break;
				} while (TRUE);
			}
			__finally {
				ReleaseLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, oldirql);
			}
			
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		KdPrint(("Unhandled RemoveNewProcess exception!\n"));
	}
}

PPCACHE GetCachedProcess(_In_ GC* globals, _In_ HANDLE p) {
	//KdPrint(("GetCachedProcess\n"));
	MM_ASSERT(globals, GANYMEDE);
	PPCACHE cached = NULL;
	KIRQL oldirql = { 0 };
	try {
		

			AcquireLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, &oldirql);
			try {
				
					PCACHE compare = { 0 };
					compare.p = p;
					PPCACHE COMPARE = &compare;
					_(COMPARE, CHARON);
					cached = RtlLookupElementGenericTableAvl(globals->T_PROC, (PVOID)COMPARE);
					if (!MM_TYPE_CHECK(cached, CHARON)) {
					//	KdPrint(("No hit for %u, not found in table.\n", HandleToULong(p)));
						return NULL;
					}
					cached->InUse = TRUE;
				
			}
			__finally {

				ReleaseLock(globals, (PVOID)&globals->PCACHE_LOCK, MM_LOCK_SPIN_LOCK, oldirql);
			}
			
	}
	__except (MM_EXCEPTION_MODE) {
		KdPrint(("Unhandled GetCachedProcess exception!\n"));
	}
	KdPrint(("GetCachedProcess PI:%u %p\n", HandleToULong(p), cached->PI));
	return cached;
}

BOOLEAN IsNewProcess(_In_ GC* globals, _In_ HANDLE p) {
	//KdPrint(("IsNewProcess\n"));
	MM_ASSERT(globals, GANYMEDE);
	if (p == NULL) return FALSE;

	PPCACHE hit = GetCachedProcess(globals, p);
	if (!MM_TYPE_CHECK(hit,CHARON)) {
		return FALSE;
	}
	if (hit->New == TRUE) {
		return TRUE;
	}
	return FALSE;
}
*/