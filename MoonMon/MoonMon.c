
/*

MoonMon.c is at the core of the driver. Callback registrations, callback functions and other core functions are located here.
More specialized code goes into it's own source and header file.

General structure of callbacks is

callback(params) {
 // asserts & quick win returns

 // setup variables and initialize them

 try{
   try{
     do{
         // populate event information struct objects
         // break out if there is a problem
         // use _(); to mark event info structs initialized
         // inspect event info structs, inspect*() inspection routines may or may not return verdicts
         // verdicts are applied to the event as it makes sense
         // if inspection permits logging, AddLogEntry() is called to send logs to userspace client
       break;
     } while(TRUE);
   }except(){ // continue handler search when debugging but execute handler for release builds
    
   }
 }__finally {
    // cleanup
  }
}

For debug builds, most exceptions are not handled so that a bug check could be generated. 
Asserts will be used sparingly but they shouldn't make into debug builds. KdPrint's are used liberally because they won't make into a release build.

Callbacks will use GetCachedProcess() to get cached information about a process without actively probing for it and add that detail to their logging.

There is a performance drawback with how the callbacks work currently. Incoming data about an event is copied into standardized structs for inspection routines.
It is then copied over once more to the logging queue linked list. It is possible to do a one or zero copy operations, however, the current priority is safe operation across
all event types, and this approach is working safely enough without causing serious performance degradation.

*/
#pragma once

#include "MoonMon.h"

// holds global context variables for the driver
GC* globals;




/*
This function enumerates and caches running processes at driver startup.
if any processes are on the process_block_list, it terminates them.
*/
void startupKillBlockedProcesses()
{
    MM_ASSERT(globals, GANYMEDE);
    NTSTATUS durum = STATUS_UNSUCCESSFUL;
    SIZE_T qmemsize = (0x10 * 0x1024);
    PVOID qmemptr = 0;
    P_SYSTEM_PROCESS_INFO_L spi;
    ULONG zwq_size;
    HANDLE _pid;
    PHANDLE pid = &_pid;
    *pid = (HANDLE)-1;

    do
    {
        qmemptr = (PVOID)ExAllocatePool2(POOL_FLAG_NON_PAGED, qmemsize, MAGIC);
        if (qmemptr == NULL) // check memory is allocated or not.
        {
            KdPrint(("startupKillBlockedProcesses: ExAllocatePool2 failiure"));
            return;
        }
        durum = ZwQuerySystemInformation(5, qmemptr, (ULONG)qmemsize, &zwq_size);
        if (durum == STATUS_INFO_LENGTH_MISMATCH)
        {
            qmemsize = qmemsize * 2; // increase qmemsize for next memory alloc
            ExFreePool(qmemptr); // free memory
        }
    } while (durum == STATUS_INFO_LENGTH_MISMATCH); // resize memory
    spi = (P_SYSTEM_PROCESS_INFO_L)qmemptr;

    while (TRUE)
    {
        *pid = spi->ProcessId;
        if ((INT64)_pid > 5) {

            PEPROCESS targp;

            KdPrint(("Looking at %llu\n", (INT64)_pid));
            NTSTATUS pstatus = PsLookupProcessByProcessId(spi->ProcessId, &targp);
            if (pstatus == STATUS_SUCCESS) {
                    PPROC_INFO PI = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(PROC_INFO), PHBE);
                    if (PI == NULL) {

                        KdPrint(("PI allocation failure!\n"));
                        break;
                    }
                    
                    
                    __try {
                        if (processInfoInit(targp, PI, NULL, FALSE)==TRUE) {
                            _(PI, PHOEBE);
                        
                            try {

                                AddNewProcess(globals, spi->ProcessId,PI);
                                ULONG id=0, actions=0;
                                if (ProcessListMatch(globals,PI, PROCESS_BLOCK_LIST, globals->PBL_COUNT,&id,&actions)) {
                                    KdPrint(("\nFound a match for a kill: [id/actions:%u/%u] [%u] ImagePathName:%wZ ; CommandLine:%wZ \n", id, actions,HandleToUlong(_pid), PI->ImageFileName, PI->CommandLine));
                                    TerminateProcess(targp);
                                    break;
                                }

                             }
                            __except (MM_EXCEPTION_MODE) {
                                KdPrint(("startupKillBlockedProcesses Exception: <%u>> %p\n", HandleToULong(_pid), targp));
                            }
                            
                        }
                
                    }
                    __finally {
                        
                     if (PI!=NULL) {
                        ExFreePool(PI);
                    }
                    }
            }
            else {
                KdPrint(("PsLookupProcessByProcessId failed for %u\n", HandleToULong(*pid)));
            }

        }
        if (spi->NextEntryOffset == 0)
            break;
        spi = (P_SYSTEM_PROCESS_INFO_L)((unsigned char*)spi + spi->NextEntryOffset); // next info 

    }

    ExFreePool(qmemptr); // free memory 
    return;
}

/*
This callback handles process creation and termination events.
Eventually, process terminate events should get their own function but right now this works well enough.
*/
void ProcessNotifyCallbackRoutine(_In_ PEPROCESS pProcess, _In_ HANDLE hPid, _In_ PPS_CREATE_NOTIFY_INFO pInfo) {
    UNREFERENCED_PARAMETER(pProcess);
    MM_ASSERT(globals, GANYMEDE);
    ULONGLONG ppid=0;
    ULONGLONG pid=0;
    if (pInfo) { // Process creation
        PROC_INFO _PI = { 0 };
        PPROC_INFO PI = &_PI;
        UINF _UI = { 0 };
        PUINF UI = &_UI;
        PI->UI = NULL;

        try {

            try {
                do {
                    RtlZeroMemory(PI, sizeof(PROC_INFO));
                    RtlCopyMemory(&pid, &hPid, sizeof(HANDLE));
                    RtlCopyMemory(&ppid, &pInfo->ParentProcessId, sizeof(HANDLE));
                    UINT16 event_type = PROCESS_CREATION;
                    LONG64 id = _id(globals);
                    PEPROCESS curp = IoGetCurrentProcess();

                    if (pInfo->FileObject != NULL && pInfo->FileObject->ReadAccess == TRUE) {

                    }
                    LONGLONG Timestamp = _ts();

                    if (!curp) {
                        KdPrint(("Failed to get a handle to the current process!\n"));
                        break;
                    }

                    BOOLEAN foundProc = FALSE;

                    // Avoid touching the PEB (not always present) if the parent or child are a system/kernel process
                    // Initial processe numeration will ideally cache those instead.
                    if (HandleToUlong(pInfo->ParentProcessId) > 5 && HandleToUlong(hPid) > 5) {
                        foundProc = processInfoInit(curp, PI, pInfo, TRUE); // current process is the parent, init with that.

                    }


                    //This initializes PI, which means it is has all the right info 
                    // and can be used after this point.
                    _(PI, PHOEBE);

                    // Add the new process to the cache with its pid acting as a 
                    // 'key' for future cache lookups.
                    PEPROCESS targp = { 0 };
                    if (NT_SUCCESS(PsLookupProcessByProcessId(hPid, &targp))) {


                        // LONGLONG idauth = 0;
                        SidLookup(globals, PsGetCurrentThread(), targp, UI);
                        if (MM_TYPE_CHECK(UI, MNEME) && UI->sid_data != NULL) {
                            /* // SID user name and domain lookup works fine..excep at boot
                            *  // Keeping around the code for it for now, but this will be done by the 
                            * // user-space agent going forward.
                            for (int j = 0; j < 6; j++) {
                                idauth <<= 8;
                                idauth |= UI->sid_data[2 + j];
                            }
                            if (UI->domain != NULL) {
                                KdPrint(("\n%p [%wZ\\", UI->sid_data, UI->domain));
                            }
                            if (UI->name != NULL) {
                                KdPrint(("%wZ] ", UI->name));
                            }
                            PULONG subauth = (PULONG)&UI->sid_data[8];
                            if (UI->sid_data != NULL && UI->sid_data_length > 2) {
                                KdPrint(("S-%u-%lld", UI->sid_data[0], idauth));
                            }


                            for (ULONG i = 0; i < (UI->sid_data_length - 2) / sizeof(ULONG); i++) {
                                KdPrint(("-%u", subauth[i]));
                            }
                            KdPrint((" {%u}\n", UI->sid_data_length));
                            */
                            PI->UI = UI;
                        }
                        AddNewProcess(globals, hPid, PI);

                    }

                    // Check the block list for a match

                    BOOLEAN skip_logging = FALSE;
                    ULONG match_id = 0;
                    NTSTATUS verdict = InspectProcessCreation(globals, PI, &skip_logging, &match_id);
                    pInfo->CreationStatus = verdict;
                    // KdPrint(("match_id/verdict:%i/%#08x\n", match_id,verdict));
                    if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                        // Make sure prior calles haven't corrupted PI somehow before 
                        // trying to use it to send information to user space
                        MM_ASSERT(PI, PHOEBE);
                        AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                        AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                        //      KdPrint((">>%u\n", pid));
                        AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                        AddLogEntry(globals, id, Timestamp, sizeof(ULONG), PFLAGS, &pInfo->Flags);

                        AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PARENT_PID, (PVOID)&ppid);
                        AddLogEntry(globals, id, Timestamp, sizeof(CLIENT_ID), CREATING_THREAD_ID, &pInfo->CreatingThreadId);

                        if (foundProc) {
                            PCacheLogEntries(globals, id, Timestamp, PI, FALSE);
                        }
                        else {
                            if (VALID_PAGE(PI->ImageFileName) && VALID_PAGE(PI->ImageFileName->Buffer)) {
                                AddLogEntry(globals, id, Timestamp, PI->ImageFileName->Length, IMAGE_FILE_NAME, PI->ImageFileName->Buffer);
                            }
                            if (VALID_PAGE(PI->CommandLine) && VALID_PAGE(PI->CommandLine->Buffer)) {
                                AddLogEntry(globals, id, Timestamp, PI->CommandLine->Length, COMMAND_LINE, PI->CommandLine->Buffer);
                            }
                        }
                        AddLogEntry(globals, id, Timestamp, sizeof(NTSTATUS), CREATION_STATUS, &pInfo->CreationStatus);
                        // if (PI->Sid != NULL) {
                       //      AddLogEntry(globals, id, Timestamp, sizeof(SID), PROCESS_SID, PI->Sid);
                       //  }
                        AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                    }
                   
                    break;
                }while (TRUE);
            }
            __except (MM_EXCEPTION_MODE) {
                KdPrint(("Unhandled ProcessNotifyCallbackRoutine (creation) exception!\n"));

            }
        }
        __finally {

        }

    }
    else { // Process Termination
        PROC_INFO _PI = { 0 };
        PPROC_INFO PI = &_PI;
        PPCACHE cached = NULL;
        try {
            try {
                RtlZeroMemory(PI, sizeof(PROC_INFO));

                RtlCopyMemory(&pid, &hPid, sizeof(HANDLE));
                if (pid == (ULONGLONG)globals->clientpid) {
                    globals->clientpid = -1;
                    KdPrint(("Client process %u exited\n", HandleToULong(hPid)));
                }
                UINT16 event_type = PROCESS_TERMINATED;
                LONG64 id = _id(globals);
                BOOLEAN foundProc = FALSE;

                LARGE_INTEGER _Timestamp = PsGetProcessExitTime();
                LONGLONG Timestamp = _Timestamp.QuadPart;

                PEPROCESS curp = PsGetCurrentProcess();
                if (!VALID_PAGE(curp)) {
                    KdPrint(("Process termination failed to get a handle to the current process!\n"));
                    return;
                }
                NTSTATUS exitStatus = PsGetProcessExitStatus(curp);

                cached = GetCachedProcess(globals, hPid);
                if (MM_TYPE_CHECK(cached, CHARON)) {
                    cached->InUse = TRUE;
                    if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                        PI = cached->PI;
                        foundProc = TRUE;
                    }
                }

                //This initializes PI, which means it is has all the right info 
                // and can be used by callees
                _(PI, PHOEBE);
                BOOLEAN skip_logging = FALSE;
                ULONG match_id = 0;
                InspectProcessTermination(globals, PI, &skip_logging, &match_id);
                if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                    MM_ASSERT(PI, PHOEBE);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                    AddLogEntry(globals, id, Timestamp, sizeof(NTSTATUS), CREATION_STATUS, &exitStatus);
                    if (foundProc) {
                        PCacheLogEntries(globals, id, Timestamp, PI, FALSE);
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                }
            }
            __except (MM_EXCEPTION_MODE) {
                KdPrint(("Unhandled ProcessNotifyCallbackRoutine (termination) exception!\n"));

            }
        }
        __finally {
            if (MM_TYPE_CHECK(cached, CHARON)) {
                cached->InUse = FALSE;
            }
            // Remove the process from the cache
            PurgeCachedProcess(globals, hPid);
        }
    }
 
}

