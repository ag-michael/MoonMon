// This module handles Yaml configuration parsing and processing
// as well as install/uninstall procedures of the application.
package MoonMon

import (
	"encoding/binary"
	"fmt"
	"io/fs"
	"log"
	"os"
	"os/exec"
	"strconv"
	"strings"

	wr "golang.org/x/sys/windows/registry"
	"gopkg.in/yaml.v3"
)

var Conf Config
var ids = make(map[uint32]bool)
var entryMap = make(map[uint32]string)
var ROTATE_COUNT uint
var ROTATE_SIZE uint

// A map of ConfigList struct objects with the
// corresponding field id value
type ConfigListMap struct {
	list ConfigList
	id   int
}

// This is the configuration struct that holds
// all Unmarshal'ed configuration content
type Config struct {
	GlobalSettings struct {
		Svc_name               string `yaml:"service_name"`
		Svc_display_name       string `yaml:"service_display_name"`
		Agent_svc_name         string `yaml:"agent_service_name"`
		Agent_svc_display_name string `yaml:"agent_service_display_name"`
		Install_path           string `yaml:"install_path"`
		Install_source_path    string `yaml:"install_source_path"`
		Remote_Config_URL      string `yaml:"remote_config_url,omitempty"`
		File_Out               string `yaml:"file_output,omitempty"`
		File_Rotate_Size       uint   `yaml:"file_output_rotate_size"`
		File_Rotate_Count      uint   `yaml:"file_output_rotate_count"`
		Keep_Excludes          bool   `yaml:"keep_excludes"`
		Resist_Tampering       bool   `yaml:"resist_tampering"`
		Process_Callback       bool   `yaml:"process_monitoring"`
		Thread_Callback        bool   `yaml:"thread_monitoring"`
		Module_Callback        bool   `yaml:"module_monitoring"`
		Object_Callback        bool   `yaml:"object_monitoring"`
		Registry_Callback      bool   `yaml:"registry_monitoring"`
		File_Callback          bool   `yaml:"file_monitoring"`
		WFP_Callback           bool   `yaml:"network_monitoring"`
	} `yaml:"global_settings"`
	ProcessBlockList             ConfigList `yaml:"process_block_list,omitempty"`
	ProcessIncludeList           ConfigList `yaml:"process_include_list,omitempty"`
	ProcessExcludeList           ConfigList `yaml:"process_exclude_list,omitempty"`
	ProcessTerminatedIncludeList ConfigList `yaml:"process_terminate_include_list,omitempty"`
	ProcessTerminatedExcludeList ConfigList `yaml:"process_terminate_exclude_list,omitempty"`
	ThreadBlockList              ConfigList `yaml:"remote_thread_block_list,omitempty"`
	ThreadIncludeList            ConfigList `yaml:"remote_thread_include_list,omitempty"`
	ThreadExcludeList            ConfigList `yaml:"remote_thread_exclude_list,omitempty"`
	ModuleBlockList              ConfigList `yaml:"image_load_block_list,omitempty"`
	ModuleIncludeList            ConfigList `yaml:"image_load_include_list,omitempty"`
	ModuleExcludeList            ConfigList `yaml:"image_load_exclude_list,omitempty"`
	ObjectBlockList              ConfigList `yaml:"object_access_block_list,omitempty"`
	ObjectIncludeList            ConfigList `yaml:"object_access_include_list,omitempty"`
	ObjectExcludeList            ConfigList `yaml:"object_access_exclude_list,omitempty"`
	RegistryBlockList            ConfigList `yaml:"registry_block_list,omitempty"`
	RegostryIncludeList          ConfigList `yaml:"registry_include_list,omitempty"`
	RegistryExcludeList          ConfigList `yaml:"registry_exclude_list,omitempty"`
	FileCreateBlockList          ConfigList `yaml:"file_create_block_list,omitempty"`
	FileCreateIncludeList        ConfigList `yaml:"file_create_include_list,omitempty"`
	FileCreateExcludeList        ConfigList `yaml:"file_create_exclude_list,omitempty"`
	FileSetInfoBlockList         ConfigList `yaml:"file_set_information_block_list,omitempty"`
	FileSetInfoIncludeList       ConfigList `yaml:"file_set_information_include_list,omitempty"`
	FileSetInfoExcludeList       ConfigList `yaml:"file_set_information_exclude_list,omitempty"`
	WfpBlockList                 ConfigList `yaml:"network_block_list,omitempty"`
	WfpIncludeList               ConfigList `yaml:"network_include_list,omitempty"`
	WfpExcludeList               ConfigList `yaml:"network_exclude_list,omitempty"`
}

