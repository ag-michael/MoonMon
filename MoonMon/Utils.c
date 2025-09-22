#pragma once

#include <ntifs.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntdef.h>
#include <wdm.h>

#include "DataTypes.h"
#include "Utils.h"

/*
Convenience wrapper for RNG generation.
*/
ULONG MM_Genrand(_In_ PULONG seed) {
	return  RtlRandomEx(seed);
}
/*
Interlocked/safe ID value generation
*/
LONG64 _id(_In_ PGC globals) {
	volatile LONG64 newid = _InterlockedIncrement64(&globals->id);
	if ((newid & 0xfffffffffffffff0) >= 0xfffffffffffffff0) {
		_InterlockedExchange64(&globals->id, 0);
	}

	return newid;
}

/*
Get the current timestamp in LONGLONG format.
*/
LONGLONG _ts() {
	LARGE_INTEGER _Timestamp = { 0 };
	KeQuerySystemTimePrecise(&_Timestamp);
	LONGLONG Timestamp = _Timestamp.QuadPart;
	return Timestamp;
}

/*
Allocates a UNICODE_STRING struct and copies the source UNICODE_STRING's
data into the newly allocated destination.
*/
void AllocateAndCopyUnicodeString(_In_ PUNICODE_STRING source, _In_ PUNICODE_STRING* dest, _In_ ULONG Tag) {

	if (source == NULL || source->Length <1 || source->Length > UNICODE_STRING_MAX_BYTES) {
		//KdPrint(("Invalid source %p [len:%u] for allocateAndCopyUnicodeString\n",source,source->Length));
		return;
	}

	*dest = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(UNICODE_STRING), Tag);
	if ((*dest) == NULL) {
		KdPrint(("UNICODE_STRING memory allocation failure.\n"));
		return;
	}
	(*dest)->Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, (SIZE_T)source->Length, Tag);
	if ((*dest)->Buffer == NULL) {
		KdPrint(("UNICODE_STRING->Buffer memory allocation failure.\n"));
		return;
	}
	RtlCopyMemory((*dest)->Buffer, source->Buffer, source->Length);
	(*dest)->Length = source->Length;
	(*dest)->MaximumLength = (*dest)->Length;

	return;
}

/*
Deallocates and cleans up a UNICODE_STRING struct to complement AllocateAndCopyUnicodeString
*/
void DeallocateAndDestroyUnicodeString(_Inout_ PUNICODE_STRING* U, _In_ ULONG Tag) {

	if (U == NULL || (*U) == NULL) {
		//KdPrint(("Invalid string %p for deallocateAndDestroyUnicodeString\n",(*U)));
		return;
	}

	if (VALID_PAGE((*U)->Buffer)) {
		ExFreePoolWithTag((*U)->Buffer, Tag);
		(*U)->Length = 0;
		(*U)->MaximumLength = 0;
		(*U)->Buffer = NULL;
	}
	if (VALID_PAGE((*U))) {
		ExFreePoolWithTag((*U), Tag);
		(*U) = NULL;
	}
}