/*
This callback processes thread events. it inspects and logs remote thread creation events only.
*/
void ThreadNotifyCallbackRoutine(_In_ HANDLE hProcess, _In_ HANDLE hThread, _In_ BOOLEAN bCreate) {
    if (bCreate) { // Only thread creation is processed
        MM_ASSERT(globals, GANYMEDE);

        // Avoid thread creations that happen as part of process creation
        if (IsNewProcess(globals, hProcess) == TRUE) {
             // KdPrint(("New process thread %u\n", hProcess));
            RemoveNewProcessStatus(globals, hProcess);
            return;
        }
        PEPROCESS targp;
        PPCACHE cached = NULL, tcached = NULL;
        HANDLE currentProcess = NULL;
        BOOLEAN foundProc = FALSE, foundTargetProc = FALSE, skip_logging = FALSE;
        ULONGLONG curpid=(ULONGLONG) - 1;
        ULONGLONG pid =(ULONGLONG)-1;
        LONGLONG Timestamp = _ts();

        UINT16 event_type = CREATE_REMOTE_THREAD;
        LONG64 id = _id(globals);
        PROC_INFO _PI = { 0 }, _TPI = { 0 };
        PPROC_INFO PI = &_PI, TPI = &_TPI;
        TC_INFO _TI;
        PTC_INFO TI = &_TI;
        TI->PI = PI;
        TI->TPI = TPI;
        
        //   KdPrint((">>curpid -> pid: %u -> %u\n", curpid, pid));
        try {
            try {
                
                do {
                    currentProcess = PsGetCurrentProcessId();
                    RtlCopyMemory(&pid, &hProcess, sizeof(HANDLE));
                    RtlCopyMemory(&curpid, &currentProcess, sizeof(HANDLE));
                    if (
                        curpid > 5 &&  // Not a system process?
                        // Thread is within the same process
                        HandleToULong(hProcess) != HandleToULong(currentProcess) && // smarter ways to do this, but this is fast.
                        pid != curpid) { //Remote thread
                        KdPrint(("Remote Thread: %u/%llu -> %u/%llu\n", HandleToUlong(currentProcess),curpid,HandleToUlong(hProcess), pid));
                        PEPROCESS curp = IoGetCurrentProcess();

                        if (NT_SUCCESS(PsLookupProcessByProcessId(hProcess, &targp))) {
                            if (!curp) {
                                KdPrint(("Failed to get a handle to the current process!\n"));
                                break;
                            }


                            ULONG tid = HandleToUlong(PsGetCurrentThreadId());
                            ULONG newtid = HandleToUlong(hThread);

                            tcached = GetCachedProcess(globals, hProcess);
                            if (MM_TYPE_CHECK(tcached, CHARON)) {
                                tcached->InUse = TRUE;
                                TPI = tcached->PI;
                                foundTargetProc = TRUE;
                            }
                            else {
                                KdPrint(("ThreadNotifyCallbackRoutine Source: Cache miss, attempting to probe..\n"));
                                foundTargetProc = processInfoInit(targp, TPI, NULL, FALSE);
                            }

                            cached = GetCachedProcess(globals, currentProcess);
                            if (MM_TYPE_CHECK(cached, CHARON)) {
                                cached->InUse = TRUE;
                                if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                                    PI = cached->PI;
                                    foundProc = TRUE;
                                }
                            }
                            else {
                                KdPrint(("ThreadNotifyCallbackRoutine Target: Cache miss, attempting to probe..\n"));
                                foundProc = processInfoInit(curp, PI, NULL, FALSE);
                            }
                            _(PI, PHOEBE);
                            _(TPI, PHOEBE);
                            _(TI, IAPETUS);
                            TI->PI = PI;
                            TI->TPI = TPI;
                            ULONG match_id = 0;
                            InspectThreadCreation(globals, TI, &skip_logging,&match_id);
                            if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                                MM_ASSERT(TI, IAPETUS);
                                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                                AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                                AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                                AddLogEntry(globals, id, Timestamp, sizeof(HANDLE), PARENT_PID, (PVOID)&curpid);
                                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), CREATING_THREAD_ID, &tid);
                                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), THREAD_ID, &newtid);

                                if (foundProc==TRUE) {
                                   // KdPrint(("TSource:%wZ\n", TI->PI->ImageFileName));
                                    PCacheLogEntries(globals, id, Timestamp, TI->PI, FALSE);
                                }
                                else {
                                 //   KdPrint(("PCache miss: %u\n", HandleToULong(currentProcess)));
                                }
                                if (foundTargetProc==TRUE) {
                                  //  KdPrint(("TTarget:%wZ\n", TI->TPI->ImageFileName));
                                    PCacheLogEntries(globals, id, Timestamp, TI->TPI, TRUE);

                                }
                                else {
                                   // KdPrint(("PCache miss: %u\n", HandleToULong(hProcess)));
                                }
                                AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                             //   printStackTrace();
                            }


                        }
                        else {
                            KdPrint(("ThreadNotifyCallbackRoutine: Failed to lookup target process based on hProcess:%u\n", HandleToULong(hProcess)));
                        }

                    }


                    break;
                } while (TRUE);
            }
            __except (MM_EXCEPTION_MODE) {
                KdPrint(("ThreadNotifyCallbackRoutine Exception: %llu\n", pid));
            }
        }
        __finally {
            if (MM_TYPE_CHECK(cached, CHARON)) {
                cached->InUse = FALSE;
            }
            else if (foundProc) {
                processInfoDelete(TI->PI);
            }

            if (MM_TYPE_CHECK(tcached, CHARON)) {
                tcached->InUse = FALSE;
            }
            else if (foundTargetProc) {
                processInfoDelete(TI->TPI);
            }
        }
    }
}

/*
This callback inspects and processes image load events. This includes DLLs as well as drivers.
*/
void ImageLoadCallbackRoutine(_In_ PUNICODE_STRING FullImageName, _In_ HANDLE hProcess, _In_ PIMAGE_INFO imageInfo) {
    MM_ASSERT(globals, GANYMEDE);

    ULONGLONG pid=0;
    if (hProcess == NULL) {
        pid = 4;
    }
    else {
        RtlCopyMemory(&pid, &hProcess, sizeof(ULONGLONG));
       
    }

    LONGLONG Timestamp = _ts();

    UINT16 event_type = IMAGE_LOADED;
    LONG64 id = _id(globals);
    MODULE_INFO _MI = { 0 };
    PMODULE_INFO MI = &_MI;
    PROC_INFO _PI = { 0 };
    MI->PI = &_PI;
    POBJECT_NAME_INFORMATION obInfo = NULL;
    UNICODE_STRING _ImageLoaded = { 0 };
    _ImageLoaded.Buffer = NULL;
    


    MI->ImageLoaded = &_ImageLoaded;
    PEPROCESS targp = NULL;
    PPCACHE cached = NULL;
    

    BOOLEAN foundProc = FALSE,skip_logging=FALSE;
    MI->ImageLoaded->Length = 0;
    try {
        try {
            if (imageInfo->ExtendedInfoPresent) {
                PIMAGE_INFO_EX MI_EX = CONTAINING_RECORD(imageInfo, IMAGE_INFO_EX, ImageInfo);
                PFLT_FILE_NAME_INFORMATION nameInfo;
                if (NT_SUCCESS(FltGetFileNameInformationUnsafe(MI_EX->FileObject, NULL, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo))) {
                    try {
                        
                        // The FileObject is a more reliable way to discover the image loaded path
                        if (NT_SUCCESS(IoQueryFileDosDeviceName(MI_EX->FileObject, &obInfo))) {
                            MI->ImageLoaded = &obInfo->Name;
                        }
                        else {
                            AllocateAndCopyUnicodeString(&nameInfo->Name, &MI->ImageLoaded, MKMK);
                        }

                    }
                    __finally {
                        FltReleaseFileNameInformation(nameInfo);
                    }
                }
                else {
                    KdPrint(("ImageLoad: Extendend Information available but failed to get file information!!\n"));
                }
            }
            else {
                if (FullImageName != NULL) {
                    KdPrint(("ImageLoaded: Extended Info Unavailable for %wZ\n", FullImageName));
                    AllocateAndCopyUnicodeString(FullImageName, &MI->ImageLoaded, MKMK);

                }
                else {
                    KdPrint(("ImageLoaded: Loaded image info not found, even under FullImageInfo\n"));
                }
            }


            if (pid > 5 &&
                NT_SUCCESS(PsLookupProcessByProcessId(hProcess, &targp))) {
                cached = GetCachedProcess(globals, hProcess);
                if (MM_TYPE_CHECK(cached, CHARON)) {
                    cached->InUse = TRUE;
                    if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                        MI->PI = cached->PI;
                        foundProc = TRUE;
                    }
                }
                else {
                    foundProc = processInfoInit(targp, MI->PI, NULL, FALSE);
                }
            }
            else {
                KdPrint(("ImageLoad: Skipping process info, pid=%llu\n", pid));
            }
            MI->ImageProperties = imageInfo->Properties;
            _(MI, MAKEMAKE);
            ULONG match_id = 0;
            InspectImageloadEvent(globals, MI, &skip_logging,&match_id);
            if (match_id != 0) {
            //    KdPrint(("module match:%u\n", match_id));
            }
            if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                MM_ASSERT(MI, MAKEMAKE);
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                //  KdPrint(("> %u\n", pid));
                AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                if (MI->ImageLoaded != NULL && MI->ImageLoaded->Buffer != NULL) {
                    AddLogEntry(globals, id, Timestamp, MI->ImageLoaded->Length, LOADED_MODULE, MI->ImageLoaded->Buffer);
                }
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MODULE_PROPERTIES, &MI->ImageProperties);
                if (foundProc) {
                    PCacheLogEntries(globals, id, Timestamp, MI->PI, FALSE);
                }
                AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
            }
            AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
        }
        __except (MM_EXCEPTION_MODE) {
            KdPrint(("ImageLoad Exception: %llu\n", pid));
        }
    }
    __finally {
        if (obInfo != NULL) {
            ExFreePool(obInfo);
        }
        else {
            DeallocateAndDestroyUnicodeString(&MI->ImageLoaded, MKMK);
        }
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = FALSE;
            
        }
        else if(foundProc) {
            processInfoDelete(MI->PI);
        }
    }
}
/*
* // No plans to use a post-callback, this exists to troubleshoot cases where
* // other drivers like processhacker interfere with process access verdicts
void ProcessObjectCallbackPost(
    _In_ PVOID context,
    _In_ POB_POST_OPERATION_INFORMATION obInfo
) {
    UNREFERENCED_PARAMETER(context);
    ULONGLONG pid = 0, tpid = 0;
    obInfo->Parameters->CreateHandleInformation.GrantedAccess;
    HANDLE currentProcess = PsGetCurrentProcessId();
    RtlCopyMemory(&pid, &currentProcess, sizeof(HANDLE));
    try {
        try {
            do {

                PEPROCESS targp = (PEPROCESS)obInfo->Object;

                if (targp != NULL) {
                    HANDLE targetprocess = PsGetProcessId(targp);
                    RtlCopyMemory(&tpid, &targetprocess, sizeof(HANDLE));
                    //PEPROCESS curp = IoGetCurrentProcess();
                    // not interested in handles to the same process
                    if (targetprocess == currentProcess) {
                        break;
                    }
                    ACCESS_MASK granted = obInfo->Parameters->CreateHandleInformation.GrantedAccess;
                    
                    if (tpid>0 && tpid == (ULONGLONG) globals->clientpid && 
                        (granted & PROCESS_TERMINATE) == PROCESS_TERMINATE){
                        KdPrint(("Client-Kill-Attempt:%llu->%llu [%#08x]\n",
                            pid, tpid,
                            granted));
                     //   TerminateProcess(curp);
                    }
                }
                break;
            } while (TRUE);
        }
        __except (MM_EXCEPTION_MODE) {
            KdPrint(("ProcessObjectCallbackPost: Unhandled exception!\n"));
        }

    }
        __finally {
        }
}
*/