// Entries in a configuration list
type ConfigList struct {
	Entries []Entry `yaml:"entries"`
}

// An entry in a config list
// All rules in an entry must match for action
// to be taken.
type Entry struct {
	ID     uint   `yaml:"id"`
	Name   string `yaml:"name,omitempty"`
	Action string `yaml:"action,omitempty"`
	Rules  []Rule `yaml:"rules"`
}

// A rule object
// used to evaluate match logic such as:
// field <condition> value
// e.g.: field_name contains "value"
type Rule struct {
	Condition string `yaml:"condition"`
	Field     string `yaml:"field"`
	Value     string `yaml:"value"`
}

// Resolves matching rule id's to the entry's name
// For logging purposes.
func ResolveRuleIdToName(match_id uint32) string {
	return entryMap[match_id]
}

// Processes a configuration list
// This means taking the parsed configuration and
// applying the correct registry entries
func processList(list ConfigList, list_type int) {
	prefix, ok := Lists[list_type]
	if !ok {
		log.Fatalf("Unsupported list type %v", list_type)
	}
	log.Printf("Processing %v\n", prefix)

	size := uint32(0)
	item := 0
	field_type := uint16(0)
	match_type := byte(0)
	id := uint32(0)
	actions := uint32(0)
	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, Settings["SVC_REGISTRY_SETTINGS"], wr.ALL_ACCESS)
	if err != nil {
		log.Printf("processList: Registry key:%v\nError:%v", Settings["SVC_REGISTRY_SETTINGS"], err)
		log.Fatal(err)
	}
	defer hkey.Close()

	for i, entry := range list.Entries {
		log.Printf("Entry %d:\n", i+1)
		if entry.ID != 0 && entry.ID < uint(0xffffffff) {
			log.Printf("%sID: %d\n", prefix, entry.ID)
			id = uint32(entry.ID)
			if !ids[id] {
				ids[id] = true
				log.Printf("ID %v is unique\n", id)
			} else {
				log.Printf("ID %v is not globally unique,skipping it\n", id)
				continue
			}
		} else {
			log.Fatalf("Invalid entry id:%d, entry IDs (rule IDs) must have a value between 1 and %d\n", entry.ID, uint(0xfffffffe))
		}
		if entry.Name != "" && len(entry.Name) > 2 {
			log.Printf("  Name: %s\n", entry.Name)
			entryMap[uint32(entry.ID)] = entry.Name
		} else if len(entry.Name) > 200 {
			log.Fatalf("Entry names can't be less than 3 characters or more than 200 characters:%v", entry.Name)
		}
		if entry.Action != "" {
			log.Printf("  Action: %s\n", entry.Action)
			actions = uint32(StringValueToKey(Actions, entry.Action))
			if actions == 0 {
				log.Fatalf("Action %v specified, but it isn't supported!", entry.Action)
			}
		}
		for j, cond := range entry.Rules {
			log.Printf("      Rule %d:\n", j+1)
			match_type = byte(StringValueToKey(Conditions, cond.Condition))
			if match_type == 0 {
				match_type = byte(StringValueToKey(Conditions_Alt, cond.Condition))
			}
			log.Printf("        Match condition (operator):[%d] %s\n", match_type, cond.Condition)
			field_type = uint16(StringValueToKey(Fields, cond.Field))
			log.Printf("        Field: [%d] %s\n", field_type, cond.Field)
			log.Printf("        Value: %s\n", cond.Value)

			data_bytes := StringToMessage(int(field_type), cond.Value)
			valueValidated := ValidateFieldValue(cond.Field, cond.Value)
			size = uint32(len(data_bytes))
			//	log.Printf("BYTES:%v\n", data_bytes)
			if !valueValidated || id == 0 || size == 0 || list_type == 0 || field_type == 0 || match_type == 0 || data_bytes == nil {
				log.Printf("Validated:%v, id:%v, size:%v, list_type:%v, field_type:%v,match_type:%v,data_bytes_nil:%v\n", valueValidated, id, size, list_type, field_type, match_type, (data_bytes == nil))

				log.Fatal("Invalid entry value somewhere...(look at the last block of config)")
			}

			valueName := fmt.Sprintf("%s%s_%d", prefix, strconv.Itoa(int(id)), item)

			regentry := fmt.Sprintf("%s%d:%d,%d,%d,%d,%d:%T", prefix, id, size, field_type, match_type, id, actions, data_bytes)
			log.Println(regentry)

			regData := make([]byte, 18+size)
			binary.LittleEndian.PutUint32(regData, size)
			binary.LittleEndian.PutUint16(regData[4:], uint16(list_type))
			binary.LittleEndian.PutUint16(regData[6:], field_type)
			regData[8] = match_type
			binary.LittleEndian.PutUint32(regData[9:], id)
			binary.LittleEndian.PutUint32(regData[13:], actions)
			copy(regData[17:], data_bytes)
			if err := hkey.SetBinaryValue(valueName, regData); err != nil {
				log.Fatalf("Error setting binary value: %v", err)
			}
			log.Printf("Registry set value on %v\n", valueName)
			item += 1
		}
	}

}

