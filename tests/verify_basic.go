// This module contains functions that verify whether unit tests passed or failed
package MoonMon

import (
	"encoding/json"
	"fmt"
	"log"
	"strings"
	"time"
)

// At the end of unit tests, a "marker" log event is generated.
// this function polls the logs until that marker shows up and exits when it does
func VerifyStart() {
	ready := false

	for !ready {
		log.Println("Checking for basic test completion marker")
		time.Sleep(5000 * time.Millisecond)
		logFile, scanner := logScanner()

		for scanner.Scan() {
			line := scanner.Bytes()
			if strings.Contains(string(line), "basic_test_done.txt") {
				ready = true
				break
			}
		}
		logFile.Close()
	}
	log.Println("Verifying basic tests")
}

// Converts a LogEntry to a json string
func jstr(e LogEntry) string {
	jsonData, err := json.Marshal(e)
	if err != nil {
		log.Println("Error marshaling JSON:", err)
		return fmt.Sprintf("%v", e)
	}
	return string(jsonData)
}

// Scans the file output JSON log file for matching unit tests
// It then logs the details of which tests failed or passed.
func VerifyCore() {
	logFile, scanner := logScanner()
	defer logFile.Close()
	testStatus := make(map[string]bool, 32)
	// Yes, these would be false by default anyways :)
	testStatus["ProcessBlockList"] = false
	testStatus["ProcessIncludeList"] = false
	testStatus["ProcessExcludeList"] = false
	testStatus["ProcessTerminateIncludeList"] = false
	testStatus["ProcessTerminateExcludeList"] = false
	testStatus["ThreadIncludeList"] = false
	testStatus["ThreadExcludeList"] = false
	testStatus["ModuleIncludeList"] = false
	testStatus["ModuleExcludeList"] = false
	testStatus["ObjectBlockList"] = false
	testStatus["ObjectIncludeList"] = false
	testStatus["ObjectExcludeList"] = false
	testStatus["RegistryBlockList"] = false
	testStatus["RegistryIncludeList"] = false
	testStatus["RegistryExcludeList"] = false
	testStatus["FileCreateBlockList"] = false
	testStatus["FileCreateIncludeList"] = false
	testStatus["FileCreateExcludeList"] = false
	testStatus["FileSetBlockList"] = false
	testStatus["FileSetIncludeList"] = false
	testStatus["FileSetExcludeList"] = false
	testStatus["WFPIncludeList"] = false
	testStatus["WFPExcludeList"] = false
	testStatus["WFPIncludeListV6"] = false
	testStatus["WFPExcludeListV6"] = false
	for scanner.Scan() {
		line := scanner.Bytes()
		var logEntry LogEntry
		if err := json.Unmarshal(line, &logEntry); err != nil {
			log.Printf("Error parsing log line:%v\n\n", err)
			continue
		}

		if strings.Contains(logEntry.RULE_ID, "99990000") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `C:\Luna_tests\PBL\test.exe`) && strings.EqualFold(logEntry.CREATION_STATUS, "0xc0000022") {
			log.Printf("++ Test passed: ProcessBlockList\n")
			testStatus["ProcessBlockList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99990000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}
		if strings.Contains(logEntry.RULE_ID, "99990100") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `C:\Luna_tests\PINC\test.exe`) {
			log.Printf("++ Test passed: ProcessIncludeList\n")
			testStatus["ProcessIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99990100") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99990200") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `C:\Luna_tests\PEXC\test.exe`) {
			log.Printf("++ Test passed: ProcessExcludeList\n")
			testStatus["ProcessExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99990200") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99990300") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `C:\Luna_tests\PTINC\test.exe`) {
			log.Printf("++ Test passed: ProcessTerminateIncludeList\n")
			testStatus["ProcessTerminateIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99990300") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99990400") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `C:\Luna_tests\PTEXC\test.exe`) {
			log.Printf("++ Test passed: ProcessTerminateExcludeList\n")
			testStatus["ProcessTerminateExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99990400") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999906000") &&
			strings.Contains(strings.ToLower(logEntry.IMAGE_FILE_NAME), `create_remote_thread.exe`) &&
			strings.Contains(strings.ToLower(logEntry.TARGET_IMAGE_FILE_NAME), `luna_tests\tinc\`) {
			log.Printf("++ Test passed: ThreadIncludeList\n")
			testStatus["ThreadIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999906000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}
		if strings.Contains(logEntry.RULE_ID, "999907000") &&
			strings.Contains(strings.ToLower(logEntry.IMAGE_FILE_NAME), `create_remote_thread.exe`) &&
			strings.Contains(strings.ToLower(logEntry.TARGET_IMAGE_FILE_NAME), `luna_tests\texc\`) {
			log.Printf("++ Test passed: ThreadExcludeList\n")
			testStatus["ThreadExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999907000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99990900") && strings.Contains(strings.ToLower(logEntry.LOADED_MODULE), `c:\luna_tests\minc\`) {
			log.Printf("++ Test passed: ModuleIncludeList\n")
			testStatus["ModuleIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99990900") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99991000") && strings.Contains(strings.ToLower(logEntry.LOADED_MODULE), `c:\luna_tests\mexc\`) {
			log.Printf("++ Test passed: ModuleExcludeList\n")
			testStatus["ModuleExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99991000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99991100") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `c:\Windows\system32\rundll32.exe`) &&
			strings.Contains(strings.ToLower(logEntry.TARGET_IMAGE_FILE_NAME), `c:\luna_tests\obl`) && strings.EqualFold(logEntry.PROCESS_HANDLE_DESIRED_ACCESS, "0x0") {
			log.Printf("++ Test passed: ObjectBlockList\n")
			testStatus["ObjectBlockList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99991100") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99991200") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `c:\Windows\system32\rundll32.exe`) &&
			strings.Contains(strings.ToLower(logEntry.TARGET_IMAGE_FILE_NAME), `c:\luna_tests\oinc`) {
			log.Printf("++ Test passed: ObjectIncludeList\n")
			testStatus["ObjectIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99991200") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "99991300") && strings.EqualFold(logEntry.IMAGE_FILE_NAME, `c:\Windows\system32\rundll32.exe`) &&
			strings.Contains(strings.ToLower(logEntry.TARGET_IMAGE_FILE_NAME), `c:\luna_tests\oexc`) {
			log.Printf("++ Test passed: ObjectExcludeList\n")
			testStatus["ObjectExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "99991300") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999914000") &&
			strings.HasPrefix(strings.ToLower(logEntry.REGISTRY_KEY), `\registry\machine\software\test_rbl`) &&
			strings.EqualFold(logEntry.REGISTRY_VALUE_NAME, `test_rbl_value`) &&
			strings.EqualFold(logEntry.CREATION_STATUS, "0xc0000022") {
			log.Printf("++ Test passed: RegistryBlockList\n")
			testStatus["RegistryBlockList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999914000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999915000") &&
			strings.HasPrefix(strings.ToLower(logEntry.REGISTRY_KEY), `\registry\machine\software\test_rinc`) &&
			strings.EqualFold(logEntry.REGISTRY_VALUE_NAME, `test_rinc_value`) {
			log.Printf("++ Test passed: RegistryIncludeList\n")
			testStatus["RegistryIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999915000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999916000") &&
			strings.HasPrefix(strings.ToLower(logEntry.REGISTRY_KEY), `\registry\machine\software\test_rexc`) &&
			strings.EqualFold(logEntry.REGISTRY_VALUE_NAME, `test_rexc_value`) {
			log.Printf("++ Test passed: RegistryExcludeList\n")
			testStatus["RegistryExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999916000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999917000") &&
			strings.HasPrefix(strings.ToLower(logEntry.FILE_FINAL_COMPONENT), `test.`) &&
			strings.Contains(strings.ToLower(logEntry.FILE_PARENT_DIR), `\luna_tests\fcbl`) &&
			strings.EqualFold(logEntry.FILE_EXTENSION, `txt`) &&
			strings.EqualFold(logEntry.CREATION_STATUS, "0xc0000022") {
			log.Printf("++ Test passed: FileCreateBlockList\n")
			testStatus["FileCreateBlockList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999917000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999918000") &&
			strings.HasPrefix(strings.ToLower(logEntry.FILE_FINAL_COMPONENT), `test.`) &&
			strings.Contains(strings.ToLower(logEntry.FILE_PARENT_DIR), `\luna_tests\fcinc`) &&
			strings.EqualFold(logEntry.FILE_EXTENSION, `txt`) {
			log.Printf("++ Test passed: FileCreateIncludeList\n")
			testStatus["FileCreateIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999918000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999919000") &&
			strings.HasPrefix(strings.ToLower(logEntry.FILE_FINAL_COMPONENT), `test.`) &&
			strings.Contains(strings.ToLower(logEntry.FILE_PARENT_DIR), `\luna_tests\fcexc`) &&
			strings.EqualFold(logEntry.FILE_EXTENSION, `txt`) {
			log.Printf("++ Test passed: FileCreateExcludeList\n")
			testStatus["FileCreateExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999919000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999920000") &&
			strings.HasSuffix(strings.ToLower(logEntry.FILE_NAME), `c:\luna_tests\fsbl\test.txt`) &&
			strings.HasSuffix(strings.ToLower(logEntry.FILE_NEW_NAME), `c:\luna_tests\fsbl\renamed.txt`) &&
			strings.EqualFold(logEntry.CREATION_STATUS, "0xc0000022") {
			log.Printf("++ Test passed: FileSetBlockList\n")
			testStatus["FileSetBlockList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999920000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999921000") &&
			strings.HasSuffix(strings.ToLower(logEntry.FILE_NAME), `c:\luna_tests\fsinc\test.txt`) &&
			strings.HasSuffix(strings.ToLower(logEntry.FILE_NEW_NAME), `c:\luna_tests\fsinc\renamed.txt`) {
			log.Printf("++ Test passed: FileSetIncludeList\n")
			testStatus["FileSetIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999921000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999922000") &&
			strings.HasSuffix(strings.ToLower(logEntry.FILE_NAME), `c:\luna_tests\fsexc\test.txt`) &&
			strings.HasSuffix(strings.ToLower(logEntry.FILE_NEW_NAME), `c:\luna_tests\fsexc\renamed.txt`) {
			log.Printf("++ Test passed: FileSetExcludeList\n")
			testStatus["FileSetExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999922000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999924000") &&
			strings.Contains(strings.ToLower(logEntry.IMAGE_FILE_NAME), `certutil`) &&
			strings.EqualFold(logEntry.NET_REMOTE_PORT, "8484") &&
			strings.EqualFold(logEntry.NET_REMOTE_IPV4_ADDR, "192.168.56.103") {
			log.Printf("++ Test passed: WFPIncludeList\n")
			testStatus["WFPIncludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999924000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999925000") &&
			strings.Contains(strings.ToLower(logEntry.IMAGE_FILE_NAME), `certutil`) &&
			strings.EqualFold(logEntry.NET_REMOTE_PORT, "8485") &&
			strings.EqualFold(logEntry.NET_REMOTE_IPV4_ADDR, "192.168.56.103") {
			log.Printf("++ Test passed: WFPExcludeList\n")
			testStatus["WFPExcludeList"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999925000") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999924100") &&
			strings.Contains(strings.ToLower(logEntry.IMAGE_FILE_NAME), `certutil`) &&
			strings.EqualFold(logEntry.NET_REMOTE_PORT, "8484") &&
			strings.HasPrefix(strings.ToLower(logEntry.NET_REMOTE_IPV6_ADDR), "fc00:cafe:babe:fee0:") {
			log.Printf("++ Test passed: WFPIncludeListV6\n")
			testStatus["WFPIncludeListV6"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999924100") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}

		if strings.Contains(logEntry.RULE_ID, "999925100") &&
			strings.Contains(strings.ToLower(logEntry.IMAGE_FILE_NAME), `certutil`) &&
			strings.EqualFold(logEntry.NET_REMOTE_PORT, "8485") &&
			strings.HasPrefix(strings.ToLower(logEntry.NET_REMOTE_IPV6_ADDR), "fc00:cafe:babe:fee0:") {
			log.Printf("++ Test passed: WFPExcludeListV6\n")
			testStatus["WFPExcludeListV6"] = true
			//log.Printf(".. %v\n", logEntry)
		} else if strings.Contains(logEntry.RULE_ID, "999925100") {
			log.Printf("[?] %v\n", jstr(logEntry))
		}
	}

	log.Printf("[-] Tests Failed:\n")
	for k, v := range testStatus {
		if !v {
			log.Printf("\t%v:%v\n", k, v)
		}
	}

	log.Printf("[+] Tests Passed:\n")
	for k, v := range testStatus {
		if v {
			log.Printf("\t%v:%v\n", k, v)
		}
	}
}
