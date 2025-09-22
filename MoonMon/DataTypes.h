#pragma once
//#define RTL_USE_AVL_TABLES 1
#include <ntdef.h>
#include <wdm.h>
#include <ntddk.h>

#define DEBUG 0
#define RELEASE 1
#define MM_BUILD DEBUG

#if MM_BUILD == DEBUG
#define MM_EXCEPTION_MODE EXCEPTION_CONTINUE_SEARCH
#else
#define MM_EXCEPTION_MODE EXCEPTION_EXECUTE_HANDLER
#endif


// Generic constants


#define BLOCKLIST_MAX 32000
#define MM_HEADER  "MOONMON"
#define PROCESS_TERMINATE (0x0001) 
#define PROCESS_QUERY_INFORMATION (0x400)
#define PROCESS_QUERY_LIMITED_INFORMATION (0x1000)
#define PROCESS_VM_READ (0x10)

// type check magic values for the '_' member
#define GANYMEDE "GANYMEDE"
#define UMBRIEL  "UMBRIEL0"
#define MAKEMAKE "MAKEMAKE"
#define PHOEBE   "PHOEBE00"
#define DEIMOS   "DEIMOS00"
#define DIONE    "DIONE000"
#define PHOBOS   "PHOBOS00"
#define EUROPA   "EUROPA00"
#define NEREID   "NEREID00"
#define CHARON   "CHARON00"
#define OBERON   "OBERON00"
#define CALLISTO "CALLISTO"
#define CALIBAN  "CALIBAN0"
#define IAPETUS  "IAPETUS0"
#define MNEME    "MNEME000"
// Pool tags
// Eventually, all data flow paths should use a unique tag
#define MAGIC 'MOON' // Default
#define IO 'IO00'
#define IO1 'IO01'
#define IO2 'IO02'
#define IO3 'IO03'
#define IO4 'IO04'

#define IO5 'IO05'

#define GNMD 'GNMD'
#define TITAN 'TITN'
#define RHEA 'RHEA'
#define ERSA 'ERSA'
#define EUPORIE 'EPRE'
#define PHBE 'PHBE'
#define MKMK 'MKMK'
#define CLBN 'CLBN'
#define MNME 'MNME'
#define CHRN 'CHRN'
#define CSTO 'CSTO'
// Lock types
#define MM_LOCK_SPIN_LOCK 0x91
#define MM_LOCK_FAST_MUTEX 0x92

// States

#define INITIALIZED 0xaa
#define UNREGISTERING 0xab

// Altitudes
#define FS_FILTER_ALTITUDE L"429998"
#define REGISTRY_ALTITUDE L"299998.31415"
#define OB_ALTITUDE L"0.031415"


#define STACK_TRACE_SIZE 16

#define LIST_SIZE_MAX_BYTES 200000000
#define LIST_PURGE_DIVISOR 8
#define LIST_KEEP_MAX  (LIST_SIZE_MAX_BYTES - (LIST_SIZE_MAX_BYTES / LIST_PURGE_DIVISOR))
#define LIST_PURGE_CONGRUENT (LIST_KEEP_MAX/20)

#define CONFIG_GROUP_MAX 256
#define PBL_ENTRIES_MAX 65535
#define PINC_ENTRIES_MAX 65535
#define PEXC_ENTRIES_MAX 65535
#define PTINC_ENTRIES_MAX 65535
#define PTEXC_ENTRIES_MAX 65535
#define TBL_ENTRIES_MAX 65535
#define TINC_ENTRIES_MAX 65535
#define TEXC_ENTRIES_MAX 65535
#define MBL_ENTRIES_MAX 65535
#define MINC_ENTRIES_MAX 65535
#define MEXC_ENTRIES_MAX 65535
#define OBL_ENTRIES_MAX 65535
#define OINC_ENTRIES_MAX 65535
#define OEXC_ENTRIES_MAX 65535
#define RBL_ENTRIES_MAX 65535
#define RINC_ENTRIES_MAX 65535
#define REXC_ENTRIES_MAX 65535
#define FCBL_ENTRIES_MAX 65535
#define FCINC_ENTRIES_MAX 65535
#define FCEXC_ENTRIES_MAX 65535
#define FSBL_ENTRIES_MAX 65535
#define FSINC_ENTRIES_MAX 65535
#define FSEXC_ENTRIES_MAX 65535
#define WBL_ENTRIES_MAX 65535
#define WINC_ENTRIES_MAX 65535
#define WEXC_ENTRIES_MAX 65535

