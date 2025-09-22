// Constant values and package-wide global variables are declared in this file
package MoonMon

// GLobal settings
var Settings map[string]string

var Fields_Max = 1024

// Field IDs, the values here must match
// what is defined in the driver's DataTypes.h file
var MATCH_ID = 0x1d
var TIMESTAMP = 0x80
var IMAGE_FILE_NAME = 0x81
var COMMAND_LINE = 0x82
var PID = 0x83
var PARENT_PID = 0x84
var PARENT_IMAGE_FILE_NAME = 0x85
var PARENT_COMMAND_LINE = 0x86
var PCWD = 0x87
var PFLAGS = 0x88
var PWINDOW_TITLE = 0x89
var CREATING_THREAD_ID = 0x90
var CREATION_STATUS = 0x91
var THREAD_ID = 0x92
var LOADED_MODULE = 0x93
var MODULE_PROPERTIES = 0x94
var TARGET_IMAGE_FILE_NAME = 0x95
var TARGET_COMMAND_LINE = 0x96
var TARGET_PID = 0x97
var PHFLAGS = 0x98
var PHOPERATION = 0x99
var PHDESIRED_ACCESS = 0x9a
var PHORIGINAL_DESIRED_ACCESS = 0x9b
var REG_KEY = 0x9c
var REG_KEY_NEW = 0x9d
var REG_VALUE_NAME = 0x9e
var REG_DATA = 0x9f
var REG_OP = 0xb0
var REG_OLD_FILE_NAME = 0xb1
var REG_NEW_FILE_NAME = 0xb2
var REG_DATA_TYPE = 0xb3
var PROCESS_SID = 0xb4
var MM_FILE_NAME = 0xb5
var MM_FILE_VOLUME = 0xb6
var MM_FILE_EXTENSION = 0xb7
var MM_FILE_SHARE = 0xb8
var MM_FILE_STREAM = 0xb9
var MM_FILE_FINAL_COMPONENT = 0xba
var MM_FILE_PARENT_DIR = 0xbb
var MM_FILE_FLAGS = 0xbc
var MM_FILE_CREATION_TIME = 0xbd
var MM_FILE_CHANGE_TIME = 0xbe
var MM_FILE_ACCESS_FLAGS = 0xbf
var MM_FILE_NEW_NAME = 0xc0
var MM_FILE_NETWORK_PROTOCOL = 0xc1
var MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR = 0xc2
var MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR = 0xc3
var MM_NAMED_PIPE_TYPE = 0xc4
var MM_NAMED_PIPE_CONFIG = 0xc5
var MM_NAMED_PIPE_STATE = 0xc6
var MM_NAMED_PIPE_END = 0xc7
var MM_FILE_DELETION = 0xc8
var MM_FILE_OPERATION = 0xc9
var MM_WFP_LAYER = 0xca
var MM_NET_DIRECTION = 0xcb
var MM_NET_IP_PROTOCOL = 0xcc
var MM_NET_ADDRESS_TYPE = 0xcd
var MM_NET_PROMISCUOUS = 0xce
var MM_NET_LOCAL_PORT = 0xcf
var MM_NET_REMOTE_PORT = 0xd0
var MM_NET_LOCAL_IPV4_ADDR = 0xd1
var MM_NET_REMOTE_IPV4_ADDR = 0xd2
var MM_NET_INTERFACE_TYPE = 0xd3
var MM_NET_INTERFACE = 0xd4
var MM_NET_LOCAL_IPV6_ADDR = 0xd5
var MM_NET_REMOTE_IPV6_ADDR = 0xd6
var TARGET_PARENT_IMAGE_FILE_NAME = 0xd7
var TARGET_PARENT_COMMAND_LINE = 0xd8
var TARGET_PWINDOW_TITLE = 0xd9
var TARGET_PCWD = 0xda
var SID_DATA = 0xdb
var ACCOUNT_NAME = 0xdc
var ACCOUNT_DOMAIN = 0xdd

var LOG_FLAGS = 0xfe
var EVENT_TYPE = 0xff
var EVENT_END = 0xfc

// Event types
var PROCESS_CREATION = 0x01
var MM_FILE_CREATE_TIME = 0x2
var NETWORK_CONNECTION = 0x3
var MM_MESSAGE = 0x4
var PROCESS_TERMINATED = 0x05
var DRIVER_LOADED = 0x6
var IMAGE_LOADED = 0x7
var CREATE_REMOTE_THREAD = 0x8
var RAW_ACCESS_READ = 0x9
var PROCESS_ACCESS = 0x10
var MM_FILE_CREATED = 0x0a
var REGISTRY_EVENT = 0x12
var MM_FILE_SET_INFO = 0x13
var MM_NETWORK_RESOURCE = 0x14

// match logic/operators

var STARTS_WITH = 0x08
var NOT_STARTS_WITH = 0x80
var ENDS_WITH = 0x09
var NOT_ENDS_WITH = 0x90
var CONTAINS = 0xc0
var NOT_CONTAINS = 0x0c
var EQUALS = 0xe0
var NOT_EQUALS = 0x0e
var AND = 0xf0
var NOT_AND = 0x0f
var GREATER_THAN = 0xde
var NOT_GREATER_THAN = 0xed
var GREAT_OR_EQUAL = 0x42
var LESS_THAN = 0x37
var NOT_LESS_THAN = 0x73
var LESS_OR_EQUAL = 0xee

// actions, these should correspond to NTSTATUS values

var MM_STATUS_ACCESS_DENIED = 0xC0000022
var MM_STATUS_NOT_FOUND = 0xC0000225
var MM_STATUS_PENDING = 0x00000103

// config entry list types

var PROCESS_BLOCK_LIST = 0x10
var PROCESS_INCLUDE_LIST = 0xd0
var PROCESS_EXCLUDE_LIST = 0xd1
var PROCESS_TERMINATED_INCLUDE_LIST = 0xd2
var PROCESS_TERMINATED_EXCLUDE_LIST = 0xd3
var THREAD_BLOCK_LIST = 0xd4
var THREAD_INCLUDE_LIST = 0xd5
var THREAD_EXCLUDE_LIST = 0xd6
var MODULE_BLOCK_LIST = 0xd7
var MODULE_INCLUDE_LIST = 0xd8
var MODULE_EXCLUDE_LIST = 0xd9
var OBJECT_BLOCK_LIST = 0xda
var OBJECT_INCLUDE_LIST = 0xdb
var OBJECT_EXCLUDE_LIST = 0xdc
var REGISTRY_BLOCK_LIST = 0xdd
var REGISTRY_INCLUDE_LIST = 0xde
var REGISTRY_EXCLUDE_LIST = 0xdf
var FILE_CREATE_BLOCK_LIST = 0xe0
var FILE_CREATE_INCLUDE_LIST = 0xe1
var FILE_CREATE_EXCLUDE_LIST = 0xe2
var FILE_SET_INFO_BLOCK_LIST = 0xe3
var FILE_SET_INFO_INCLUDE_LIST = 0xe4
var FILE_SET_INFO_EXCLUDE_LIST = 0xe5
var WFP_BLOCK_LIST = 0xe6
var WFP_INCLUDE_LIST = 0xe7
var WFP_EXCLUDE_LIST = 0xe8

