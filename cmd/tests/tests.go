// This package and module run unit tests to validate MoonMon's features
package main

import (
	"io"
	"log"
	"os"
	"os/exec"
	"strings"

	luna "github.com/ag-michael/MoonMon/pkg/luna"
	tests "github.com/ag-michael/MoonMon/tests"
	wr "golang.org/x/sys/windows/registry"

	"gopkg.in/yaml.v3"
)

// Reboots after tests are done.
// prevents a reboot loop by checking if the current boot was started by itself.
// Does not actually reboot unless the reboot param is set to true
func Reboot(reboot bool) {
	// If the reboot test has been triggered, don't reboot again
	// else, it will reboot/test infinitely
	if luna.Settings["POST_REBOOT"] == "TRUE" {
		log.Println("Skipping reboot, currently in a post-reboot state.")
		return
	}
	file, err := os.Create(luna.Settings["REBOOT_TEST_PATH"])
	if err != nil {
		log.Fatalf("Can't reboot, unable to create the reboot marker file %v:%v\n", luna.Settings["REBOOT_TEST_PATH"], err)
	}
	defer file.Close()
	_hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, `Software\Microsoft\Windows\CurrentVersion\RunOnce`, wr.ALL_ACCESS)
	if err != nil {
		log.Fatalf("Reboot: Runonce registry key open error.\nError:%v", err)
	}
	defer _hkey.Close()
	_hkey.SetStringValue("reboot_test", luna.Settings["INSTALL_PATH"]+"\\tests.exe "+os.Args[1])
	log.Println("Rebooting test system")
	if reboot {
		exec.Command("shutdown.exe", "/r", "/t", "0").Run()
	}
}

// Test parameter setup
// TODO: Load these from a Yaml file
func setupTestValues() {
	luna.Settings["TEST_PBL_PATH"] = `C:\Luna_tests\PBL\`
	luna.Settings["TEST_PINC_PATH"] = `C:\Luna_tests\PINC\`
	luna.Settings["TEST_PEXC_PATH"] = `C:\Luna_tests\PEXC\`
	luna.Settings["TEST_PTINC_PATH"] = `C:\Luna_tests\PTINC\`
	luna.Settings["TEST_PTEXC_PATH"] = `C:\Luna_tests\PTEXC\`
	luna.Settings["TEST_TINC_PATH"] = `C:\Luna_tests\TINC\`
	luna.Settings["TEST_TEXC_PATH"] = `C:\Luna_tests\TEXC\`
	luna.Settings["TEST_MINC_PATH"] = `C:\Luna_tests\MINC\`
	luna.Settings["TEST_MEXC_PATH"] = `C:\Luna_tests\MEXC\`
	luna.Settings["TEST_OBL_PATH"] = `C:\Luna_tests\OBL\`
	luna.Settings["TEST_OINC_PATH"] = `C:\Luna_tests\OINC\`
	luna.Settings["TEST_OEXC_PATH"] = `C:\Luna_tests\OEXC\`
	luna.Settings["TEST_RBL_PATH"] = `C:\Luna_tests\RBL\`
	luna.Settings["TEST_RINC_PATH"] = `C:\Luna_tests\RINC\`
	luna.Settings["TEST_REXC_PATH"] = `C:\Luna_tests\REXC\`
	luna.Settings["TEST_FCBL_PATH"] = `C:\Luna_tests\FCBL\`
	luna.Settings["TEST_FCINC_PATH"] = `C:\Luna_tests\FCINC\`
	luna.Settings["TEST_FCEXC_PATH"] = `C:\Luna_tests\FCEXC\`
	luna.Settings["TEST_FSBL_PATH"] = `C:\Luna_tests\FSBL\`
	luna.Settings["TEST_FSINC_PATH"] = `C:\Luna_tests\FSINC\`
	luna.Settings["TEST_FSEXC_PATH"] = `C:\Luna_tests\FSEXC\`
	luna.Settings["TEST_WINC_PATH"] = `C:\Luna_tests\WINC\`
	luna.Settings["TEST_WINC_URL"] = `http://192.168.56.103:8484`
	luna.Settings["TEST_WEXC_PATH"] = `C:\Luna_tests\WEXC\`
	luna.Settings["TEST_WEXC_URL"] = `http://192.168.56.103:8485`

	luna.Settings["TEST_WINCV6_PATH"] = `C:\Luna_tests\WINCV6\`
	luna.Settings["TEST_WINCV6_URL"] = `http://[FC00:cafe:babe:fee0::1]:8484`
	luna.Settings["TEST_WEXCV6_PATH"] = `C:\Luna_tests\WEXCV6\`
	luna.Settings["TEST_WEXCV6_URL"] = `http://[FC00:cafe:babe:fee0::1]:8485`
}