#define NEW_PROCS_MAX 32000

// Event types
#define PROCESS_CREATION 0x01
#define MM_FILE_CREATE_TIME 0x2
#define NETWORK_CONNECTION 0x3
#define MM_MESSAGE 0x4
#define PROCESS_TERMINATED 0x05
#define DRIVER_LOADED 0x6
#define IMAGE_LOADED 0x7
#define CREATE_REMOTE_THREAD 0x8
#define RAW_ACCESS_READ 0x9
#define PROCESS_ACCESS 0x10
#define MM_FILE_CREATED 0x0a
#define REGISTRY_EVENT 0x12
#define MM_FILE_SET_INFO 0x13
#define MM_NETWORK_RESOURCE 0x14

// Field types
#define MATCH_ID 0x1d
#define TIMESTAMP 0x80
#define IMAGE_FILE_NAME 0x81
#define COMMAND_LINE 0x82
#define PID 0x83
#define PARENT_PID 0x84
#define PARENT_IMAGE_FILE_NAME 0x85
#define PARENT_COMMAND_LINE 0x86
#define PCWD 0x87
#define PFLAGS 0x88
#define PWINDOW_TITLE 0x89
#define CREATING_THREAD_ID 0x90
#define CREATION_STATUS 0x91
#define THREAD_ID 0x92
#define LOADED_MODULE 0x93
#define MODULE_PROPERTIES 0x94
#define TARGET_IMAGE_FILE_NAME 0x95
#define TARGET_COMMAND_LINE 0x96
#define TARGET_PID 0x97
#define PHFLAGS 0x98
#define PHOPERATION 0x99
#define PHDESIRED_ACCESS 0x9a
#define PHORIGINAL_DESIRED_ACCESS 0x9b
#define REG_KEY 0x9c
#define REG_KEY_NEW 0x9d
#define REG_VALUE_NAME 0x9e
#define REG_DATA 0x9f
#define REG_OP 0xb0
#define REG_OLD_FILE_NAME 0xb1
#define REG_NEW_FILE_NAME 0xb2
#define REG_DATA_TYPE 0xb3
#define PROCESS_SID 0xb4
#define MM_FILE_NAME 0xb5
#define MM_FILE_VOLUME 0xb6
#define MM_FILE_EXTENSION 0xb7
#define MM_FILE_SHARE 0xb8
#define MM_FILE_STREAM 0xb9
#define MM_FILE_FINAL_COMPONENT 0xba
#define MM_FILE_PARENT_DIR 0xbb
#define MM_FILE_FLAGS 0xbc
#define MM_FILE_CREATION_TIME 0xbd
#define MM_FILE_CHANGE_TIME 0xbe
#define MM_FILE_ACCESS_FLAGS 0xbf
#define MM_FILE_NEW_NAME 0xc0
#define MM_FILE_NETWORK_PROTOCOL 0xc1
#define MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR 0xc2
#define MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR 0xc3
#define MM_NAMED_PIPE_TYPE 0xc4
#define MM_NAMED_PIPE_CONFIG 0xc5
#define MM_NAMED_PIPE_STATE 0xc6
#define MM_NAMED_PIPE_END 0xc7
#define MM_FILE_DELETION 0xc8
#define MM_FILE_OPERATION 0xc9
#define MM_WFP_LAYER 0xca
#define MM_NET_DIRECTION 0xcb
#define MM_NET_IP_PROTOCOL 0xcc
#define MM_NET_ADDRESS_TYPE 0xcd
#define MM_NET_PROMISCUOUS 0xce
#define MM_NET_LOCAL_PORT 0xcf
#define MM_NET_REMOTE_PORT 0xd0
#define MM_NET_LOCAL_IPV4_ADDR 0xd1
#define MM_NET_REMOTE_IPV4_ADDR 0xd2
#define MM_NET_INTERFACE_TYPE 0xd3
#define MM_NET_INTERFACE 0xd4
#define MM_NET_LOCAL_IPV6_ADDR 0xd5
#define MM_NET_REMOTE_IPV6_ADDR 0xd6
#define TARGET_PARENT_IMAGE_FILE_NAME 0xd7
#define TARGET_PARENT_COMMAND_LINE 0xd8
#define TARGET_PWINDOW_TITLE 0xd9
#define TARGET_PCWD 0xda
#define SID_DATA 0xdb
#define ACCOUNT_NAME 0xdc
#define ACCOUNT_DOMAIN 0xdd