// A map of registry value name prefixes and
// The corresponding list's value
var Lists = map[int]string{
	PROCESS_BLOCK_LIST:              "CFG_PBL_",
	PROCESS_INCLUDE_LIST:            "CFG_PINC_",
	PROCESS_EXCLUDE_LIST:            "CFG_PEXC_",
	PROCESS_TERMINATED_INCLUDE_LIST: "CFG_PTINC_",
	PROCESS_TERMINATED_EXCLUDE_LIST: "CFG_PTEXC_",
	THREAD_BLOCK_LIST:               "CFG_TBL_",
	THREAD_INCLUDE_LIST:             "CFG_TINC_",
	THREAD_EXCLUDE_LIST:             "CFG_TEXC_",
	MODULE_BLOCK_LIST:               "CFG_MBL_",
	MODULE_INCLUDE_LIST:             "CFG_MINC_",
	MODULE_EXCLUDE_LIST:             "CFG_MEXC",
	OBJECT_BLOCK_LIST:               "CFG_OBL_",
	OBJECT_INCLUDE_LIST:             "CFG_OINC_",
	OBJECT_EXCLUDE_LIST:             "CFG_OEXC_",
	REGISTRY_BLOCK_LIST:             "CFG_RBL_",
	REGISTRY_INCLUDE_LIST:           "CFG_RINC_",
	REGISTRY_EXCLUDE_LIST:           "CFG_REXC_",
	FILE_CREATE_BLOCK_LIST:          "CFG_FCBL_",
	FILE_CREATE_INCLUDE_LIST:        "CFG_FCINC_",
	FILE_CREATE_EXCLUDE_LIST:        "CFG_FCEXC_",
	FILE_SET_INFO_BLOCK_LIST:        "CFG_FSBL_",
	FILE_SET_INFO_INCLUDE_LIST:      "CFG_FSINC_",
	FILE_SET_INFO_EXCLUDE_LIST:      "CFG_FSEXC_",
	WFP_BLOCK_LIST:                  "CFG_WBL_",
	WFP_INCLUDE_LIST:                "CFG_WINC_",
	WFP_EXCLUDE_LIST:                "CFG_WEXC_",
}

// Match operators
var Conditions = map[int]string{
	STARTS_WITH:      "startswith",
	NOT_STARTS_WITH:  "notstartswith",
	ENDS_WITH:        "endswith",
	NOT_ENDS_WITH:    "notendswith",
	CONTAINS:         "contains",
	NOT_CONTAINS:     "notcontains",
	EQUALS:           "equals",
	NOT_EQUALS:       "notequals",
	AND:              "and",
	NOT_AND:          "notand",
	GREATER_THAN:     "greaterthan",
	NOT_GREATER_THAN: "notgreaterthan",
	GREAT_OR_EQUAL:   "greaterorequal",
	LESS_THAN:        "lessthan",
	NOT_LESS_THAN:    "notlessthan",
	LESS_OR_EQUAL:    "lessorequal",
}

// Alternative match operators, the more the merrier! :)
var Conditions_Alt = map[int]string{
	STARTS_WITH:      "starts_with",
	NOT_STARTS_WITH:  "not_starts_with",
	ENDS_WITH:        "ends_with",
	NOT_ENDS_WITH:    "not_ends_with",
	CONTAINS:         "in",
	NOT_CONTAINS:     "not_in",
	EQUALS:           "==",
	NOT_EQUALS:       "!=",
	AND:              "&",
	NOT_AND:          "!&",
	GREATER_THAN:     ">",
	NOT_GREATER_THAN: "!>",
	GREAT_OR_EQUAL:   ">=",
	LESS_THAN:        "<",
	NOT_LESS_THAN:    "!<",
	LESS_OR_EQUAL:    "<=",
}
var Actions = map[int]string{

	MM_STATUS_ACCESS_DENIED: "accessdenied",
	MM_STATUS_NOT_FOUND:     "notfound",
	MM_STATUS_PENDING:       "pending",
}

var Events = map[int]string{
	PROCESS_CREATION:     "PROCESS_CREATION",
	MM_FILE_CREATE_TIME:  "FILE_CREATE_TIME",
	NETWORK_CONNECTION:   "NETWORK_CONNECTION",
	MM_MESSAGE:           "SYSTEM_MESSAGE",
	PROCESS_TERMINATED:   "PROCESS_TERMINATED",
	DRIVER_LOADED:        "DRIVER_LOADED",
	IMAGE_LOADED:         "IMAGE_LOADED",
	CREATE_REMOTE_THREAD: "CREATE_REMOTE_THREAD",
	RAW_ACCESS_READ:      "RAW_ACCESS_READ",
	PROCESS_ACCESS:       "PROCESS_ACCESS",
	MM_FILE_CREATED:      "FILE_CREATED",
	REGISTRY_EVENT:       "REGISTRY_EVENT",
	MM_FILE_SET_INFO:     "FILE_SET_INFORMATION",
	MM_NETWORK_RESOURCE:  "NETWORK_RESOURCE_EVENT",
}

