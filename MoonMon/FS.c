#include "FS.h"


/*
Uses the LIST_TYPE parameter to determine which File Create list to use,
and searches the config group list found for entries that match the corresponding
values of the PFILE_INFO (FI) parameter's members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
*/
BOOLEAN FileCreateEventListMatch(_In_ GC* globals, _In_ PFILE_INFO FI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(FI, DEIMOS);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case FILE_CREATE_BLOCK_LIST:
		PL = globals->FCBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("FileCreateEventListMatch: Invalid list FCBL\n"));
			return FALSE;
		}
		break;
	case FILE_CREATE_INCLUDE_LIST:
		PL = globals->FCINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("FileCreateEventListMatch: Invalid list FCINC\n"));
			return FALSE;
		}
		break;
	case FILE_CREATE_EXCLUDE_LIST:
		PL = globals->FCEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("FileCreateEventListMatch: Invalid list FCEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("FileCreateEventListMatch: list type!\n"));
		return FALSE;
	}

	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("FileCreateEventListMatch: Invalid list entry\n"));
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
				case MM_FILE_FLAGS:
					CheckUlongEntry(entry,FI->CreateOptions, &matches);
					break;
				case MM_FILE_NAME:
					//KdPrint(("Checking file_name [%wZ] against %wZ\n", FI->Name, entry->item));
					CheckUnicodeEntry(entry, FI->Name, &matches);
					break;
				case MM_FILE_VOLUME:
					CheckUnicodeEntry(entry, FI->Volume, &matches);
					break;
				case MM_FILE_SHARE:
					CheckUnicodeEntry(entry, FI->Share, &matches);
					break;
				case MM_FILE_EXTENSION:
					CheckUnicodeEntry(entry, FI->Extension, &matches);
					break;
				case MM_FILE_STREAM:
					CheckUnicodeEntry(entry, FI->Stream, &matches);
					break;
				case MM_FILE_FINAL_COMPONENT:
					CheckUnicodeEntry(entry, FI->FinalComponent, &matches);
					break;
				case MM_FILE_PARENT_DIR:
					CheckUnicodeEntry(entry, FI->ParentDir, &matches);
					break;
				case IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->ImageFileName, &matches);
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->ParentCommandLine, &matches);
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->WindowTitle, &matches);
					break;
				case PCWD:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->CWD, &matches);
					break;
				case TARGET_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry,FI->PI->ImageFileName, &matches);
					break;

				default:
					KdPrint(("FileCreateEventListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
			//	KdPrint(("FileCreateEventListMatch: Matches! %u, %u vs %u\n", PL[i]->id, PL[i]->count, matches));
				break;
			}
			else {
			//  KdPrint(("FileCreateEventListMatch: No matches %u, %u vs %u\n", PL[i]->id,PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("FileCreateEventListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Uses the LIST_TYPE parameter to determine which File Create list to use,
and searches the config group list found for entries that match the corresponding
values of the PFILE_SET_INFO (FI) parameter's members.

When all items in the config group match, it returns TRUE.
When there is a match,It will stop evaluation of additional config group items in the config group list.

The id parameter's value is set to the matching config group's id.
The actions parameter is set to the matching config group's action value.
The PL_COUNT parameter is the number of items in the config list.

*/
BOOLEAN FileSetEventListMatch(_In_ GC* globals, _In_ PFILE_SET_INFO FI, _In_ ULONG LIST_TYPE, _In_ ULONG PL_COUNT, _Out_ PULONG id, _Out_ PULONG actions) {
	MM_ASSERT(globals, GANYMEDE);
	MM_ASSERT(FI, DIONE);
	BOOLEAN matched = FALSE;
	ULONG matches = 0;
	CONFIG_GROUP** PL;
	*actions = 0; *id = 0;
	switch (LIST_TYPE) {
	case FILE_SET_INFO_BLOCK_LIST:
		PL = globals->FSBL;
		if (!VALID_PAGE(PL)) {
			KdPrint(("FileSetEventListMatch: Invalid list FSBL\n"));
			return FALSE;
		}
		break;
	case FILE_SET_INFO_INCLUDE_LIST:
		PL = globals->FSINC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("FileSetEventListMatch: Invalid list FSINC\n"));
			return FALSE;
		}
		break;
	case FILE_SET_INFO_EXCLUDE_LIST:
		PL = globals->FSEXC;
		if (!VALID_PAGE(PL)) {
			KdPrint(("FileSetEventListMatch: Invalid list FSEXC\n"));
			return FALSE;
		}
		break;

	default:
		KdPrint(("FileSetEventListMatch: list type!\n"));
		return FALSE;
	}

	for (ULONG i = 0; i < PL_COUNT; i++) {
		if (!MM_TYPE_CHECK(PL[i], CALIBAN)) {
			KdPrint(("FileSetEventListMatch: Invalid list entry\n"));
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
				case MM_FILE_OPERATION:
					CheckUlongEntry(entry, FI->Operation, &matches);
					break;
				case MM_FILE_NAME:
					CheckUnicodeEntry(entry, FI->Name, &matches);
					break;
				case MM_FILE_NEW_NAME:
					CheckUnicodeEntry(entry, FI->NewName, &matches);
					break;
				case MM_FILE_VOLUME:
					CheckUnicodeEntry(entry, FI->Volume, &matches);
					break;
				case MM_FILE_SHARE:
					CheckUnicodeEntry(entry, FI->Share, &matches);
					break;
				case MM_FILE_EXTENSION:
					CheckUnicodeEntry(entry, FI->Extension, &matches);
					break;
				case MM_FILE_STREAM:
					CheckUnicodeEntry(entry, FI->Stream, &matches);
					break;
				case MM_FILE_FINAL_COMPONENT:
					CheckUnicodeEntry(entry, FI->FinalComponent, &matches);
					break;
				case MM_FILE_PARENT_DIR:
					CheckUnicodeEntry(entry, FI->ParentDir, &matches);
					break;
				case MM_FILE_CREATION_TIME:
					CheckLonglongEntry(entry, FI->CreationTime.QuadPart, &matches);
					break;
				case MM_FILE_CHANGE_TIME:
					CheckLonglongEntry(entry, FI->ChangeTime.QuadPart, &matches);
					break;
				case MM_FILE_ACCESS_FLAGS:
					CheckUlongEntry(entry, (ULONG) FI->AccessFlags, &matches);
					break;
				case MM_FILE_NETWORK_PROTOCOL:
					CheckUlongEntry(entry, FI->Protocol, &matches);
					break;
				case MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR:
					CheckUshortEntry(entry, FI->ProtocolMajorVersion, &matches);
					break;
				case MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR:
					CheckUshortEntry(entry, FI->ProtocolMinorVersion, &matches);
					break;
				case MM_NAMED_PIPE_TYPE:
					CheckUlongEntry(entry, FI->NamedPipeType, &matches);
					break;
				case MM_NAMED_PIPE_CONFIG:
					CheckUlongEntry(entry, FI->NamedPipeConfiguration, &matches);
					break;
				case MM_NAMED_PIPE_STATE:
					CheckUlongEntry(entry, FI->NamedPipeState, &matches);
					break;
				case MM_NAMED_PIPE_END:
					CheckUlongEntry(entry, FI->NamedPipeEnd, &matches);
					break;
				case MM_FILE_DELETION:
					matches = matches == entry->itemBoolean ? matches + 1 : matches;
					break;
				case IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->ImageFileName, &matches);
					break;
				case PARENT_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->ParentImageFileName, &matches);
					break;
				case COMMAND_LINE:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->CommandLine, &matches);
					break;
				case PARENT_COMMAND_LINE:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->ParentCommandLine, &matches);
					break;
				case PWINDOW_TITLE:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->WindowTitle, &matches);
					break;
				case PCWD:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->CWD, &matches);
					break;
				case TARGET_IMAGE_FILE_NAME:
					if (MM_TYPE_CHECK(FI->PI, PHOEBE))
					CheckUnicodeEntry(entry, FI->PI->ImageFileName, &matches);
					break;

				default:
					KdPrint(("FileSetEventListMatch: Warning, invalid field_type:%x\n", entry->field_type));
					break;
				}

				++j;
			} while (j < PL[i]->count);

			if (matches >= PL[i]->count) {
				matched = TRUE;
				*id = PL[i]->id;
				*actions = PL[i]->actions;
				//KdPrint(("FileSetEventListMatch:Match %u, %u vs %u\n", PL[i]->id, PL[i]->count, matches));
				break;
			}
			else {
				//  KdPrint(("FileSetEventListMatch:No matches %u, %u vs %u\n", PL[i]->id ,PL[i]->count, matches));
			}
		}except(MM_EXCEPTION_MODE) {
			KdPrint(("FileSetEventListMatch: Unhandled exception while evaluating item.\n"));
		}
	}
	return matched;
}

