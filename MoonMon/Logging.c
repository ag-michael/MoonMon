
#include "DataTypes.h"
#include "Utils.h"
#include "Logging.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
    int _fltused = 0;
#ifdef __cplusplus
}
#endif
*/

/*
Cleans up and dallocates a log entry
*/
void DeleteLogEntry(_In_ PGC G,_Inout_ LOG_ENTRY* entry) {
    MM_ASSERT(entry, EUROPA);
    G->L_SIZE_BYTES -= entry->size;
   // InterlockedAdd(&G->L_SIZE_BYTES, ~entry->size+1);
    if (G->L_SIZE_BYTES < 0)G->L_SIZE_BYTES = 0;
    entry->_ = 0;
    ExFreePoolWithTag(entry->data, EUPORIE);
    ExFreePoolWithTag(entry, ERSA);

}

/*
Adds log entries to the global log linked-list.
Each log entry corresponds to a unique field within an event and
has an event-specific id and timestamp. 
This information along with the field type will eventually be sent to the user-space client.

The log entry Format is MOONMON<ULONG:size><USHORT:type><LONG64:id><LONGLONG:Timestamp><DATA>
On x64 at least, lONG64 and LONGLONG have the same size, the difference is just semantics.

If the list is full, it attempts to non-deterministically purge oldest entries. 
There is unfortunaley no good way around this data-loss event which can happen when the system is 
running low on resources and the user-space client isn't keeping up with the volume of logs being generated.
*/
void AddLogEntry(_In_ PGC G, _In_ LONG64 id, _In_ LONGLONG Timestamp, _In_ ULONG size, _In_ USHORT type, _In_ PVOID Data) {
    
    try {
        do {
            if (!VALID_PAGE(Data)) {
                KdPrint(("Critical, AddLogentry received an invalid Data pointer:%p\n", Data));
                return;
            }
            if (size == 0) {
                KdPrint(("AddLogEntry: 0 size request!\n"));
                return;
            }
            size_t sz_ul = sizeof(ULONG);
            size_t sz_us = sizeof(USHORT);
            size_t sz_ll = sizeof(LONGLONG);
            size_t sz_l64 = sizeof(LONG64);
            ULONG header_fields_size = (ULONG)(7 + sz_ul +sz_l64 + sz_us + sz_ll);
            ULONG max_sz = ((ULONG)(-1)) - header_fields_size - 8;
            const char* header = MM_HEADER;
            LOG_ENTRY* Entry = (LOG_ENTRY*)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(LOG_ENTRY), ERSA);
            if (Entry == NULL) {
                KdPrint(("LOG_ENTRY allocation failure\n"));
                break;
            }
            Entry->_ = 0;

            Entry->size = 0;

            if (size > max_sz) {
                KdPrint(("AddLogEntry size too large!:%u\n",size));
                ExFreePool(Entry);
                break;
            }
            Entry->size += size + header_fields_size;
            if (Entry->size < 7) {
                ExFreePool(Entry);
                break;
            }
            Entry->data = ExAllocatePool2(POOL_FLAG_NON_PAGED, Entry->size,EUPORIE);
            if (Entry->data == NULL) {
                ExFreePool(Entry);
                KdPrint(("Entry->data allocation failure\n"));
                break;
            }

            memset(Entry->data, 0x41, (size_t)Entry->size);

            char* data_offset = Entry->data;

            // Event Format is MOONMON<ULONG:size><USHORT:type><LONG64:id><LONGLONG:Timestamp><DATA>

            //header
            memcpy(data_offset, header, 7);
            data_offset += 7;

            // Size
            memcpy(data_offset, &size, sz_ul);
            data_offset += sz_ul;

            // Type
            memcpy(data_offset, &type, sz_us);
            data_offset += sz_us;

            // ID
            memcpy(data_offset, &id, sz_l64);
            data_offset += sz_l64;

            // Timestamp

            memcpy(data_offset, &Timestamp, sz_ll);
            data_offset += sz_ll;

            // Data
            memcpy((data_offset), (Data), (size_t)(size));

            KIRQL oldirql = KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)&G->LLOCK);
            try {

                if (G->L_SIZE_BYTES < LIST_SIZE_MAX_BYTES) {
                    InsertTailList(&G->L, &Entry->Link);
                    G->L_SIZE_BYTES += Entry->size;
                }
                else {
                    KdPrint(("List Full:%u/%u, attempting to remove the oldest entries.\n", G->L_SIZE_BYTES, LIST_SIZE_MAX_BYTES));

                    /*
                    if LIST_SIZE_MAX_BYTES is 200MB and LIST_PURGE_DIVISOR is 8
                    Then keepa_max is set to (LIST_SIZE_MAX_BYTES - (LIST_SIZE_MAX_BYTES / LIST_PURGE_DIVISOR)) which would be 175MB
                    a random number between 0 and 20% of what is going to be deleted (0-8.75MB) is the jitter value which is added to keep_max.
                    this is to add some level of non-determinism to the process. In this example, the final log size kept could be
                    between 175MB and 183.75MB.
                    */

                    ULONG keep_max = LIST_KEEP_MAX;
                    ULONG jitter = (MM_Genrand((PULONG)&Timestamp) % LIST_PURGE_CONGRUENT);
                    keep_max = jitter <= 0 ? keep_max : keep_max + jitter;

                    do {
                        PLIST_ENTRY oldest = G->L.Blink;
                        LOG_ENTRY* entry = CONTAINING_RECORD(oldest, LOG_ENTRY, Link);
                        if (entry == NULL) {
                          //  KdPrint(("!"));
                            break;
                        }


                        RemoveEntryList(oldest);
                        DeleteLogEntry(G, entry);
                        //    KdPrint(("."));
                    } while (G->L_SIZE_BYTES > (LONG)keep_max);
                    KdPrint(("New List size:%u/%u\n", G->L_SIZE_BYTES, keep_max));

                    // Now that some space is free, don't forget to add the log entry.
                    InsertTailList(&G->L, &Entry->Link);
                    G->L_SIZE_BYTES += Entry->size;

                }
            }
            finally {
                _(Entry,EUROPA);
                KeReleaseSpinLock((PKSPIN_LOCK)&G->LLOCK, oldirql);
                
            }
            break;
        }while (TRUE);
     }
     __except (MM_EXCEPTION_MODE) {
         KdPrint(("Unhandled AddLogEntry exception!\n"));
     }
}