// Processes global settings parsed from the Yaml configs
// And updates the correct registry keys and values
func processGlobalSettings(C Config) {
	G := C.GlobalSettings
	Settings["SVC_PATH"] = G.Install_path + "\\MoonMon.sys"
	Settings["SVC_NAME"] = G.Svc_name
	Settings["SVC_DISPLAY_NAME"] = G.Svc_display_name
	Settings["AGENT_SVC_PATH"] = G.Install_path + "\\luna.exe"
	Settings["AGENT_SVC_NAME"] = G.Agent_svc_name
	Settings["AGENT_DISPLAY_NAME"] = G.Agent_svc_display_name
	Settings["CONFIG_PATH"] = G.Install_path + "\\Config"
	Settings["FILE_OUTPUT"] = G.File_Out
	ROTATE_SIZE = G.File_Rotate_Size
	ROTATE_COUNT = G.File_Rotate_Count
	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, Settings["SVC_REGISTRY_SETTINGS"], wr.ALL_ACCESS)
	if err != nil {
		log.Printf("processGlobalSettings: Registry key:%v\nError:%v", Settings["SVC_REGISTRY_SETTINGS"], err)
		log.Fatal(err)
	}

	hkey.SetStringValue("SVC_PATH", Settings["SVC_PATH"])
	hkey.SetStringValue("AGENT_SVC_PATH", Settings["AGENT_SVC_PATH"])
	hkey.SetStringValue("CONFIG_PATH", Settings["CONFIG_PATH"])
	hkey.SetStringValue("FILE_OUTPUT", Settings["FILE_OUTPUT"])

	true_byte := make([]byte, 1)
	true_byte[0] = byte(1)
	if G.Keep_Excludes {
		hkey.SetBinaryValue("KEEP_EXCLUDES", true_byte)
	}
	if G.Resist_Tampering {
		hkey.SetBinaryValue("RESIST_TAMPERING", true_byte)
	}
	if G.Process_Callback {
		hkey.SetBinaryValue("PROCESS_CALLBACK", true_byte)
	}
	if G.Thread_Callback {
		hkey.SetBinaryValue("THREAD_CALLBACK", true_byte)
	}
	if G.Module_Callback {
		hkey.SetBinaryValue("MODULE_CALLBACK", true_byte)
	}
	if G.Object_Callback {
		hkey.SetBinaryValue("OBJECT_CALLBACK", true_byte)
	}
	if G.Registry_Callback {
		hkey.SetBinaryValue("REGISTRY_CALLBACK", true_byte)
	}
	if G.File_Callback {
		hkey.SetBinaryValue("FILE_CALLBACK", true_byte)
	}
	if G.WFP_Callback {
		hkey.SetBinaryValue("WFP_CALLBACK", true_byte)
	}
}

