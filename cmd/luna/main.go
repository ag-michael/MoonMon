// Luna is the user-space agent for MoonMon
// This package handles the installation, configuration,
// uninstallation and log processing for MoonMon.
package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"maps"
	"os"
	"runtime"
	"strconv"
	"strings"
	"time"
	"unsafe"

	luna "github.com/ag-michael/MoonMon/pkg/luna"
	"github.com/jedib0t/go-pretty/v6/table"
	w "golang.org/x/sys/windows"
	"golang.org/x/sys/windows/svc"
)

// this struct is used to track/collect
// log data from moonmon (driver)
// the id parameter of each log-entry is used as a key
// to track fields for events from the driver.
type kv struct {
	id    int
	value map[string]string
}

// Takes raw byte slice`data`  from the MoonMon driver and
// processes up to `byteSize` bytes of slice.
// If this function is being called it means some
// output of the data is expected.
// When monitoring isn't desired, this function shouldn't be called.
func processData(data []byte, bytesSize uint32) {

	bytesdone := uint32(0)

	var size uint32
	var dtype uint16
	var id uint64
	var logbuffer []kv

	// a log entry with the right headers is at least 8 bytes long
	if bytesSize < 8 {
		return
	}

	for {

		if (bytesSize - bytesdone) <= 8 {
			break
		}
		// Each log entry should begin with the MOOMON pre-amble
		// For whatever reason, if we don't get a response that starts with that,
		// scan the data until we find the pre-amble.
		// if we run out of data, stop processing.
		if !bytes.Equal((data[bytesdone:])[:7], []byte("MOONMON")) {
			bytesdone += 1
			if (bytesSize - bytesdone) <= 8 {
				return
			}
			continue
		}
		bytesdone += 7 // pre-amble size

		size = binary.LittleEndian.Uint32(data[bytesdone:])
		bytesdone += 4
		if size > bytesSize-bytesdone {
			log.Printf("invalid size!:%d\n", size)
			continue
		}

		dtype = binary.LittleEndian.Uint16(data[bytesdone:])
		bytesdone += 2

		id = binary.LittleEndian.Uint64(data[bytesdone:])
		bytesdone += 8

		ts := binary.LittleEndian.Uint64(data[bytesdone:])
		timestamp := luna.ToTime(ts)

		bytesdone += 8

		type_s, ok := luna.Fields[int(dtype)]
		if !ok {
			type_s = "BAD_FIELD_NAME:" + strconv.Itoa(int(dtype))
		}
		//log.Printf("Got %v byte packet, type %v, id: %v, ts: %v\n",size,type_s,id,timestamp)
		// translate the data into message[field_name] = parsed_string_value
		message_value := luna.MessageToString(int(dtype), data[bytesdone:(bytesdone+size)])
		message := map[string]string{type_s: message_value}

		// Attempt to translate SID values to user and domain names
		if dtype == uint16(luna.SID_DATA) && len(message_value) > 0 {
			domain, username := luna.LookupSidAccount(message_value)
			message["ACCOUNT_NAME"] = username
			message["ACCOUNT_DOMAIN"] = domain
		}

		if int(dtype) == luna.EVENT_END {
			// if this is a an event_end entry, then
			// find all events associated with the event id
			// add timestamp information and
			// convert it into json and write them out for logging
			m := make(map[string]string, luna.Fields_Max)

			m["ID"] = strconv.Itoa(int(id))
			m["TIMESTAMP"] = timestamp.UTC().Format(time.RFC3339Nano)
			//	m = append(m, ts)
			for _, v := range logbuffer {
				if v.id == int(id) {
					maps.Copy(m, v.value)
				}
			}

			jsonData, err := json.Marshal(m)
			if err != nil {
				log.Println("Error marshaling JSON:", err)
				return
			}

			luna.Logs <- string(jsonData)

			// supposedly, settings things to nil after using them
			// helps with GC
			jsonData = nil
			m = nil
		} else {
			// for non-event_end messages, create a kv object with the id as key
			// then append it to the logbuffer slice
			k := kv{id: int(id), value: message}
			logbuffer = append(logbuffer, k)
			if (bytesdone + size) > (bytesSize - 7) {
				break
			}
		}

		bytesdone += size

		message = nil
	}
	data = nil
}