/*
This callback inspects and blocks process object access events. Desktop and Thread access events are yet to be considered for inclusion.
It will prevent sensitive permissions to the agent/client if RESIST_TAMPERING is on.
If the tampering is caused by a kernel mode requestor, it will sleep forever, stalling the calling process.
*/
OB_PREOP_CALLBACK_STATUS ProcessObjectCallbackPre(_In_ PVOID context, _In_ POB_PRE_OPERATION_INFORMATION obInfo) {
    UNREFERENCED_PARAMETER(context);

    MM_ASSERT(globals, GANYMEDE);
    if (obInfo->ObjectType != *PsProcessType) {
        return OB_PREOP_SUCCESS;
    }
    ULONG verdict = 0;
    ULONGLONG pid = 0,tpid=0;
    HANDLE currentProcess = PsGetCurrentProcessId();
    if (currentProcess == NULL) {
        pid = 4;
    }
    else {
        RtlCopyMemory(&pid, &currentProcess, sizeof(HANDLE));

    }

    LONGLONG Timestamp = _ts();

    UINT16 event_type = PROCESS_ACCESS;
    LONG64 id = _id(globals);
    OBPROC _OB = { 0 };
    POBPROC OB = &_OB;
    PROC_INFO _PI = { 0 }, _TPI = { 0 };
    OB->PI = &_PI, OB->TPI = &_TPI;
    
    PPCACHE cached = NULL, tcached = NULL;
    BOOLEAN foundProc = FALSE, foundTargetProc = FALSE, skip_logging = FALSE,Terminating=FALSE;

    try {
        try {
            do {

                PEPROCESS targp = (PEPROCESS)obInfo->Object;

                if (targp != NULL) {
                    HANDLE targetprocess = PsGetProcessId(targp);
                    RtlCopyMemory(&tpid, &targetprocess, sizeof(HANDLE));
                    if (obInfo->KernelHandle && tpid == (ULONGLONG)globals->clientpid ) {
                        KdPrint(("Kernel Handle to Client!:%u\n", HandleToULong(targetprocess)));
                        PEPROCESS curp = PsGetCurrentProcess();
                        if (curp != NULL) {
                            KdPrint(("Terminating the assasin and taking a nap!\n"));
                            obInfo->Parameters->CreateHandleInformation.DesiredAccess = 0;
                            obInfo->Parameters->CreateHandleInformation.OriginalDesiredAccess = 0;
                            obInfo->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
                            obInfo->Parameters->DuplicateHandleInformation.OriginalDesiredAccess = 0;
                            obInfo->Flags = 0;
                            
                            Terminating = TRUE;
                            TerminateProcess(curp);

                        }
                        if (globals->RESIST_TAMPERING) {
                            NapTime(); // Because another driver tried to kill our client
                        }
                    }
                    if (globals->RESIST_TAMPERING && tpid > 0 && tpid == (ULONGLONG)globals->clientpid) {
                        obInfo->Parameters->CreateHandleInformation.DesiredAccess = 0;
                        obInfo->Parameters->CreateHandleInformation.OriginalDesiredAccess = 0;
                        obInfo->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
                        obInfo->Parameters->DuplicateHandleInformation.OriginalDesiredAccess = 0;
                    }
                    // not interested in handles to the same process
                    if (targetprocess == currentProcess) break; 
               
                    if (obInfo->Operation == OB_OPERATION_HANDLE_CREATE) {
                        OB->DesiredAccess = obInfo->Parameters->CreateHandleInformation.DesiredAccess;
                        OB->OriginalDesiredAccess = obInfo->Parameters->CreateHandleInformation.OriginalDesiredAccess;
                    }
                    if (obInfo->Operation == OB_OPERATION_HANDLE_DUPLICATE) {
                        OB->DesiredAccess = obInfo->Parameters->DuplicateHandleInformation.DesiredAccess;
                        OB->OriginalDesiredAccess = obInfo->Parameters->DuplicateHandleInformation.OriginalDesiredAccess;
                    }

                    tcached = GetCachedProcess(globals, targetprocess);
                    if (MM_TYPE_CHECK(tcached, CHARON)) {
                        tcached->InUse = TRUE;
                        OB->TPI = tcached->PI;
                        foundTargetProc = TRUE;
                    }
                    else {

                        if (NT_SUCCESS(PsLookupProcessByProcessId(targetprocess, &targp))) {
                            
                            foundTargetProc = processInfoInit(targp, OB->TPI, NULL, FALSE);
                        }
                    }
                    
                }
                PEPROCESS curp = IoGetCurrentProcess();
                if (curp != NULL) {
                    cached = GetCachedProcess(globals, currentProcess);
                    if (MM_TYPE_CHECK(cached, CHARON)) {
                        cached->InUse = TRUE;
                        if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                            OB->PI = cached->PI;
                            foundProc = TRUE;
                        }
                    }
                    else {
                        foundProc = processInfoInit(curp, OB->PI, NULL, FALSE);
                    }
                }

                _(OB, OBERON);
                ULONG match_id = 0;
                verdict = InspectObjectAccessEvent(globals,OB, &skip_logging,&match_id);
                if (verdict != 0) {
                    KdPrint(("OBL verdict:%u/%#08x\n",match_id,verdict));
                    if (verdict == (ULONG)-1) {
                        obInfo->Parameters->CreateHandleInformation.DesiredAccess = 0;
                        obInfo->Parameters->CreateHandleInformation.OriginalDesiredAccess = 0;
                        obInfo->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
                        obInfo->Parameters->DuplicateHandleInformation.OriginalDesiredAccess = 0;
                    }
                    else {
                        obInfo->Parameters->CreateHandleInformation.DesiredAccess &= verdict;
                        obInfo->Parameters->CreateHandleInformation.OriginalDesiredAccess &= verdict;
                        obInfo->Parameters->DuplicateHandleInformation.DesiredAccess &= verdict;
                        obInfo->Parameters->DuplicateHandleInformation.OriginalDesiredAccess &= verdict;
                    }
                   if (obInfo->Operation == OB_OPERATION_HANDLE_CREATE) {
                       OB->DesiredAccess = obInfo->Parameters->CreateHandleInformation.DesiredAccess;
                       OB->OriginalDesiredAccess = obInfo->Parameters->CreateHandleInformation.OriginalDesiredAccess;
                   }
                   if (obInfo->Operation == OB_OPERATION_HANDLE_DUPLICATE) {
                       OB->DesiredAccess = obInfo->Parameters->DuplicateHandleInformation.DesiredAccess;
                       OB->OriginalDesiredAccess = obInfo->Parameters->DuplicateHandleInformation.OriginalDesiredAccess;
                   }
                }
                if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                    MM_ASSERT(OB, OBERON);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                    //  KdPrint(("> %u\n", pid));
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), TARGET_PID, (PVOID)&tpid);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONG), PHOPERATION, &OB->Operation);
                    AddLogEntry(globals, id, Timestamp, sizeof(ACCESS_MASK), PHDESIRED_ACCESS, &OB->DesiredAccess);
                    AddLogEntry(globals, id, Timestamp, sizeof(ACCESS_MASK), PHORIGINAL_DESIRED_ACCESS, &OB->OriginalDesiredAccess);
                    if (foundProc) {

                        PCacheLogEntries(globals, id, Timestamp, OB->PI, FALSE);
                    }
                    if (foundTargetProc) {
                        PCacheLogEntries(globals, id, Timestamp, OB->TPI, TRUE);

                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                }
                break;
            } while (TRUE);
            }
            __except (MM_EXCEPTION_MODE) {
                KdPrint(("ProcessObjectCallbackPre: Unhandled exception!\n"));
            }

    }
    __finally {
        
        
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = FALSE;
        }
        else if (foundProc) {
            processInfoDelete(OB->PI);
        }
        
        if (MM_TYPE_CHECK(tcached, CHARON)) {
            tcached->InUse = FALSE;
        }
        else if(foundTargetProc){
            processInfoDelete(OB->TPI);
        }
    }
    if (Terminating) {
        KdPrint(("Final DesiredAccess:%#08x\n", obInfo->Parameters->CreateHandleInformation.DesiredAccess));
    }

    return OB_PREOP_SUCCESS;
}

/*
This callback inspects registry events. 
Only some registry operations are supported. read operations are highly sensitive to performance degradation and as a result are being avoided in this callback.
*/
NTSTATUS RegistryCallbackRoutine(_In_ PVOID CallbackContext, _In_ PVOID arg1, _In_ PVOID arg2) {
    UNREFERENCED_PARAMETER(CallbackContext);
    MM_ASSERT(globals, GANYMEDE);
    NTSTATUS status = STATUS_SUCCESS;
    REG_NOTIFY_CLASS notifyClass = (REG_NOTIFY_CLASS)(ULONG_PTR)arg1;
    
    if (notifyClass != RegNtPreCreateKeyEx
        && notifyClass != RegNtPreCreateKey
        && notifyClass != RegNtPreDeleteKey
        && notifyClass != RegNtPreSetValueKey
        && notifyClass != RegNtPreDeleteValueKey
        && notifyClass != RegNtPreRenameKey
        && notifyClass != RegNtPreReplaceKey
        && notifyClass != RegNtPreRestoreKey
        && notifyClass != RegNtPreFlushKey

        && notifyClass != RegNtPostSetValueKey
        && notifyClass != RegNtPostCreateKey
        && notifyClass != RegNtPostDeleteKey
        && notifyClass != RegNtPostDeleteValueKey
      //  && notifyClass != RegNtPostEnumerateKey
     //   && notifyClass != RegNtPostEnumerateValueKey
        && notifyClass != RegNtPostFlushKey
       // && notifyClass != RegNtPostOpenKey
        && notifyClass != RegNtPostQueryKeySecurity
        && notifyClass != RegNtPostRenameKey
        && notifyClass != RegNtPostReplaceKey
        && notifyClass != RegNtPostRestoreKey
        && notifyClass != RegNtPostSetKeySecurity

        ) {
      //  if(notifyClass != 7 && notifyClass != 22 && notifyClass != 28 && notifyClass != 29 && notifyClass != 23 && notifyClass != 14 && notifyClass != 8 && notifyClass != 25 && notifyClass != 3 && notifyClass != 18 && notifyClass != 36)
       // KdPrint(("[R] %u\n", notifyClass));
        return status;
    }
    REG_POST_OPERATION_INFORMATION* regPostInfo;
    REG_SET_VALUE_KEY_INFORMATION* regSetInfo;
    ULONGLONG pid = 0;
    HANDLE currentProcess = { 0 };
    currentProcess = PsGetCurrentProcessId();
    if (currentProcess == NULL) {
        pid = 4;
        currentProcess = (HANDLE) pid;
    }
    else {
        RtlCopyMemory(&pid, &currentProcess, sizeof(ULONGLONG));

    }


    LONGLONG Timestamp = _ts();

    UINT16 event_type = REGISTRY_EVENT;
    LONG64 id = _id(globals);

    UNICODE_STRING NewFileName = { 0 };
    BOOLEAN foundProc = FALSE;
    BOOLEAN queryValueInfo = FALSE, freeRegKey = FALSE;
    BOOLEAN skip_logging = FALSE;
    REG_INFO _RI = { 0 };
    PREG_INFO RI = &_RI;
    RI->RegistryKey = NULL;
    RI->RegistryValueName = NULL;
    RI->RegistryKeyNew = NULL;
    POBJECT_NAME_INFORMATION regObInfo = NULL;
    PFILE_NAME_INFORMATION fileInfo = NULL;
    
    PPCACHE cached = NULL;


    try {
        try {
            do {
                cached = GetCachedProcess(globals, currentProcess);
                if (MM_TYPE_CHECK(cached, CHARON)) {
                    cached->InUse = TRUE;
                    if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                        RI->PI = cached->PI;
                        foundProc = TRUE;
                    }
                }
                if (notifyClass == RegNtPostSetValueKey
                    || notifyClass == RegNtPostCreateKey
                    || notifyClass == RegNtPostDeleteKey
                    // || notifyClass == RegNtPostDeleteValueKey
                    // || notifyClass == RegNtPostEnumerateKey
                     //|| notifyClass == RegNtPostEnumerateValueKey
                    || notifyClass == RegNtPostFlushKey
                    || notifyClass == RegNtPostOpenKey
                    || notifyClass == RegNtPostQueryKeySecurity
                    //  || notifyClass == RegNtPostRenameKey
                     // || notifyClass == RegNtPostReplaceKey
                     // || notifyClass == RegNtPostRestoreKey
                    || notifyClass == RegNtPostSetKeySecurity) {

                    regPostInfo = (REG_POST_OPERATION_INFORMATION*)arg2;
                    REG_SZ;
                    status = CmCallbackGetKeyObjectIDEx(&globals->Cookie, regPostInfo->Object, NULL, &RI->RegistryKey, 0);
                    if (NT_SUCCESS(status)) {
                        try {
                            freeRegKey = TRUE;
                            if (notifyClass == RegNtPostSetValueKey) {
                                regSetInfo = (REG_SET_VALUE_KEY_INFORMATION*)regPostInfo->PreInformation;
                                try {
                                    //       KdPrint(("RegNtPostSetValueKey:[%u][%u]:%p,%p,%p\n", notifyClass, pid, regSetInfo, regSetInfo->ValueName, regSetInfo->Data));
                                    queryValueInfo = TRUE;
                                    RI->RegistryValueName = regSetInfo->ValueName;
                                    RI->Data = regSetInfo->Data;
                                    RI->DataSize = regSetInfo->DataSize < globals->REG_DATA_MAX_SIZE ? regSetInfo->DataSize : globals->REG_DATA_MAX_SIZE;
                                    RI->DataType = regSetInfo->Type;

                                }
                                __except (MM_EXCEPTION_MODE) {
                                    KdPrint(("RegNtPostSetValueKey Exception #1:[%u][%llu]:%p,%p,%p\n", notifyClass, pid, regSetInfo, regSetInfo->ValueName, regSetInfo->Data));
                                    CmCallbackReleaseKeyObjectIDEx(RI->RegistryKey);
                                    break;
                                }
                            }

                        }
                        __finally {

                        }
                    }

                }
                else if (notifyClass == RegNtPreCreateKeyEx || notifyClass == RegNtPreCreateKey) {
                    PREG_PRE_CREATE_KEY_INFORMATION regPreInfo = (PREG_PRE_CREATE_KEY_INFORMATION)arg2;
                    RI->RegistryKey = regPreInfo->CompleteName;

                }
                else if (notifyClass == RegNtPreDeleteKey) {
                    PREG_DELETE_KEY_INFORMATION regPreInfo = (PREG_DELETE_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED,resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo, MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreDeleteKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            RI->RegistryKey = &regObInfo->Name;
                            //  KdPrint(("RegNtPreDeleteKey:[%p] %wZ\n", RI->RegistryKey, RI->RegistryKey));
                        }
                    }
                }
                else if (notifyClass == RegNtPreSetValueKey) {
                    PREG_SET_VALUE_KEY_INFORMATION regPreInfo = (PREG_SET_VALUE_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED,resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo,MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreSetValueKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            RI->RegistryKey = &regObInfo->Name;
                         //     KdPrint(("RegNtPreSetValueKey:[%p] %wZ\n", RI->RegistryKey, RI->RegistryKey));
                            if (regPreInfo->Data != NULL && regPreInfo->DataSize > 0 && (regPreInfo->ValueName != NULL && regPreInfo->ValueName->Length > 0)) {
                             //   KdPrint(("RegNtPreSetValueKey:[%p] %wZ, ValueName: [%wZ]\n", RI->RegistryKey, RI->RegistryKey, regPreInfo->ValueName));
                                RI->Data = regPreInfo->Data;
                                RI->DataSize = regPreInfo->DataSize;
                                RI->DataType = regPreInfo->Type;
                                RI->RegistryValueName = regPreInfo->ValueName;
                                queryValueInfo = TRUE;
                            }
                        }
                    }
                    else {
                        KdPrint(("RegNtPreSetValueKey: Error with ObQueryNameString call:%#08x\n",obstatus));
                    }
                }
                else if (notifyClass == RegNtPreDeleteValueKey) {
                    PREG_DELETE_VALUE_KEY_INFORMATION regPreInfo = (PREG_DELETE_VALUE_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED,resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo, MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreDeleteValueKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            RI->RegistryKey = &regObInfo->Name;
                            RI->Data = NULL;
                            RI->DataSize = 0;
                            if (regPreInfo->ValueName != NULL) {
                                RI->RegistryValueName = regPreInfo->ValueName;
                                queryValueInfo = TRUE;
                            }
                        }
                    }
                }
                else if (notifyClass == RegNtPreRenameKey) {
                    PREG_RENAME_KEY_INFORMATION regPreInfo = (PREG_RENAME_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo, MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreRenameKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            RI->RegistryKey = &regObInfo->Name;
                            if (regPreInfo->NewName != NULL) {
                                RI->RegistryKeyNew = regPreInfo->NewName;
                            }
                        }
                    }
                    else {
                        KdPrint(("RegNtPreRenameKey: Error with ObQueryNameString: %x\n", obstatus));
                    }
                }
                else if (notifyClass == RegNtPreReplaceKey) {
                    PREG_REPLACE_KEY_INFORMATION regPreInfo = (PREG_REPLACE_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo, MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreReplaceKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            RI->RegistryKey = &regObInfo->Name;
                            if (regPreInfo->NewFileName != NULL) {
                                RI->NewFileName = regPreInfo->NewFileName;
                            }
                            if (regPreInfo->NewFileName != NULL) {
                                RI->OldFilename = regPreInfo->OldFileName;
                            }
                        }
                    }
                }
                else if (notifyClass == RegNtPreRestoreKey) {
                    PREG_RESTORE_KEY_INFORMATION regPreInfo = (PREG_RESTORE_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED,resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    ULONG fileInfoLength = UNICODE_STRING_MAX_BYTES;
                    fileInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, fileInfoLength, MAGIC);
                    if (!VALID_PAGE(fileInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    IO_STATUS_BLOCK  iob = { 0 };
                    
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo, MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreRestoreKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            if (NT_SUCCESS(ZwQueryInformationFile(regPreInfo->FileHandle, &iob, (PVOID)fileInfo, fileInfoLength, FileNameInformation))) {
                                RI->RegistryKey = &regObInfo->Name;
                                if (regPreInfo->FileHandle != NULL) {
                                    NewFileName.Buffer = fileInfo->FileName;
                                    NewFileName.Length = (USHORT)fileInfo->FileNameLength;
                                    NewFileName.MaximumLength = NewFileName.Length;
                                    RI->NewFileName = &NewFileName;
                                }

                            }
                        }
                    }
                }
                else if (notifyClass == RegNtPreFlushKey) {
                    PREG_FLUSH_KEY_INFORMATION regPreInfo = (PREG_FLUSH_KEY_INFORMATION)arg2;
                    ULONG resultLength = 2048;
                    regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                    if (!VALID_PAGE(regObInfo)) {
                        KdPrint(("RegistryCallback: allocation error\n"));
                        return STATUS_NO_MEMORY;
                    }
                    
                    NTSTATUS obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    if (obstatus == STATUS_INFO_LENGTH_MISMATCH) {
                        ExFreePoolWithTag(regObInfo, MAGIC);
                        regObInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, resultLength, MAGIC);
                        if (!VALID_PAGE(regObInfo)) {
                            KdPrint(("RegistryCallback: allocation error\n"));
                            return STATUS_NO_MEMORY;
                        }
                        obstatus = ObQueryNameString(regPreInfo->Object, regObInfo, resultLength, &resultLength);
                    }
                    if (NT_SUCCESS(obstatus)) {
                        if (resultLength < 1) {
                            ExFreePool(regObInfo);
                            regObInfo = NULL;
                            KdPrint(("RegNtPreFlushKey: Error with ObQueryNameString, resultLength<1\n"));
                        }
                        else {
                            RI->RegistryKey = &regObInfo->Name;


                        }
                    }
                }
                RI->notifyClass = notifyClass;

                _(RI, UMBRIEL);
                ULONG match_id = 0;
                status = InspectRegistryEvent(globals, RI, &skip_logging,&match_id);
                
                if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                    MM_ASSERT(RI, UMBRIEL);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                    AddLogEntry(globals, id, Timestamp, sizeof(REG_NOTIFY_CLASS), REG_OP, &RI->notifyClass);
                    AddLogEntry(globals, id, Timestamp, sizeof(NTSTATUS), CREATION_STATUS, &status);
                    if (foundProc) {
                        PCacheLogEntries(globals, id, Timestamp, RI->PI, FALSE);
                    }
                    if (queryValueInfo && RI->RegistryValueName != NULL) {
                        AddLogEntry(globals, id, Timestamp, RI->RegistryValueName->Length, REG_VALUE_NAME, RI->RegistryValueName->Buffer);
                        if (RI->Data != NULL && RI->DataSize > 0) {
                            AddLogEntry(globals, id, Timestamp, RI->DataSize, REG_DATA, RI->Data);
                        }
                        if (RI->DataType > 0) {
                            AddLogEntry(globals, id, Timestamp, sizeof(ULONG), REG_DATA_TYPE, &RI->DataType);
                        }
                    }
                    if (RI->RegistryKey != NULL && RI->RegistryKey->Buffer != NULL && RI->RegistryKey->Length > 0) {
                        // KdPrint(("RegistryKey: [%u] %p\n", notifyClass, RI->RegistryKey));
                        AddLogEntry(globals, id, Timestamp, RI->RegistryKey->Length, REG_KEY, RI->RegistryKey->Buffer);

                    }
                    if (RI->RegistryKeyNew != NULL && RI->RegistryKeyNew->Buffer != NULL && RI->RegistryKeyNew->Length > 0) {
                        AddLogEntry(globals, id, Timestamp, RI->RegistryKeyNew->Length, REG_KEY_NEW, RI->RegistryKeyNew->Buffer);
                    }
                    if (RI->OldFilename != NULL && RI->OldFilename->Buffer != NULL && RI->OldFilename->Length > 0) {
                        AddLogEntry(globals, id, Timestamp, RI->OldFilename->Length, REG_OLD_FILE_NAME, RI->OldFilename->Buffer);
                    }
                    if (RI->NewFileName != NULL && RI->NewFileName->Buffer != NULL && RI->NewFileName->Length > 0) {
                        AddLogEntry(globals, id, Timestamp, RI->NewFileName->Length, REG_NEW_FILE_NAME, RI->NewFileName->Buffer);
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                }
                
                break;
            } while (TRUE);
        }
        except (MM_EXCEPTION_MODE) {
            KdPrint(("RegistryCallbackRoutine: Unhandled exception!\n"));
        }
    }
    finally {
        if (regObInfo != NULL) {
            ExFreePool(regObInfo);
        }
        if (fileInfo != NULL) {
            ExFreePool(fileInfo);
        }
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = FALSE;
        }
        

        if (RI->RegistryKey != NULL && freeRegKey) {
            CmCallbackReleaseKeyObjectIDEx(RI->RegistryKey);
        }
    }
   // KdPrint(("Reg verdict:%u/%u\n",  status));
    return status;
}

