// This module contains basic unit tests to validate MoonMon's features
package MoonMon

import (
	"log"
	"os"
	"os/exec"
	"strconv"

	wr "golang.org/x/sys/windows/registry"

	luna "github.com/ag-michael/MoonMon/pkg/luna"
)

func TestBasicDone() {
	exec.Command("cmd.exe", "/c", "echo", "done", ">", `c:\windows\temp\basic_test_done.txt`).Run()
}
func TestProcessBlockList() {
	log.Println("Testing: TestProcessBlockList")

	err := os.MkdirAll(luna.Settings["TEST_PBL_PATH"], 0700)
	if err != nil {
		log.Printf("Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["SMOL"], luna.Settings["TEST_PBL_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("Error:%v\n%v", err, out)

	}
	exec.Command(luna.Settings["TEST_PBL_PATH"]+"test.exe", "TEST_PBL").Run()
	log.Println("Testing-finished: TestProcessBlockList")

}

func TestProcessIncludeList() {
	log.Println("Testing: TestProcessIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_PINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestProcessIncludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["SMOL"], luna.Settings["TEST_PINC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestProcessIncludeList Error:%v\n%v", err, out)

	}
	exec.Command(luna.Settings["TEST_PINC_PATH"]+"test.exe", "TEST_PINC").Run()
	log.Println("Testing-finished: TestProcessIncludeList")

}
func TestProcessExcludeList() {
	log.Println("Testing: TestProcessExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_PEXC_PATH"], 0700)
	if err != nil {
		log.Printf("Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["SMOL"], luna.Settings["TEST_PEXC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("Error:%v\n%v", err, out)

	}
	exec.Command(luna.Settings["TEST_PEXC_PATH"]+"test.exe", "TEST_PEXC").Run()
	log.Println("Testing-finished: TestProcessExcludeList")

}

func TestProcessTerminateIncludeList() {
	// Prepare for test
	log.Println("Testing: TestProcessTerminateIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_PTINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestProcessTerminateIncludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["SMOL"], luna.Settings["TEST_PTINC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestProcessTerminateIncludeList Error:%v\n%v", err, out)

	}
	cmd = exec.Command("cmd.exe", "/c", "start", luna.Settings["TEST_PTINC_PATH"]+"test.exe")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestProcessTerminateIncludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command("cmd.exe", "/c", "taskkill", "/F", "/IM:test.exe")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestProcessTerminateIncludeList taskkill Error:%v\n%v", err, out)

	}
	log.Println("Testing-finished: TestProcessTerminateIncludeList")

}

func TestProcessTerminateExcludeList() {
	// Prepare for test
	log.Println("Testing: TestProcessTerminateExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_PTEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestProcessTerminateExcludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["SMOL"], luna.Settings["TEST_PTEXC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestProcessTerminateExcludeList Error:%v\n%v", err, out)

	}
	cmd = exec.Command("cmd.exe", "/c", "start", luna.Settings["TEST_PTEXC_PATH"]+"test.exe")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestProcessTerminateExcludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command("cmd.exe", "/c", "taskkill", "/F", "/IM:test.exe")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestProcessTerminateExcludeList taskkill Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestProcessTerminateExcludeList")

}

func TestThreadIncludeList() {
	// Prepare for test
	log.Println("Testing: TestThreadIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_TINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestThreadIncludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["TINY"], luna.Settings["TEST_TINC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestThreadIncludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command(luna.Settings["TEST_TINC_PATH"] + "\\test.exe")
	cmd.Start()
	cmdpid := strconv.Itoa(cmd.Process.Pid)
	log.Printf("Test process started:%v\n", cmdpid)
	cmd = exec.Command(luna.Settings["CREATE_REMOTE_THREAD"], "-debug", "-pid", cmdpid)
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestThreadIncludeList CREATE_REMOTE_THREAD Error:%v\n%s", err, out)

	}
	log.Println(string(out))
	log.Println("Testing-finished: TestThreadIncludeList")

}

func TestThreadExcludeList() {
	// Prepare for test
	log.Println("Testing: TestThreadExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_TEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestThreadExcludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["TINY"], luna.Settings["TEST_TEXC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestThreadExcludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command(luna.Settings["TEST_TEXC_PATH"] + "\\test.exe")
	cmd.Start()
	cmdpid := strconv.Itoa(cmd.Process.Pid)
	log.Printf("Test process started:%v\n", cmdpid)
	cmd = exec.Command(luna.Settings["CREATE_REMOTE_THREAD"], "-debug", "-pid", cmdpid)
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestThreadExcludeList CREATE_REMOTE_THREAD Error:%v\n%s", err, out)

	}
	log.Println(string(out))
	log.Println("Testing-finished: TestThreadExcludeList")

}