// Field names for translating binary log fields
// Into human-readable field names such as with JSON logging.
var Fields = map[int]string{
	MATCH_ID:                               "RULE_ID",
	TIMESTAMP:                              "Timestamp",
	IMAGE_FILE_NAME:                        "IMAGE_FILE_NAME",
	COMMAND_LINE:                           "COMMAND_LINE",
	PID:                                    "PID",
	PARENT_PID:                             "PARENT_PID",
	PARENT_IMAGE_FILE_NAME:                 "PARENT_IMAGE_FILE_NAME",
	PARENT_COMMAND_LINE:                    "PARENT_COMMAND_LINE",
	PCWD:                                   "CURRENT_WORKING_DIRECTORY",
	PFLAGS:                                 "PROCESS_FLAGS",
	PWINDOW_TITLE:                          "WINDOW_TITLE",
	CREATING_THREAD_ID:                     "CREATING_THREAD_ID",
	CREATION_STATUS:                        "CREATION_STATUS",
	THREAD_ID:                              "THREAD_ID",
	LOADED_MODULE:                          "LOADED_MODULE",
	MODULE_PROPERTIES:                      "MODULE_PROPERTIES",
	TARGET_IMAGE_FILE_NAME:                 "TARGET_IMAGE_FILE_NAME",
	TARGET_COMMAND_LINE:                    "TARGET_COMMAND_LINE",
	TARGET_PID:                             "TARGET_PID",
	PHFLAGS:                                "PROCESS_HANDLE_FLAGS",
	PHOPERATION:                            "PROCESS_HANDLE_OPERATION",
	PHDESIRED_ACCESS:                       "PROCESS_HANDLE_DESIRED_ACCESS",
	PHORIGINAL_DESIRED_ACCESS:              "PROCESS_HANDLE_ORIGINAL_DESIRED_ACCESS",
	REG_KEY:                                "REGISTRY_KEY",
	REG_KEY_NEW:                            "REGISTRY_KEY_NEW",
	REG_VALUE_NAME:                         "REGISTRY_VALUE_NAME",
	REG_DATA:                               "REGISTRY_DATA",
	REG_OP:                                 "REGISTRY_OPERATION",
	REG_OLD_FILE_NAME:                      "REGISTRY_OLD_FILE_NAME",
	REG_NEW_FILE_NAME:                      "REGISTRY_NEW_FILE_NAME",
	REG_DATA_TYPE:                          "REGISTRY_DATA_TYPE",
	PROCESS_SID:                            "PROCESS_SID",
	MM_FILE_NAME:                           "FILE_NAME",
	MM_FILE_VOLUME:                         "FILE_VOLUME",
	MM_FILE_EXTENSION:                      "FILE_EXTENSION",
	MM_FILE_SHARE:                          "FILE_SHARE",
	MM_FILE_STREAM:                         "FILE_STREAM",
	MM_FILE_FINAL_COMPONENT:                "FILE_FINAL_COMPONENT",
	MM_FILE_PARENT_DIR:                     "FILE_PARENT_DIR",
	MM_FILE_FLAGS:                          "FILE_FLAGS",
	MM_FILE_CREATION_TIME:                  "FILE_CREATION_TIME",
	MM_FILE_CHANGE_TIME:                    "FILE_CHANGE_TIME",
	MM_FILE_ACCESS_FLAGS:                   "FILE_ACCESS_FLAGS",
	MM_FILE_NEW_NAME:                       "FILE_NEW_NAME",
	MM_FILE_NETWORK_PROTOCOL:               "FILE_NETWORK_PROTOCOL",
	MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR: "FILE_NETWORK_PROTOCOL_VERSION_MAJOR",
	MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR: "FILE_NETWORK_PROTOCOL_VERSION_MINOR",
	MM_NAMED_PIPE_TYPE:                     "NAMED_PIPE_TYPE",
	MM_NAMED_PIPE_CONFIG:                   "NAMED_PIPE_CONFIG",
	MM_NAMED_PIPE_STATE:                    "NAMED_PIPE_STATE",
	MM_NAMED_PIPE_END:                      "NAMED_PIPE_END",
	MM_FILE_DELETION:                       "FILE_DELETION",
	MM_FILE_OPERATION:                      "FILE_OPERATION",
	MM_WFP_LAYER:                           "WFP_LAYER",
	MM_NET_DIRECTION:                       "NET_DIRECTION",
	MM_NET_IP_PROTOCOL:                     "NET_IP_PROTOCOL",
	MM_NET_ADDRESS_TYPE:                    "NET_ADDRESS_TYPE",
	MM_NET_PROMISCUOUS:                     "NET_PROMISCUOUS",
	MM_NET_LOCAL_PORT:                      "NET_LOCAL_PORT",
	MM_NET_REMOTE_PORT:                     "NET_REMOTE_PORT",
	MM_NET_LOCAL_IPV4_ADDR:                 "NET_LOCAL_IPV4_ADDR",
	MM_NET_REMOTE_IPV4_ADDR:                "NET_REMOTE_IPV4_ADDR",
	MM_NET_INTERFACE_TYPE:                  "NET_INTERFACE_TYPE",
	MM_NET_INTERFACE:                       "NET_INTERFACE",
	MM_NET_LOCAL_IPV6_ADDR:                 "NET_LOCAL_IPV6_ADDR",
	MM_NET_REMOTE_IPV6_ADDR:                "NET_REMOTE_IPV6_ADDR",
	TARGET_PARENT_IMAGE_FILE_NAME:          "TARGET_PARENT_IMAGE_FILE_NAME",
	TARGET_PARENT_COMMAND_LINE:             "TARGET_PARENT_COMMAND_LINE",
	TARGET_PWINDOW_TITLE:                   "TARGET_WINDOW_TITLE",
	TARGET_PCWD:                            "TARGET_CURRENT_WORKING_DIRECTORY",
	SID_DATA:                               "SID",
	ACCOUNT_NAME:                           "ACCOUNT",
	ACCOUNT_DOMAIN:                         "ACCOUNT_DOMAIN",

	EVENT_TYPE: "EVENT_TYPE",
	EVENT_END:  "EVENT_END",
	LOG_FLAGS:  "LOG_FLAGS",
}

// FILE_INFORMATIN_CLASS enum values, this includes operations such as file renaming,deletion,
// named pipe activity, attribute changes and much more. Not all fields are well-tested.
// See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ne-wdm-_file_information_class
var FILEOPS = map[int]string{
	1:  "FileDirectoryInformation",
	2:  "FileFullDirectoryInformation",
	3:  "FileBothDirectoryInformation",
	4:  "FileBasicInformation",
	5:  "FileStandardInformation",
	6:  "FileInternalInformation",
	7:  "FileEaInformation",
	8:  "FileAccessInformation",
	9:  "FileNameInformation",
	10: "FileRenameInformation",
	11: "FileLinkInformation",
	12: "FileNamesInformation",
	13: "FileDispositionInformation",
	14: "FilePositionInformation",
	15: "FileFullEaInformation",
	16: "FileModeInformation",
	17: "FileAlignmentInformation",
	18: "FileAllInformation",
	19: "FileAllocationInformation",
	20: "FileEndOfFileInformation",
	21: "FileAlternateNameInformation",
	22: "FileStreamInformation",
	23: "FilePipeInformation",
	24: "FilePipeLocalInformation",
	25: "FilePipeRemoteInformation",
	26: "FileMailslotQueryInformation",
	27: "FileMailslotSetInformation",
	28: "FileCompressionInformation",
	29: "FileObjectIdInformation",
	30: "FileCompletionInformation",
	31: "FileMoveClusterInformation",
	32: "FileQuotaInformation",
	33: "FileReparsePointInformation",
	34: "FileNetworkOpenInformation",
	35: "FileAttributeTagInformation",
	36: "FileTrackingInformation",
	37: "FileIdBothDirectoryInformation",
	38: "FileIdFullDirectoryInformation",
	39: "FileValidDataLengthInformation",
	40: "FileShortNameInformation",
	41: "FileIoCompletionNotificationInformation",
	42: "FileIoStatusBlockRangeInformation",
	43: "FileIoPriorityHintInformation",
	44: "FileSfioReserveInformation",
	45: "FileSfioVolumeInformation",
	46: "FileHardLinkInformation",
	47: "FileProcessIdsUsingFileInformation",
	48: "FileNormalizedNameInformation",
	49: "FileNetworkPhysicalNameInformation",
	50: "FileIdGlobalTxDirectoryInformation",
	51: "FileIsRemoteDeviceInformation",
	52: "FileUnusedInformation",
	53: "FileNumaNodeInformation",
	54: "FileStandardLinkInformation",
	55: "FileRemoteProtocolInformation",
	56: "FileRenameInformationBypassAccessCheck",
	57: "FileLinkInformationBypassAccessCheck",
	58: "FileVolumeNameInformation",
	59: "FileIdInformation",
	60: "FileIdExtdDirectoryInformation",
	61: "FileReplaceCompletionInformation",
	62: "FileHardLinkFullIdInformation",
	63: "FileIdExtdBothDirectoryInformation",
	64: "FileDispositionInformationEx",
	65: "FileRenameInformationEx",
	66: "FileRenameInformationExBypassAccessCheck",
	67: "FileDesiredStorageClassInformation",
	68: "FileStatInformation",
	69: "FileMemoryPartitionInformation",
	70: "FileStatLxInformation",
	71: "FileCaseSensitiveInformation",
	72: "FileLinkInformationEx",
	73: "FileLinkInformationExBypassAccessCheck",
	74: "FileStorageReserveIdInformation",
	75: "FileCaseSensitiveInformationForceAccessCheck",
	76: "FileKnownFolderInformation",
	77: "FileStatBasicInformation",
	78: "FileId64ExtdDirectoryInformation",
	79: "FileId64ExtdBothDirectoryInformation",
	80: "FileIdAllExtdDirectoryInformation",
	81: "FileIdAllExtdBothDirectoryInformation",
	82: "FileStreamReservationInformation",
	83: "FileMupProviderInfo",
}