// Connects to the driver and enters an infinite-loop
// that reads log data from the driver and calls processData() to process it.
func Luna() {
	// This is a hard-coded device name for now
	// TOOD: Dynamically discover the device name using the service name
	fileName := `\Device\MoonMon`

	var hDriver w.Handle
	var ioStatusBlock w.IO_STATUS_BLOCK
	var objectAttributes w.OBJECT_ATTRIBUTES
	var ufileName w.NTUnicodeString

	// We need to convert the device file name from
	// go's default utf8 to utf16 that's the windows default
	namePtr, err := w.UTF16PtrFromString(fileName)
	if err != nil {
		log.Fatalf("Error converting filename to UTF16: %v", err)
	}

	w.RtlInitUnicodeString(&ufileName, namePtr)

	// Initialize the objectattributes parameter with the unicode string
	// file name
	objectAttributes.Length = uint32(unsafe.Sizeof(objectAttributes))
	objectAttributes.RootDirectory = 0
	objectAttributes.ObjectName = &ufileName
	objectAttributes.Attributes = w.OBJ_CASE_INSENSITIVE

	allocationSize := int64(0)

	// Open the device file for reading
	ntStatus := w.NtCreateFile(
		&hDriver,
		uint32(w.GENERIC_READ|w.GENERIC_WRITE),
		&objectAttributes,
		&ioStatusBlock,
		&allocationSize,
		uint32(w.FILE_ATTRIBUTE_NORMAL),
		0,
		uint32(w.FILE_OPEN_IF),
		0,
		0,
		0,
	)

	if ntStatus != nil {
		log.Fatalf("Failed to open handle to MoonMon: %v", ntStatus)
	}

	defer w.CloseHandle(hDriver)
	// pre-allocate a data buffer for processing logs
	// this could easily be made dynamic (config-driven) if needed
	// But this static 100MB value seems to work, although it adds to
	// The memory footprint of the user-space agent.
	data := make([]byte, 100000000)
	var bytesRead uint32
	ol := new(w.Overlapped)
	for {
		// To avoid burning cpu-cycles, sleep for a second between iterations
		// So long as in that second, the buffer doesn't fill up, all is good.

		time.Sleep(2000 * time.Millisecond)
		err = w.ReadFile(w.Handle(hDriver), data, &bytesRead, ol)
		if err != nil {
			continue
		}

		if bytesRead > 0 {
			//   log.Println(string(data))
			//log.Printf("Got %d bytes",bytesRead)

			// This could be made into a go routine.
			// However, keeping it non-threaded seems to work great.
			// It reduces the code's complexity and it seems to keep-up with
			// intense workloads just fine. If it can't finish fast-enough
			// before the buffer fills up again, then it is better to consider
			// async processing after processData() atomically updates a worker queue.
			//
			// After some benchmarking, if needed, this approach should be revisited.
			//
			// This approach unfortunately means a corner-case where events of the same callback/id
			// might span multiple reads from the kernel. This means multiple log entries with the same id
			// are possible when the system is under intense stress and overloading buffers.
			// But there shouldn't be (TM) any data loss.
			processData(data, bytesRead)
		}
	}

}

// Loads the config, sets up the windows services, runs them
// and calls Luna() to start processing logs.
func runLuna(service_name string) {
	runtime.GOMAXPROCS(2) // capping it at 2 cpus max for now

	// Load the config, using the service name to
	// discover the config path among other things
	luna.LoadConfig(service_name)

	// attempt to start/setup the driver before running the agent service
	luna.ServiceSetupMoonMon(true)
	// start the service manager go routine
	go RunService(luna.Settings["SVC_NAME"])

	// Start the core agent routine
	Luna()

}