#define LOG_FLAGS 0xFE
#define EVENT_TYPE 0XFF
#define EVENT_END 0XFC

// Field data types
typedef enum MM_DATA_TYPE {
   MM_BAD_TYPE, 
   MM_META,
   MM_ENUM,
   MM_BOOL,
   MM_UINT8,
   MM_INT8,
   MM_UINT16,
   MM_INT16,
   MM_UINT32,
   MM_INT32,
   MM_UINT64,
   MM_INT64,
   MM_IPV4,
   MM_IPV6,
   MM_UNICODE
} MM_TYPE;


// process params offset in PEB
#define OFFSET_IMAGE_FILE_NAME 0x60
#define OFFSET_COMMAND_LINE 0x70
#define OFFSET_CWD 0x38
#define OFFSET_WINDOW_TITLE 0xB0
#define OFFSET_PROCESS_PARAMS 0x20

// config entry list types
#define LIST_COUNT 26

#define PROCESS_BLOCK_LIST 0x10
#define PROCESS_INCLUDE_LIST 0xd0
#define PROCESS_EXCLUDE_LIST 0xd1
#define PROCESS_TERMINATED_INCLUDE_LIST 0xd2
#define PROCESS_TERMINATED_EXCLUDE_LIST 0xd3
#define THREAD_BLOCK_LIST 0xd4
#define THREAD_INCLUDE_LIST 0xd5
#define THREAD_EXCLUDE_LIST 0xd6
#define MODULE_BLOCK_LIST 0xd7
#define MODULE_INCLUDE_LIST 0xd8
#define MODULE_EXCLUDE_LIST 0xd9
#define OBJECT_BLOCK_LIST 0xda
#define OBJECT_INCLUDE_LIST 0xdb
#define OBJECT_EXCLUDE_LIST 0xdc
#define REGISTRY_BLOCK_LIST 0xdd
#define REGISTRY_INCLUDE_LIST 0xde
#define REGISTRY_EXCLUDE_LIST 0xdf
#define FILE_CREATE_BLOCK_LIST 0xe0
#define FILE_CREATE_INCLUDE_LIST 0xe1
#define FILE_CREATE_EXCLUDE_LIST 0xe2
#define FILE_SET_INFO_BLOCK_LIST 0xe3
#define FILE_SET_INFO_INCLUDE_LIST 0xe4
#define FILE_SET_INFO_EXCLUDE_LIST 0xe5
#define WFP_BLOCK_LIST 0xe6
#define WFP_INCLUDE_LIST 0xe7
#define WFP_EXCLUDE_LIST 0xe8

// match logic operators

#define STARTS_WITH 0x08
#define NOT_STARTS_WITH 0x80
#define ENDS_WITH 0x09
#define NOT_ENDS_WITH 0x90
#define CONTAINS 0xc0
#define NOT_CONTAINS 0x0c
#define EQUALS 0xe0
#define NOT_EQUALS 0x0e
#define AND 0xf0
#define NOT_AND 0x0f
#define GREATER_THAN 0xde
#define NOT_GREATER_THAN 0xed
#define GREAT_OR_EQUAL 0x42
#define LESS_THAN 0x37
#define NOT_LESS_THAN 0x73
#define LESS_OR_EQUAL 0xee

