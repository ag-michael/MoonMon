#include "Object.h"

/*
Uses the LIST_TYPE parameter to determine which process list to use,
and searches the config group list found for entries that match the corresponding
values of the POBPROC (OB) parameter's members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
*/
BOOLEAN ObjectAccessListMatch(_In_ GC* globals, _In_ POBPROC OB, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(OB, OBERON);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case OBJECT_BLOCK_LIST:
		PL = globals->OBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ObjectAccessListMatch: Invalid list OBL\n"));
			return FALSE;
		}
		break;
	case OBJECT_INCLUDE_LIST:
		PL = globals->OINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ObjectAccessListMatch: Invalid list OINC\n"));
			return FALSE;
		}
		break;
	case OBJECT_EXCLUDE_LIST:
		PL = globals->OEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ObjectAccessListMatch: Invalid list OEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("ObjectAccessListMatch: list type!\n"));
		return FALSE;
	}

	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("ObjectAccessListMatch: Invalid list entry\n"));
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
				case PHOPERATION:
					CheckUlongEntry(entry, OB->Operation, &matches);
					break;
				case PHORIGINAL_DESIRED_ACCESS:
					CheckUlongEntry(entry, OB->OriginalDesiredAccess, &matches);
					break;

				case IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE)) {
					//	KdPrint(("OBL: Image file name:[%wZ]\n", OB->PI->ImageFileName));
						CheckUnicodeEntry(entry, OB->PI->ImageFileName, &matches);

					}
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE))
					CheckUnicodeEntry(entry, OB->PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE))
					CheckUnicodeEntry(entry, OB->PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE))
					CheckUnicodeEntry(entry, OB->PI->ParentCommandLine, &matches);
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE))
					CheckUnicodeEntry(entry, OB->PI->WindowTitle, &matches);
					break;
				case PCWD:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE))
					CheckUnicodeEntry(entry, OB->PI->CWD, &matches);
					break;
				case TARGET_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(OB->PI, PHOEBE)) {
					//	KdPrint(("OBL Target Image file name:[%wZ]\n", OB->TPI->ImageFileName));
						CheckUnicodeEntry(entry, OB->TPI->ImageFileName, &matches);
					}
					break;
				case TARGET_PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(OB->TPI, PHOEBE))
					CheckUnicodeEntry(entry, OB->TPI->ParentImageFileName, &matches);
					break;
				case TARGET_COMMAND_LINE:
					if (MM_TYPE_CHECK(OB->TPI, PHOEBE))
					CheckUnicodeEntry(entry, OB->TPI->CommandLine, &matches);
					break;
				case TARGET_PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(OB->TPI, PHOEBE))
					CheckUnicodeEntry(entry, OB->TPI->ParentCommandLine, &matches);
					break;
				case TARGET_PWINDOW_TITLE:
					if (MM_TYPE_CHECK(OB->TPI, PHOEBE))
					CheckUnicodeEntry(entry, OB->TPI->WindowTitle, &matches);
					break;
				case TARGET_PCWD:
					if (MM_TYPE_CHECK(OB->TPI, PHOEBE))
					CheckUnicodeEntry(entry, OB->TPI->CWD, &matches);
					break;
				default:
					KdPrint(("ObjectAccessListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
				break;
			}
			else {
			//	  KdPrint(("No matches %u vs %u\n", PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("ObjectAccessListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Inspects the objetct information OB) struct's members.
If the corresponding block list has a hit, 0xffffffff is returned, unless
the matching block rule has an action value set, in which case that value is returned as the value for clearing the desired access mask.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
*/
ULONG InspectObjectAccessEvent(_In_ PGC globals, _In_ POBPROC OB, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id=0, actions=0,mask = 0;

	if (globals->OBL_COUNT > 0) {
	//	KdPrint(("OBL!\n"));
		if (ObjectAccessListMatch(globals, OB, OBJECT_BLOCK_LIST, globals->OBL_COUNT, &id, &actions) == TRUE) {
			KdPrint(("OBJECT_BLOCK_LIST match\n"));
			if (actions == 0) {
				mask = (ULONG) -1;
			}
				*match_id = id;
			
		}
	}
	if (!mask && globals->OINC_COUNT > 0) {
		if (ObjectAccessListMatch(globals, OB, OBJECT_INCLUDE_LIST, globals->OINC_COUNT, &id, &actions) == TRUE) {
			if (*match_id == 0)
				*match_id = id;
			*skip_logging = FALSE;
			return actions; // if there is an include list, exclude lists won't be processed
		}
		
	}
	if (*skip_logging == FALSE && globals->OEXC_COUNT > 0) {
		if (ObjectAccessListMatch(globals, OB, OBJECT_EXCLUDE_LIST, globals->OEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}

	}

	return mask;
}