/*
A wrapper for IoQueryFileDosDeviceName
*/
PUNICODE_STRING QueryDosFileName(_In_ PFILE_OBJECT FileObject) {
	POBJECT_NAME_INFORMATION obInfo = NULL;
	PUNICODE_STRING result = NULL;
	if (NT_SUCCESS(IoQueryFileDosDeviceName(FileObject, &obInfo))) {
		result = &obInfo->Name;
	}
	return result;
}
/*
void AssertLock(_In_ PGC globals, _In_ PVOID lock, _In_ KIRQL maxirql) {
	NT_ASSERT(KeGetCurrentIrql() <= maxirql);
	BOOLEAN goodlock = FALSE;
	for (int i = 0; i < globals->LOCKS_SIZE; i++) {
		if (lock == globals->LOCKS[i]) {
			goodlock = TRUE;
		}
	}

	NT_ASSERT(goodlock == TRUE && lock != NULL);
}
_Acquires_lock_(lock)
_Has_lock_kind_(_Lock_kind_mutex_)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_raises_(APC_LEVEL)
void AcquireFMutex(_In_ PFAST_MUTEX lock) {

	ExAcquireFastMutex((PFAST_MUTEX)lock);

}
_Acquires_lock_(lock)
_Has_lock_kind_(_Lock_kind_spin_lock_)
_IRQL_raises_(DISPATCH_LEVEL)
_IRQL_saves_
KIRQL AcquireSpinLock(_In_  PKSPIN_LOCK lock) {
	return KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)lock);
}
_Releases_lock_(lock)
_Has_lock_kind_(_Lock_kind_mutex_)
_IRQL_raises_(APC_LEVEL)
void ReleaseFMutex(_In_ PFAST_MUTEX lock) {
	ExReleaseFastMutex((PFAST_MUTEX)lock);
}

_Releases_lock_(lock)
_Has_lock_kind_(_Lock_kind_spin_lock_)
void ReleaseSpinLock(_In_ PKSPIN_LOCK lock, _In_ _IRQL_restores_ KIRQL oldirql) {
	KeReleaseSpinLock((PKSPIN_LOCK)lock, oldirql);
}
*/

//void AcquireLock(_In_ PGC globals, _In_  PVOID lock, _In_ UINT8 type, _Out_ _IRQL_saves_ PKIRQL oldirql) {
//	UNREFERENCED_PARAMETER(globals);
//	UNREFERENCED_PARAMETER(type);
//
//	/*
//	if (type == MM_LOCK_FAST_MUTEX) {
//		AssertLock(globals, lock, APC_LEVEL);
//		AcquireFMutex((PFAST_MUTEX)lock);
//	}
//	else
//		
//		if (type == MM_LOCK_SPIN_LOCK) {
//		*oldirql = AcquireSpinLock((PKSPIN_LOCK)lock);
//	}*/
//		*oldirql = AcquireSpinLock((PKSPIN_LOCK)lock);
//}
//
//
//void ReleaseLock(_In_ PGC globals, _In_  PVOID lock, _In_ UINT8 type, _In_ _IRQL_restores_ KIRQL oldirql) {
//	UNREFERENCED_PARAMETER(globals);
//	UNREFERENCED_PARAMETER(type);
//	/*
//	if (type == MM_LOCK_FAST_MUTEX) {
//		AssertLock(globals, lock, APC_LEVEL);
//		ReleaseFMutex((PFAST_MUTEX)lock);
//	}
//	else 
//		
//		if (type == MM_LOCK_SPIN_LOCK) {
//		ReleaseSpinLock((PKSPIN_LOCK)lock, oldirql);
//	}
//	*/
//	ReleaseSpinLock((PKSPIN_LOCK)lock, oldirql);
//}

/*
Evaluates a config list entry (PCFI) against a ULONG value
using supported match operators and increments the value of the matches parameter.
*/
void CheckUlongEntry(_In_ PCFI entry, _In_ ULONG toMatch, _In_  PULONG matches) {
	if (!MM_TYPE_CHECK(entry, CALLISTO)) return;
	switch (entry->match_type) {
	case AND:
	if (((toMatch) & entry->itemUlong) == entry->itemUlong){
		++(*matches);
	}
	break;
	case NOT_AND:
		if (((toMatch)&entry->itemUlong) != entry->itemUlong) {
			++(*matches);
		}
	case EQUALS:
		if (toMatch == entry->itemUlong) {
			++(*matches);
		}
		break;
	case NOT_EQUALS:
		if (toMatch != entry->itemUlong) {
			++(*matches);
		}
		break;
	case NOT_LESS_THAN:
	case GREATER_THAN:
		if (toMatch > entry->itemUlong) {
			++(*matches);
		}
		break;
	case GREAT_OR_EQUAL:
		if (toMatch >= entry->itemUlong) {
			++(*matches);
		}
		break;
	case NOT_GREATER_THAN:
	case LESS_THAN:
		if (toMatch < entry->itemUlong) {
			++(*matches);
		}
		break;
	case LESS_OR_EQUAL:
		if (toMatch <= entry->itemUlong) {
			++(*matches);
		}
		break;
	default:
		KdPrint(("[!] checkUlongEntry: Warning, invalid match_type:%x\n", entry->match_type));
		break;
	}
}