// actions. these should correspond to NTSTATUS values

#define MM_STATUS_ACCESS_DENIED 0xC0000022
#define MM_STATUS_NOT_FOUND 0xC0000225
#define MM_STATUS_PENDING 0x00000103


// structs;


typedef struct KEY_VALUE {
    USHORT key;
    wchar_t* value;
}KV, * PKV;

typedef struct LOG_ENTRY {
    LIST_ENTRY Link;
    ULONG size;
    char *data;
    UINT64 _; // EUROPA
} LOG_ENTRY, *PLOG_ENTRY;

/*
CONFIG_ENTRY/CFG/PCFG: Used by Config.c to parse config entries
CONFIG_ITEM/CFI/PCFI: discrete config items that may be part of a config group,
  a rule to block powershell.exe with a commandline ABC will have two CFI's under the same CONFIG_GROUP,
  which comprises the whole rule after being parsed.
CONFIG_GROUP - each of these objects contain an entries array which contain CONFIG_ITEM/CFI's under a shared id.
  count is the number of such sub-items and action is the desired verdict or decision when there is a match.
  for each type of config list (e.g.: PROCESS_BLOCK_LIST) there is an array of CONFIG_GROUP in the global context,
  which is populated after parsing the configs at load time.
*/
typedef struct CONFIG_ENTRY {

    ULONG size;
    USHORT type;
    USHORT field_type;
    unsigned char match_type;
    ULONG id;
    ULONG actions;
    char* data;
    UINT64 _;
} CFG, * PCFG;

typedef struct CONFIG_ITEM {
    unsigned char match_type;
    BOOLEAN itemBoolean;
    INT8 itemInt8;
    UINT8 itemUint8, ipv6Prefix;
    USHORT field_type;
    USHORT itemUshort;
    ULONG itemUlong, ipv4Subnet, ipv4SubnetMask;
    LONGLONG itemLonglong;
    UINT8* ipv6Subnet;
    PUNICODE_STRING item;
    UINT64 _; //CALLISTO
} CFI, *PCFI;

typedef struct _CONFIG_GROUP {
    CFI *entries[CONFIG_GROUP_MAX];
    ULONG count;
    ULONG id;
    ULONG actions;
    UINT64 _;//CALIBAN
} CONFIG_GROUP;
/*
Below are event-specific structs.
They contain a member '_' to help identify memory related bugs and
facilitate asserts (data validity assumptions).
*/

typedef struct USER_INFO {
    PUCHAR sid_data;
    UCHAR sid_data_length;
    PUNICODE_STRING name,domain;
    UINT64 _; //MNEME
} UINF,*PUINF;

typedef struct PROCESS_INFO {
    UNICODE_STRING* ImageFileName;
    UNICODE_STRING* ParentImageFileName;
    UNICODE_STRING* CommandLine;
    UNICODE_STRING* ParentCommandLine;
    UNICODE_STRING* WindowTitle;
    UNICODE_STRING* CWD;
    PUINF UI;
    UINT64 _; //PHOEBE
} PROC_INFO, * PPROC_INFO;

typedef struct THREAD_CREATION_INFO {
    PPROC_INFO PI, TPI;
    UINT64 _; //IAPETUS
} TC_INFO, * PTC_INFO;

typedef struct MODULE_INFO {
    UNICODE_STRING* ImageLoaded;
    ULONG ImageProperties;
    PPROC_INFO PI;
    UINT64 _; //MAKEMAKE
} MODULE_INFO, * PMODULE_INFO;

typedef struct REGISTRY_INFO {
    ULONG notifyClass;
    UNICODE_STRING* RegistryKey;
    UNICODE_STRING* RegistryKeyNew;
    UNICODE_STRING* RegistryValueName;
    UNICODE_STRING* NewFileName;
    UNICODE_STRING* OldFilename;
    PVOID Data;
    ULONG DataSize;
    ULONG DataType;
    PPROC_INFO PI;
    UINT64 _; //UMBREON
} REG_INFO,*PREG_INFO;