// Load test settings from a local Yaml file
func LoadTestSettings(config string) {
	if !strings.HasSuffix(config, ".yaml") {
		log.Fatalf("LoadTestSettings:Specified config file does not end in .yaml\n")
	}
	var installConf luna.Config
	content, err := os.ReadFile(config)
	if err != nil {
		log.Fatalf("Error reading file: %v", err)
	}

	if err := yaml.Unmarshal([]byte(content), &installConf); err != nil {
		log.Fatal(err)
	}
	G := installConf.GlobalSettings
	luna.Settings["INSTALL_PATH"] = G.Install_path
	luna.Settings["SVC_NAME"] = G.Svc_name
	luna.Settings["SVC_DISPLAY_NAME"] = G.Svc_display_name
	luna.Settings["AGENT_SVC_NAME"] = G.Agent_svc_name
	luna.Settings["AGENT_DISPLAY_NAME"] = G.Agent_svc_display_name
	luna.Settings["CONFIG_PATH"] = G.Install_path + "\\Config"
	luna.Settings["REBOOT_TEST_PATH"] = G.Install_path + "\\.reboot_test"
	luna.Settings["TEST_LOG_PATH"] = G.Install_path + "\\tests.log"
	luna.Settings["FILE_OUTPUT"] = G.File_Out
	luna.Settings["SMOL"] = G.Install_path + "\\smol.exe"
	luna.Settings["TINY"] = G.Install_path + "\\tiny.exe"
	luna.Settings["CREATE_REMOTE_THREAD"] = G.Install_path + "\\create_remote_thread.exe"
	info, err := os.Stat(luna.Settings["SMOL"])
	if os.IsNotExist(err) {
		log.Fatalf("LoadTestSettings: Path %v does not exist\n", luna.Settings["SMOL"])
	}
	if info.IsDir() {
		log.Fatalf("LoadTestSettings: Path %v is not a directory\n", luna.Settings["SMOL"])
	}

	luna.Settings["POST_REBOOT"] = "FALSE"
	err = nil
	_, err = os.Stat(luna.Settings["REBOOT_TEST_PATH"])
	if err == nil {
		luna.Settings["POST_REBOOT"] = "TRUE"
		log.Printf("Reboot worked, re-testing post-reboot.\n")
		err = os.Remove(luna.Settings["REBOOT_TEST_PATH"])
		if err != nil {
			log.Printf("Warning: Failed to remove post-reboot marker file %v:%v\n", luna.Settings["REBOOT_TEST_PATH"], err)
		}
	} else {
		log.Printf("Post-reboot marker not found:%v\n", err)
	}

	setupTestValues()
}

// Runs unit tests in a series
func RunTests() {
	log.Println("Running tests")
	tests.TestProcessBlockList()
	tests.TestProcessIncludeList()
	tests.TestProcessExcludeList()
	tests.TestProcessTerminateIncludeList()
	tests.TestProcessTerminateExcludeList()
	tests.TestThreadIncludeList()
	tests.TestThreadExcludeList()
	tests.TestModuleIncludeList()
	tests.TestModuleExcludeList()
	tests.TestObjectBlockList()
	tests.TestObjectIncludeList()
	tests.TestObjectExcludeList()
	tests.TestRegistryBlockList()
	tests.TestRegistryIncludeList()
	tests.TestRegistryExcludeList()
	tests.TestFileCreateBlockList()
	tests.TestFileCreateIncludeList()
	tests.TestFileCreateExcludeList()
	tests.TestFileSetBlockList()
	tests.TestFileSetIncludeList()
	tests.TestFileSetExcludeList()
	tests.TestWFPIncludeList()
	tests.TestWFPExcludeList()
	tests.TestWFPIncludeListV6()
	tests.TestWFPExcludeListV6()
	tests.TestBasicDone()
	log.Println("Finished running tests")
}

// Verifies results of unit tests
func VerifyTests() {
	log.Println("Verifying test results")
	tests.VerifyStart()
	tests.VerifyCore()
}

// Configures a file output for test logging in addition to STDOUT
func setupOutput() {
	logfile, err := os.OpenFile(luna.Settings["TEST_LOG_PATH"], os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0700)
	if err != nil {
		log.Println(err)
	}
	multiWriter := io.MultiWriter(os.Stdout, logfile)
	log.SetOutput(multiWriter)
}

func main() {
	if len(os.Args) < 2 {
		log.Fatalf(`
Usage:
  %v  c:\\some\\path\\config.yaml [Reboot]
`, os.Args[0])
	}

	luna.Settings = make(map[string]string, 256)
	LoadTestSettings(os.Args[1])

	setupOutput()
	log.Printf("Loaded test settings\n")
	RunTests()
	VerifyTests()
	if len(os.Args) == 3 && strings.EqualFold(os.Args[2], "reboot") {
		Reboot(true)
	} else {
		Reboot(false)
	}
}