func TestModuleIncludeList() {
	// Prepare for test
	log.Println("Testing: TestModuleIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_MINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestModuleIncludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", `c:\windows\system32\kernel32.dll`, luna.Settings["TEST_MINC_PATH"]+"\\MINC.dll")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestModuleIncludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	exec.Command(`C:\Windows\System32\rundll32.exe`, luna.Settings["TEST_MINC_PATH"]+"\\MINC.dll,#1").Run()
	log.Println("Testing-finished: TestModuleIncludeList")

}

func TestModuleExcludeList() {
	// Prepare for test
	log.Println("Testing: TestModuleExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_MEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestModuleExcludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", `c:\windows\system32\kernel32.dll`, luna.Settings["TEST_MEXC_PATH"]+"\\MEXC.dll")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestModuleExcludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	exec.Command(`C:\Windows\System32\rundll32.exe`, luna.Settings["TEST_MEXC_PATH"]+"\\MEXC.dll,#1").Run()
	log.Println("Testing-finished: TestModuleExcludeList")

}

func TestObjectBlockList() {
	// Prepare for test
	log.Println("Testing: TestObjectBlockList")

	err := os.MkdirAll(luna.Settings["TEST_OBL_PATH"], 0700)
	if err != nil {
		log.Printf("TestObjectBlockList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["TINY"], luna.Settings["TEST_OBL_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestObjectBlockList Error:%v\n%s", err, out)

	}
	// Actual Test
	cmd = exec.Command(luna.Settings["TEST_OBL_PATH"] + "\\test.exe")
	cmd.Start()
	cmdpid := strconv.Itoa(cmd.Process.Pid)
	log.Printf("Test process started:%v\n", cmdpid)
	cmd = exec.Command(`C:\Windows\System32\rundll32.exe`, `C:\Windows\System32\comsvcs.dll`, `MiniDump`, cmdpid, luna.Settings["TEST_OBL_PATH"]+"\\dump.dmp", "full")
	_, err = cmd.Output()
	if err != nil {
		log.Printf("TestObjectBlockList rundll32 Error:%v\n%s", err, out)

	}
	//log.Println(string(cout))
	log.Println("Testing-finished: TestObjectBlockList")

}

func TestObjectIncludeList() {
	// Prepare for test
	log.Println("Testing: TestObjectIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_OINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestObjectIncludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["TINY"], luna.Settings["TEST_OINC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestObjectIncludeList Error:%v\n%s", err, out)

	}
	// Actual Test
	cmd = exec.Command(luna.Settings["TEST_OINC_PATH"] + "\\test.exe")
	cmd.Start()
	cmdpid := strconv.Itoa(cmd.Process.Pid)
	log.Printf("Test process started:%v\n", cmdpid)
	cmd = exec.Command(`C:\Windows\System32\rundll32.exe`, `C:\Windows\System32\comsvcs.dll`, `MiniDump`, cmdpid, luna.Settings["TEST_OINC_PATH"]+"dump.dmp", "full")
	_, err = cmd.Output()
	if err != nil {
		log.Printf("TestObjectIncludeList rundll32 Error:%v\n%s", err, out)

	}

	log.Println("Testing-finished: TestObjectIncludeList")

}

func TestObjectExcludeList() {
	// Prepare for test
	log.Println("Testing: TestObjectExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_OEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestObjectExcludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", luna.Settings["TINY"], luna.Settings["TEST_OEXC_PATH"]+"test.exe")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestObjectExcludeList Error:%v\n%s", err, out)

	}
	// Actual Test
	cmd = exec.Command(luna.Settings["TEST_OEXC_PATH"] + "test.exe")
	cmd.Start()
	cmdpid := strconv.Itoa(cmd.Process.Pid)
	log.Printf("Test process started:%v\n", cmdpid)
	cmd = exec.Command(`C:\Windows\System32\rundll32.exe`, `C:\Windows\System32\comsvcs.dll`, `MiniDump`, cmdpid, luna.Settings["TEST_OEXC_PATH"]+"dump.dmp", "full")
	_, err = cmd.Output()
	if err != nil {
		log.Printf("TestObjectExcludeList rundll32 Error:%v\n%s", err, out)

	}

	log.Println("Testing-finished: TestObjectExcludeList")

}