/*
This callback sets up the registry keys for the filesystem minifilter.
*/
NTSTATUS FsFilterRegistrySetup() {
    MM_ASSERT(globals, GANYMEDE);
    HANDLE hKey = NULL, hSubKey = NULL, hInstKey = NULL;
    NTSTATUS status = STATUS_SUCCESS;

    OBJECT_ATTRIBUTES keyAttr, subKeyAttr;
    UNICODE_STRING  instKeyName = { 0 };
    UNICODE_STRING AltitudeName = RTL_CONSTANT_STRING(L"Altitude");
    UNICODE_STRING subKey = RTL_CONSTANT_STRING(L"Instances");
    UNICODE_STRING valueName = RTL_CONSTANT_STRING(L"DefaultInstance");
    UNICODE_STRING flagsName = RTL_CONSTANT_STRING(L"Flags");
    PUNICODE_STRING keyPath = globals->DRIVER_KEY_PATH;
    WCHAR name[] = L"MoonMonFsFilterInstance";

    RtlInitUnicodeString(&instKeyName, name);
    InitializeObjectAttributes(&keyAttr, keyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    

    status = ZwOpenKey(&hKey, KEY_WRITE, &keyAttr);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to open key for FsFilter installation:%wZ\n", keyPath));
        return status;
    }
    
    InitializeObjectAttributes(&subKeyAttr, &subKey, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, hKey, NULL);
    status = ZwCreateKey(&hSubKey, KEY_WRITE, &subKeyAttr, 0, NULL, 0, NULL);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create key for FsFilter installation:%wZ [%wZ]\n", keyPath,&subKey));
        ZwClose(hKey);
        return status;
    }

    status = ZwSetValueKey(hSubKey, &valueName, 0, REG_SZ, name, sizeof(name));
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to set key value for FsFilter installation: [%wZ][%wZ][%ls]\n", keyPath, &subKey,name));
        ZwClose(hKey);
        return status;
    }

    InitializeObjectAttributes(&subKeyAttr, &instKeyName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, hSubKey, NULL);
    status = ZwCreateKey(&hInstKey, KEY_WRITE, &subKeyAttr, 0, NULL, 0, NULL);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create key for FsFilter installation: [%wZ][%wZ]\n", keyPath, &instKeyName));
        ZwClose(hKey);
        return status;
    }

    status = ZwSetValueKey(hInstKey, &AltitudeName, 0, REG_SZ, FS_FILTER_ALTITUDE, sizeof(FS_FILTER_ALTITUDE));
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to set key value for FsFilter installation: [%wZ][%wZ][%ls]\n", keyPath, &AltitudeName, FS_FILTER_ALTITUDE));
        ZwClose(hKey);
        return status;
    }

    ULONG flags = 0;
    status = ZwSetValueKey(hInstKey, &flagsName, 0, REG_DWORD, &flags, sizeof(flags));
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to set key value for FsFilter installation: [%wZ][%wZ][%ls]\n", keyPath, &AltitudeName, FS_FILTER_ALTITUDE));
        ZwClose(hKey);
        return status;
    }

    KdPrint(("Finished settingup registry keys for the Filesystem minifilter.\n"));
    ZwClose(hKey);
    return status;
}

/*
This callback inspects and blocks file create operations. 
This of course means file "open" attempts as well as actual creation of new files.
*/
FLT_PREOP_CALLBACK_STATUS FsPreCreateCallback(_In_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _In_  PVOID* CompletionContext) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    MM_ASSERT(globals, GANYMEDE);
    
    //KdPrint(("FsPreCreateCallback:%wZ\n", Data->Iopb->TargetFileObject->FileName));
    FLT_PREOP_CALLBACK_STATUS verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
    NTSTATUS status;
    FLT_FILE_NAME_INFORMATION _fileInfo = { 0 };
    PFLT_FILE_NAME_INFORMATION fileInfo = &_fileInfo;
    FILE_INFO _FI = { 0 };
    PFILE_INFO FI = &_FI;
    ULONGLONG pid = 0;
    BOOLEAN skip_logging = FALSE, foundProc = FALSE;
    HANDLE currentProcess = PsGetCurrentProcessId();
    
    if (currentProcess == NULL) {
        pid = 4;
    }
    else {
        RtlCopyMemory(&pid, &currentProcess, sizeof(ULONGLONG));

    }

   
    LONGLONG Timestamp = _ts();
    PPCACHE cached = NULL;
    UINT16 event_type = MM_FILE_CREATED;
    LONG64 id = _id(globals);
    try {
        try {
            do {
                FI->CreateOptions = Data->Iopb->Parameters.Create.Options;

                status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &fileInfo);
                if (NT_SUCCESS(status)) {
                    try {
                        status = FltParseFileNameInformation(fileInfo);
                        if (!NT_SUCCESS(status)) {
                            KdPrint(("Failed FltParseFileNameInformation: %x [%wZ]\n", status, Data->Iopb->TargetFileObject->FileName));
                            FI->Name = &Data->Iopb->TargetFileObject->FileName;
                        }
                        else {

                            FI->Name = &fileInfo->Name;
                            FI->Volume = &fileInfo->Volume;
                            FI->Share = &fileInfo->Share;
                            FI->Extension = &fileInfo->Extension;
                            FI->Stream = &fileInfo->Stream;
                            FI->FinalComponent = &fileInfo->FinalComponent;
                            FI->ParentDir = &fileInfo->ParentDir;
                            
                            // KdPrint(("FsPreCreate: Name: [%wZ] Volume: [%wZ] Share:[%wZ] Extension: [%wZ] Stream: [%wZ] FinalComponent: [%wZ] ParentDir: [%wZ]\n",
                            //     FI->Name, FI->Volume, FI->Share, FI->Extension, FI->Stream, FI->FinalComponent, FI->ParentDir));
                        }
                    }
                    __finally {
                        FltReleaseFileNameInformation(fileInfo);
                    }
                }
                else {
                    if (status != STATUS_OBJECT_PATH_NOT_FOUND) {
                        KdPrint(("Failed FltGetFileNameInformation: %x [%wZ]\n", status, Data->Iopb->TargetFileObject->FileName));
                    }
                    FI->Name = &Data->Iopb->TargetFileObject->FileName;
                }

                cached = GetCachedProcess(globals, currentProcess);

                if (MM_TYPE_CHECK(cached, CHARON)) {
                    cached->InUse = TRUE;
                    if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                        FI->PI = cached->PI;
                        foundProc = TRUE;
                    }
                }
                _(FI, DEIMOS);
                ULONG match_id = 0;
                verdict = InspectFsCreateEvent(globals, FI, &skip_logging,&match_id);
                if (!NT_SUCCESS(verdict)) {
                  //  KdPrint(("FsCreateCallback verdict:%#08x", verdict));
                    Data->IoStatus.Status = verdict;
                    Data->IoStatus.Information = 0;
                    verdict = FLT_PREOP_COMPLETE;
                }
                else {
                    verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
                }
                if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                    MM_ASSERT(FI, DEIMOS);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                    AddLogEntry(globals, id, Timestamp, sizeof(NTSTATUS), CREATION_STATUS, &Data->IoStatus.Status);
                    if (FI->CreateOptions > 0) {
                        AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_FILE_FLAGS, &FI->CreateOptions);
                    }
                    if (foundProc) {
                        PCacheLogEntries(globals, id, Timestamp, FI->PI, FALSE);
                    }


                    if (FI->Name != NULL && FI->Name->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->Name->Length, MM_FILE_NAME, FI->Name->Buffer);
                    }
                    if (FI->Volume != NULL && FI->Volume->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->Volume->Length, MM_FILE_VOLUME, FI->Volume->Buffer);
                    }
                    if (FI->Share != NULL && FI->Share->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->Share->Length, MM_FILE_SHARE, FI->Share->Buffer);
                    }
                    if (FI->Extension != NULL && FI->Extension->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->Extension->Length, MM_FILE_EXTENSION, FI->Extension->Buffer);
                    }
                    if (FI->Stream != NULL && FI->Stream->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->Stream->Length, MM_FILE_STREAM, FI->Stream->Buffer);
                    }
                    if (FI->FinalComponent != NULL && FI->FinalComponent->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->FinalComponent->Length, MM_FILE_FINAL_COMPONENT, FI->FinalComponent->Buffer);
                    }
                    if (FI->ParentDir != NULL && FI->ParentDir->Length != 0) {
                        AddLogEntry(globals, id, Timestamp, FI->ParentDir->Length, MM_FILE_PARENT_DIR, FI->ParentDir->Buffer);
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                }
                break;
            } while (TRUE);
        }
        __except (MM_EXCEPTION_MODE) {
            KdPrint(("FsPreCreateCallback: Unhandled exception!\n"));
        }
    }
    __finally {
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = FALSE;
        }
        
    }
    return verdict;
}