/*
Evaluates a config list entry (PCFI) against a UINT8 value
using supported match operators and increments the value of the matches parameter.
*/
void CheckUint8Entry(_In_ PCFI entry, _In_ UINT8 toMatch, _In_  PULONG matches) {
	if (!MM_TYPE_CHECK(entry, CALLISTO)) return;
	switch (entry->match_type) {
	case AND:
		if (((toMatch)&entry->itemUint8) == entry->itemUint8) {
			++(*matches);
		}
		break;
	case NOT_AND:
		if (((toMatch)&entry->itemUint8) != entry->itemUint8) {
			++(*matches);
		}
	case EQUALS:
		if (toMatch == entry->itemUint8) {
			++(*matches);
		}
		break;
	case NOT_EQUALS:
		if (toMatch != entry->itemUint8) {
			++(*matches);
		}
		break;
	case NOT_LESS_THAN:
	case GREATER_THAN:
		if (toMatch > entry->itemUint8) {
			++(*matches);
		}
		break;
	case GREAT_OR_EQUAL:
		if (toMatch >= entry->itemUint8) {
			++(*matches);
		}
		break;
	case NOT_GREATER_THAN:
	case LESS_THAN:
		if (toMatch < entry->itemUint8) {
			++(*matches);
		}
		break;
	case LESS_OR_EQUAL:
		if (toMatch <= entry->itemUint8) {
			++(*matches);
		}
		break;
	default:
		KdPrint(("[!] checkUint8Entry: Warning, invalid match_type:%x\n", entry->match_type));
		break;
	}
}

/*
Evaluates a config list entry (PCFI) against a INT8 value
using supported match operators and increments the value of the matches parameter.
*/
void CheckInt8Entry(_In_ PCFI entry, _In_ INT8 toMatch, _In_  PULONG matches) {
	if (!MM_TYPE_CHECK(entry, CALLISTO)) return;
	switch (entry->match_type) {
	case AND:
		if (((toMatch)&entry->itemInt8) == entry->itemInt8) {
			++(*matches);
		}
		break;
	case NOT_AND:
		if (((toMatch)&entry->itemInt8) != entry->itemInt8) {
			++(*matches);
		}
	case EQUALS:
		if (toMatch == entry->itemInt8) {
			++(*matches);
		}
		break;
	case NOT_EQUALS:
		if (toMatch != entry->itemInt8) {
			++(*matches);
		}
		break;
	case NOT_LESS_THAN:
	case GREATER_THAN:
		if (toMatch > entry->itemInt8) {
			++(*matches);
		}
		break;
	case GREAT_OR_EQUAL:
		if (toMatch >= entry->itemInt8) {
			++(*matches);
		}
		break;
	case NOT_GREATER_THAN:
	case LESS_THAN:
		if (toMatch < entry->itemInt8) {
			++(*matches);
		}
		break;
	case LESS_OR_EQUAL:
		if (toMatch <= entry->itemInt8) {
			++(*matches);
		}
		break;
	default:
		KdPrint(("[!] checkInt8Entry: Warning, invalid match_type:%x\n", entry->match_type));
		break;
	}
}