func TestRegistryBlockList() {
	// Prepare for test
	log.Println("Testing: TestRegistryBlockList")
	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, `Software`, wr.ALL_ACCESS)
	if err != nil {
		log.Fatalf("TestRegistryBlockList: Registry key:Software\nError:%v", err)
	}
	defer hkey.Close()

	// Actual Test

	testkey, _, err := wr.CreateKey(hkey, "TEST_RBL", wr.ALL_ACCESS)
	if err != nil {
		log.Printf("TestRegistryBlockList: Error creating the settings sub-key:%v\n", err)
	} else {
		defer testkey.Close()
		log.Println("TestRegistryBlockList: Test key creation worked, attempting to set test value.")

		err = testkey.SetStringValue("TEST_RBL_VALUE", "TEST_RBL_VALUE_DATA")
		if err != nil {
			log.Printf("Error reported when copying config files:%v\n", err)
		}
	}

	log.Println("Testing-finished: TestRegistryBlockList")

}

func TestRegistryIncludeList() {
	// Prepare for test
	log.Println("Testing: TestRegistryIncludeList")

	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, `Software`, wr.ALL_ACCESS)
	if err != nil {
		log.Fatalf("TestRegistryIncludeList: Registry key:Software\nError:%v", err)
	}
	defer hkey.Close()

	// Actual Test

	testkey, _, err := wr.CreateKey(hkey, "TEST_RINC", wr.ALL_ACCESS)
	if err != nil {
		log.Printf("TestRegistryIncludeList: Error creating the settings sub-key:%v\n", err)
	} else {
		defer testkey.Close()
		log.Println("TestRegistryIncludeList: Test key creation worked, attempting to set test value.")

		err = testkey.SetStringValue("TEST_RINC_VALUE", "TEST_RINC_VALUE_DATA")
		if err != nil {
			log.Printf("Error reported when copying config files:%v\n", err)
		}
	}
	log.Println("Testing-finished: TestRegistryIncludeList")

}

func TestRegistryExcludeList() {
	// Prepare for test
	log.Println("Testing: TestRegistryExcludeList")

	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, `Software`, wr.ALL_ACCESS)
	if err != nil {
		log.Fatalf("TestRegistryExcludeList: Registry key:Software\nError:%v", err)
	}
	defer hkey.Close()

	// Actual Test

	testkey, _, err := wr.CreateKey(hkey, "TEST_REXC", wr.ALL_ACCESS)
	if err != nil {
		log.Printf("TestRegistryExcludeList: Error creating the settings sub-key:%v\n", err)
	} else {
		defer testkey.Close()
		log.Println("TestRegistryExcludeList: Test key creation worked, attempting to set test value.")

		err = testkey.SetStringValue("TEST_REXC_VALUE", "TEST_REXC_VALUE_DATA")
		if err != nil {
			log.Printf("Error reported when copying config files:%v\n", err)
		}
	}

	log.Println("Testing-finished: TestRegistryExcludeList")

}

