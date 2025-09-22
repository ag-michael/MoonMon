#pragma once

#include <ntdef.h>
#include "DataTypes.h"


/*
Asserts if the _ element of the typedef struct p equals the predefined magic value.
This will ideally detect things like memory corruption, type confusion, NULL deref and similar issues.
The _ member should be set to a valid value only once at init time.
These magic values are static and predictable, they won't thwart exploitation,
they just help find bugs when running tests.

The NT_ASSERT is triggered on debug builds only.
MM_ASSERT is not a replacement for MM_TYPE_CHECK and/or other bounds and safety checks,
it is a debugging macro.
*/
#if MM_BUILD == DEBUG
#define MM_ASSERT(p,__) NT_ASSERT(p!= NULL && (memcmp(&p->_,__,8)==0))
#endif
#if MM_BUILD == RELEASE
#define MM_ASSERT(p,__) if(p!= NULL && (memcmp(&p->_,__,8)==0)) DbgPrint("!!MM_ASSERT!! %p\n",p)
#endif
/*
Checks if the _ element of the typedef struct p equals the predefined magic value.
Unlike MM_ASSERT this macro does not assert, it only returns a boolean result.
The _ member should be set to a valid value only once at init time.
These magic values are static and predictable, they won't thwart exploitation,
they just help find bugs when running tests.
*/
#define MM_TYPE_CHECK(p,__) (p!=NULL && (memcmp(&p->_,__,8)==0))

/*
Initializes the '_' member of the struct p with static value __
*/
#define _(p,__) (strlen(__)>=8 && memcpy(&p->_,__,8))


#define VALID_PAGE(p) (p!=NULL && (((INT64)p) > 0xFFFF000000000000) && (((INT64)p)  < 0xFFFFFFFFFFFFFF00))

#define htonl(l)                  \
   ((((l) & 0xFF000000) >> 24) | \
   (((l) & 0x00FF0000) >> 8)  |  \
   (((l) & 0x0000FF00) << 8)  |  \
   (((l) & 0x000000FF) << 24))
#define ntohl(l) htonl(l)

#define ntohs(s) ((((s) & 0x00FF) << 8) | (((s) & 0xFF00) >> 8))
#define htons(s) ntohs(s)

LONG64 _id(_In_ PGC globals);
LONGLONG _ts();
ULONG MM_Genrand(_In_ PULONG seed);

void AllocateAndCopyUnicodeString(_In_ PUNICODE_STRING source, _In_ PUNICODE_STRING* dest, _In_ ULONG Tag);
void DeallocateAndDestroyUnicodeString(_Inout_ PUNICODE_STRING* U, _In_ ULONG Tag);
void CheckUnicodeEntry(_In_ PCFI entry, _In_ PUNICODE_STRING toMatch, _In_  PULONG matches);
void CheckUlongEntry(_In_ PCFI entry, _In_ ULONG toMatch, _In_  PULONG matches);
void CheckUshortEntry(_In_ PCFI entry, _In_ USHORT toMatch, _In_  PULONG matches);
void CheckLonglongEntry(_In_ PCFI entry, _In_ LONGLONG toMatch, _In_  PULONG matches);
void CheckUint8Entry(_In_ PCFI entry, _In_ UINT8 toMatch, _In_  PULONG matches);
void CheckInt8Entry(_In_ PCFI entry, _In_ INT8 toMatch, _In_  PULONG matches);
void CheckIpv4(_In_ PCFI entry, _In_ ULONG toMatch, _In_ BOOLEAN littleEndian, _In_  PULONG matches);
void CheckIpv6(_In_ PCFI entry, _In_ UINT8* toMatch, _In_  PULONG matches);
//void AssertLock(_In_ PGC globals, _In_ PVOID lock, _In_ KIRQL maxirql);

void destroySidInfo(_In_ PUINF UI);
void SidLookup(_In_ PGC globals, _In_opt_ PETHREAD t, _In_ PEPROCESS p, _In_ PUINF UI);
//void getSidAccount(_In_ PGC globals, _In_ PVOID _sid, _Out_ PUNICODE_STRING* name, _Out_ PUNICODE_STRING* domain);
//NTSTATUS LoadKsecdd(_In_ PGC globals);
//_Acquires_lock_(lock)
//_Has_lock_kind_(_Lock_kind_spin_lock_)
//_IRQL_raises_(DISPATCH_LEVEL)
//_IRQL_saves_
//KIRQL AcquireSpinLock(_In_  PKSPIN_LOCK lock);
//
//_Releases_lock_(lock)
//_Has_lock_kind_(_Lock_kind_spin_lock_)
//void ReleaseSpinLock(_In_ PKSPIN_LOCK lock, _In_ _IRQL_restores_ KIRQL oldirql);
//
//_Acquires_lock_(lock)
//_Has_lock_kind_(_Lock_kind_mutex_)
//_IRQL_requires_max_(APC_LEVEL)
//_IRQL_raises_(APC_LEVEL)
//void AcquireFMutex(_In_ PFAST_MUTEX lock);
//
//_Releases_lock_(lock)
//_Has_lock_kind_(_Lock_kind_mutex_)
//_IRQL_raises_(APC_LEVEL)
//void ReleaseFMutex(_In_ PFAST_MUTEX lock);
//
//void AcquireLock(_In_ PGC globals, _In_  PVOID lock, _In_ UINT8 type, _Out_ _IRQL_saves_ PKIRQL oldirql);
////_IRQL_raises_(APC_LEVEL)
//void ReleaseLock(_In_ PGC globals, _In_  PVOID lock, _In_ UINT8 type, _In_ _IRQL_restores_ KIRQL oldirql);
BOOLEAN Equals(_In_ UNICODE_STRING * toSearch, _In_ UNICODE_STRING * toBesearched);
BOOLEAN EndsWith(_In_ UNICODE_STRING* suffix, _In_ UNICODE_STRING* string);
BOOLEAN StartsWith(_In_ UNICODE_STRING* prefix, _In_ UNICODE_STRING* string);
BOOLEAN Contains(_In_ UNICODE_STRING* toSearch, _In_ UNICODE_STRING* toBesearched);
void printStackTrace();

PUNICODE_STRING QueryDosFileName(_In_ PFILE_OBJECT FileObject);