/*
Evaluates a config list entry (PCFI) against a USHORT value
using supported match operators and increments the value of the matches parameter.
*/
void CheckUshortEntry(_In_ PCFI entry, _In_ USHORT toMatch, _In_  PULONG matches) {
	if (!MM_TYPE_CHECK(entry, CALLISTO)) return;
	switch (entry->match_type) {
	case AND:
		if (((toMatch)&entry->itemUshort) == entry->itemUshort) {
			++(*matches);
		}
		break;
	case NOT_AND:
		if (((toMatch)&entry->itemUshort) != entry->itemUshort) {
			++(*matches);
		}
	case EQUALS:
	//	KdPrint(("Ushort:%hu == %hu\n", toMatch, entry->itemUshort));
		if (toMatch == entry->itemUshort) {
			++(*matches);
		}
		break;
	case NOT_EQUALS:
		if (toMatch != entry->itemUshort) {
			++(*matches);
		}
		break;
	case NOT_LESS_THAN:
	case GREATER_THAN:
		//KdPrint(("Ushort:%hu > %hu\n", toMatch, entry->itemUshort));
		if (toMatch > entry->itemUshort) {
			//KdPrint(("Ushort:Match %hu\n", toMatch));
			++(*matches);
		}
		break;
	case GREAT_OR_EQUAL:
		if (toMatch >= entry->itemUshort) {
			++(*matches);
		}
		break;
	case NOT_GREATER_THAN:
	case LESS_THAN:
		if (toMatch < entry->itemUshort) {
			++(*matches);
		}
		break;
	case LESS_OR_EQUAL:
		//KdPrint(("Ushort:%hu <= %hu\n", toMatch, entry->itemUshort));
		if (toMatch <= entry->itemUshort) {
			//KdPrint(("Ushort:Match %hu\n", toMatch));
			++(*matches);
		}
		break;
	default:
		KdPrint(("[!] checkUshortEntry: Warning, invalid match_type:%x\n", entry->match_type));
		break;
	}
}
/*
Evaluates a config list entry (PCFI) against a LONGLONG value
using supported match operators and increments the value of the matches parameter.
*/
void CheckLonglongEntry(_In_ PCFI entry, _In_ LONGLONG toMatch, _In_  PULONG matches) {
	if (!MM_TYPE_CHECK(entry, CALLISTO)) return;
	switch (entry->match_type) {
	case AND:
		if (((toMatch)&entry->itemLonglong) == entry->itemLonglong) {
			++(*matches);
		}
		break;
	case NOT_AND:
		if (((toMatch)&entry->itemLonglong) != entry->itemLonglong) {
			++(*matches);
		}
	case EQUALS:
		if (toMatch == entry->itemLonglong) {
			++(*matches);
		}
		break;
	case NOT_EQUALS:
		if (toMatch != entry->itemLonglong) {
			++(*matches);
		}
		break;
	case NOT_LESS_THAN:
	case GREATER_THAN:
		if (toMatch > entry->itemLonglong) {
			++(*matches);
		}
		break;
	case GREAT_OR_EQUAL:
		if (toMatch >= entry->itemLonglong) {
			++(*matches);
		}
		break;
	case NOT_GREATER_THAN:
	case LESS_THAN:
		if (toMatch < entry->itemLonglong) {
			++(*matches);
		}
		break;
	case LESS_OR_EQUAL:
		if (toMatch <= entry->itemLonglong) {
			++(*matches);
		}
		break;
	default:
		KdPrint(("[!] checkLonglongEntry: Warning, invalid match_type:%x\n", entry->match_type));
		break;
	}
}
/*
Evaluates a config list entry (PCFI) against a ULONG IPv4 address value,
using CIDR suffix matching.
The littleEndian parameter can be set to have it call htonl before the comparison.

The ipv4SubnetMask value in the PCFI entry will be used to determine how many
bits to compare for a match.
*/
void CheckIpv4(_In_ PCFI entry, _In_ ULONG toMatch,_In_ BOOLEAN littleEndian, _In_  PULONG matches) {
	if (!MM_TYPE_CHECK(entry, CALLISTO) || !toMatch) return;
	// Ipv4 values support only subnet matching for performance and simplicity

	if (littleEndian == TRUE) {
		toMatch = htonl(toMatch);
	}
	//KdPrint(("IPV4:%#08x -?-> %#08x/%#08x == %#08x\n", toMatch, entry->ipv4Subnet, entry->ipv4SubnetMask, (toMatch & entry->ipv4SubnetMask)));
	if ((toMatch & entry->ipv4SubnetMask) == entry->ipv4Subnet) {
	//	KdPrint(("IPV4:Match!%#08x\n", toMatch));
		++(*matches);
	}
}