/*
Inspects the PFILE_SET_INFO (FI) struct's members.
If the corresponding block list has a hit, an access denied status is returned. unless
the matching block rule has an action value set, in which case that value is returned as the block status code.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule) id value.
*/
NTSTATUS InspectFsSetEvent(_In_ PGC globals, _In_ PFILE_SET_INFO FI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NTSTATUS fltstatus = STATUS_SUCCESS;
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;

	if (globals->FSBL_COUNT > 0) {
		if (FileSetEventListMatch(globals, FI, FILE_SET_INFO_BLOCK_LIST, globals->FSBL_COUNT, &id, &actions) == TRUE) {
			*match_id = id;
			if (actions == 0) {
				fltstatus = STATUS_ACCESS_DENIED;
			}
			else {
				fltstatus = (NTSTATUS) actions;
				
			}
		}
	}
	if (globals->FSINC_COUNT > 0) {
		if (FileSetEventListMatch(globals, FI, FILE_SET_INFO_INCLUDE_LIST, globals->FSINC_COUNT, &id, &actions) == TRUE) {
			if (*match_id == 0)
				*match_id = id;
			*skip_logging = FALSE;
			return fltstatus; // if there is an include list, exclude lists won't be processed
		}
		
	}
	if (*skip_logging == FALSE && globals->FSEXC_COUNT > 0) {
		if (FileSetEventListMatch(globals, FI, FILE_SET_INFO_EXCLUDE_LIST, globals->FSEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}
	}

	return fltstatus;
}

/*
Inspects the PFILE_INFO (FI) struct's members.
If the corresponding block list has a hit, an access denied status is returned. unless
the matching block rule has an action value set, in which case that value is returned as the block status code.

If the inclusion list has a hit, skip_logging's value is set to FALSE and the exclusion list is not processed.
The skip_logging value is set to TRUE if the exclusion list has a hit.

The match_id parameter's value is set to the matching config list entry (rule) id value.
*/
NTSTATUS InspectFsCreateEvent(_In_ PGC globals, _In_ PFILE_INFO FI, _Out_ PBOOLEAN skip_logging, _Out_ PULONG match_id) {
	NTSTATUS fltstatus = STATUS_SUCCESS;
	NT_ASSERT(match_id != NULL);

	*skip_logging = FALSE;
	*match_id = 0;
	ULONG id, actions;

	if (globals->FCBL_COUNT > 0) {
		if (FileCreateEventListMatch(globals,FI, FILE_CREATE_BLOCK_LIST, globals->FCBL_COUNT, &id, &actions)== TRUE) {
			*match_id = id;
			if (actions == 0) {
				fltstatus = STATUS_ACCESS_DENIED;
			}
			else {
				fltstatus = (NTSTATUS) actions;
				
			}
		}
	}
	if (globals->FCINC_COUNT > 0) {
		if (FileCreateEventListMatch(globals, FI, FILE_CREATE_INCLUDE_LIST, globals->FCINC_COUNT, &id, &actions) == TRUE) {
			if (*match_id == 0)
				*match_id = id;

			*skip_logging = FALSE;
			return fltstatus; // if there is an include list, exclude lists won't be processed
		}
	}
		
	if (*skip_logging == FALSE && globals->FCEXC_COUNT > 0) {
		if (FileCreateEventListMatch(globals,FI, FILE_CREATE_EXCLUDE_LIST, globals->FCEXC_COUNT, &id, &actions) == TRUE) {
			*skip_logging = TRUE;
			*match_id = id;
		}
	}

	return fltstatus;
}