//REG_NOTIFY_CLASS enum values
// See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ne-wdm-_reg_notify_class
// This is used to translate integer values to human-readable strings.
var REGOPS = map[int]string{
	0:  "RegNtDeleteKey",
	1:  "RegNtSetValueKey",
	2:  "RegNtDeleteValueKey",
	3:  "RegNtSetInformationKey",
	4:  "RegNtRenameKey",
	5:  "RegNtEnumerateKey",
	6:  "RegNtEnumerateValueKey",
	7:  "RegNtQueryKey",
	8:  "RegNtQueryValueKey",
	9:  "RegNtQueryMultipleValueKey",
	10: "RegNtPreCreateKey",
	11: "RegNtPostCreateKey",
	12: "RegNtPreOpenKey",
	13: "RegNtPostOpenKey",
	14: "RegNtKeyHandleClose",
	15: "RegNtPostDeleteKey",
	16: "RegNtPostSetValueKey",
	17: "RegNtPostDeleteValueKey",
	18: "RegNtPostSetInformationKey",
	19: "RegNtPostRenameKey",
	20: "RegNtPostEnumerateKey",
	21: "RegNtPostEnumerateValueKey",
	22: "RegNtPostQueryKey",
	23: "RegNtPostQueryValueKey",
	24: "RegNtPostQueryMultipleValueKey",
	25: "RegNtPostKeyHandleClose",
	26: "RegNtPreCreateKeyEx",
	27: "RegNtPostCreateKeyEx",
	28: "RegNtPreOpenKeyEx",
	29: "RegNtPostOpenKeyEx",
	30: "RegNtPreFlushKey",
	31: "RegNtPostFlushKey",
	32: "RegNtPreLoadKey",
	33: "RegNtPostLoadKey",
	34: "RegNtPreUnLoadKey",
	35: "RegNtPostUnLoadKey",
	36: "RegNtPreQueryKeySecurity",
	37: "RegNtPostQueryKeySecurity",
	38: "RegNtPreSetKeySecurity",
	39: "RegNtPostSetKeySecurity",
	40: "RegNtCallbackObjectContextCleanup",
	41: "RegNtPreRestoreKey",
	42: "RegNtPostRestoreKey",
	43: "RegNtPreSaveKey",
	44: "RegNtPostSaveKey",
	45: "RegNtPreReplaceKey",
	46: "RegNtPostReplaceKey",
	47: "RegNtPreQueryKeyName",
	48: "RegNtPostQueryKeyName",
	49: "RegNtPreSaveMergedKey",
	50: "RegNtPostSaveMergedKey",
	51: "MaxRegNtNotifyClass",
}

// Same as REGOPS, but REGOPS_SUPPORTED includes only
// The operations supported for configuration.
var REGOPS_SUPPORTED = map[int]string{
	0: "RegNtDeleteKey",
	1: "RegNtSetValueKey",
	2: "RegNtDeleteValueKey",
	//3:  "RegNtSetInformationKey",
	4: "RegNtRenameKey",
	//5:  "RegNtEnumerateKey",
	//6:  "RegNtEnumerateValueKey",
	//7:  "RegNtQueryKey",
	//8:  "RegNtQueryValueKey",
	//9:  "RegNtQueryMultipleValueKey",
	10: "RegNtPreCreateKey",
	11: "RegNtPostCreateKey",
	//12: "RegNtPreOpenKey",
	13: "RegNtPostOpenKey",
	//14: "RegNtKeyHandleClose",
	15: "RegNtPostDeleteKey",
	16: "RegNtPostSetValueKey",
	//17: "RegNtPostDeleteValueKey",
	//18: "RegNtPostSetInformationKey",
	//19: "RegNtPostRenameKey",
	//20: "RegNtPostEnumerateKey",
	//21: "RegNtPostEnumerateValueKey",
	//22: "RegNtPostQueryKey",
	//23: "RegNtPostQueryValueKey",
	//24: "RegNtPostQueryMultipleValueKey",
	//25: "RegNtPostKeyHandleClose",
	26: "RegNtPreCreateKeyEx",
	//27: "RegNtPostCreateKeyEx",
	//28: "RegNtPreOpenKeyEx",
	//29: "RegNtPostOpenKeyEx",
	30: "RegNtPreFlushKey",
	31: "RegNtPostFlushKey",
	//32: "RegNtPreLoadKey",
	//33: "RegNtPostLoadKey",
	//34: "RegNtPreUnLoadKey",
	//35: "RegNtPostUnLoadKey",
	//36: "RegNtPreQueryKeySecurity",
	37: "RegNtPostQueryKeySecurity",
	//38: "RegNtPreSetKeySecurity",
	39: "RegNtPostSetKeySecurity",
	//40: "RegNtCallbackObjectContextCleanup",
	41: "RegNtPreRestoreKey",
	//42: "RegNtPostRestoreKey",
	//43: "RegNtPreSaveKey",
	//44: "RegNtPostSaveKey",
	45: "RegNtPreReplaceKey",
	//46: "RegNtPostReplaceKey",
	//47: "RegNtPreQueryKeyName",
	//48: "RegNtPostQueryKeyName",
	//49: "RegNtPreSaveMergedKey",
	//50: "RegNtPostSaveMergedKey",
	//51: "MaxRegNtNotifyClass",
}