/*
This callback inspects and blocks SetInformation events on files. 
This means a myriad of different types of operations such as renaming, deleting, linking, mounting, attribute changes and more.
Logging of all of these operations should work seamlessly,however blocking and correct field population is WIP.
*/
FLT_PREOP_CALLBACK_STATUS FsPreSetInformationCallback(_In_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _In_  PVOID* CompletionContext) {
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(FltObjects);
    MM_ASSERT(globals, GANYMEDE);
    FILE_SET_INFO _FI = { 0 };
    PFILE_SET_INFO FI = &_FI;
    UNICODE_STRING NewName = { 0 };
    FI->NewName = NULL;
    FI->Operation = Data->Iopb->Parameters.SetFileInformation.FileInformationClass;
    FLT_PREOP_CALLBACK_STATUS verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
    PPCACHE cached = NULL;
    BOOLEAN npipe = FALSE, rprotocol = FALSE, newname = FALSE;
    try{
        try{
            do{
        if(FI->Operation == FileDirectoryInformation){
          /*  FILE_DIRECTORY_INFORMATION* ffinfo = (FILE_DIRECTORY_INFORMATION*)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->ChangeTime = ffinfo->ChangeTime;
            FI->CreationTime = ffinfo->CreationTime;
            if (FI->ChangeTime.QuadPart < 1 || FI->CreationTime.QuadPart < 1) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }
            
            */
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

        else if(FI->Operation == FileFullDirectoryInformation){
           /* FILE_FULL_DIR_INFORMATION* ffinfo = (FILE_FULL_DIR_INFORMATION*)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->ChangeTime = ffinfo->ChangeTime;
            FI->CreationTime = ffinfo->CreationTime;
            if (FI->ChangeTime.QuadPart < 1 || FI->CreationTime.QuadPart < 1) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }
            */
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

        else if(FI->Operation == FileBothDirectoryInformation){
           /* FILE_BOTH_DIR_INFORMATION* ffinfo = (FILE_BOTH_DIR_INFORMATION*)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            
            FI->ChangeTime = ffinfo->ChangeTime;
            FI->CreationTime = ffinfo->CreationTime;
            if (FI->ChangeTime.QuadPart < 1 || FI->CreationTime.QuadPart < 1) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }
            */
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

        else if(FI->Operation == FileBasicInformation){
            FILE_BASIC_INFORMATION* ffinfo = (FILE_BASIC_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->ChangeTime = ffinfo->ChangeTime;
            FI->CreationTime = ffinfo->CreationTime;
            if (FI->ChangeTime.QuadPart < 1 || FI->CreationTime.QuadPart < 1) {
                verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
                break;
            }

        }
        
        else if(FI->Operation == FileAccessInformation){
            FILE_ACCESS_INFORMATION* ffinfo = (FILE_ACCESS_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->AccessFlags = ffinfo->AccessFlags;
        }

        else if(FI->Operation == FileNameInformation){
            FILE_NAME_INFORMATION* ffinfo = (FILE_NAME_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->NewName = &NewName;
            FI->NewName->Buffer = (PWCH) ffinfo->FileName;
            FI->NewName->Length = (USHORT) ffinfo->FileNameLength;
            FI->NewName->MaximumLength = FI->NewName->Length;
            newname = TRUE;
        }

        else if(FI->Operation == FileRenameInformation){
            FILE_RENAME_INFORMATION* ffinfo = (FILE_RENAME_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->NewName = &NewName;
            FI->NewName->Buffer = (PWCH)ffinfo->FileName;
            FI->NewName->Length = (USHORT)ffinfo->FileNameLength;
            FI->NewName->MaximumLength = FI->NewName->Length;
            newname = TRUE;
        }

        else if(FI->Operation == FileLinkInformation){
            FILE_LINK_INFORMATION* ffinfo = (FILE_LINK_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            
            FI->NewName = &NewName;
            FI->NewName->Buffer = (PWCH)ffinfo->FileName;
            FI->NewName->Length = (USHORT) ffinfo->FileNameLength;
            FI->NewName->MaximumLength = FI->NewName->Length;
            newname = TRUE;
        }

        else if(FI->Operation == FileNamesInformation){
        //    FILE_NAMES_INFORMATION* ffinfo = (FILE_NAMES_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

        else if(FI->Operation == FileDispositionInformation){
            FILE_DISPOSITION_INFORMATION* ffinfo = (FILE_DISPOSITION_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            if (ffinfo->DeleteFile == FALSE) {
                verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
                break;
            }
            FI->DeleteFile = ffinfo->DeleteFile;
            
        }

        else if(FI->Operation == FileFullEaInformation){
         //   FILE_FULL_EA_INFORMATION* ffinfo = (FILE_FULL_EA_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

        else if(FI->Operation == FileStreamInformation){
           // FILE_STREAM_INFORMATION* ffinfo = (FILE_STREAM_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

        else if(FI->Operation == FilePipeInformation){
          //  FILE_PIPE_INFORMATION* ffinfo = (FILE_PIPE_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
          verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
          break;
        }

        else if(FI->Operation == FilePipeLocalInformation){
            FILE_PIPE_LOCAL_INFORMATION* ffinfo = (FILE_PIPE_LOCAL_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->NamedPipeType = ffinfo->NamedPipeType;
            FI->NamedPipeConfiguration = ffinfo->NamedPipeConfiguration;
            FI->NamedPipeState = ffinfo->NamedPipeState;
            FI->NamedPipeEnd = ffinfo->NamedPipeEnd;
            npipe = TRUE;
        }

        else if(FI->Operation == FilePipeRemoteInformation){
          //  FILE_PIPE_REMOTE_INFORMATION* ffinfo = (FILE_PIPE_REMOTE_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
          verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
          break;
        }

        else if(FI->Operation == FileNetworkOpenInformation){
            FILE_NETWORK_OPEN_INFORMATION* ffinfo = (FILE_NETWORK_OPEN_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->ChangeTime = ffinfo->ChangeTime;
            FI->CreationTime = ffinfo->CreationTime;
            if (FI->ChangeTime.QuadPart < 1 || FI->CreationTime.QuadPart < 1) {
                verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
                break;
            }
        }

        else if(FI->Operation == FileHardLinkInformation){
          //  FILE_LINKS_INFORMATION* ffinfo = (FILE_LINKS_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
          verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
          break;
        }

        else if(FI->Operation == FileRemoteProtocolInformation){
            FILE_REMOTE_PROTOCOL_INFORMATION* ffinfo = (FILE_REMOTE_PROTOCOL_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->Protocol = ffinfo->Protocol;
            FI->ProtocolMajorVersion = ffinfo->ProtocolMajorVersion;
            FI->ProtocolMinorVersion = ffinfo->ProtocolMinorVersion;
            rprotocol = TRUE; 
        }

        else if(FI->Operation == FileRenameInformationBypassAccessCheck){
            FILE_RENAME_INFORMATION* ffinfo = (FILE_RENAME_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            FI->NewName = &NewName;
            FI->NewName->Buffer = (PWCH)ffinfo->FileName;
            FI->NewName->Length = (USHORT) ffinfo->FileNameLength;
            FI->NewName->MaximumLength = FI->NewName->Length;
            newname = TRUE;
        }

        else if(FI->Operation == FileLinkInformationBypassAccessCheck){
           // FILE_LINK_INFORMATION* ffinfo = (FILE_LINK_INFORMATION*) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            verdict =  FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }
        else {
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
            break;
        }

                
 
    NTSTATUS status;
    FLT_FILE_NAME_INFORMATION _fileInfo = { 0 };
    PFLT_FILE_NAME_INFORMATION fileInfo = &_fileInfo;
    

    ULONGLONG pid = 0;
    BOOLEAN skip_logging = FALSE, foundProc = FALSE;
    HANDLE currentProcess = PsGetCurrentProcessId();

    if (currentProcess == NULL) {
        pid = 4;
    }
    else {
        RtlCopyMemory(&pid, &currentProcess, sizeof(ULONGLONG));

    }

    LONGLONG Timestamp = _ts();

    UINT16 event_type = MM_FILE_SET_INFO;
    LONG64 id = _id(globals);

    
    status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &fileInfo);
    if (NT_SUCCESS(status)) {
        try {
            status = FltParseFileNameInformation(fileInfo);
            if (!NT_SUCCESS(status)) {
                KdPrint(("FsPreSetInformationCallback:Failed FltParseFileNameInformation: %x [%wZ]\n", status, Data->Iopb->TargetFileObject->FileName));
                FI->Name = &Data->Iopb->TargetFileObject->FileName;
            }
            else {
                FI->Name = &fileInfo->Name;
                FI->Volume = &fileInfo->Volume;
                FI->Share = &fileInfo->Share;
                FI->Extension = &fileInfo->Extension;
                FI->Stream = &fileInfo->Stream;
                FI->FinalComponent = &fileInfo->FinalComponent;
                FI->ParentDir = &fileInfo->ParentDir;

                // KdPrint(("FsPreCreate: Name: [%wZ] Volume: [%wZ] Share:[%wZ] Extension: [%wZ] Stream: [%wZ] FinalComponent: [%wZ] ParentDir: [%wZ]\n",
                //     FI->Name, FI->Volume, FI->Share, FI->Extension, FI->Stream, FI->FinalComponent, FI->ParentDir));
            }
        }
        __finally {
            FltReleaseFileNameInformation(fileInfo);
        }
    }
    else {
        if (status != STATUS_OBJECT_PATH_NOT_FOUND) {
            KdPrint(("FsPreSetInformationCallback:Failed FltGetFileNameInformation: %x [%wZ]\n", status, Data->Iopb->TargetFileObject->FileName));
        }
        FI->Name = &Data->Iopb->TargetFileObject->FileName;
    }
    
    cached = GetCachedProcess(globals, currentProcess);
    
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = TRUE;
            if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                FI->PI = cached->PI;
                foundProc = TRUE;
            }
        }


        _(FI, DIONE);
        ULONG match_id = 0;
        verdict = InspectFsSetEvent(globals, FI, &skip_logging,&match_id);
        if(!NT_SUCCESS(verdict)){
            Data->IoStatus.Status = verdict;
            verdict = FLT_PREOP_COMPLETE;
        }
        else {
            verdict = FLT_PREOP_SUCCESS_NO_CALLBACK;
        }
        if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
            MM_ASSERT(FI, DIONE);
            AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
            AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
            AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
            AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_FILE_OPERATION, &FI->Operation);
            AddLogEntry(globals, id, Timestamp, sizeof(NTSTATUS), CREATION_STATUS, &Data->IoStatus.Status);
            if (foundProc) {
                PCacheLogEntries(globals, id, Timestamp, FI->PI, FALSE);
            }


            if (FI->Name != NULL && FI->Name->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->Name->Length, MM_FILE_NAME, FI->Name->Buffer);
            }
            if (FI->Volume != NULL && FI->Volume->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->Volume->Length, MM_FILE_VOLUME, FI->Volume->Buffer);
            }
            if (FI->Share != NULL && FI->Share->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->Share->Length, MM_FILE_SHARE, FI->Share->Buffer);
            }
            if (FI->Extension != NULL && FI->Extension->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->Extension->Length, MM_FILE_EXTENSION, FI->Extension->Buffer);
            }
            if (FI->Stream != NULL && FI->Stream->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->Stream->Length, MM_FILE_STREAM, FI->Stream->Buffer);
            }
            if (FI->FinalComponent != NULL && FI->FinalComponent->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->FinalComponent->Length, MM_FILE_FINAL_COMPONENT, FI->FinalComponent->Buffer);
            }
            if (FI->ParentDir != NULL && FI->ParentDir->Length != 0) {
                AddLogEntry(globals, id, Timestamp, FI->ParentDir->Length, MM_FILE_PARENT_DIR, FI->ParentDir->Buffer);
            }


            if (FI->CreationTime.QuadPart > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(LONGLONG), MM_FILE_CREATION_TIME, &FI->CreationTime.QuadPart);
            }
            if (FI->ChangeTime.QuadPart > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(LONGLONG), MM_FILE_CHANGE_TIME, &FI->ChangeTime.QuadPart);
            }
            if (FI->AccessFlags > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(ACCESS_MASK), MM_FILE_ACCESS_FLAGS, &FI->AccessFlags);
            }
            if (newname == TRUE && FI->NewName != NULL) {
                AddLogEntry(globals, id, Timestamp, FI->NewName->Length, MM_FILE_NEW_NAME, FI->NewName->Buffer);
            }
            if (rprotocol && FI->Protocol > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_FILE_NETWORK_PROTOCOL, &FI->Protocol);
            }
            if (rprotocol && FI->ProtocolMajorVersion > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(USHORT), MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR, &FI->ProtocolMajorVersion);
            }
            if (rprotocol && FI->ProtocolMinorVersion) {
                AddLogEntry(globals, id, Timestamp, sizeof(USHORT), MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR, &FI->ProtocolMinorVersion);
            }
            if (npipe && FI->NamedPipeType > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_NAMED_PIPE_TYPE, &FI->NamedPipeType);
            }
            if (npipe && FI->NamedPipeConfiguration > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_NAMED_PIPE_CONFIG, &FI->NamedPipeConfiguration);
            }
            if (npipe && FI->NamedPipeState > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_NAMED_PIPE_STATE, &FI->NamedPipeState);
            }
            if (npipe && FI->NamedPipeEnd > 0) {
                AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MM_NAMED_PIPE_END, &FI->NamedPipeEnd);
            }
            if (FI->DeleteFile == TRUE) {
                AddLogEntry(globals, id, Timestamp, sizeof(BOOLEAN), MM_FILE_DELETION, &FI->DeleteFile);
            }

            AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
        }
        break;
            } while (TRUE);
        }
        except (MM_EXCEPTION_MODE) {
            KdPrint(("FsPreSetInformationCallback: Unhandled exception!\n"));
        }
    }
    finally {
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = FALSE;
        }
    }
    return verdict;
}

