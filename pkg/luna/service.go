// This module contains service installation/uninstallation functions
package MoonMon

import (
	"fmt"
	"log"
	"os/exec"
	"time"

	"golang.org/x/sys/windows"
	wr "golang.org/x/sys/windows/registry"
	"golang.org/x/sys/windows/svc"
	"golang.org/x/sys/windows/svc/mgr"
)

// Uninstall the driver
func ServiceRemoveMoonMon() {
	m, err := mgr.Connect()
	if err != nil {
		log.Fatal("Failed to open a handle to the service manager")
	}
	defer m.Disconnect()

	s, err := m.OpenService(Settings["SVC_NAME"])
	if err != nil {
		log.Printf("ServiceRemoveMoonMon:Warning, couldn't open the service:%v\n", err)
	} else {
		_, err := s.Control(svc.Stop)
		if err != nil {
			log.Printf("ServiceRemoveMoonMon: Warning, service stop message didn't succeed:%v\n", err)
		}
		err = s.Delete()
		if err != nil {
			log.Printf("ServiceRemoveMoonMon: Error, unable to delete the agent service!: %v\n", err)
		}
	}

}

// Install the MoonMon driver
func ServiceSetupMoonMon(start bool) {
	log.Printf("ServiceSetupMoonMon: Setting up service with name %s, binpath %s\n", Settings["SVC_NAME"], Settings["SVC_PATH"])
	var conf mgr.Config
	conf.ServiceType = windows.SERVICE_KERNEL_DRIVER
	conf.StartType = windows.SERVICE_BOOT_START
	conf.ErrorControl = windows.SERVICE_ERROR_NORMAL
	conf.DisplayName = Settings["SVC_DISPLAY_NAME"]

	m, err := mgr.Connect()
	if err != nil {
		log.Fatal("Failed to open a handle to the service manager")
	}
	defer m.Disconnect()
	s, err := m.OpenService(Settings["SVC_NAME"])

	if err != nil {
		s, err = m.CreateService(Settings["SVC_NAME"], Settings["SVC_PATH"], conf)
		if err != nil {
			log.Printf("err:%v,%v: %v\n", Settings["SVC_NAME"], Settings["AGENT_SVC_PATH"], err)
			return
		}
		// service created succesfully
		// Unfortunately, the svc library doesn't honor the SERVICE_BOOT_START starttype value
		// so, this is a temporary hack until the github issue is resolved
		// https://github.com/golang/go/issues/74768

		out, err := exec.Command("C:\\Windows\\System32\\sc.exe", "config", Settings["SVC_NAME"], "start=", "boot").Output()
		if err != nil {
			log.Printf("Error reported when running sc.exe to set start type:%v\n", err)
		} else {
			log.Println(string(out))
		}
	}
	defer s.Close()

	Settings["SVC_REGISTRY"] = fmt.Sprintf(`System\CurrentControlSet\Services\%s`, Settings["SVC_NAME"])

	hkey, err := wr.OpenKey(wr.LOCAL_MACHINE, Settings["SVC_REGISTRY"], wr.ALL_ACCESS)
	if err != nil {
		log.Fatalf("ServiceSetupMoonMon: Registry key:%v\nError:%v", Settings["SVC_REGISTRY"], err)
	}
	defer hkey.Close()

	_, _, err = wr.CreateKey(hkey, "Settings", wr.ALL_ACCESS)
	if err != nil {
		log.Printf("Error creating the settings sub-key:%v\n", err)
	}
	if start {
		log.Println("Starting service..")
		time.Sleep(1 * time.Second)
		err = s.Start()
		if err != nil {
			log.Printf("Warning: Failed to start service:%v\n%v", Settings["SVC_NAME"], err)
		}
	}
}

// Uninstalls the Luna user-space agent service
func ServiceRemoveLuna() {
	m, err := mgr.Connect()
	if err != nil {
		log.Fatal("ServiceRemoveLuna: Failed to open a handle to the service manager")
	}
	defer m.Disconnect()

	s, err := m.OpenService(Settings["AGENT_SVC_NAME"])
	if err != nil {
		log.Printf("ServiceRemoveLuna: Warning, couldn't open the service:%v\n", err)
	} else {
		_, err := s.Control(svc.Stop)
		if err != nil {
			log.Printf("ServiceRemoveLuna: Warning, service stop message didn't succeed:%v\n", err)
		}
		err = s.Delete()
		if err != nil {
			log.Printf("ServiceRemoveLuna: Error, unable to delete the agent service!: %v\n", err)
		}
	}

}

// Installs the Luna user-space agent service
func ServiceSetupLuna() {
	log.Printf("serviceSetupLuna: Setting up service with name %s, binpath %s\n", Settings["AGENT_SVC_NAME"], Settings["AGENT_SVC_PATH"])
	var conf mgr.Config
	conf.ServiceType = 0
	conf.StartType = 2
	conf.ErrorControl = 1
	conf.DisplayName = Settings["AGENT_SVC_DISPLAY_NAME"]

	m, err := mgr.Connect()
	if err != nil {
		log.Fatal("Failed to open a handle to the service manager")
	}
	defer m.Disconnect()
	s, err := m.OpenService(Settings["AGENT_SVC_NAME"])

	if err != nil {
		log.Printf("Warning:%v\n", err)
		s, err = m.CreateService(Settings["AGENT_SVC_NAME"],
			Settings["AGENT_SVC_PATH"], conf, Settings["SVC_NAME"])
		if err != nil {
			log.Printf("err:%v,%v: %v\n", Settings["AGENT_SVC_NAME"], Settings["AGENT_SVC_PATH"], err)
			return
		}
	}
	defer s.Close()
	log.Println("Starting service..")
	time.Sleep(1 * time.Second)
	err = s.Start()
	if err != nil {
		log.Printf("Warning: Failed to start service:%v\n%v", Settings["AGENT_SVC_NAME"], err)
	}
}

type SvcLuna struct{}

// This function is called by the service manager to mange state and get status information from it
// For Luna's purposes, it only needs to let the service manager know it is running.
// Luna supports install/uninstall actions but it does not support managing it using the service manager.
func (m *SvcLuna) Execute(args []string, r <-chan svc.ChangeRequest, status chan<- svc.Status) (bool, uint32) {

	//const cmdsAccepted = svc.AcceptStop | svc.AcceptShutdown | svc.AcceptPauseAndContinue
	const cmdsAccepted = 0 // For now, it won't accept any service control requests
	tick := time.Tick(5 * time.Second)

	status <- svc.Status{State: svc.StartPending}

	status <- svc.Status{State: svc.Running, Accepts: cmdsAccepted}

	for {
		select {
		case <-tick:
		case c := <-r:
			switch c.Cmd {
			case svc.Interrogate:
				status <- c.CurrentStatus
			case svc.Stop, svc.Shutdown:
				log.Printf("Shutting down Luna")
				//			break loop
			case svc.Pause:
				status <- svc.Status{State: svc.Paused, Accepts: cmdsAccepted}
			case svc.Continue:
				status <- svc.Status{State: svc.Running, Accepts: cmdsAccepted}
			default:
				log.Printf("Unexpected service control request #%d", c)
			}
		}
	}

	//	return false, 1
}
