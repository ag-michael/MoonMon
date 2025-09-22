// This module contains functions to setup and perform logging
package MoonMon

import (
	"bufio"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"sort"
	"time"

	"golang.org/x/sys/windows/svc/eventlog"
)

var El *eventlog.Log
var written_bytes uint
var eventlog_installed bool
var file_output_running bool
var Logs = make(chan string)

// Open the log file after attempting rotation per configured settings
func RotateOpen(path string, flag int, mode os.FileMode) (*os.File, error) {
	_, err := os.Stat(path)
	if err == nil && !os.IsNotExist(err) {

		newpath := fmt.Sprintf("%s_%d", path, time.Now().Unix())
		//log.Printf("<<< Renaming\n")
		err := os.Rename(path, newpath)
		if err != nil {
			EvlogErr(1, fmt.Sprintf("Error renaming log file:%v \n", err))
			fhandle, err := os.OpenFile(path, flag, mode)
			return fhandle, err
			// It's useful to return nil/err when debugging
			// but we don't want the agent to stop working if renaming fails
			// under normal conditions.
			//return nil, err
		}

		logPath := filepath.Dir(Settings["FILE_OUTPUT"])
		listing := os.DirFS(logPath)
		log_files, err := fs.Glob(listing, "*_*")

		if err != nil {
			log.Printf("RotateOpen: Error locating log files under %v:%v", logPath, err)
			return nil, err
		}
		sort.Strings(log_files)
		listing_count := len(log_files)
		if listing_count >= int(ROTATE_COUNT) {
			for _, log_file := range log_files[:listing_count-int(ROTATE_COUNT)] {
				EvlogInfo(1, "Removing log file:"+log_file)
				err = os.Remove(logPath + `\` + log_file)
				if err != nil {
					EvlogErr(1, fmt.Sprintf("Error removing log file %v:%v\n", log_file, err))
				}
			}
		}
	}
	fhandle, err := os.OpenFile(path, flag, mode)
	return fhandle, err
}

// Log informational events to the windows event log
func EvlogInfo(id int, message string) {
	log.Println(message)
	if El != nil {
		El.Info(uint32(id), message)
	}
}

// Log error events to the windows event log
func EvlogErr(id int, message string) {
	log.Println(message)
	if El != nil {
		El.Error(uint32(id), message)
	}
}

// If file output is configured, take log data in a channel and
// write it to a log file, triggering rotation when it reaches
// the pre-configured threshold.
func FileOutput() {
	file_output, exists := Settings["FILE_OUTPUT"]
	if exists {
		if file_output_running {
			return
		}
		EvlogInfo(1, "File output set to:"+file_output)
		for {
			fout, err := RotateOpen(file_output, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0600)

			if err != nil {
				EvlogErr(2, "Error opening log file:"+file_output)
				log.Fatalf("Unable to open log file %v:%v", file_output, err)

			}
			logWriter := bufio.NewWriter(fout)
			EvlogInfo(1, "Opened file output,processing log events..")
			file_output_running = true
			for {
				content := <-Logs
				//	log.Println(content)
				w_bytes, err := logWriter.WriteString(content + "\n")
				written_bytes += uint(w_bytes)
				if err != nil {
					log.Printf("Error '%v', failed to write log:\n%v\n\n", err, content)
				}
				logWriter.Flush()

				if written_bytes > ROTATE_SIZE {
					logWriter = nil
					EvlogInfo(1, fmt.Sprintf("Existing log loop, %v written bytes > %v\n", written_bytes, ROTATE_SIZE))
					written_bytes = 0
					break
				}
			}
			err = fout.Close()

			if err != nil {
				log.Printf("Error closing log file:%v", err)
			}
			fout = nil
			//	time.Sleep(180000 * time.Millisecond)
		}
	} else {
		EvlogInfo(1, "File output has not been configured.")
	}
}

// Initialize windows event logging and start the file output thread
func InitLogs() {
	svcname := Settings["AGENT_SVC_NAME"]
	written_bytes = 0

	if !eventlog_installed {
		const supports = eventlog.Error | eventlog.Warning | eventlog.Info
		eventlog.InstallAsEventCreate(svcname, supports)
		El, err := eventlog.Open(svcname)

		if err != nil {
			log.Fatalf("Failed to open event log: %v", err)
		}
		eventlog_installed = true
		El.Info(1, "Agent startup:"+svcname)
	}
	go FileOutput()
}