// FWPS_BUILTIN_LAYERS enum values, See: https://learn.microsoft.com/en-us/windows/win32/api/fwpsu/ne-fwpsu-fwps_builtin_layers
// This is used to translate integer values to human-readable strings.
var WFPLAYERS = map[int]string{
	0:  "FWPS_LAYER_INBOUND_IPPACKET_V4",
	1:  "FWPS_LAYER_INBOUND_IPPACKET_V4_DISCARD",
	2:  "FWPS_LAYER_INBOUND_IPPACKET_V6",
	3:  "FWPS_LAYER_INBOUND_IPPACKET_V6_DISCARD",
	4:  "FWPS_LAYER_OUTBOUND_IPPACKET_V4",
	5:  "FWPS_LAYER_OUTBOUND_IPPACKET_V4_DISCARD",
	6:  "FWPS_LAYER_OUTBOUND_IPPACKET_V6",
	7:  "FWPS_LAYER_OUTBOUND_IPPACKET_V6_DISCARD",
	8:  "FWPS_LAYER_IPFORWARD_V4",
	9:  "FWPS_LAYER_IPFORWARD_V4_DISCARD",
	10: "FWPS_LAYER_IPFORWARD_V6",
	11: "FWPS_LAYER_IPFORWARD_V6_DISCARD",
	12: "FWPS_LAYER_INBOUND_TRANSPORT_V4",
	13: "FWPS_LAYER_INBOUND_TRANSPORT_V4_DISCARD",
	14: "FWPS_LAYER_INBOUND_TRANSPORT_V6",
	15: "FWPS_LAYER_INBOUND_TRANSPORT_V6_DISCARD",
	16: "FWPS_LAYER_OUTBOUND_TRANSPORT_V4",
	17: "FWPS_LAYER_OUTBOUND_TRANSPORT_V4_DISCARD",
	18: "FWPS_LAYER_OUTBOUND_TRANSPORT_V6",
	19: "FWPS_LAYER_OUTBOUND_TRANSPORT_V6_DISCARD",
	20: "FWPS_LAYER_STREAM_V4",
	21: "FWPS_LAYER_STREAM_V4_DISCARD",
	22: "FWPS_LAYER_STREAM_V6",
	23: "FWPS_LAYER_STREAM_V6_DISCARD",
	24: "FWPS_LAYER_DATAGRAM_DATA_V4",
	25: "FWPS_LAYER_DATAGRAM_DATA_V4_DISCARD",
	26: "FWPS_LAYER_DATAGRAM_DATA_V6",
	27: "FWPS_LAYER_DATAGRAM_DATA_V6_DISCARD",
	28: "FWPS_LAYER_INBOUND_ICMP_ERROR_V4",
	29: "FWPS_LAYER_INBOUND_ICMP_ERROR_V4_DISCARD",
	30: "FWPS_LAYER_INBOUND_ICMP_ERROR_V6",
	31: "FWPS_LAYER_INBOUND_ICMP_ERROR_V6_DISCARD",
	32: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V4",
	33: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V4_DISCARD",
	34: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V6",
	35: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V6_DISCARD",
	36: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V4",
	37: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V4_DISCARD",
	38: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V6",
	39: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V6_DISCARD",
	40: "FWPS_LAYER_ALE_AUTH_LISTEN_V4",
	41: "FWPS_LAYER_ALE_AUTH_LISTEN_V4_DISCARD",
	42: "FWPS_LAYER_ALE_AUTH_LISTEN_V6",
	43: "FWPS_LAYER_ALE_AUTH_LISTEN_V6_DISCARD",
	44: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4",
	45: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4_DISCARD",
	46: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6",
	47: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6_DISCARD",
	48: "FWPS_LAYER_ALE_AUTH_CONNECT_V4",
	49: "FWPS_LAYER_ALE_AUTH_CONNECT_V4_DISCARD",
	50: "FWPS_LAYER_ALE_AUTH_CONNECT_V6",
	51: "FWPS_LAYER_ALE_AUTH_CONNECT_V6_DISCARD",
	52: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4",
	53: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4_DISCARD",
	54: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V6",
	55: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V6_DISCARD",
	56: "FWPS_LAYER_INBOUND_MAC_FRAME_ETHERNET",
	57: "FWPS_LAYER_OUTBOUND_MAC_FRAME_ETHERNET",
	58: "FWPS_LAYER_INBOUND_MAC_FRAME_NATIVE",
	59: "FWPS_LAYER_OUTBOUND_MAC_FRAME_NATIVE",
	60: "FWPS_LAYER_NAME_RESOLUTION_CACHE_V4",
	61: "FWPS_LAYER_NAME_RESOLUTION_CACHE_V6",
	62: "FWPS_LAYER_ALE_RESOURCE_RELEASE_V4",
	63: "FWPS_LAYER_ALE_RESOURCE_RELEASE_V6",
	64: "FWPS_LAYER_ALE_ENDPOINT_CLOSURE_V4",
	65: "FWPS_LAYER_ALE_ENDPOINT_CLOSURE_V6",
	66: "FWPS_LAYER_ALE_CONNECT_REDIRECT_V4",
	67: "FWPS_LAYER_ALE_CONNECT_REDIRECT_V6",
	68: "FWPS_LAYER_ALE_BIND_REDIRECT_V4",
	69: "FWPS_LAYER_ALE_BIND_REDIRECT_V6",
	70: "FWPS_LAYER_STREAM_PACKET_V4",
	71: "FWPS_LAYER_STREAM_PACKET_V6",
	72: "FWPS_LAYER_INGRESS_VSWITCH_ETHERNET",
	73: "FWPS_LAYER_EGRESS_VSWITCH_ETHERNET",
	74: "FWPS_LAYER_INGRESS_VSWITCH_TRANSPORT_V4",
	75: "FWPS_LAYER_INGRESS_VSWITCH_TRANSPORT_V6",
	76: "FWPS_LAYER_EGRESS_VSWITCH_TRANSPORT_V4",
	77: "FWPS_LAYER_EGRESS_VSWITCH_TRANSPORT_V6",
	78: "FWPS_LAYER_INBOUND_TRANSPORT_FAST",
	79: "FWPS_LAYER_OUTBOUND_TRANSPORT_FAST",
	80: "FWPS_LAYER_INBOUND_MAC_FRAME_NATIVE_FAST",
	81: "FWPS_LAYER_OUTBOUND_MAC_FRAME_NATIVE_FAST",
	82: "FWPS_LAYER_INBOUND_RESERVED2",
	83: "FWPS_LAYER_RESERVED_LAYER_9",
	84: "FWPS_LAYER_RESERVED_LAYER_10",
	85: "FWPS_LAYER_OUTBOUND_NETWORK_CONNECTION_POLICY_V4",
	86: "FWPS_LAYER_OUTBOUND_NETWORK_CONNECTION_POLICY_V6",
	87: "FWPS_LAYER_IPSEC_KM_DEMUX_V4",
	88: "FWPS_LAYER_IPSEC_KM_DEMUX_V6",
	89: "FWPS_LAYER_IPSEC_V4",
	90: "FWPS_LAYER_IPSEC_V6",
	91: "FWPS_LAYER_IKEEXT_V4",
	92: "FWPS_LAYER_IKEEXT_V6",
	93: "FWPS_LAYER_RPC_UM",
	94: "FWPS_LAYER_RPC_EPMAP",
	95: "FWPS_LAYER_RPC_EP_ADD",
	96: "FWPS_LAYER_RPC_PROXY_CONN",
	97: "FWPS_LAYER_RPC_PROXY_IF",
	98: "FWPS_LAYER_KM_AUTHORIZATION",
	99: "FWPS_BUILTIN_LAYER_MAX",
}

