#include "Registry.h"

/*
Uses the LIST_TYPE parameter to determine which process list to use,
and searches the config group list found for entries that match the corresponding
values of the PREG_INFO (RI) parameter's members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
*/
BOOLEAN RegistryEventListMatch(_In_ GC* globals, _In_ PREG_INFO RI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(RI, UMBRIEL);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case REGISTRY_BLOCK_LIST:
		PL = globals->RBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("RegistryEventListMatch: Invalid list RBL\n"));
			return FALSE;
		}
		break;
	case REGISTRY_INCLUDE_LIST:
		PL = globals->RINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("RegistryEventListMatch: Invalid list RINC\n"));
			return FALSE;
		}
		break;
	case REGISTRY_EXCLUDE_LIST:
		PL = globals->REXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("RegistryEventListMatch: Invalid list REXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("RegistryEventListMatch: list type!\n"));
		return FALSE;
	}

	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("RegistryEventListMatch: Invalid list entry\n"));
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
				case REG_OP:
					CheckUlongEntry(entry, RI->notifyClass, &matches);
					break;
				case REG_DATA_TYPE:
					CheckUlongEntry(entry, RI->DataType, &matches);
					break;
				case REG_KEY:
					CheckUnicodeEntry(entry, RI->RegistryKey, &matches);
					break;
				case REG_VALUE_NAME:
					CheckUnicodeEntry(entry, RI->RegistryValueName, &matches);
					break;
				case REG_KEY_NEW:
					CheckUnicodeEntry(entry, RI->RegistryKeyNew, &matches);
					break;
				case REG_OLD_FILE_NAME:
					CheckUnicodeEntry(entry, RI->OldFilename, &matches);
					break;
				case REG_NEW_FILE_NAME:
					CheckUnicodeEntry(entry, RI->NewFileName, &matches);
					break;
				case IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->ImageFileName, &matches);
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->ParentCommandLine, &matches);
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->WindowTitle, &matches);
					break;
				case PCWD:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->CWD, &matches);
					break;
				case TARGET_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(RI->PI, PHOEBE))
					CheckUnicodeEntry(entry, RI->PI->ImageFileName, &matches);
					break;
				case REG_DATA:
					break;
				default:
					KdPrint(("RegistryEventListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
			//	KdPrint(("RegistryEventListMatch:Found matches %u vs %u\n", PL[i]->count, matches));
				break;
			}
			else {
				  // KdPrint(("RegistryEventListMatch:No matches %u vs %u\n", PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("RegistryEventListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Inspects the registry information (RI) struct's members.
If the corresponding block list has a hit, an access denied status is returned, unless
the matching block rule has an action value set, in which case that value is returned as the block status code.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
*/
NTSTATUS InspectRegistryEvent(_In_ PGC globals, _In_ PREG_INFO RI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NTSTATUS status = STATUS_SUCCESS;
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;

	if (globals->RBL_COUNT > 0) {
		if (RegistryEventListMatch(globals, RI, REGISTRY_BLOCK_LIST, globals->RBL_COUNT, &id, &actions) == TRUE) {
			*match_id = id;
			if (actions == 0) {
				status = STATUS_ACCESS_DENIED;
			}
			else {
				status = (NTSTATUS)actions;

			}
		}
	}
	if (globals->RINC_COUNT > 0) {
		if (RegistryEventListMatch(globals, RI, REGISTRY_INCLUDE_LIST, globals->RINC_COUNT, &id, &actions) == TRUE) {
			if(*match_id == 0)
			*match_id = id;
			*skip_logging = FALSE;
			return status; // if there is an include list, exclude lists won't be processed
		}
			
	}
	if (*skip_logging == FALSE && globals->REXC_COUNT > 0) {
		if (RegistryEventListMatch(globals, RI, REGISTRY_EXCLUDE_LIST, globals->REXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			if (*match_id == 0)
			*match_id = id;
		}
	}

	return status;
}