/*
Evaluates a config list entry (PCFI) against a UINT8* IPv6 address data (IN6_ADDR),
by comparing how many prefix bits of the address match the item in the config list entry.

The ipv6Prefix value in the PCFI entry will be used to determine how many
bits to compare for a match.
*/
void CheckIpv6(_In_ PCFI entry, _In_ UINT8 *toMatch, _In_  PULONG matches) {
	if (toMatch == NULL || !MM_TYPE_CHECK(entry, CALLISTO) || !VALID_PAGE(entry->ipv6Subnet)) return;


	BOOLEAN subnet_match = FALSE;
	UINT8 bits_left = entry->ipv6Prefix, i = 0,matching_bits=0;

	// there are probably faster ways to do this.
	for (i; i < 16; i++) {
		if (bits_left >= 8) {
			if (toMatch[i] == entry->ipv6Subnet[i]) {
				matching_bits += 8;
				bits_left -= 8;
				if (bits_left > 0) continue;
				else{
					if (matching_bits == entry->ipv6Prefix) subnet_match = TRUE;

					break;
				}
			}
			else break;
		}
		else {
			UINT8 last_byte = toMatch[i] << (8 - bits_left);
			UINT8 last_entry_byte = entry->ipv6Subnet[i] << (8 - bits_left);
			if (last_byte == last_entry_byte) {
				subnet_match = TRUE;
				
			}
			matching_bits += (8-bits_left);

			break;
		}
	}

	if (subnet_match == TRUE) {
		//KdPrint(("IPV6 Match: entry:%p toMatch:%p, matching_bits:%u\n", toMatch, entry->ipv6Subnet, matching_bits));
		++(*matches);
	}
	else {
		//KdPrint(("IPV6 No Match: entry:%p toMatch:%p, matching_bits:%u\n", toMatch, entry->ipv6Subnet, matching_bits));
	}
}
/*
Evaluates a config list entry (PCFI) against a pointer to a UNICODE_STRING value
using supported match operators and increments the value of the matches parameter.
*/
void CheckUnicodeEntry(_In_ PCFI entry, _In_ PUNICODE_STRING toMatch, _In_  PULONG matches) {
	if (toMatch == NULL || !MM_TYPE_CHECK(entry,CALLISTO)) return;

	switch (entry->match_type) {
	case EQUALS:
		if (Equals(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	case NOT_EQUALS:
		if (!Equals(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	case STARTS_WITH:
		if (StartsWith(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	case NOT_STARTS_WITH:
		
		if (!StartsWith(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	case ENDS_WITH:
		if (EndsWith(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	case NOT_ENDS_WITH:
		if (!EndsWith(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	case CONTAINS:
		if (Contains(entry->item, toMatch)) {
			++(*matches);
		}
		break;	
	case NOT_CONTAINS:
		if (!Contains(entry->item, toMatch)) {
			++(*matches);
		}
		break;
	default:
		KdPrint(("[!] checkUnicodeEntry: Warning, invalid match_type:%x\n", entry->match_type));
		break;
	}
}

/*
Compares the UNICODE_STRING* string parameter to see if it ends with the suffix parameter (case-insensitive).

*/
BOOLEAN EndsWith(_In_ UNICODE_STRING* suffix, _In_ UNICODE_STRING* string) {
	if (!VALID_PAGE(suffix) || !VALID_PAGE(string))
		return FALSE;
	//   KdPrint(("> %p %u [%wZ] endswith %p %u [%wZ]?\n", string,string->Length, string, suffix,suffix->Length, suffix));
	//   KdPrint((">> %p  endswith %p ?\n", string,   suffix));
	 //  KdPrint(("{%x %x}_{%x %x}", string->Buffer[string->Length/2], string->Buffer[(string->Length)/2-1],
	 //      suffix->Buffer[suffix->Length/2], suffix->Buffer[(suffix->Length/2)-1]));
	BOOLEAN match = FALSE;
	UNICODE_STRING lower_string = { 0 };
	lower_string.Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, string->Length, MAGIC);
	if (lower_string.Buffer == NULL) {
		KdPrint(("endswith allocate failure\n"));
		return FALSE;
	}
	lower_string.MaximumLength = string->Length;
	if (!NT_SUCCESS(RtlDowncaseUnicodeString(&lower_string, string, FALSE))) {
		KdPrint(("RtlDowncaseUnicodeString failure!\n"));
		match = RtlSuffixUnicodeString(suffix, string, FALSE);
	}
	else {
		match = RtlSuffixUnicodeString(suffix, &lower_string, FALSE);
	}
	ExFreePoolWithTag(lower_string.Buffer, MAGIC);
	// if (match) KdPrint(("match!!\n"));
	return match;
}

/*
Compares the UNICODE_STRING* string parameter to see if it starts with the prefix parameter (case-insensitive).

*/
BOOLEAN StartsWith(_In_ UNICODE_STRING* prefix, _In_ UNICODE_STRING* string) {
	if (!VALID_PAGE(prefix) || !VALID_PAGE(string))
		return FALSE;
	//KdPrint(("> %p [%wZ] startswith %p [%wZ]?\n", string, string, prefix, prefix));
	BOOLEAN match = FALSE;
	UNICODE_STRING lower_string = { 0 };
	lower_string.Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, string->Length, MAGIC);
	if (lower_string.Buffer == NULL) {
		KdPrint(("startswith allocate failure\n"));
		return FALSE;
	}
	lower_string.MaximumLength = string->Length;
	if (!NT_SUCCESS(RtlDowncaseUnicodeString(&lower_string, string, FALSE))) {
		KdPrint(("RtlDowncaseUnicodeString failure!\n"));
		match = RtlPrefixUnicodeString(prefix, string, FALSE);
	}
	else {
		//KdPrint(("> %p [%wZ] startswith %p [%wZ]?\n", &lower_string, &lower_string, prefix, prefix));
		match = RtlPrefixUnicodeString(prefix, &lower_string, FALSE);
	}
	ExFreePoolWithTag(lower_string.Buffer, MAGIC);
	return match;
}

/*
Compares the UNICODE_STRING* toBesearched parameter to see if it contains the toSearch parameter (case-insensitive).

*/
BOOLEAN Contains(_In_ UNICODE_STRING* toSearch, _In_ UNICODE_STRING* toBesearched) {
	if (!VALID_PAGE(toSearch) || !VALID_PAGE(toBesearched))
		return FALSE;
	BOOLEAN match = FALSE;
	UNICODE_STRING lower_string = { 0 };
	lower_string.Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, toBesearched->Length+1, MAGIC);
	if (lower_string.Buffer == NULL) {
		KdPrint(("contains allocate failure\n"));
		return FALSE;
	}
	lower_string.MaximumLength = toBesearched->Length;
	if (!NT_SUCCESS(RtlDowncaseUnicodeString(&lower_string, toBesearched, FALSE))) {
		KdPrint(("RtlDowncaseUnicodeString failure!\n"));
		ExFreePoolWithTag(lower_string.Buffer, MAGIC);
		return FALSE;
	}
	//KdPrint(("[%wZ] contains [%wZ] ?? length\n", lower_string, toSearch));
	unsigned char* string = (unsigned char*)lower_string.Buffer;
	unsigned char* search = (unsigned char*)toSearch->Buffer;
	if (string == NULL || search == NULL) return FALSE;
	ULONG toBeSearchedLength = lower_string.Length;
	ULONG toSearchLength = toSearch->Length;

	if (toBeSearchedLength >= toSearchLength) {
		
		do {
			
			SIZE_T matches = RtlCompareMemory(string, search, toSearchLength);
			if (matches == toSearchLength) {
			//	  KdPrint(("Contains: %ls contains %ls\n", string, search));
				match = TRUE;
				break;
			}

			++string;
			--toBeSearchedLength;

		} while (toBeSearchedLength > 0 && toBeSearchedLength >= toSearchLength);
	}
	ExFreePoolWithTag(lower_string.Buffer, MAGIC);
	return match;
}
/*
Compares the UNICODE_STRING* toBesearched parameter to see if it is an exact match with the toSearch parameter (case-insensitive).

*/
BOOLEAN Equals(_In_ UNICODE_STRING* toSearch, _In_ UNICODE_STRING* toBesearched) {
	if (!VALID_PAGE(toSearch) || !VALID_PAGE(toBesearched))
		return FALSE;
	BOOLEAN match = FALSE;
	if (toSearch->Length != toBesearched->Length) {
		return FALSE;
	}
	else {
		match = Contains(toSearch, toBesearched);
	//	KdPrint(("EQUALS:[%wZ] == [%wZ]\n", toSearch, toBesearched));
	}
	return match;
}
void printStackTrace() {
	//return;

	PVOID stackTrace[STACK_TRACE_SIZE] = { 0 };
	USHORT capturedFrames = 0;

	capturedFrames = RtlCaptureStackBackTrace(0, STACK_TRACE_SIZE, stackTrace, NULL);
	for (int i = 0; i < capturedFrames; i++) {
		KdPrint(("Stack trace [%u]: %p\n", i, stackTrace[i]));
	}
}
/*
NTSTATUS LoadKsecdd(_In_ PGC globals) {
	UNICODE_STRING ksecdd = { 0 };
	RtlInitUnicodeString(&ksecdd, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\KSecDD");
	NTSTATUS status = ZwLoadDriver(&ksecdd);
	if (status == STATUS_IMAGE_ALREADY_LOADED) {
		globals->KSECDD_LOADED = TRUE;
	}
	else if (!NT_SUCCESS(status)) {
		KdPrint(("Ksecdd load attempt failed:%#08x\n", status));
		
	}
    else {
	globals->KSECDD_LOADED = TRUE;
	}
	return status;
}
*/
/*
void getSidAccount(_In_ PGC globals, _In_ PVOID _sid, _Out_ PUNICODE_STRING* name, _Out_ PUNICODE_STRING* domain) {
	SID* sid = (SID*)_sid;
	SID_NAME_USE nameuse = { 0 };
	ULONG buflen = 4096,dbuflen = 4096;

	(*name) = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(UNICODE_STRING), MNME);
	if ((*name) == NULL) {
		KdPrint(("UNICODE_STRING memory allocation failure.\n"));
		return;
	}
	(*domain) = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(UNICODE_STRING), MNME);
	if (domain == NULL) {
		KdPrint(("UNICODE_STRING memory allocation failure.\n"));
		return;
	}
	(*name)->Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, buflen, MNME);
	if ((*name)->Buffer == NULL) {
		KdPrint(("UNICODE_STRING->Buffer memory allocation failure.\n"));
		return;
	}
	(*domain)->Buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, dbuflen, MNME);
	if ((*domain)->Buffer == NULL) {
		KdPrint(("UNICODE_STRING->Buffer memory allocation failure.\n"));
		return;
	}
	(*name)->Length = (USHORT) buflen;
	(*name)->MaximumLength = (USHORT) buflen;;
	(*domain)->Length = (USHORT) dbuflen;
	(*domain)->MaximumLength = (USHORT) dbuflen;
	KdPrint(("getSidAccount> pre-SecLookupAccountSid\n"));
	if (globals->KSECDD_LOADED) {
		NTSTATUS status = SecLookupAccountSid(sid, &buflen, (*name), &dbuflen, (*domain), &nameuse);
		if (!NT_SUCCESS(status)) {
			KdPrint(("SecLookupAccountSid failed:%#08x\n", status));
			if (status == STATUS_BUFFER_TOO_SMALL) {
				KdPrint(("STATUS_BUFFER_TOO_SMALL:%u/%u\n", buflen, dbuflen));
			}
			return;
		}
	}
}
*/

/*
Cleans up SID/user lookup information
*/
void destroySidInfo(_In_ PUINF UI) {
	if (UI == NULL) return;
/*
	if (UI->domain != NULL && UI->domain->Length > 0) {
		DeallocateAndDestroyUnicodeString(&UI->domain, MNME);

	}
	if (UI->name != NULL && UI->name->Length > 0) {
		DeallocateAndDestroyUnicodeString(&UI->name, MNME);
	}*/
	ExFreePoolWithTag(UI->sid_data, MNME);
	UI->sid_data_length = 0;
}

/*
Takes a PEPROCESS and PETHREAD structure pointers and looks up SID information for the
associated user. It populates the right fields under the user information (UI) parameter.
*/
void SidLookup(_In_ PGC globals,_In_opt_ PETHREAD t,_In_ PEPROCESS p, _In_ PUINF UI) {
	UNREFERENCED_PARAMETER(globals);
	PSECURITY_SUBJECT_CONTEXT subjectContext = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(SECURITY_SUBJECT_CONTEXT), MNME);
	if (subjectContext == NULL) {
		KdPrint(("sidLookup allocation failure\n"));
		return;
	}
	TOKEN_USER* TUser = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(TOKEN_USER), MNME);
	if (TUser == NULL) {
		KdPrint(("sidLookup allocation failure\n"));
		return;
	}
		SeCaptureSubjectContextEx(t, p, subjectContext);
		SeLockSubjectContext(subjectContext);
		try {
			PACCESS_TOKEN PToken = SeQuerySubjectContextToken(subjectContext);
			if (PToken != NULL) {
				NTSTATUS tokenstatus = SeQueryInformationToken(PToken, TokenUser, (PVOID)TUser);
				if (NT_SUCCESS(tokenstatus)) {
					PUCHAR _sid = (PUCHAR)TUser->User.Sid;
					SID* sid = (SID*)*((PUINT64)&_sid[0]);


					if (RtlValidSid(sid)) {
						UI->sid_data_length = 8 + (sizeof(ULONG) * sid->SubAuthorityCount);
						UI->sid_data = ExAllocatePool2(POOL_FLAG_NON_PAGED, UI->sid_data_length, MNME);
						if (UI->sid_data == NULL) {
							return;
						}
						if (UI->sid_data_length > 8) {
							UI->sid_data[0] = sid->Revision;
							UI->sid_data[1] = sid->SubAuthorityCount;
							RtlCopyMemory(&UI->sid_data[2], &sid->IdentifierAuthority, 6);
							PULONG subauth = sid->SubAuthority;
							RtlCopyMemory(&UI->sid_data[8], (PUCHAR)subauth, sizeof(ULONG) * sid->SubAuthorityCount);
							UI->sid_data = UI->sid_data;
							UI->name = NULL, UI->domain = NULL;
						}
						// hangs boot because ksecdd isn't ready yet :(
						// haven't found a solution. Moved this to user-space.
					//	getSidAccount(globals,sid,&UI->name,&UI->domain);
						_(UI, MNEME);
					}
					else {
						KdPrint(("Invalid SID:%p/%p\n", TUser, sid));
					}
				}
				else {
					KdPrint(("SeQueryInformationToken failed:%#08x\n", tokenstatus));
				}
			}
		}
	finally {
		SeUnlockSubjectContext(subjectContext);
		SeReleaseSubjectContext(subjectContext);
		if (TUser != NULL) {
			ExFreePool(TUser);
		}
		if (subjectContext != NULL) {
			ExFreePool(subjectContext);
		}
	}
}