typedef struct PROCESS_OBJECT_INFO {
    ULONG Operation;
    ULONG Flags;
    ACCESS_MASK DesiredAccess;
    ACCESS_MASK OriginalDesiredAccess;

    PPROC_INFO PI,TPI;

    UINT64 _; //OBERON
} OBPROC, * POBPROC;

typedef struct FS_FILE_INFORMATION {
    ULONG CreateOptions;
    UNICODE_STRING* Name;
    UNICODE_STRING* Volume;
    UNICODE_STRING* Share;
    UNICODE_STRING* Extension;
    UNICODE_STRING* Stream;
    UNICODE_STRING* FinalComponent;
    UNICODE_STRING* ParentDir;
    PPROC_INFO PI;
    UINT64 _; // DEIMOS
} FILE_INFO, *PFILE_INFO;

typedef struct FS_SET_FILE_INFORMATION {
    ULONG Operation;
    UNICODE_STRING* Name;
    UNICODE_STRING* Volume;
    UNICODE_STRING* Share;
    UNICODE_STRING* Extension;
    UNICODE_STRING* Stream;
    UNICODE_STRING* FinalComponent;
    UNICODE_STRING* ParentDir;
    PPROC_INFO PI;
   
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER ChangeTime;
    ACCESS_MASK  AccessFlags;
    UNICODE_STRING* NewName;
    ULONG Protocol;
    USHORT ProtocolMajorVersion;
    USHORT ProtocolMinorVersion;
    ULONG NamedPipeType, NamedPipeConfiguration, NamedPipeState, NamedPipeEnd;
    BOOLEAN DeleteFile;
    UINT64 _; //DIONE
} FILE_SET_INFO, * PFILE_SET_INFO;

typedef struct WFP_CLASSIFY_INFORMATION {
    INT8 Direction;
    UINT8 ipProtocol;
    UINT8 addrType;
    UINT8 promiscuous;
    UINT16 layer,localPort, remotePort;
    UINT32 localIpv4Addr, remoteIpv4Addr, interfaceType;
    UINT64 interface;


    UINT8* localIpv6Addr, *remoteIpv6Addr;
    PPROC_INFO PI;
    UINT64 _; //NEREID
} WFP_INFO, *PWFP_INFO;


typedef struct PROCESS_CACHE_INFO {
    UINT64 hits;
    HANDLE p;
    BOOLEAN New;
    BOOLEAN InUse;
    PPROC_INFO PI;
    UINT64 _; //CHARON
} PCACHE,*PPCACHE;


