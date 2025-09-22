#include "Imageload.h"

/*
Uses the LIST_TYPE parameter to determine which thred creation list to use,
and searches the config group list found for entries that match the corresponding
values of the PMODULE_INFO value (MI)'s members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
*/
BOOLEAN ModuleLoadListMatch(_In_ PGC globals, _In_ PMODULE_INFO MI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(MI, MAKEMAKE);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case MODULE_BLOCK_LIST:
		PL = globals->MBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ModuleLoadListMatch: Invalid list MBL\n"));
			return FALSE;
		}
		break;
	case MODULE_INCLUDE_LIST:
		PL = globals->MINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ModuleLoadListMatch: Invalid list MINC\n"));
			return FALSE;
		}
		break;
	case MODULE_EXCLUDE_LIST:
		PL = globals->MEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("ModuleLoadListMatch: Invalid list MEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("ModuleLoadListMatch: list type!\n"));
		return FALSE;
	}

	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("ModuleLoadListMatch: Invalid list entry\n"));
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
				case LOADED_MODULE:
					CheckUnicodeEntry(entry, MI->ImageLoaded, &matches);
					//KdPrint(("ImageLoaded match??:%wZ\n", MI->ImageLoaded));
					break;
				case MODULE_PROPERTIES:
					CheckUlongEntry(entry, MI->ImageProperties, &matches);
					break;
				case IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(MI->PI, PHOEBE))
					CheckUnicodeEntry(entry, MI->PI->ImageFileName, &matches);
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(MI->PI, PHOEBE))
					CheckUnicodeEntry(entry, MI->PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(MI->PI, PHOEBE))
					CheckUnicodeEntry(entry, MI->PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(MI->PI, PHOEBE))
					CheckUnicodeEntry(entry, MI->PI->ParentCommandLine, &matches);
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(MI->PI, PHOEBE))
					CheckUnicodeEntry(entry, MI->PI->WindowTitle, &matches);
					break;
				case PCWD:
					if (MM_TYPE_CHECK(MI->PI, PHOEBE))
					CheckUnicodeEntry(entry, MI->PI->CWD, &matches);
					break;

				default:
					KdPrint(("[!] Warning, invalid field_type:%x", entry->field_type));
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
				//   KdPrint(("No matches %u vs %u\n", PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("ModuleLoadListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Inspects the module information (MI) struct's members.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule)'s id value.
*/
void InspectImageloadEvent(_In_ PGC globals, _In_ PMODULE_INFO MI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;

	if (globals->MINC_COUNT > 0) {
		if (ModuleLoadListMatch(globals, MI, MODULE_INCLUDE_LIST, globals->MINC_COUNT, &id, &actions) == TRUE) {
				*match_id = id;
			*skip_logging = FALSE;
			return; // if there is an include list, exclude lists won't be processed
		}
		
	}
	if (*skip_logging == FALSE && globals->MEXC_COUNT > 0) {
		if (ModuleLoadListMatch(globals, MI, MODULE_EXCLUDE_LIST, globals->MEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}

	}
}