// Same as WFPLAYERS, but WFPLAYERS_SUPPORTED includes only
// The operations supported for configuration.
var WFPLAYERS_SUPPORTED = map[int]string{
	//0:  "FWPS_LAYER_INBOUND_IPPACKET_V4",
	//1:  "FWPS_LAYER_INBOUND_IPPACKET_V4_DISCARD",
	//2:  "FWPS_LAYER_INBOUND_IPPACKET_V6",
	//3:  "FWPS_LAYER_INBOUND_IPPACKET_V6_DISCARD",
	//4:  "FWPS_LAYER_OUTBOUND_IPPACKET_V4",
	//5:  "FWPS_LAYER_OUTBOUND_IPPACKET_V4_DISCARD",
	//6:  "FWPS_LAYER_OUTBOUND_IPPACKET_V6",
	//7:  "FWPS_LAYER_OUTBOUND_IPPACKET_V6_DISCARD",
	//8:  "FWPS_LAYER_IPFORWARD_V4",
	//9:  "FWPS_LAYER_IPFORWARD_V4_DISCARD",
	//10: "FWPS_LAYER_IPFORWARD_V6",
	//11: "FWPS_LAYER_IPFORWARD_V6_DISCARD",
	//12: "FWPS_LAYER_INBOUND_TRANSPORT_V4",
	//13: "FWPS_LAYER_INBOUND_TRANSPORT_V4_DISCARD",
	//14: "FWPS_LAYER_INBOUND_TRANSPORT_V6",
	//15: "FWPS_LAYER_INBOUND_TRANSPORT_V6_DISCARD",
	//16: "FWPS_LAYER_OUTBOUND_TRANSPORT_V4",
	//17: "FWPS_LAYER_OUTBOUND_TRANSPORT_V4_DISCARD",
	//18: "FWPS_LAYER_OUTBOUND_TRANSPORT_V6",
	//19: "FWPS_LAYER_OUTBOUND_TRANSPORT_V6_DISCARD",
	//20: "FWPS_LAYER_STREAM_V4",
	//21: "FWPS_LAYER_STREAM_V4_DISCARD",
	//22: "FWPS_LAYER_STREAM_V6",
	//23: "FWPS_LAYER_STREAM_V6_DISCARD",
	24: "FWPS_LAYER_DATAGRAM_DATA_V4",
	//25: "FWPS_LAYER_DATAGRAM_DATA_V4_DISCARD",
	26: "FWPS_LAYER_DATAGRAM_DATA_V6",
	//27: "FWPS_LAYER_DATAGRAM_DATA_V6_DISCARD",
	//28: "FWPS_LAYER_INBOUND_ICMP_ERROR_V4",
	//29: "FWPS_LAYER_INBOUND_ICMP_ERROR_V4_DISCARD",
	//30: "FWPS_LAYER_INBOUND_ICMP_ERROR_V6",
	//31: "FWPS_LAYER_INBOUND_ICMP_ERROR_V6_DISCARD",
	//32: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V4",
	//33: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V4_DISCARD",
	//34: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V6",
	//35: "FWPS_LAYER_OUTBOUND_ICMP_ERROR_V6_DISCARD",
	36: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V4",
	//37: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V4_DISCARD",
	38: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V6",
	//39: "FWPS_LAYER_ALE_RESOURCE_ASSIGNMENT_V6_DISCARD",
	40: "FWPS_LAYER_ALE_AUTH_LISTEN_V4",
	//41: "FWPS_LAYER_ALE_AUTH_LISTEN_V4_DISCARD",
	42: "FWPS_LAYER_ALE_AUTH_LISTEN_V6",
	//43: "FWPS_LAYER_ALE_AUTH_LISTEN_V6_DISCARD",
	44: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4",
	//45: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4_DISCARD",
	46: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6",
	//47: "FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6_DISCARD",
	48: "FWPS_LAYER_ALE_AUTH_CONNECT_V4",
	//49: "FWPS_LAYER_ALE_AUTH_CONNECT_V4_DISCARD",
	50: "FWPS_LAYER_ALE_AUTH_CONNECT_V6",
	//51: "FWPS_LAYER_ALE_AUTH_CONNECT_V6_DISCARD",
	//52: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4",
	//53: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4_DISCARD",
	//54: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V6",
	//55: "FWPS_LAYER_ALE_FLOW_ESTABLISHED_V6_DISCARD",
	//56: "FWPS_LAYER_INBOUND_MAC_FRAME_ETHERNET",
	//57: "FWPS_LAYER_OUTBOUND_MAC_FRAME_ETHERNET",
	//58: "FWPS_LAYER_INBOUND_MAC_FRAME_NATIVE",
	//59: "FWPS_LAYER_OUTBOUND_MAC_FRAME_NATIVE",
	//60: "FWPS_LAYER_NAME_RESOLUTION_CACHE_V4",
	//61: "FWPS_LAYER_NAME_RESOLUTION_CACHE_V6",
	//62: "FWPS_LAYER_ALE_RESOURCE_RELEASE_V4",
	//63: "FWPS_LAYER_ALE_RESOURCE_RELEASE_V6",
	//64: "FWPS_LAYER_ALE_ENDPOINT_CLOSURE_V4",
	//65: "FWPS_LAYER_ALE_ENDPOINT_CLOSURE_V6",
	//66: "FWPS_LAYER_ALE_CONNECT_REDIRECT_V4",
	//67: "FWPS_LAYER_ALE_CONNECT_REDIRECT_V6",
	//68: "FWPS_LAYER_ALE_BIND_REDIRECT_V4",
	//69: "FWPS_LAYER_ALE_BIND_REDIRECT_V6",
	//70: "FWPS_LAYER_STREAM_PACKET_V4",
	//71: "FWPS_LAYER_STREAM_PACKET_V6",
	//72: "FWPS_LAYER_INGRESS_VSWITCH_ETHERNET",
	//73: "FWPS_LAYER_EGRESS_VSWITCH_ETHERNET",
	//74: "FWPS_LAYER_INGRESS_VSWITCH_TRANSPORT_V4",
	//75: "FWPS_LAYER_INGRESS_VSWITCH_TRANSPORT_V6",
	//76: "FWPS_LAYER_EGRESS_VSWITCH_TRANSPORT_V4",
	//77: "FWPS_LAYER_EGRESS_VSWITCH_TRANSPORT_V6",
	//78: "FWPS_LAYER_INBOUND_TRANSPORT_FAST",
	//79: "FWPS_LAYER_OUTBOUND_TRANSPORT_FAST",
	//80: "FWPS_LAYER_INBOUND_MAC_FRAME_NATIVE_FAST",
	//81: "FWPS_LAYER_OUTBOUND_MAC_FRAME_NATIVE_FAST",
	//82: "FWPS_LAYER_INBOUND_RESERVED2",
	//83: "FWPS_LAYER_RESERVED_LAYER_9",
	//84: "FWPS_LAYER_RESERVED_LAYER_10",
	//85: "FWPS_LAYER_OUTBOUND_NETWORK_CONNECTION_POLICY_V4",
	//86: "FWPS_LAYER_OUTBOUND_NETWORK_CONNECTION_POLICY_V6",
	//87: "FWPS_LAYER_IPSEC_KM_DEMUX_V4",
	//88: "FWPS_LAYER_IPSEC_KM_DEMUX_V6",
	//89: "FWPS_LAYER_IPSEC_V4",
	//90: "FWPS_LAYER_IPSEC_V6",
	//91: "FWPS_LAYER_IKEEXT_V4",
	//92: "FWPS_LAYER_IKEEXT_V6",
	//93: "FWPS_LAYER_RPC_UM",
	//94: "FWPS_LAYER_RPC_EPMAP",
	//95: "FWPS_LAYER_RPC_EP_ADD",
	//96: "FWPS_LAYER_RPC_PROXY_CONN",
	//97: "FWPS_LAYER_RPC_PROXY_IF",
	//98: "FWPS_LAYER_KM_AUTHORIZATION",
	//99: "FWPS_BUILTIN_LAYER_MAX",
}