/*
FS filter event Debugging
*/
NTSTATUS FsFilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags) {
    UNREFERENCED_PARAMETER(Flags);

    KdPrint(("FsFilterUnload\n"));
    NTSTATUS status = STATUS_SUCCESS;
    return status;
}

/*
FS filter event Debugging
*/
NTSTATUS FsInstanceSetup(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_SETUP_FLAGS Flags, DEVICE_TYPE VolumeDeviceType, FLT_FILESYSTEM_TYPE VolumeFilesystemType) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);
    UNREFERENCED_PARAMETER(VolumeFilesystemType);

    KdPrint(("FsInstanceSetup\n"));
    NTSTATUS status = STATUS_SUCCESS;
    return status;
}

/*
FS filter event Debugging
*/
NTSTATUS FsInstanceQueryTeardown(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    KdPrint(("FsInstanceQueryTeardown\n"));
    NTSTATUS status = STATUS_SUCCESS;
    return status;
}

/*
FS filter event Debugging
*/
void FsInstanceTeardownStart(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Reason) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Reason);

    KdPrint(("FsInstanceTeardownStart\n"));
}

/*
FS filter event Debugging
*/
void FsInstanceTeardownComplete(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Reason) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Reason);

    KdPrint(("FsInstanceTeardownComplete\n"));
}

/*
This callback is for debugging/instrospecting WFP filter events.
*/
NTSTATUS NetNotifyCallback(_In_ FWPS_CALLOUT_NOTIFY_TYPE notifyType, _In_ const GUID* filterKey, _In_ FWPS_FILTER* filter) {
    UNREFERENCED_PARAMETER(filter);
    MM_ASSERT(globals, GANYMEDE);
    UNICODE_STRING guid_string = RTL_CONSTANT_STRING(L"{NO GUID}");

    if (filterKey) {
        RtlStringFromGUID(filterKey, &guid_string);
    }
    if (notifyType == FWPS_CALLOUT_NOTIFY_ADD_FILTER) {
        KdPrint(("WFP Filter added: %wZ\n", guid_string));
        // Attempt to deny unauthorized filter additions here.
    }
    if (notifyType == FWPS_CALLOUT_NOTIFY_DELETE_FILTER) {
        KdPrint(("WFP Filter deleted: %wZ\n", guid_string));
    }
    if (filterKey) {
        RtlFreeUnicodeString(&guid_string);
    }
    return STATUS_SUCCESS;
}