func TestFileCreateBlockList() {
	// Prepare for test
	log.Println("Testing: TestFileCreateBlockList")

	err := os.MkdirAll(luna.Settings["TEST_FCBL_PATH"], 0700)
	if err != nil {
		log.Printf("TestFileCreateBlockList: Mkdir:%v\n", err)
	}

	// Actual Test
	cmd := exec.Command("cmd.exe", "/c", "echo", "TEST_FCBL", ">", luna.Settings["TEST_FCBL_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestFileCreateBlockList Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestFileCreateBlockList")

}

func TestFileCreateIncludeList() {
	// Prepare for test
	log.Println("Testing: TestFileCreateIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_FCINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestFileCreateIncludeList: Mkdir:%v\n", err)
	}
	// Actual Test
	cmd := exec.Command("cmd.exe", "/c", "echo", "TEST_FCINC", ">", luna.Settings["TEST_FCINC_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestFileCreateIncludeList Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestFileCreateIncludeList")

}

func TestFileCreateExcludeList() {
	// Prepare for test
	log.Println("Testing: TestFileCreateExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_FCEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestFileCreateExcludeList: Mkdir:%v\n", err)
	}
	// Actual Test
	cmd := exec.Command("cmd.exe", "/c", "echo", "TEST_FCEXC", ">", luna.Settings["TEST_FCEXC_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestFileCreateExcludeList Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestFileCreateExcludeList")

}

func TestFileSetBlockList() {
	// Prepare for test
	log.Println("Testing: TestFileSetBlockList")

	err := os.MkdirAll(luna.Settings["TEST_FSBL_PATH"], 0700)
	if err != nil {
		log.Printf("TestFileSetBlockList: Mkdir:%v\n", err)
	}

	cmd := exec.Command("cmd.exe", "/c", "echo", "TEST_FSBL", ">", luna.Settings["TEST_FSBL_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestFileSetBlockList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command("cmd.exe", "/c", "rename", luna.Settings["TEST_FSBL_PATH"]+"test.txt", "renamed.txt")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestFileSetBlockList Rename Error:%v\n%v", err, out)

	}
	log.Println("Testing-finished: TestFileSetBlockList")

}

func TestFileSetIncludeList() {
	// Prepare for test
	log.Println("Testing: TestFileSetIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_FSINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestFileSetIncludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "echo", "TEST_FSINC", ">", luna.Settings["TEST_FSINC_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestFileSetIncludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command("cmd.exe", "/c", "rename", luna.Settings["TEST_FSINC_PATH"]+"test.txt", "renamed.txt")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestFileSetIncludeList Rename Error:%v\n%v", err, out)

	}
	log.Println("Testing-finished: TestFileSetIncludeList")

}

func TestFileSetExcludeList() {
	// Prepare for test
	log.Println("Testing: TestFileSetExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_FSEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestFileSetExcludeList: Mkdir:%v\n", err)
	}
	cmd := exec.Command("cmd.exe", "/c", "echo", "TEST_FSEXC", ">", luna.Settings["TEST_FSEXC_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestFileSetExcludeList Error:%v\n%v", err, out)

	}
	// Actual Test
	cmd = exec.Command("cmd.exe", "/c", "rename", luna.Settings["TEST_FSEXC_PATH"]+"test.txt", "renamed.txt")
	out, err = cmd.Output()
	if err != nil {
		log.Printf("TestFileSetExcludeList Rename Error:%v\n%v", err, out)

	}
	log.Println("Testing-finished: TestFileSetExcludeList")

}

func TestWFPIncludeList() {
	// Prepare for test
	log.Println("Testing: TestWFPIncludeList")

	err := os.MkdirAll(luna.Settings["TEST_WINC_PATH"], 0700)
	if err != nil {
		log.Printf("TestWFPIncludeList: Mkdir:%v\n", err)
	}
	// Actual Test

	cmd := exec.Command("cmd.exe", "/c", "certutil.exe", "-urlcache", "-f", luna.Settings["TEST_WINC_URL"], luna.Settings["TEST_WINC_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestWFPIncludeList Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestWFPIncludeList")

}

func TestWFPExcludeList() {
	// Prepare for test
	log.Println("Testing: TestWFPExcludeList")

	err := os.MkdirAll(luna.Settings["TEST_WEXC_PATH"], 0700)
	if err != nil {
		log.Printf("TestWFPExcludeList: Mkdir:%v\n", err)
	}
	// Actual Test

	cmd := exec.Command("cmd.exe", "/c", "certutil.exe", "-urlcache", "-f", luna.Settings["TEST_WEXC_URL"], luna.Settings["TEST_WEXC_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestWFPExcludeList Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestWFPExcludeList")

}

func TestWFPIncludeListV6() {
	// Prepare for test
	log.Println("Testing: TestWFPIncludeListV6")

	err := os.MkdirAll(luna.Settings["TEST_WINCV6_PATH"], 0700)
	if err != nil {
		log.Printf("TestWFPIncludeListV6: Mkdir:%v\n", err)
	}
	// Actual Test

	cmd := exec.Command("cmd.exe", "/c", "certutil.exe", "-urlcache", "-f", luna.Settings["TEST_WINCV6_URL"], luna.Settings["TEST_WINCV6_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestWFPIncludeListV6 Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestWFPIncludeListV6")

}

func TestWFPExcludeListV6() {
	// Prepare for test
	log.Println("Testing: TestWFPExcludeListV6")

	err := os.MkdirAll(luna.Settings["TEST_WEXCV6_PATH"], 0700)
	if err != nil {
		log.Printf("TestWFPExcludeListV6: Mkdir:%v\n", err)
	}
	// Actual Test

	cmd := exec.Command("cmd.exe", "/c", "certutil.exe", "-urlcache", "-f", luna.Settings["TEST_WEXCV6_URL"], luna.Settings["TEST_WEXCV6_PATH"]+"test.txt")
	out, err := cmd.Output()
	if err != nil {
		log.Printf("TestWFPExcludeListV6 Error:%v\n%v", err, out)

	}

	log.Println("Testing-finished: TestWFPExcludeListV6")

}