// Takes the service (driver) name to disconver the configuration file path
// And it then processes Yaml configs discovered to apply the settinges defined in them
// For the agent itself as well as for the driver.
func LoadConfig(service_name string) {
	configDir := ""
	Conf = Config{}
	Settings["SVC_REGISTRY"] = fmt.Sprintf(`System\CurrentControlSet\Services\%s`, service_name)
	Settings["SVC_REGISTRY_SETTINGS"] = fmt.Sprintf("%s\\Settings", Settings["SVC_REGISTRY"])

	_hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, Settings["SVC_REGISTRY"], wr.ALL_ACCESS)
	if err != nil {
		log.Printf("LoadConfig: Registry key:%v\nError:%v", Settings["SVC_REGISTRY"], err)
		log.Fatal(err)
	}
	defer _hkey.Close()
	configDir, keytype, err := _hkey.GetStringValue("Path") // The driver install dir
	if err != nil || keytype != wr.SZ {
		log.Fatalf("LoadConfig: Could not locate install path from service settings:%v\n", err)
	}
	hkey, err := wr.OpenKey(_hkey, "Settings", wr.ALL_ACCESS)
	if err != nil {
		log.Printf("LoadConfig: Registry key:%v\nError:%v", Settings["SVC_REGISTRY_SETTINGS"], err)
		log.Fatal(err)
	}
	defer hkey.Close()

	configDir += "\\Config" // Config files are always in the install dir\Config
	info, err := os.Stat(configDir)
	if os.IsNotExist(err) {
		log.Fatalf("LoadConfig: Path %v does not exist\n", configDir)
	}
	if !info.IsDir() {
		log.Fatalf("LoadConfig: Path %v is not a directory\n", configDir)
	}

	listing := os.DirFS(configDir)
	yaml_files, err := fs.Glob(listing, "*.yaml")
	if err != nil {
		log.Fatalf("LoadConfig: Error locating yaml config files under %v:%v", configDir, err)
	}
	for _, yaml_file := range yaml_files {
		content, err := os.ReadFile(configDir + "\\" + yaml_file)
		if err != nil {
			log.Fatalf("LoadConfig: Error reading file: %v", err)
		}
		log.Printf("Processing yaml file:%v\n", configDir+"\\"+yaml_file)
		if err := yaml.Unmarshal([]byte(content), &Conf); err != nil {
			log.Fatal(err)
		}
		processGlobalSettings(Conf)
		InitLogs()
		var ConfigLists = []ConfigListMap{
			{Conf.ProcessBlockList, PROCESS_BLOCK_LIST},
			{Conf.ProcessIncludeList, PROCESS_INCLUDE_LIST},
			{Conf.ProcessExcludeList, PROCESS_EXCLUDE_LIST},
			{Conf.ProcessTerminatedIncludeList, PROCESS_TERMINATED_INCLUDE_LIST},
			{Conf.ProcessTerminatedExcludeList, PROCESS_TERMINATED_EXCLUDE_LIST},
			{Conf.ThreadBlockList, THREAD_BLOCK_LIST},
			{Conf.ThreadIncludeList, THREAD_INCLUDE_LIST},
			{Conf.ThreadExcludeList, THREAD_EXCLUDE_LIST},
			{Conf.ModuleBlockList, MODULE_BLOCK_LIST},
			{Conf.ModuleIncludeList, MODULE_INCLUDE_LIST},
			{Conf.ModuleExcludeList, MODULE_EXCLUDE_LIST},
			{Conf.ObjectBlockList, OBJECT_BLOCK_LIST},
			{Conf.ObjectIncludeList, OBJECT_INCLUDE_LIST},
			{Conf.ObjectExcludeList, OBJECT_EXCLUDE_LIST},
			{Conf.RegistryBlockList, REGISTRY_BLOCK_LIST},
			{Conf.RegostryIncludeList, REGISTRY_INCLUDE_LIST},
			{Conf.RegistryExcludeList, REGISTRY_EXCLUDE_LIST},
			{Conf.FileCreateBlockList, FILE_CREATE_BLOCK_LIST},
			{Conf.FileCreateIncludeList, FILE_CREATE_INCLUDE_LIST},
			{Conf.FileCreateExcludeList, FILE_CREATE_EXCLUDE_LIST},
			{Conf.FileSetInfoBlockList, FILE_SET_INFO_BLOCK_LIST},
			{Conf.FileSetInfoIncludeList, FILE_SET_INFO_INCLUDE_LIST},
			{Conf.FileSetInfoExcludeList, FILE_SET_INFO_EXCLUDE_LIST},
			{Conf.WfpBlockList, WFP_BLOCK_LIST},
			{Conf.WfpIncludeList, WFP_INCLUDE_LIST},
			{Conf.WfpExcludeList, WFP_EXCLUDE_LIST},
		}
		if !strings.EqualFold(Settings["SVC_NAME"], service_name) {
			log.Printf("LoadConfig: Service name in config does not match current service name:%v != %v\n", Settings["SVC_NAME"], service_name)
		}
		for _, confmap := range ConfigLists {
			processList(confmap.list, confmap.id)
		}
	}

}