/*
This callback handles classify requests from WFP in accordance with Callout registrations.
Supported WFP layers are inspected, blocking is currently unsupported.
*/
void NetClassifyCallback(
    _In_ const FWPS_INCOMING_VALUES* inFixedValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES* meta,
    _Inout_opt_ void* layerData,
    _In_opt_ const void* classifyContext,
    _In_ const FWPS_FILTER* filter,
    _In_ UINT64 flowContext,
    _Inout_ FWPS_CLASSIFY_OUT* classifyOut
    ) {
    UNREFERENCED_PARAMETER(filter);
    UNREFERENCED_PARAMETER(layerData);
    classifyOut->actionType = FWP_ACTION_CONTINUE; // continue processing other callouts/filters
    MM_ASSERT(globals, GANYMEDE);
    ULONGLONG pid = 0;
    UINT16 layer = inFixedValues->layerId;
    BOOLEAN skip_logging = FALSE, foundProc = FALSE, netconn=TRUE,littleEndian = TRUE;
    INT8 Direction = -1;
    UINT8* tmp = NULL;

    HANDLE currentProcess = (HANDLE) - 1;
    FWPS_INCOMING_VALUE *IV = inFixedValues->incomingValue;
    
    UNICODE_STRING app = { 0 };
    app.Buffer = NULL;
    UNICODE_STRING unknownProc = RTL_CONSTANT_STRING(L"<unknown process>");
    UNICODE_STRING* procPath = &unknownProc;

    
    WFP_INFO _NI = { 0 };
    PWFP_INFO NI = &_NI;
    NI->PI = NULL;
    NI->remoteIpv6Addr = NULL;
    LONGLONG Timestamp = _ts();
    PPCACHE cached = NULL;
    UINT16 event_type = NETWORK_CONNECTION;
    LONG64 id = _id(globals);

    try {
        try {
            _(NI, NEREID);
            do {
                if ((meta->currentMetadataValues & FWPS_METADATA_FIELD_PROCESS_ID) != 0) {
                    pid = (ULONG)meta->processId;

                }


                if (((meta->currentMetadataValues & FWPS_METADATA_FIELD_PACKET_DIRECTION) != 0)) {
                    Direction = (INT8)meta->packetDirection;
                    NI->Direction = Direction;
                }


                if (inFixedValues) {
                     // IPV4 traffic, this should include TCP,UDP, ICMP,etc..
                    if (
                        layer == FWPS_LAYER_ALE_AUTH_CONNECT_V4
                        || layer == FWPS_LAYER_ALE_AUTH_LISTEN_V4
                        || layer == FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4
                        || layer == FWPS_LAYER_DATAGRAM_DATA_V4
                        ) {
                        try {
                            if (classifyContext != NULL) {
                                KdPrint(("WFP context:%p\n", classifyContext)); // shouldn't be called, but you never know..
                            }

                            UINT8 ipProtocol = 0;
                            UINT16 localPort = 0, remotePort = 0;
                            UINT32 localIp = 0, remoteIp = 0;

                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_ALE_APP_ID].value.type == FWP_BYTE_BLOB_TYPE) {
                                app.Buffer = (PWCH)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_ALE_APP_ID].value.byteBlob->data;
                                app.Length = (USHORT)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_ALE_APP_ID].value.byteBlob->size - 2;
                                app.MaximumLength = app.Length;
                            }

                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_ARRIVAL_INTERFACE].value.type == FWP_UINT64) {
                                NI->interface = *IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_ARRIVAL_INTERFACE].value.uint64;
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_ARRIVAL_INTERFACE_TYPE].value.type == FWP_UINT32) {
                                    NI->interfaceType = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_ARRIVAL_INTERFACE_TYPE].value.uint32;
                                }
                            }
                            else if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_INTERFACE].value.type == FWP_UINT64) {
                                NI->interface = *IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_INTERFACE].value.uint64;
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_INTERFACE_TYPE].value.type == FWP_UINT32) {
                                    NI->interfaceType = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_INTERFACE_TYPE].value.uint32;
                                }
                            }

                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL].value.type == FWP_UINT8) {
                                ipProtocol = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL].value.uint8;
                                NI->ipProtocol = ipProtocol;
                            }
                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS].value.type == FWP_UINT32) {
                                localIp = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS].value.uint32;

                            }
                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS].value.type == FWP_UINT32) {
                                remoteIp = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS].value.uint32;
                               // KdPrint(("REMOTE IPx:%#08x\n", remoteIp));
                            }

                            if (Direction != 1 && ((meta->currentMetadataValues & FWPS_METADATA_FIELD_PACKET_DIRECTION) != 0)) {
                                localIp = htonl(localIp);
                                remoteIp = htonl(remoteIp);
                                littleEndian = FALSE;
                            }
                                NI->localIpv4Addr = localIp;
                                NI->remoteIpv4Addr = remoteIp;
                          //      KdPrint(("REMOTE IP:%#08x\n", remoteIp));

                         //   if (ipProtocol == 6 || ipProtocol == 17) {
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT].value.type == FWP_UINT16) {
                                    localPort = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT].value.uint16;
                                }
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT].value.type == FWP_UINT16) {
                                    remotePort = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT].value.uint16;
                                }
                          //  }
                            NI->localPort = localPort;
                            NI->remotePort = remotePort;
                        //    KdPrint((">>> protocol:%u,direction:%u,localport:%u,remoteport:%u,localip:%#08x,remoteip:%#08x\n", ipProtocol, Direction, localPort, remotePort, localIp,remoteIp));

                        }
                        __except (MM_EXCEPTION_MODE) {
                            KdPrint(("Unhandled WFP IPV4 event exception!\n"));
                        }
                    }
                    // IPV6 traffic, this should include TCP, UDP, ICMP, etc..
                    else if (
                        layer == FWPS_LAYER_ALE_AUTH_CONNECT_V6
                        || layer == FWPS_LAYER_ALE_AUTH_LISTEN_V6
                        || layer == FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6
                        || layer == FWPS_LAYER_DATAGRAM_DATA_V6
                        ) {
                        try {
                            UINT8 ipProtocol = 0;
                            UINT16 localPort = 0, remotePort = 0;
                            IN6_ADDR* localIpAddress = NULL, * remoteIpAddress = NULL;
                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_ALE_APP_ID].value.type == FWP_BYTE_BLOB_TYPE) {
                                app.Buffer = (PWCH)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_ALE_APP_ID].value.byteBlob->data;
                                app.Length = (USHORT)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_ALE_APP_ID].value.byteBlob->size - 2;
                                app.MaximumLength = app.Length;
                            }

                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_ARRIVAL_INTERFACE].value.type == FWP_UINT64) {
                                NI->interface = *IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_ARRIVAL_INTERFACE].value.uint64;
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_ARRIVAL_INTERFACE_TYPE].value.type == FWP_UINT32) {
                                    NI->interfaceType = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_ARRIVAL_INTERFACE_TYPE].value.uint32;
                                }
                            }
                            else if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_INTERFACE].value.type == FWP_UINT64) {
                                NI->interface = *IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_INTERFACE].value.uint64;
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_INTERFACE_TYPE].value.type == FWP_UINT32) {
                                    NI->interfaceType = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_INTERFACE_TYPE].value.uint32;
                                }
                            }


                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_PROTOCOL].value.type == FWP_UINT8) {
                                ipProtocol = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_PROTOCOL].value.uint8;
                                NI->ipProtocol = ipProtocol;
                            }
                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS].value.type == FWP_BYTE_ARRAY16_TYPE) {
                                localIpAddress = (IN6_ADDR*)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS].value.byteArray16->byteArray16;
                                NI->localIpv6Addr = (UINT8*)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS].value.byteArray16->byteArray16;
                            }
                            if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS].value.type == FWP_BYTE_ARRAY16_TYPE) {
                                remoteIpAddress = (IN6_ADDR*)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS].value.byteArray16->byteArray16;
                                NI->remoteIpv6Addr = (UINT8*)IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS].value.byteArray16->byteArray16;
                            }

                            if (ipProtocol == 6 || ipProtocol == 17) {

                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_PORT].value.type == FWP_UINT16) {
                                    localPort = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_PORT].value.uint16;
                                    
                                }
                                if (IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_PORT].value.type == FWP_UINT16) {
                                    remotePort = IV[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_PORT].value.uint16;
                                   
                                }
                            }
                            NI->localPort = localPort;
                            NI->remotePort = remotePort;
                      //      KdPrint((">>>>>> protocol:%u,direction:%u,localport:%u,remoteport:%u,localip:%p,remoteip:%p\n", ipProtocol, Direction, localPort, remotePort, NI->localIpv6Addr, NI->remoteIpv6Addr));

                        }
                        __except (MM_EXCEPTION_MODE) {
                            KdPrint(("Unhandled WFP IPV6 event exception!\n"));
                        }
                    }
                    // IPV4 resource assignment, this includes bind() and opening raw sockets and/or promiscous interface access
                    else if (layer == FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V4) {

                        try {
                            event_type = MM_NETWORK_RESOURCE;
                            netconn = FALSE;
                            UINT32 localIpAddress = 0, interfaceType = 0;
                            UINT64 localInterface = 0;
                            UINT8 addressType = 0, promiscuous = 0;

                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_ALE_APP_ID].value.type == FWP_BYTE_BLOB_TYPE) {
                                app.Buffer = (PWCH)IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_ALE_APP_ID].value.byteBlob->data;
                                app.Length = (USHORT)IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_ALE_APP_ID].value.byteBlob->size - 2;
                                app.MaximumLength = app.Length;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_ADDRESS].value.type == FWP_UINT32) {
                                localIpAddress = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_ADDRESS].value.uint32;
                                NI->localIpv4Addr = htonl(localIpAddress);
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_ADDRESS_TYPE].value.type == FWP_UINT8) {
                                addressType = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_ADDRESS_TYPE].value.uint8;
                                NI->addrType = addressType;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_ALE_PROMISCUOUS_MODE].value.type != FWP_EMPTY) {
                                promiscuous = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_ALE_PROMISCUOUS_MODE].value.uint8;
                                NI->promiscuous = promiscuous;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_INTERFACE].value.type == FWP_UINT64) {
                                localInterface = *IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_INTERFACE].value.uint64;
                                NI->interface = localInterface;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_INTERFACE_TYPE].value.type == FWP_UINT32) {
                                interfaceType = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_INTERFACE_TYPE].value.uint32;
                                NI->interfaceType = interfaceType;
                            }
                            
                          /* KdPrint(("[V4 RESOURCE] [pid:%u] [promiscuous:%u] [iftype:%#x] [interface:%#016llx] [addrtype:%#x] [addr:%#08x]",
                                pid,
                                promiscuous,
                                interfaceType,
                                localInterface,
                                addressType,
                                localIpAddress

                                ));
                          
                            KdPrint(("\n"));
                            */
                        }
                        __except (MM_EXCEPTION_MODE) {
                            KdPrint(("Unhandled WFP IPV4 resource event exception!\n"));
                        }
                    }
                    // IPV46resource assignment, this includes bind() and opening raw sockets and/or promiscous interface access
                    else if (layer == FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V6) {
                        // [V6 RESOURCE] 12 FWP_BYTE_BLOB_TYPE 11 1 UINT8 0 4 UINT64 3 UINT32

                        /*KdPrint(("[V6 RESOURCE] %u %u %u %u %u %u\n",
                            IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_APP_ID].value.type,
                            IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS].value.type,
                            IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS_TYPE].value.type,
                            IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_PROMISCUOUS_MODE].value.type,
                            IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_INTERFACE].value.type,
                            IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_INTERFACE_TYPE].value.type
                            ));
                            */
                        try {
                            event_type = MM_NETWORK_RESOURCE;
                            netconn = FALSE;
                            IN6_ADDR _localIpAddress = { 0 };
                            IN6_ADDR* localIpAddress = &_localIpAddress;
                            UINT32 interfaceType = 0;
                            UINT64 localInterface = 0;
                            UINT8 addressType = 0, promiscuous = 0;

                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_APP_ID].value.type == FWP_BYTE_BLOB_TYPE) {
                                app.Buffer = (PWCH)IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_APP_ID].value.byteBlob->data;
                                app.Length = (USHORT)IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_APP_ID].value.byteBlob->size - 2;
                                app.MaximumLength = app.Length;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS].value.type == FWP_BYTE_ARRAY16_TYPE) {
                                localIpAddress = (IN6_ADDR*)IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS].value.byteArray16->byteArray16;
                                NI->localIpv6Addr = (UINT8*)IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS].value.byteArray16->byteArray16;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS_TYPE].value.type == FWP_UINT8) {
                                addressType = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS_TYPE].value.uint8;
                                NI->addrType = addressType;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_PROMISCUOUS_MODE].value.type != FWP_EMPTY) {
                                promiscuous = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_ALE_PROMISCUOUS_MODE].value.uint8;
                                NI->promiscuous = promiscuous;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_INTERFACE].value.type == FWP_UINT64) {
                                localInterface = *IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_INTERFACE].value.uint64;
                                NI->interface = localInterface;
                            }
                            if (IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_INTERFACE_TYPE].value.type == FWP_UINT32) {
                                interfaceType = IV[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_INTERFACE_TYPE].value.uint32;
                                NI->interfaceType = interfaceType;
                            }
       
                        }
                        __except (MM_EXCEPTION_MODE) {
                            KdPrint(("Unhandled WFP IPV6 resource event exception!\n"));
                        }

                    }
                    // DNS cache logging is WIP, this is just for debugging
                    else if (layer == FWPS_LAYER_NAME_RESOLUTION_CACHE_V4) {
                        KdPrint(("[V4 DNS] %u %u %u %u\n",
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V4_ALE_APP_ID].value.type,
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V4_IP_REMOTE_ADDRESS].value.type,
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V4_PEER_NAME].value.type,
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V4_ALE_USER_ID].value.type
                            ));
                    }
                    else if (layer == FWPS_LAYER_NAME_RESOLUTION_CACHE_V6) {
                        KdPrint(("[V6 DNS] %u %u %u %u\n",
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V6_ALE_APP_ID].value.type,
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V6_IP_REMOTE_ADDRESS].value.type,
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V6_PEER_NAME].value.type,
                            IV[FWPS_FIELD_NAME_RESOLUTION_CACHE_V6_ALE_USER_ID].value.type
                            ));
                    }
                    else {
                        KdPrint(("WFP:Layer id:%x, flow context:%llx\n", layer, flowContext));
                    }

                }
                else {
                    KdPrint(("Warning,No infixedvalues!!\n"));
                }

                if (pid != 0) {
                    currentProcess = (HANDLE)meta->processId;
                }
                else {
                    currentProcess = PsGetCurrentProcessId();
                    RtlCopyMemory(&pid, &currentProcess, sizeof(HANDLE));
                }

                cached = GetCachedProcess(globals, currentProcess);
                if (MM_TYPE_CHECK(cached, CHARON)) {
                    cached->InUse = TRUE;
                    if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                        NI->PI = cached->PI;
                        foundProc = TRUE;
                    }
                }
                NI->layer = layer;

                
                ULONG match_id = 0;
                InspectNetworkEvent(globals, NI, littleEndian,&skip_logging,&match_id);

                if (skip_logging == FALSE || globals->KEEP_EXCLUDES) {
                    MM_ASSERT(NI, NEREID);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONG), MATCH_ID, &match_id);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_TYPE, &event_type);
                    AddLogEntry(globals, id, Timestamp, sizeof(ULONGLONG), PID, (PVOID)&pid);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), MM_WFP_LAYER, (PVOID)&layer);

                    if (foundProc) {
                        PCacheLogEntries(globals, id, Timestamp, NI->PI, FALSE);
                    }
                    if (netconn == TRUE) {
                        AddLogEntry(globals, id, Timestamp, sizeof(INT8), MM_NET_DIRECTION, (PVOID)&NI->Direction);
                        AddLogEntry(globals, id, Timestamp, sizeof(UINT8), MM_NET_IP_PROTOCOL, (PVOID)&NI->ipProtocol);
                        AddLogEntry(globals, id, Timestamp, sizeof(UINT16), MM_NET_REMOTE_PORT, (PVOID)&NI->remotePort);
                        if (NI->remoteIpv4Addr != 0) {
                            AddLogEntry(globals, id, Timestamp, sizeof(UINT32), MM_NET_REMOTE_IPV4_ADDR, (PVOID)&NI->remoteIpv4Addr);
                        }
                        if (NI->remoteIpv6Addr != NULL) {
                            AddLogEntry(globals, id, Timestamp, sizeof(FWP_BYTE_ARRAY16), MM_NET_REMOTE_IPV6_ADDR, (PVOID)NI->remoteIpv6Addr);
                        }
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT8), MM_NET_ADDRESS_TYPE, (PVOID)&NI->addrType);
                    if (netconn == FALSE) {
                        AddLogEntry(globals, id, Timestamp, sizeof(UINT8), MM_NET_PROMISCUOUS, (PVOID)&NI->promiscuous);
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), MM_NET_LOCAL_PORT, (PVOID)&NI->localPort);
                    if (NI->localIpv4Addr != 0 || netconn == FALSE) {
                        AddLogEntry(globals, id, Timestamp, sizeof(UINT32), MM_NET_LOCAL_IPV4_ADDR, (PVOID)&NI->localIpv4Addr);
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT32), MM_NET_INTERFACE_TYPE, (PVOID)&NI->interfaceType);
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT64), MM_NET_INTERFACE, (PVOID)&NI->interface);
                    if (NI->localIpv6Addr != NULL) {
                        AddLogEntry(globals, id, Timestamp, sizeof(FWP_BYTE_ARRAY16), MM_NET_LOCAL_IPV6_ADDR, (PVOID)NI->localIpv6Addr);
                    }
                    if (NI->PI != NULL && NI->PI->ImageFileName == NULL) {
                        if (app.Length > 0 && app.Buffer!=NULL) {
                            AddLogEntry(globals, id, Timestamp, app.Length, IMAGE_FILE_NAME, app.Buffer);
                        }
                    }
                    AddLogEntry(globals, id, Timestamp, sizeof(UINT16), EVENT_END, &event_type);
                }
                break;
            } while (TRUE);
        }
        __except (MM_EXCEPTION_MODE) {
            KdPrint(("NetClassifyCallback: Unhandled exception!\n"));
        }
    }
    __finally {
        if (tmp != NULL) {
            ExFreePoolWithTag(tmp, MAGIC);

            if (procPath != NULL) {
                ExFreePoolWithTag(procPath, MAGIC);
            }
        }
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = FALSE;
        }
    }
}

/*
This callback ensures WFP callout registrations are done whenever the BFE is started
*/
VOID NTAPI
BFECallback(
    IN OUT void* context,
    IN FWPM_SERVICE_STATE  newState
) {
    UNREFERENCED_PARAMETER(context);
    KdPrint(("New BFE state:%i\n", newState));
    if (globals->WFP_CALLBACK && globals->WFP_Registered == FALSE && newState == FWPM_SERVICE_RUNNING) {
        KdPrint(("BFE runnnig, making sure WFP callback is setup\n"));
        SetupWFP(globals, (FWPS_CALLOUT_NOTIFY_FN)NetNotifyCallback, (FWPS_CALLOUT_CLASSIFY_FN)NetClassifyCallback);
    }
}

/*
Registes all the callbacks for the driver if their respective global flag is enabled.
This includes setting up filesystem and WFP minifilters.
*/
NTSTATUS registerCallbacks() {
    if (!MM_TYPE_CHECK(globals, GANYMEDE)) return STORE_DATA_STRUCTURE_CORRUPTION;
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING RegistryAltitude = RTL_CONSTANT_STRING(REGISTRY_ALTITUDE);
    UNICODE_STRING OBAltitude = RTL_CONSTANT_STRING(OB_ALTITUDE);

    if (globals->PROCESS_CALLBACK) {
        status = PsSetCreateProcessNotifyRoutineEx2(PsCreateProcessNotifySubsystems, (PVOID)ProcessNotifyCallbackRoutine, FALSE);
        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to register process callback routine: %x\n", status));
            return status;
        }
        KdPrint(("Registered Process callback routine: %x\n", status));
    }

    if (globals->THREAD_CALLBACK) {
        status = PsSetCreateThreadNotifyRoutine(ThreadNotifyCallbackRoutine);

        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to register thread callback routine: %x\n", status));
            return status;
        }
        KdPrint(("Registered Thread callback routine: %x\n", status));
    }

    if (globals->MODULE_CALLBACK) {
        status = PsSetLoadImageNotifyRoutine(ImageLoadCallbackRoutine);

        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to register ImageLoad callback routine: %x\n", status));
            return status;
        }
        KdPrint(("Registered ImageLoad callback routine.\n"));
    }

    if (globals->OBJECT_CALLBACK) {
        OB_OPERATION_REGISTRATION op = {
            PsProcessType,
            OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE,
            ProcessObjectCallbackPre,NULL
        };
        OB_CALLBACK_REGISTRATION obreg = {
            OB_FLT_REGISTRATION_VERSION,
            1,
            OBAltitude,
            NULL,
            &op
        };
        status = ObRegisterCallbacks(&obreg, &globals->ObRegHandle);
        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to register Process Object callback routine: %x\n", status));
            return status;
        }
        KdPrint(("Registered Process Object callback routine: %x\n", status));
    }

    if (globals->REGISTRY_CALLBACK) {
        status = CmRegisterCallbackEx(
            RegistryCallbackRoutine,
            &RegistryAltitude,
            globals->Driver,
            NULL,
            &globals->Cookie,
            NULL
        );
        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to register Registry callback routine: %x\n", status));
            return status;
        }
        KdPrint(("Registered Registry callback routine.\n"));
    }
    if (globals->FILE_CALLBACK) {
        if (NT_SUCCESS(FsFilterRegistrySetup())) {
            const FLT_OPERATION_REGISTRATION Callbacks[] = {
                {IRP_MJ_CREATE,0,FsPreCreateCallback,NULL},
                {IRP_MJ_SET_INFORMATION,0,FsPreSetInformationCallback,NULL},
                {IRP_MJ_OPERATION_END}
            };

            const FLT_REGISTRATION fsReg = {
                sizeof(FLT_REGISTRATION),
                FLT_REGISTRATION_VERSION,
                0,
                NULL,
                Callbacks,
                FsFilterUnload,
                FsInstanceSetup,
                FsInstanceQueryTeardown,
                FsInstanceTeardownStart,
                FsInstanceTeardownComplete,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            };

            status = FltRegisterFilter(globals->Driver, &fsReg, &FsFilter);
            if (status != STATUS_SUCCESS) {
                KdPrint(("Failed to register Filesystem mini-filter: %x\n", status));
                return status;
            }
            else {
                status = FltStartFiltering(FsFilter);
                if (status != STATUS_SUCCESS) {
                    KdPrint(("Failed to start FsFiltering: %x\n", status));
                    FltUnregisterFilter(FsFilter);
                    return status;
                }
            }
        }
        else {
            KdPrint(("Failed to register FileSystem minifilter, exiting: %x\n", status));
            return status;
        }
        KdPrint(("Finished setting up and registering the Filesystem minifilter.\n"));
    }
    if (globals->WFP_CALLBACK) {
        if (!NT_SUCCESS(FwpmBfeStateSubscribeChanges(globals->Device, BFECallback, NULL, &globals->BFENotifyHandle))) {
            KdPrint(("Failed to register BFE state notification handle!\n"));
        }
        else {
            KdPrint(("Registered a BFE callback:%p\n", globals->BFENotifyHandle));

            if (FwpmBfeStateGet() == FWPM_SERVICE_RUNNING) {
                SetupWFP(globals, (FWPS_CALLOUT_NOTIFY_FN)NetNotifyCallback, (FWPS_CALLOUT_CLASSIFY_FN)NetClassifyCallback);
            }
        }
    }
    return status;
}

