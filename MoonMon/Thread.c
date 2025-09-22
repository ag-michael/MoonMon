#include "Thread.h"

/*
Uses the LIST_TYPE parameter to determine which thred creation list to use,
and searches the config group list found for entries that match the corresponding
values of the PTC_INFO value (TI)'s members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
*/
BOOLEAN ThreadCreationListMatch(_In_ GC* globals, _In_ PTC_INFO TI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT,_Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(TI, IAPETUS);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case THREAD_BLOCK_LIST:
		PL = globals->TBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ThreadCreationListMatch: Invalid list TBL\n"));
			return FALSE;
		}
		break;
	case THREAD_INCLUDE_LIST:
		PL = globals->TINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ThreadCreationListMatch: Invalid list TINC\n"));
			return FALSE;
		}
		break;
	case THREAD_EXCLUDE_LIST:
		PL = globals->TEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ThreadCreationListMatch: Invalid list TEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("ThreadCreationListMatch: list type!\n"));
		return FALSE;
	}
	
	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("ThreadCreationListMatch: Invalid list entry\n"));
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
					if (MM_TYPE_CHECK(TI->PI, PHOEBE))
					CheckUnicodeEntry(entry, TI->PI->ImageFileName, &matches);
					//KdPrint(("+IMAGE_FILE_NAME:%u\n", matches));
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(TI->PI, PHOEBE))
					CheckUnicodeEntry(entry, TI->PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(TI->PI, PHOEBE))
					CheckUnicodeEntry(entry, TI->PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(TI->PI, PHOEBE))
					CheckUnicodeEntry(entry, TI->PI->ParentCommandLine, &matches);
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(TI->PI, PHOEBE))
					CheckUnicodeEntry(entry, TI->PI->WindowTitle, &matches);
					break;
				case PCWD:
					if (MM_TYPE_CHECK(TI->PI, PHOEBE))
					CheckUnicodeEntry(entry, TI->PI->CWD, &matches);
					break;
				case TARGET_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(TI->TPI, PHOEBE))
					CheckUnicodeEntry(entry, TI->TPI->ImageFileName, &matches);
					//KdPrint(("+TARGET_IMAGE_FILE_NAME:%u\n", matches));
					break;
				case TARGET_PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(TI->TPI, PHOEBE))
					CheckUnicodeEntry(entry, TI->TPI->ParentImageFileName, &matches);
					break;
				case TARGET_COMMAND_LINE:
					if (MM_TYPE_CHECK(TI->TPI, PHOEBE))
					CheckUnicodeEntry(entry, TI->TPI->CommandLine, &matches);
					break;
				case TARGET_PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(TI->TPI, PHOEBE))
					CheckUnicodeEntry(entry, TI->TPI->ParentCommandLine, &matches);
					break;
				case TARGET_PWINDOW_TITLE:
					if (MM_TYPE_CHECK(TI->TPI, PHOEBE))
					CheckUnicodeEntry(entry, TI->TPI->WindowTitle, &matches);
					break;
				case TARGET_PCWD:
					if (MM_TYPE_CHECK(TI->TPI, PHOEBE))
					CheckUnicodeEntry(entry, TI->TPI->CWD, &matches);
					break;
				default:
					KdPrint(("ThreadCreationListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
			//	KdPrint(("ThreadCreationListMatch:Match %u, %u vs %u\n", PL[i]->id, PL[i]->count, matches));
				break;
			}
			else {
				//   KdPrint(("ThreadCreationListMatch:No matches %u, %u vs %u\n",PL[i]->id, PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("ThreadCreationListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Inspects the thread information (TI) struct's members.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
*/
void InspectThreadCreation(_In_ PGC globals, _In_ PTC_INFO TI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;

	if (globals->PTINC_COUNT > 0) {
		if (ThreadCreationListMatch(globals, TI, THREAD_INCLUDE_LIST, globals->PTINC_COUNT,&id,&actions) == TRUE) {
				*match_id = id;
		
			*skip_logging = FALSE;
			return; // if there is an include list, exclude lists won't be processed
		}
		
	}
	if (*skip_logging == FALSE && globals->PTEXC_COUNT > 0) {
		if (ThreadCreationListMatch(globals, TI, THREAD_EXCLUDE_LIST, globals->PTEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}

	}
}