// Metadata that associates string-value field names
// With additional type information and documentation.
type FieldInfo struct {
	Name         string
	FieldType    string
	ValidRanges  []string
	DefaultValue string
	Description  string
}

// Static metadata definitions for FieldInfo field information.
// Hopefully, this will make testing easier.
var FieldInformation = []FieldInfo{
	{Name: "FILE_OPERATION", FieldType: "enum", ValidRanges: []string{"fileops"}, DefaultValue: "FileRenameInformationEx", Description: "FILE_INFORMATIN_CLASS enum values, this includes operations such as file renaming,deletion, named pipe activity, attribute changes and much more. Not all fields are well-tested. See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ne-wdm-_file_information_class"},
	{Name: "WFP_LAYER", FieldType: "enum", ValidRanges: []string{"wfplayers"}, DefaultValue: "FWPS_LAYER_ALE_AUTH_CONNECT_V4", Description: "FWPS_BUILTIN_LAYERS enum values, See: https://learn.microsoft.com/en-us/windows/win32/api/fwpsu/ne-fwpsu-fwps_builtin_layers"},
	{Name: "REGISTRY_OPERATION", FieldType: "enum", ValidRanges: []string{"regops"}, DefaultValue: "RegNtPostCreateKeyEx", Description: "REG_NOTIFY_CLASS enum values, See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ne-wdm-_reg_notify_class"},
	{Name: "LOADED_MODULE", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.dll", Description: "The .DLL or .SYS module being loaded."},
	{Name: "IMAGE_FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.exe", Description: "The process' image file path."},
	{Name: "COMMAND_LINE", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "notepad.exe Z:\\does-not-exist.txt", Description: "The process' command line."},
	{Name: "PARENT_IMAGE_FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.exe", Description: "The parent process' command line."},
	{Name: "PARENT_COMMAND_LINE", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "notepad.exe Z:\\does-not-exist.txt", Description: "The parent process' command line"},
	{Name: "CURRENT_WORKING_DIRECTORY", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\dir", Description: "The current working directory of the process, as observed the process start time."},
	{Name: "WINDOW_TITLE", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "Windows Powershell", Description: "The window title for the process."},
	{Name: "PROCESS_HANDLE_OPERATION", FieldType: "hex-uint32", ValidRanges: []string{"0x1", "0x2"}, DefaultValue: "0x1", Description: "OB_OPERATION_HANDLE_CREATE (0x1) or OB_OPERATION_HANDLE_DUPLICATE (0x2), See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_ob_pre_operation_information"},
	{Name: "PROCESS_HANDLE_ORIGINAL_DESIRED_ACCESS", FieldType: "hex-uint32", ValidRanges: []string{"uint32"}, DefaultValue: "0xffffffff", Description: "ACCESS_MASK values, See: https://learn.microsoft.com/en-us/windows/win32/procthread/process-security-and-access-rights"},
	{Name: "REGISTRY_KEY", FieldType: "string", ValidRanges: []string{"unicode_registry_key"}, DefaultValue: "\\Registry\\Machine\\SYSTEM\\does-not-exist", Description: "The registry key involved in the operation."},
	{Name: "REGISTRY_KEY_NEW", FieldType: "string", ValidRanges: []string{"unicode_registry_key"}, DefaultValue: "\\Registry\\Machine\\SYSTEM\\does-not-exist", Description: "The newly minted registry key."},
	{Name: "REGISTRY_VALUE_NAME", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "Z:\\does-not-exist.txt", Description: "The registry value name"},
	{Name: "REGISTRY_DATA", FieldType: "string", ValidRanges: []string{"bytes", "unsupported"}, DefaultValue: "Z:\\does-not-exist.txt", Description: "The registry data involved, UTF-8 encoded."},
	{Name: "REGISTRY_OLD_FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.reg", Description: "The .reg file or hive involved for loading or opening registry information."},
	{Name: "REGISTRY_NEW_FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.reg", Description: "The .reg file or hive involved for exporting or saving registry information."},
	{Name: "REGISTRY_DATA_TYPE", FieldType: "uint16", ValidRanges: []string{"0-12"}, DefaultValue: "0", Description: "The data type of the registry entry, See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey"},
	{Name: "FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.txt", Description: "Absolute file path."},
	{Name: "FILE_VOLUME", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "Z:", Description: "The DOS Volume name."},
	{Name: "FILE_EXTENSION", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "txt", Description: "The file's extension (if any)."},
	{Name: "FILE_SHARE", FieldType: "string", ValidRanges: []string{"unicode_file_path_unc"}, DefaultValue: "\\192.168.56.103\\dummy\\file.txt", Description: "The UNC file path for files on network shares."},
	{Name: "FILE_STREAM", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: ":Zone.Identifier", Description: "The NTFS ADS file stream."},
	{Name: "FILE_FINAL_COMPONENT", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "does-not-exist.txt", Description: "The file's name."},
	{Name: "FILE_PARENT_DIR", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\", Description: "The parent directory of the file."},
	{Name: "FILE_FLAGS", FieldType: "hex-uint32", ValidRanges: []string{"uint32"}, DefaultValue: "0x1", Description: "See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-iocreatefileex"},
	{Name: "FILE_ACCESS_FLAGS", FieldType: "hex-uint32", ValidRanges: []string{"uint32"}, DefaultValue: "0x00020000", Description: "See: https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/access-mask"},
	{Name: "FILE_NEW_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.txt", Description: "The new file name for a rename operation."},
	{Name: "FILE_NETWORK_PROTOCOL", FieldType: "hex-uint32", ValidRanges: []string{"uint32"}, DefaultValue: "0x00020000", Description: "The network protocol for the file share involved, such as SMB. See: https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-file_remote_protocol_info"},
	{Name: "FILE_NETWORK_PROTOCOL_VERSION_MAJOR", FieldType: "uint16", ValidRanges: []string{"uint16"}, DefaultValue: "1", Description: ""},
	{Name: "FILE_NETWORK_PROTOCOL_VERSION_MINOR", FieldType: "uint16", ValidRanges: []string{"uint16"}, DefaultValue: "1", Description: ""},
	{Name: "NAMED_PIPE_TYPE", FieldType: "hex-uint32", ValidRanges: []string{"0x1", "0x2"}, DefaultValue: "0x1", Description: "See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_local_information"},
	{Name: "NAMED_PIPE_CONFIG", FieldType: "hex-uint32", ValidRanges: []string{"0x0", "0x1", "0x2"}, DefaultValue: "0x0", Description: "See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_local_information"},
	{Name: "NAMED_PIPE_STATE", FieldType: "hex-uint32", ValidRanges: []string{"0x1", "0x2", "0x3", "0x4"}, DefaultValue: "0x3", Description: "See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_local_information"},
	{Name: "NAMED_PIPE_END", FieldType: "hex-uint32", ValidRanges: []string{"0x0", "0x1"}, DefaultValue: "0x0", Description: "See: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_local_information"},
	{Name: "FILE_DELETION", FieldType: "string", ValidRanges: []string{"unsupported", "true", "false"}, DefaultValue: "true", Description: "Indicates a file was deleted"},
	{Name: "NET_DIRECTION", FieldType: "decimal-byte", ValidRanges: []string{"0", "1"}, DefaultValue: "0x1", Description: "Direction of network traffic"},
	{Name: "NET_IP_PROTOCOL", FieldType: "decimal-byte", ValidRanges: []string{"byte"}, DefaultValue: "17", Description: "The IP network protocl value for the traffiic."},
	{Name: "NET_ADDRESS_TYPE", FieldType: "decimal-byte", ValidRanges: []string{"0", "1", "2", "3", "4", "5"}, DefaultValue: "0x1", Description: "The network address type, such as unicast,broadcast and anycast. See: https://learn.microsoft.com/en-us/windows/win32/api/nldef/ne-nldef-nl_address_type"},
	{Name: "NET_PROMISCUOUS", FieldType: "decimal-byte", ValidRanges: []string{"0", "1"}, DefaultValue: "0x1", Description: "Indicates the network resource assigned permits promiscuous access."},
	{Name: "NET_LOCAL_PORT", FieldType: "uint16", ValidRanges: []string{"uint16"}, DefaultValue: "80", Description: "Transport layer local port."},
	{Name: "NET_REMOTE_PORT", FieldType: "uint16", ValidRanges: []string{"uint16"}, DefaultValue: "8080", Description: "Transport layer remote port"},
	{Name: "NET_LOCAL_IPV4_ADDR", FieldType: "string", ValidRanges: []string{"ipv4_string"}, DefaultValue: "127.0.0.1/8", Description: "IPV4 Subnet with a CIDR formatted prefix"},
	{Name: "NET_REMOTE_IPV4_ADDR", FieldType: "string", ValidRanges: []string{"ipv4_string"}, DefaultValue: "1.1.1.1/32", Description: "IPV4 Subnet with a CIDR formatted prefix"},
	{Name: "NET_INTERFACE_TYPE", FieldType: "hex-uint32", ValidRanges: []string{"uint32"}, DefaultValue: "0x75", Description: "IANA defined values for types of network interfaces. See: https://www.iana.org/assignments/ianaiftype-mib/ianaiftype-mib"},
	{Name: "NET_LOCAL_IPV6_ADDR", FieldType: "string", ValidRanges: []string{"ipv6_string"}, DefaultValue: "::1/128", Description: "IPV6 network address along with the prefix."},
	{Name: "NET_REMOTE_IPV6_ADDR", FieldType: "string", ValidRanges: []string{"ipv6_string"}, DefaultValue: "::1/128", Description: "IPV6 network address along with the prefix."},
	{Name: "TARGET_PARENT_IMAGE_FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.exe", Description: "The parent process path of the target process."},
	{Name: "TARGET_PARENT_COMMAND_LINE", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "notepad.exe Z:\\does-not-exist.txt", Description: "The parent process' command line of the target process."},
	{Name: "TARGET_WINDOW_TITLE", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "Notepad", Description: "The window title of the target process."},
	{Name: "TARGET_CURRENT_WORKING_DIRECTORY", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\dir\\", Description: "The current working directory of the target process, as observed during it's creation."},
	{Name: "TARGET_IMAGE_FILE_NAME", FieldType: "string", ValidRanges: []string{"unicode_file_path"}, DefaultValue: "Z:\\does-not-exist.exe", Description: "The process image path of the target process."},
	{Name: "TARGET_COMMAND_LINE", FieldType: "string", ValidRanges: []string{"unicode_string"}, DefaultValue: "notepad.exe Z:\\does-not-exist.txt", Description: "The command line of the target process."},
}

// This is a dumb but useful array of Dos Device volume letters
// To be used to dynamically discover new drives.
var DosDeviceLetters = []string{"A:", "B:", "C:", "D:", "E:", "F:", "G:", "H:", "I:", "J:", "K:", "L:", "M:", "N:", "O:", "P:", "Q:", "R:", "S:", "T:", "U:", "V:", "W:", "X:", "Y:", "Z:"}

// A map of DOS device letters and global paths
var DosDeviceMap = make(map[string]string)