/*
Unregisters callbacks and tears down filesystem and WFP minifilters before unloading the configuration.
*/
NTSTATUS  UnregisterMoonMon(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    if (!MM_TYPE_CHECK(globals, GANYMEDE)) return STORE_DATA_STRUCTURE_CORRUPTION;
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE curp = PsGetCurrentProcessId();
    PPCACHE cached = GetCachedProcess(globals, curp);
    PPROC_INFO PI = NULL;
    BOOLEAN foundProc = FALSE;
    if (MM_TYPE_CHECK(cached, CHARON)) {
        cached->InUse = TRUE;
        if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
            PI = cached->PI;
            foundProc = TRUE;
        }
    }
    KdPrint(("Unregister MoonMon:%u\n", HandleToULong(curp)));
    if (foundProc && VALID_PAGE(PI->ImageFileName) && PI->ImageFileName->Length > 0) {
        KdPrint(("Unregisteration initiated by:%wZ\n", PI->ImageFileName));
    }
    if (globals->PROCESS_CALLBACK) {
        status = PsSetCreateProcessNotifyRoutineEx2(PsCreateProcessNotifySubsystems, (PVOID)ProcessNotifyCallbackRoutine, TRUE);

        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to remove process callback routine.\n"));
            return status;
        }
    }

    if (globals->THREAD_CALLBACK) {
        status = PsRemoveCreateThreadNotifyRoutine(ThreadNotifyCallbackRoutine);
        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to remove thread callback routine.\n"));
            return status;
        }
    }

    if (globals->MODULE_CALLBACK) {
        status = PsRemoveLoadImageNotifyRoutine(ImageLoadCallbackRoutine);
        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to remove ImageLoad callback routine.\n"));
            return status;
        }
    }

    if (globals->OBJECT_CALLBACK && globals->ObRegHandle) {
        ObUnRegisterCallbacks(globals->ObRegHandle);
    }

    if (globals->REGISTRY_CALLBACK && globals->Cookie.QuadPart != 0) {
        status = CmUnRegisterCallback(globals->Cookie);
        if (status != STATUS_SUCCESS) {
            KdPrint(("Failed to remove Registry callback routine.\n"));
            return status;
        }
    }

    if (globals->FILE_CALLBACK) {
        FltUnregisterFilter(FsFilter);
    }
   
    if (globals->WFP_CALLBACK) {
        UnregisterWFP(globals);
        if (globals->BFENotifyHandle != NULL && !NT_SUCCESS(FwpmBfeStateUnsubscribeChanges0(globals->BFENotifyHandle))) {
            KdPrint(("Failed to unregister BFE notify callback!\n"));
        }

    }
    KdPrint(("callback unregistrations done\n"));
   /* BOOLEAN more_items = TRUE;
    do {
        LOG_ENTRY* entry = (LOG_ENTRY*)RemoveHeadList(&globals->L);
        if (entry == (PLIST_ENTRY) &globals->L) {
            more_items = FALSE;
        }
        else {
            ExFreePoolWithTag(entry->data, 'M001');
            ExFreePoolWithTag(entry, 'M000');
        }
    } while (more_items);
    KdPrint(("Finished clearing logs in memory\n"));
    */

    
    
    UnloadConfig(globals);
    KdPrint(("Unregstrations done, existing MoonMon.\n"));
    return STATUS_SUCCESS;
}

/*
IRP_MJ_READ handler for the driver.
The agent/client interacts performs IRP_MJ_READ operations, 
which causes this function to read available logs into the user-space buffer.

*/
NTSTATUS ReadLogEntry(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T bytes = 0;
    try {
        do {
            PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
            ULONG len = stack->Parameters.Read.Length;
            // Driver hasn't finished initializing, don't attempt read logs.
            if (!MM_TYPE_CHECK(globals,GANYMEDE) || globals->State != INITIALIZED) {
                status = STATUS_PENDING;
                break;
            }
            KIRQL oldirql = KeAcquireSpinLockRaiseToDpc((PKSPIN_LOCK)&globals->LLOCK);
            try {

                if (IsListEmpty(&globals->L)) {
                    status = STATUS_NO_MORE_ENTRIES;
                    break;
                }
                NT_ASSERT(Irp->MdlAddress);

                unsigned char* log_data = (unsigned char*)MmGetSystemAddressForMdlSafe(Irp->MdlAddress, HighPagePriority);
                if (!log_data) {
                    KdPrint(("ReadLogEntry: Bad buffer, insufficient resources?\n"));
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }
                RtlZeroMemory(log_data, len);
                BOOLEAN copy_successful = FALSE;
                do { // Keep copying logs into the userspace Mdl until it fills up or there are no logs left.

                    PLIST_ENTRY head = RemoveHeadList(&globals->L);
                    PLOG_ENTRY entry = CONTAINING_RECORD(head, LOG_ENTRY, Link);
                    if (!MM_TYPE_CHECK(entry,EUROPA)) {
                        KdPrint(("Critical: type check failed for a log entry object!!\n"));
                        break;
                    }
                    if (len < (bytes + entry->size)) {
                        KdPrint(("ReadLogEntry, buffer size insufficient:%u , data size:%u\n", len, entry->size));
                        InsertHeadList(&globals->L, head);
                        if (!bytes) { 
                           //This is the first write. since no data was copied over, the status has to be set like this
                           //if previous iterations copied over data, then we can return status_success, 
                           // bytes would be whatever was copied until then
                           // But since what is to be written is more than the client's buffer, we have to return this tatus
                            status = STATUS_INVALID_BUFFER_SIZE;
                            break;
                        }
                        break;

                    }


                    RtlCopyMemory(log_data, entry->data, entry->size);
                    log_data = ((unsigned char*)log_data) + entry->size;
                    bytes += entry->size;

                    DeleteLogEntry(globals, entry);
                    copy_successful = TRUE; // this makes sure at least one copy+delete cycle was done
                } while (!(IsListEmpty(&globals->L)) && bytes < (len - 12));
                if (copy_successful == TRUE && (len - bytes) > 7) {
                    // add a 7-byte POSTAMBLE to help the user-space client
                    // process entries better.
                    RtlCopyMemory(log_data, POSTAMBLE, 7);
                }
            }
            finally {
                KeReleaseSpinLock((PKSPIN_LOCK)&globals->LLOCK, oldirql);
            }
            break;
        } while (TRUE);
    }
    __except (MM_EXCEPTION_MODE) {
        KdPrint(("Unhandled ReadLogEntry exception!\n"));
    }
    if (bytes > 0) {
        KdPrint(("ReadLogEntry: Completed reqeust with bytes read:%llu, list size: %i\n", bytes,globals->L_SIZE_BYTES));
    }
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytes;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}



/*
Handles client's attaching with IRP_MJ_CREATE.
Right now it does not validate the client's properties, it only logs them.
*/
NTSTATUS MoonMonDispatchCreate(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    if (globals->clientpid > 0) {
        Irp->IoStatus.Status = STATUS_RESOURCE_IN_USE;
        KdPrint(("Client denied,existing client with pid:%llu\n", globals->clientpid));
    }
    else {
        HANDLE curp = PsGetCurrentProcessId();
        RtlCopyMemory(&globals->clientpid, &curp, sizeof(LONGLONG));
        PPCACHE cached = GetCachedProcess(globals, curp);
        PPROC_INFO PI = NULL;
        BOOLEAN foundProc = FALSE;
        if (MM_TYPE_CHECK(cached, CHARON)) {
            cached->InUse = TRUE;
            if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
                PI = cached->PI;
                foundProc = TRUE;
            }
        }
        KdPrint(("MoonMon: IRP_MJ_CREATE\n"));
        if (foundProc && VALID_PAGE(PI->ImageFileName) && PI->ImageFileName->Length > 0) {
            KdPrint(("Client connected:%wZ\n", PI->ImageFileName));
        }

        Irp->IoStatus.Status = STATUS_SUCCESS;
    }
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

/*
Handles clients closing their handle to the driver with IRP_MJ_CLOSE.
*/
NTSTATUS MoonMonDispatchClose(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    HANDLE curp = PsGetCurrentProcessId();
    KdPrint(("MoonMon: IRP_MJ_CLOSE:%llu/%u\n", globals->clientpid, HandleToULong(curp)));
    globals->clientpid = -1;
    PPCACHE cached = GetCachedProcess(globals, curp);
    PPROC_INFO PI = NULL;
    BOOLEAN foundProc = FALSE;
    if (MM_TYPE_CHECK(cached, CHARON)) {
        cached->InUse = TRUE;
        if (MM_TYPE_CHECK(cached->PI, PHOEBE)) {
            PI = cached->PI;
            foundProc = TRUE;
        }
    }
    
    if (foundProc && VALID_PAGE(PI->ImageFileName) && PI->ImageFileName->Length > 0) {
        KdPrint(("Client connected:%wZ\n", PI->ImageFileName));
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

/*
Runs at driver startup.
Initializes global variables, calls functions to load the config, register callbacks and enumerate system state.
*/
NTSTATUS InitMoonMon(_In_ PDRIVER_OBJECT   DriverObject,_In_ PUNICODE_STRING RegistryPath,_In_ PDEVICE_OBJECT deviceObject) {

    globals = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(struct GLOBAL_CONTEXT), MAGIC);
    if (globals == NULL) {
        KdPrint(("initMoonMon allocation failure!\n"));
        return STATUS_NO_MEMORY;
    }
    memset(globals, 0, sizeof(struct GLOBAL_CONTEXT));
    NTSTATUS status = initConfigLists(globals);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to initialize config lists\n"));
        return status;
        
    }
   /*
   globals->T_PROC =(PRTL_AVL_TABLE) ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(RTL_AVL_TABLE), CHRN);
   if (globals->T_PROC == NULL) {
       KdPrint(("initMoonMon allocation failure!\n"));
       return STATUS_NO_MEMORY;
   }*/
   globals->DRIVER_KEY_PATH = RegistryPath;
   globals->REG_DATA_MAX_SIZE = 32000;
   globals->clientpid = -1;
   globals->clientppid = -1;
   globals->Driver = DriverObject;
   globals->Device = deviceObject;
   globals->L_SIZE = 0;
   globals->L_SIZE_BYTES = 0;
   globals->PBL_COUNT = 0;
   globals->PINC_COUNT = 0;
   globals->PEXC_COUNT = 0;
   InitializeListHead(&globals->L);
   KeInitializeSpinLock(&globals->LLOCK);
   KeInitializeSpinLock(&globals->PCACHE_LOCK);
   globals->LOCKS[0] = &globals->PCACHE_LOCK;
   globals->LOCKS[1] = &globals->LLOCK;
   globals->LOCKS_SIZE = 2;
   globals->BFENotifyHandle = NULL;

   globals->PROCESS_CALLBACK = TRUE;
   globals->THREAD_CALLBACK = TRUE;
   globals->MODULE_CALLBACK = TRUE;
   globals->OBJECT_CALLBACK = TRUE;
   globals->REGISTRY_CALLBACK = TRUE;
   globals->FILE_CALLBACK = TRUE;
   globals->WFP_CALLBACK = TRUE;

   globals->KEEP_EXCLUDES = FALSE;
   /*RtlInitializeGenericTableAvl(
       globals->T_PROC,
       compareProcCache,
       newProcCache,
       freeProcCache,
       NULL
   );*/
   LONGLONG Timestamp = _ts();
   /*
   log id's are generated randomly at init time
   but they're sequential until the driver is unloaded.
   the _id() helper uses interlocked addition to get the latest id
   in a thread-safe manner.
   */
   globals->id = MM_Genrand(&((ULONG)Timestamp));
   // Globals init is done after this point!
   _(globals, GANYMEDE);

    status = LoadConfig(globals,RegistryPath);
    if (!NT_SUCCESS(status)) {
        KdPrint(("loadConfig failed!\n"));
        return STATUS_INVALID_PARAMETER;
    }
    //LoadKsecdd(globals); // Sid lookup hangs/prevents-boot without this :(
    if (globals->PROCESS_CALLBACK) {
        startupKillBlockedProcesses();
    }
    status = registerCallbacks();
    if (NT_SUCCESS(status)) {
       globals->State = INITIALIZED;
    }
    return status;
}