// TODO: Uninstall protection

// Uninstalls the Luna agent as well as the
// MoonMon driver. It requires the driver's serivce name
// as a parameter
func UninstallLuna(service_name string) {
	Settings = make(map[string]string, 256)
	LoadConfig(service_name)
	log.Printf("Attempting to uninstall")
	ServiceRemoveLuna()
	ServiceRemoveMoonMon()
	log.Printf("Done uninstalling")

	var reboot string
	fmt.Print("Reboot? [Y/N]:")
	fmt.Scan(&reboot)
	if strings.EqualFold(reboot, "y") || strings.EqualFold(reboot, "yes") {
		exec.Command("shutdown.exe", "/r", "/t", "0").Run()
	}
}

// The config parameter should be a local path to the installation package
// or an http(s) URL from where a zip formatted installation package can be downloaded.
// After validating the installation source directory, it copies over content from it to
// the install target directory in the install_source\Config\luna.yaml configuration file.
// It then configures services for the driver as well as the user-space agent.
func InstallLuna(config string) {
	var installConf Config
	srcInstallDir := ""
	configFile := config
	if !strings.HasPrefix(strings.ToLower(config), "http") && !strings.HasSuffix(config, ".yaml") {
		log.Fatalf("Specified config file does not end in .yaml\n")
	}
	err := os.Chdir(`c:\Windows\Temp`)
	if err != nil {
		log.Printf("Warning, couldn't Chdir to C:\\Windows\\Temp\n")
	}
	if strings.HasPrefix(strings.ToLower(config), "http") {

		err := os.MkdirAll("mm_temp", 0700)
		if err != nil && os.IsExist(err) {
			log.Printf("Warning, install temp directory exists already\n")

		} else if err != nil {
			log.Fatalf("Unable to create temporary path for installation\n")
		}
		if !DownloadFile(config, "install.zip") {
			log.Fatalf("Failed to download install package from the url:%v\n", config)
		}
		err = Unzip("install.zip", "mm_temp")
		if err != nil {
			log.Fatalf("Failed to extract install.zip downloaded from from url:%v, error:%v\n", config, err)
		}
		err = os.Chdir(`c:\Windows\Temp\mm_temp`)
		if err != nil {
			log.Printf("Warning, couldn't Chdir to C:\\Windows\\Temp\n")
		}
		luna_yaml := `c:\windows\temp\mm_temp\Config\luna.yaml`
		_, err = os.Stat(luna_yaml)
		if os.IsExist(err) || err == nil {
			configFile = luna_yaml
		} else {
			log.Fatalf("Error, could not find luna.yaml under extracted path %v\n", luna_yaml)
		}
		srcInstallDir = `c:\Windows\Temp\mm_temp`
	}
	content, err := os.ReadFile(configFile)
	if err != nil {
		log.Fatalf("Error reading file: %v", err)
	}

	if err := yaml.Unmarshal([]byte(content), &installConf); err != nil {
		log.Fatal(err)
	}
	processGlobalSettings(installConf)
	if srcInstallDir == "" {
		srcInstallDir = installConf.GlobalSettings.Install_source_path
	}
	log.Printf("Installing from source directory %v\n", srcInstallDir)

	Settings["SVC_REGISTRY"] = fmt.Sprintf(`System\CurrentControlSet\Services\%s`, Settings["SVC_NAME"])

	var srcLuna = srcInstallDir + "\\*.exe"
	var srcMoonMon = srcInstallDir + "\\MoonMon.sys"
	var srcConfigDir = srcInstallDir + "\\Config"

	if len(srcInstallDir) > 0 {
		info, err := os.Stat(srcInstallDir)
		if os.IsNotExist(err) {
			log.Fatalf("Install source Path %v does not exist\n", srcInstallDir)

		}
		if info != nil && !info.IsDir() {
			log.Fatalf("Source install path %v is not a directory\n", srcInstallDir)
		}

	} else {
		log.Fatal("No value specified for srcInstallDir")
	}
	info, err := os.Stat(srcMoonMon)
	if os.IsNotExist(err) {
		log.Fatal("MoonMon.sys not found in the install source directory.")
	}
	if info.IsDir() {
		log.Fatal("MoonMon.sys found but is a directory!")
	}
	info, err = os.Stat(srcConfigDir)
	if os.IsNotExist(err) {
		log.Fatalf("%v not found in the install source directory.\n", srcConfigDir)
	}
	if !info.IsDir() {
		log.Fatalf("%v found but is not a directory!\n", srcConfigDir)
	}
	listing := os.DirFS(srcConfigDir)
	yaml_files, err := fs.Glob(listing, "*.yaml")
	if err != nil || len(yaml_files) < 1 {
		log.Fatalf("Error locating yaml config files under %v:%v", srcConfigDir, err)
	}

	var installDir = installConf.GlobalSettings.Install_path
	var installConfDir = installDir + "\\Config"

	if len(installDir) > 0 {
		_, err := os.Stat(installDir)
		if os.IsNotExist(err) {
			log.Printf("Path %v does not exist\n", installDir)
			err = os.MkdirAll(installDir, 0700)
			if err != nil {
				log.Fatalf("Path %v does not exist, and could not create it:%v\n", installDir, err)
			}
		}
		info, _ = os.Stat(installDir)
		if !info.IsDir() {
			log.Fatalf("Path %v is not a directory\n", installDir)
		}

	} else {
		log.Fatal("No value specified for installDir")
	}

	_, err = os.Stat(installConfDir)
	if os.IsNotExist(err) {
		log.Printf("Path %v does not exist\n", installConfDir)
		err = os.MkdirAll(installConfDir, 0700)
		if err != nil {
			log.Fatalf("Path %v does not exist, and could not create it:%v\n", installConfDir, err)
		}
	}
	info, _ = os.Stat(installConfDir)
	if !info.IsDir() {
		log.Fatalf("Path %v is not a directory\n", installConfDir)
	}

	cmd := exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", srcLuna, installDir)
	out, err := cmd.Output()
	if err != nil {
		log.Printf("Error reported when copying agent:%v\n", err)
	}
	log.Println(string(out))
	cmd = exec.Command("cmd.exe", "/c", "copy", "/B", "/Y", "/V", srcMoonMon, installDir)
	out, err = cmd.Output()
	if err != nil {
		log.Printf("Error reported when copying driver:%v\n", err)
	}
	log.Println(string(out))

	cmd = exec.Command("cmd.exe", "/c", "xcopy", "/E", "/Y", srcConfigDir, installConfDir)
	out, err = cmd.Output()
	if err != nil {
		log.Printf("Error reported when copying config files:%v\n", err)
	}
	log.Println(string(out))
	ServiceSetupMoonMon(false)
	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, Settings["SVC_REGISTRY"], wr.ALL_ACCESS)
	if err != nil {
		log.Printf("InstallLuna: Registry key:%v\nError:%v\n", Settings["SVC_REGISTRY"], err)
		log.Fatal(err)
	}
	defer hkey.Close()
	err = hkey.SetStringValue("Path", installDir)
	if err != nil {
		log.Printf("Error setting install path dir:%v\n", err)
	}
	ServiceSetupLuna()
}