// This runs as  a go routine that handles the interaction
// with the service manager
func RunService(name string) {

	err := svc.Run(name, &luna.SvcLuna{})
	if err != nil {
		log.Println("RunService: Error running service in Service Control mode.")
	}

}

// Prints field documentation in an ascii-table format
func printFields() {
	tw := table.NewWriter()
	tw.SetOutputMirror(os.Stdout)
	tw.AppendHeader(table.Row{"Field Name", "Field Type", "Valid Value Ranges", "Default\\Example", "Description"})

	for _, field := range luna.FieldInformation {
		vranges := luna.ParseRanges(field.FieldType, field.ValidRanges)
		if len(vranges) == 1 {
			tw.AppendRows([]table.Row{[]interface{}{field.Name, field.FieldType, vranges[0], field.DefaultValue, field.Description}})
		} else {
			tw.AppendRows([]table.Row{[]interface{}{field.Name, field.FieldType, vranges[0], field.DefaultValue, field.Description}})
			for _, vr := range vranges[1:] {
				tw.AppendRows([]table.Row{[]interface{}{"", "", vr, "", ""}})
			}
		}
		vranges = nil
		tw.AppendSeparator()
	}
	tw.Render()
}

// Prints field documentation in a markdown table format
func printFieldsMD() {
	fmt.Println("| Field Name | Field Type | Default\\Example | Description | Valid Value Ranges |")
	fmt.Println("| ------------------------------ | ----------------- | -------------------- | ------------------------------------------------ | --------------------------------------------------------------- |")
	for _, field := range luna.FieldInformation {
		vranges := luna.ParseRanges(field.FieldType, field.ValidRanges)
		fmt.Printf("| %v | %v | %v | %v | %v |\n", field.Name, field.FieldType, field.DefaultValue, field.Description, strings.Join(vranges, ","))

		vranges = nil
	}
}

// Sets up the application's log output to be
// Both STDOUT and a hard-coded log file.
// When running as a service, errors and debug logs that happen
// prior to the configuration being loaded or better log destinations
// being setup need to be logged somewhere, a simple static log file is the
// best approach.
func setupOutput() {
	// This is the best way to debug windows services
	// a simple file log is easier to work with than windows event logging

	logfile, err := os.OpenFile("c:\\windows\\temp\\app.log", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0600)
	if err != nil {
		log.Println(err)
	}
	//defer logfile.Close()
	multiWriter := io.MultiWriter(os.Stdout, logfile)
	log.SetOutput(multiWriter)
}

// luna.exe's main function
// Evaluates command line arguments to the application and takes actions accordingly
func main() {
	exe := os.Args[0]
	if len(os.Args) < 2 {
		log.Fatalf(`
Usage:
  %v service_name
  %v uninstall service_name
  %v install c:\\some\\path\\config.yaml
  %v install https://example.com/package.zip
  %v fields
  %v fields-md
`, exe, exe, exe, exe, exe, exe)
	}
	// Initialize the global Settings map
	luna.Settings = make(map[string]string, 256)

	// Start a thread that syncs dos volume names to nt device paths
	go luna.LookupDoSDevicePaths()

	// Sleep a bit at startup so dos device volumes get a chance to be enumerated.
	time.Sleep(3 * time.Second)

	if strings.EqualFold(os.Args[1], "install") {
		if len(os.Args) < 3 {
			log.Fatalf("install command specified but no path to a config.yaml path or url was given.\n")
		}
		setupOutput()
		luna.InstallLuna(os.Args[2])
	} else if strings.EqualFold(os.Args[1], "uninstall") {
		if len(os.Args) < 3 {
			log.Fatalf("uninstall command specified but no service name was given.\n")
		}
		setupOutput()
		luna.UninstallLuna(os.Args[2])
	} else if strings.EqualFold(os.Args[1], "fields") {
		printFields()
	} else if strings.EqualFold(os.Args[1], "fields-md") {
		printFieldsMD()
	} else {
		setupOutput()
		runLuna(os.Args[1])
	}
}
