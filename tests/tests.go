package MoonMon

import (
	"bufio"
	"log"
	"os"

	luna "github.com/ag-michael/MoonMon/pkg/luna"
)

// Log entries are Unmarshal'd from JSON file output logs into this struct
type LogEntry struct {
	IMAGE_FILE_NAME string `json:"IMAGE_FILE_NAME"`

	ID                                     string `json:"ID"`
	RULE_ID                                string `json:"RULE_ID"`
	TIMESTAMP                              string `json:"Timestamp"`
	EVENT_TYPE                             string `json:"EVENT_TYPE"`
	CREATION_STATUS                        string `json:"CREATION_STATUS"`
	FILE_OPERATION                         string `json:"FILE_OPERATION"`
	WFP_LAYER                              string `json:"WFP_LAYER"`
	REGISTRY_OPERATION                     string `json:"REGISTRY_OPERATION"`
	LOADED_MODULE                          string `json:"LOADED_MODULE"`
	COMMAND_LINE                           string `json:"COMMAND_LINE"`
	PARENT_IMAGE_FILE_NAME                 string `json:"PARENT_IMAGE_FILE_NAME"`
	PARENT_COMMAND_LINE                    string `json:"PARENT_COMMAND_LINE"`
	CURRENT_WORKING_DIRECTORY              string `json:"CURRENT_WORKING_DIRECTORY"`
	WINDOW_TITLE                           string `json:"WINDOW_TITLE"`
	PROCESS_HANDLE_OPERATION               string `json:"PROCESS_HANDLE_OPERATION"`
	PROCESS_HANDLE_ORIGINAL_DESIRED_ACCESS string `json:"PROCESS_HANDLE_ORIGINAL_DESIRED_ACCESS"`
	PROCESS_HANDLE_DESIRED_ACCESS          string `json:"PROCESS_HANDLE_DESIRED_ACCESS"`
	REGISTRY_KEY                           string `json:"REGISTRY_KEY"`
	REGISTRY_KEY_NEW                       string `json:"REGISTRY_KEY_NEW"`
	REGISTRY_VALUE_NAME                    string `json:"REGISTRY_VALUE_NAME"`
	REGISTRY_DATA                          string `json:"REGISTRY_DATA"`
	REGISTRY_OLD_FILE_NAME                 string `json:"REGISTRY_OLD_FILE_NAME"`
	REGISTRY_NEW_FILE_NAME                 string `json:"REGISTRY_NEW_FILE_NAME"`
	REGISTRY_DATA_TYPE                     string `json:"REGISTRY_DATA_TYPE"`
	FILE_NAME                              string `json:"FILE_NAME"`
	FILE_VOLUME                            string `json:"FILE_VOLUME"`
	FILE_EXTENSION                         string `json:"FILE_EXTENSION"`
	FILE_SHARE                             string `json:"FILE_SHARE"`
	FILE_STREAM                            string `json:"FILE_STREAM"`
	FILE_FINAL_COMPONENT                   string `json:"FILE_FINAL_COMPONENT"`
	FILE_PARENT_DIR                        string `json:"FILE_PARENT_DIR"`
	FILE_FLAGS                             string `json:"FILE_FLAGS"`
	FILE_ACCESS_FLAGS                      string `json:"FILE_ACCESS_FLAGS"`
	FILE_NEW_NAME                          string `json:"FILE_NEW_NAME"`
	FILE_NETWORK_PROTOCOL                  string `json:"FILE_NETWORK_PROTOCOL"`
	FILE_NETWORK_PROTOCOL_VERSION_MAJOR    string `json:"FILE_NETWORK_PROTOCOL_VERSION_MAJOR"`
	FILE_NETWORK_PROTOCOL_VERSION_MINOR    string `json:"FILE_NETWORK_PROTOCOL_VERSION_MINOR"`
	NAMED_PIPE_TYPE                        string `json:"NAMED_PIPE_TYPE"`
	NAMED_PIPE_CONFIG                      string `json:"NAMED_PIPE_CONFIG"`
	NAMED_PIPE_STATE                       string `json:"NAMED_PIPE_STATE"`
	NAMED_PIPE_END                         string `json:"NAMED_PIPE_END"`
	FILE_DELETION                          string `json:"FILE_DELETION"`
	NET_DIRECTION                          string `json:"NET_DIRECTION"`
	NET_IP_PROTOCOL                        string `json:"NET_IP_PROTOCOL"`
	NET_ADDRESS_TYPE                       string `json:"NET_ADDRESS_TYPE"`
	NET_PROMISCUOUS                        string `json:"NET_PROMISCUOUS"`
	NET_LOCAL_PORT                         string `json:"NET_LOCAL_PORT"`
	NET_REMOTE_PORT                        string `json:"NET_REMOTE_PORT"`
	NET_LOCAL_IPV4_ADDR                    string `json:"NET_LOCAL_IPV4_ADDR"`
	NET_REMOTE_IPV4_ADDR                   string `json:"NET_REMOTE_IPV4_ADDR"`
	NET_INTERFACE_TYPE                     string `json:"NET_INTERFACE_TYPE"`
	NET_LOCAL_IPV6_ADDR                    string `json:"NET_LOCAL_IPV6_ADDR"`
	NET_REMOTE_IPV6_ADDR                   string `json:"NET_REMOTE_IPV6_ADDR"`
	TARGET_PARENT_IMAGE_FILE_NAME          string `json:"TARGET_PARENT_IMAGE_FILE_NAME"`
	TARGET_PARENT_COMMAND_LINE             string `json:"TARGET_PARENT_COMMAND_LINE"`
	TARGET_WINDOW_TITLE                    string `json:"TARGET_WINDOW_TITLE"`
	TARGET_CURRENT_WORKING_DIRECTORY       string `json:"TARGET_CURRENT_WORKING_DIRECTORY"`
	TARGET_IMAGE_FILE_NAME                 string `json:"TARGET_IMAGE_FILE_NAME"`
	TARGET_COMMAND_LINE                    string `json:"TARGET_COMMAND_LINE"`
}

// Creates a bufio scanner for unit-test verification of file output logs
func logScanner() (*os.File, *bufio.Scanner) {
	logFile, err := os.Open(luna.Settings["FILE_OUTPUT"])
	if err != nil {
		log.Fatalf("Unable to open log file %v:%v\n", luna.Settings["FILE_OUTPUT"], err)
	}
	scanner := bufio.NewScanner(logFile)

	return logFile, scanner
}