/*
GLOBAL_CONTEXT/GC/PGC: the big long struct of global variables.
  This is a simple way of synchronizing data and state globally.
  There are more clever approaches but this is maintainable and has no performance impact,
  since we pass around pointers to one instance/object of the struct.
*/
typedef struct GLOBAL_CONTEXT {
    PDRIVER_OBJECT   Driver;
    PDEVICE_OBJECT   Device;
    USHORT State;
    BOOLEAN WFP_Registered;
    BOOLEAN KSECDD_LOADED;
    HANDLE BFENotifyHandle;
    KSPIN_LOCK LLOCK;
    KSPIN_LOCK PCACHE_LOCK;
    LARGE_INTEGER Cookie;
    _Guarded_by_(LLOCK)  LIST_ENTRY   L;

    BOOLEAN PROCESS_CALLBACK;
    BOOLEAN THREAD_CALLBACK;
    BOOLEAN MODULE_CALLBACK;
    BOOLEAN OBJECT_CALLBACK;
    BOOLEAN REGISTRY_CALLBACK;
    BOOLEAN FILE_CALLBACK;
    BOOLEAN WFP_CALLBACK;
    BOOLEAN KEEP_EXCLUDES;
    BOOLEAN RESIST_TAMPERING;
    PVOID LOCKS[32];
    UINT8 LOCKS_SIZE;
    PVOID ObRegHandle;
    _Guarded_by_(LLOCK) ULONG L_SIZE;
    _Guarded_by_(LLOCK) LONG L_SIZE_BYTES;

    


    CONFIG_GROUP** PBL; // Process block list
    CONFIG_GROUP** PINC;  // Process include list
    CONFIG_GROUP** PEXC; // Process exclude list
    ULONG PBL_COUNT;
    ULONG PINC_COUNT;
    ULONG PEXC_COUNT;

    CONFIG_GROUP** PTBL; // Process terminate block list
    CONFIG_GROUP** PTINC; // Process terminate include list
    CONFIG_GROUP** PTEXC; // Process terminate exclude list
    ULONG PTINC_COUNT;
    ULONG PTEXC_COUNT;

    CONFIG_GROUP** TBL; // Thread block list
    CONFIG_GROUP** TINC; // Thread include list
    CONFIG_GROUP** TEXC; // Thread exclude list
    ULONG TBL_COUNT;
    ULONG TINC_COUNT;
    ULONG TEXC_COUNT;

    CONFIG_GROUP** MBL; // Module load block list
    CONFIG_GROUP** MINC; // Module load include list
    CONFIG_GROUP** MEXC; // Module load exclude list
    ULONG MBL_COUNT;
    ULONG MINC_COUNT;
    ULONG MEXC_COUNT;

    CONFIG_GROUP** OBL; //  Object access block list
    CONFIG_GROUP** OINC; // Object access  include list
    CONFIG_GROUP** OEXC; // Object access  exclude list
    ULONG OBL_COUNT;
    ULONG OINC_COUNT;
    ULONG OEXC_COUNT;

    CONFIG_GROUP** RBL; // Registry event block list
    CONFIG_GROUP** RINC; // Registry event include list
    CONFIG_GROUP** REXC; // Registry event exclude list
    ULONG RBL_COUNT;
    ULONG RINC_COUNT;
    ULONG REXC_COUNT;

    CONFIG_GROUP** FCBL; // File create block list
    CONFIG_GROUP** FCINC; // File create include list
    CONFIG_GROUP** FCEXC; // File create exclude list
    ULONG FCBL_COUNT;
    ULONG FCINC_COUNT;
    ULONG FCEXC_COUNT;

    CONFIG_GROUP** FSBL; // File set information block list
    CONFIG_GROUP** FSINC; // File set information include list
    CONFIG_GROUP** FSEXC; // File set information exclude list
    ULONG FSBL_COUNT;
    ULONG FSINC_COUNT;
    ULONG FSEXC_COUNT;

    CONFIG_GROUP** WBL; // WFP/Network event block list
    CONFIG_GROUP** WINC; // WFP/Network event include list
    CONFIG_GROUP** WEXC; // WFP/Network event exclude list
    ULONG WBL_COUNT;
    ULONG WINC_COUNT;
    ULONG WEXC_COUNT;

    PUNICODE_STRING DRIVER_KEY_PATH;
    ULONG REG_DATA_MAX_SIZE;
    

    _Guarded_by_(PCACHE_LOCK)  PCACHE NEW_PROCS[NEW_PROCS_MAX];
    _Guarded_by_(PCACHE_LOCK)  ULONG NEW_PROCS_INDEX;
    _Guarded_by_(PCACHE_LOCK)  PVOID T_PROC;
    _Interlocked_ LONG64 id;
    LONGLONG clientpid,clientppid;
    UINT64 _; // GANYMEDE
} GC, *PGC;


// This is an opaque struct defined to facilitate process info lookup
typedef struct _SYSTEM_PROCESS_INFO_L
{

    ULONG                   NextEntryOffset;
    ULONG                   NumberOfThreads;
    LARGE_INTEGER           Reserved[3];
    LARGE_INTEGER           CreateTime;
    LARGE_INTEGER           UserTime;
    LARGE_INTEGER           KernelTime;
    UNICODE_STRING          ImageName;
    ULONG                   BasePriority;
    HANDLE                  ProcessId;
    HANDLE                  InheritedFromProcessId;
}_SYSTEM_PROCESS_INFO_L, * P_SYSTEM_PROCESS_INFO_L;