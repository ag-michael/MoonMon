package MoonMon

import (
	"encoding/binary"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"
	"unsafe"

	w "golang.org/x/sys/windows"
)

var ntpathlock sync.RWMutex

// Verifies that the ipv4 cidr string is valid
func VerifyIPv4(cidr string) bool {
	v4 := parseIpv4(cidr)
	if v4 > 0 {
		return true
	}
	return false
}

// Verifies that the ipv6 address string with a prefix is valid
func VerifyIPv6(cidr string) bool {
	v6 := parseIpv6(cidr)
	if v6 != nil {
		return true
	}
	return false
}

// Takes a CIDR formatted IPv4 address and returns
// a 64bit value with the first half being the address and
// the second half being the mask
func parseIpv4(cidr string) uint64 {
	mask := uint32(0xffffffff)
	ipsubnet := uint64(0x0)
	cidr = strings.Trim(cidr, " ")
	if !strings.Contains(cidr, "/") {
		log.Printf("Bad IPv4 CIDR subnet value:%v\n", cidr)
		return 0
	}
	suffix, err := strconv.Atoi(strings.Split(cidr, "/")[1])
	if err != nil {
		log.Printf("Bad IPv4 CIDR subnet value:%v\n", cidr)
	}
	mask >>= (32 - suffix)
	ip := strings.Split(cidr, "/")[0]
	v4 := net.ParseIP(ip)
	if v4 == nil {
		fmt.Printf("Failed to parse IPv4 string value:%v\n", ip)
		return 0
	}

	ipv4 := v4.To4()
	if ipv4 == nil {
		fmt.Printf("Failed to parse/convert IPv4 value:%v\n", ip)
		return 0
	}
	ip32 := binary.LittleEndian.Uint32(ipv4)

	ipsubnet |= uint64(ip32 & mask)
	ipsubnet <<= 32
	ipsubnet |= uint64(mask)
	log.Printf("CIDR Subnet: %s, IP+Mask:%#x, IP: %#x, Mask: %#x\n", cidr, ipsubnet, ip32, mask)

	return ipsubnet
}

// Takes an IPv6 subnet address string and returns a byte slice
// The first 16 bytes contain the IPv6 address, the last byte is
// the ipv6 address prefix length in bits.
func parseIpv6(subnet string) []byte {
	subnet = strings.Trim(subnet, " ")
	if !strings.Contains(subnet, "/") {
		log.Printf("Bad IPV6 subnet value:%v\n", subnet)
		return nil
	}
	_prefix, err := strconv.Atoi(strings.Split(subnet, "/")[1])

	if err != nil {
		log.Printf("Bad IPV6 subnet value:%v\n", subnet)
	}
	prefix := uint8(_prefix)
	ip := strings.Split(subnet, "/")[0]

	v6 := net.ParseIP(ip)
	if v6 == nil {
		fmt.Printf("Failed to parse IPv6 value:%v\n", ip)
		return nil
	}

	ipv6 := v6.To16()
	if ipv6 == nil {
		fmt.Printf("Failed to parse IPv6 value:%v\n", ip)
		return nil
	}
	v6subnet := []byte(ipv6)
	v6subnet = append(v6subnet, byte(prefix))
	log.Printf("V6 Subnet:%v\n", v6subnet)
	return v6subnet
}

// Takes a byte slice containing an IPv4 address
// and returns a properly formatted IPv4 string
func formatIpv4(data []byte) string {
	if len(data) < 4 {
		return "bad ip"
	}

	ipAddr := net.IP(data[:4])
	return ipAddr.String()
}

// Takes a byte slice containing an ipv6 address
// and returns a properly formatted ipv6 string
func formatIpv6(data []byte) string {
	ipv6Addr := net.IP(data)
	return ipv6Addr.String()
}

// Takes a byte slice containing SID data and
// returns a properly formatted SID string
func convertSidToString(data []byte) string {
	s_sid := "SID Conversion failure"
	if len(data) < 9 {
		return s_sid
	}
	revision := data[0]
	subAuthorityCount := data[1]
	//fmt.Printf("revision:%v,subauthcount:%v\n", revision, subAuthorityCount)
	var idauth = uint64(0)
	for _, b := range data[2:8] {
		idauth <<= 8
		idauth |= uint64(b)
	}
	s_sid = fmt.Sprintf("S-%d-%d", revision, idauth)

	//subauth := make([]uint32,subAuthorityCount)
	subauth := unsafe.Slice((*uint32)(unsafe.Pointer(&data[8])), len(data)-8)
	for i := 0; i < int(subAuthorityCount); i++ {
		s_sid = fmt.Sprintf("%s-%d", s_sid, subauth[i])
	}
	//fmt.Println(s_sid)
	return s_sid
}

// Takes a properly formatted SID string and attempts to
// lookup the associated username and domain values if available.
func LookupSidAccount(sidString string) (string, string) {
	username := ""
	domain := ""
	//TODO: Look up a cache instead of looking this up every time!
	sid, err := syscall.StringToSid(sidString)
	if err != nil {
		log.Printf("lookupSidAccount:Error converting SID string to SID object: %v", err)
	}

	username, domain, _, err = sid.LookupAccount("") // The empty string indicates the local system
	if err != nil {
		log.Printf("lookupSidAccount: Error looking up account for SID: %v", err)
	}
	return domain, username
}

// Continually searches all DOS volume letters to
// see if a drive/volume is associated and
// caches the result under DosDeviceMap for translation
func LookupDoSDevicePaths() {
	log.Println("LookupDoSDevicePaths")
	buffer := make([]uint16, 256)
	for {
		time.Sleep(1 * time.Second)
		ntpathlock.Lock()
		for _, letter := range DosDeviceLetters {
			letter16, _ := syscall.UTF16PtrFromString(letter)
			n, err := w.QueryDosDevice(
				letter16,
				&buffer[0],
				256,
			)
			if err != nil {
				//	log.Printf("LookupDoSDevicePaths:Error calling QueryDosDevice: %v\n", err)
				continue
			}
			ntpath := syscall.UTF16ToString(buffer[:n])
			//	log.Printf("%s -> %s\n", letter, ntpath)
			DosDeviceMap[letter] = ntpath
		}
		ntpathlock.Unlock()
	}
}

// Converts an NTPath string to a DOS volume name
// E.g.: \Device\HarddiskVolume2\ -> C:\
func NtpathToDospath(path string) string {
	ntpathlock.RLock()
	defer ntpathlock.RUnlock()
	pathlen := len(path)
	for letter, ntpath := range DosDeviceMap {
		if pathlen < len(ntpath) || !strings.HasPrefix(strings.ToUpper(path), strings.ToUpper(ntpath)) {
			continue
		}
		path = strings.Replace(strings.ToUpper(path), strings.ToUpper(ntpath), letter, 1)

	}
	return path
}

// Takes a DOS volume path and converts it to an NTPath string
// E.g.: C:\ -> \Device\HarddiskVolume2\
func DospathToNtpath(path string) string {
	//log.Printf("DospathToNtpath:%s\n", path)
	ntpathlock.RLock()
	defer ntpathlock.RUnlock()
	pathlen := len(path)
	if len(DosDeviceMap) < 1 {
		log.Printf("Warning: DosDeviceMap is empty!\n")
	}
	for letter, ntpath := range DosDeviceMap {

		if pathlen < len(ntpath) || !strings.HasPrefix(strings.ToUpper(path), strings.ToUpper(letter)) {
			//log.Printf("Skipping:%s -> %s\n", letter, ntpath)
			continue
		}
		//log.Printf("Replacing:%s -> %s", letter, ntpath)
		path = strings.Replace(strings.ToUpper(path), strings.ToUpper(letter), ntpath, 1)

	}
	return path
}

// Converts a DOS path to a global root path
// E.g.: C:\ -> \??\C:\
func DospathToGlobalroot(path string) string {

	for _, letter := range DosDeviceLetters {
		if strings.HasPrefix(strings.ToUpper(path), letter) {
			return `\??\` + path
		}
	}
	return path
}

/*
Parses human-friendly strings from config settings
to a binary format (byte slice) the driver can use to make decisions.
*/
func StringToMessage(field int, data string) []byte {
	datalen := len(data)
	if datalen < 1 {
		return nil
	}
	defer func() {
		if err := recover(); err != nil {
			fmt.Printf("Exception: %v\n", err)
			fmt.Println(data)

		}
	}()
	var message []byte

	switch field {
	// string values, converted from utf8 to utf16

	case COMMAND_LINE:
		fallthrough

	case PARENT_COMMAND_LINE:
		fallthrough

	case TARGET_COMMAND_LINE:
		fallthrough
	case TARGET_PARENT_COMMAND_LINE:
		fallthrough
	case TARGET_PWINDOW_TITLE:
		fallthrough

	case PWINDOW_TITLE:
		fallthrough
	case LOADED_MODULE:
		fallthrough
	case REG_VALUE_NAME:
		fallthrough
	case REG_KEY:
		fallthrough
	case REG_KEY_NEW:
		fallthrough

	case MM_FILE_SHARE:
		fallthrough
	case MM_FILE_EXTENSION:
		fallthrough
	case MM_FILE_STREAM:
		fallthrough
	case MM_FILE_FINAL_COMPONENT:
		fallthrough

	case ACCOUNT_NAME:
		fallthrough
	case ACCOUNT_DOMAIN:
		fallthrough
	case REG_DATA:
		message = StringToUTF16Bytes(strings.ToLower(data))

	case IMAGE_FILE_NAME:
		fallthrough
	case PARENT_IMAGE_FILE_NAME:
		fallthrough
	case TARGET_IMAGE_FILE_NAME:
		fallthrough
	case TARGET_PARENT_IMAGE_FILE_NAME:
		message = StringToUTF16Bytes(strings.ToLower(DospathToGlobalroot(data)))
	case TARGET_PCWD:
		fallthrough
	case PCWD:
		message = StringToUTF16Bytes(strings.ToLower(data))
	case REG_OLD_FILE_NAME:
		fallthrough
	case REG_NEW_FILE_NAME:
		fallthrough
	case MM_FILE_NAME:
		fallthrough
	case MM_FILE_VOLUME:
		fallthrough
	case MM_FILE_PARENT_DIR:
		/*if !strings.HasPrefix(data, `\`) {
				data = `\` + data
		}
		if !strings.HasSuffix(data, `\`) {
				data = data + `\`
		}*/
		log.Printf("Dospath:%s\n", data)
		nt_path := strings.ToLower(DospathToNtpath(data))
		message = StringToUTF16Bytes(nt_path)
		log.Printf("NTPath:%s\n", nt_path)
	case MM_FILE_NEW_NAME:
		message = StringToUTF16Bytes(strings.ToLower(DospathToGlobalroot(data)))

	case MM_NET_DIRECTION:
		fallthrough
	case MM_NET_IP_PROTOCOL:
		fallthrough
	case MM_NET_ADDRESS_TYPE:
		fallthrough
	case MM_NET_PROMISCUOUS:
		if datalen >= 1 {
			intval, err := strconv.Atoi(data)
			if err != nil {
				message = nil
				fmt.Printf("Warning: byte field parsing error for '%v', field %v\n", data, Fields[field])
			} else {
				message = make([]byte, 2)
				message[0] = byte(intval)
			}
		}
		// 16 bit fields, decimal string
	case REG_DATA_TYPE:
		fallthrough
	case MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR:
		fallthrough
	case MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR:
		fallthrough

	case MM_NET_LOCAL_PORT:
		fallthrough
	case MM_NET_REMOTE_PORT:
		if datalen >= 1 {
			intval, err := strconv.ParseUint(data, 10, 16)
			if err != nil {
				fmt.Printf("Warning: uint16 field parsing error for '%v', field %v\n", data, Fields[field])
				message = nil
			} else {
				message = make([]byte, 2)
				binary.LittleEndian.PutUint16(message, uint16(intval))
			}
		}
		// 32 bit int fields, hex string values

	case PFLAGS:
		fallthrough
	case PHFLAGS:
		fallthrough
	case PHOPERATION:
		fallthrough
	case PHDESIRED_ACCESS:
		fallthrough
	case PHORIGINAL_DESIRED_ACCESS:
		fallthrough
	case CREATION_STATUS:
		fallthrough
	case MM_NAMED_PIPE_TYPE:
		fallthrough
	case MM_NAMED_PIPE_CONFIG:
		fallthrough
	case MM_NAMED_PIPE_STATE:
		fallthrough
	case MM_NAMED_PIPE_END:
		fallthrough
	case MM_FILE_FLAGS:
		fallthrough
	case MM_FILE_ACCESS_FLAGS:
		fallthrough

	case MM_NET_INTERFACE_TYPE:
		fallthrough
	case MM_FILE_NETWORK_PROTOCOL:
		if datalen >= 1 {
			intval, err := strconv.ParseUint(strings.TrimLeft(data, "0x"), 16, 32)
			if err != nil {
				fmt.Printf("Warning: uint32 field parsing error for '%v', field %v\n", data, Fields[field])
				message = nil
			} else {
				message = make([]byte, 4)
				binary.LittleEndian.PutUint32(message, uint32(intval))
			}
		}
		// IP fields

	case MM_NET_LOCAL_IPV4_ADDR:
		fallthrough
	case MM_NET_REMOTE_IPV4_ADDR:
		if datalen >= 4 {
			message = make([]byte, 8)
			binary.LittleEndian.PutUint64(message, parseIpv4(data))
			log.Printf("V4 MSG for %v:%v\n", data, message)
		}
	case MM_NET_LOCAL_IPV6_ADDR:
		fallthrough
	case MM_NET_REMOTE_IPV6_ADDR:
		if datalen >= 4 {
			message = parseIpv6(data)
			if message == nil {
				fmt.Printf("Warning, failed to parse ipv6 value:%v\n", data)
			}
		}
	case MM_FILE_OPERATION:
		if datalen >= 4 {
			fileop := StringValueToKey(FILEOPS, data)

			if fileop == 0 {
				fmt.Printf("Warning: BAD_FILE_OPERATION:%v\n", data)
				message = nil
			} else {
				message = make([]byte, 4)
				binary.LittleEndian.PutUint32(message, uint32(fileop))
			}
		}
		// special translated fields such as enums
	case REG_OP:
		if datalen >= 4 {
			regop := StringValueToKey(REGOPS, data)

			if regop == 0 {
				fmt.Printf("Warning: BAD_REGISTRY_OPERATION:%v\n", data)
				message = nil
			} else {
				message = make([]byte, 2)
				binary.LittleEndian.PutUint16(message, uint16(regop))
			}
		}
	case MM_WFP_LAYER:
		if datalen >= 4 {
			layer := StringValueToKey(WFPLAYERS, data)

			if layer == 0 {
				fmt.Printf("Warning: BAD_WFP_LAYER:%v\n", data)
				message = nil
			} else {
				message = make([]byte, 2)
				binary.LittleEndian.PutUint16(message, uint16(layer))
			}
		}

		// Unsupported fields
	case EVENT_TYPE:
		fallthrough
	case MM_FILE_CREATION_TIME:
		fallthrough

	case MM_NET_INTERFACE:
		fallthrough
	case MM_FILE_CHANGE_TIME:
		fallthrough
	case PID:
		fallthrough
	case PARENT_PID:
		fallthrough
	case TARGET_PID:
		fallthrough
	case CREATING_THREAD_ID:
		fallthrough
	case THREAD_ID:
		fallthrough

	case MATCH_ID:
		fallthrough
	case SID_DATA:
		fallthrough
	case MM_FILE_DELETION:
		fallthrough
	case MODULE_PROPERTIES:
		fallthrough
	default:
		fmt.Printf("Warning, unsupported field:%v\n", Fields[field])
		message = nil
	}

	return message
}

/*
Parses binary message data from the driver and formats it
to a string that is fit for logging.
*/
func MessageToString(field int, data []byte) string {

	message := "<data parsing error>"
	datalen := len(data)
	if datalen < 1 {
		return message
	}
	defer func() {
		if err := recover(); err != nil {
			fmt.Printf("Exception: %v\n", err)
			fmt.Println(hex.EncodeToString(data))

		}
	}()

	switch field {

	case COMMAND_LINE:
		fallthrough

	case PARENT_COMMAND_LINE:
		fallthrough

	case TARGET_COMMAND_LINE:
		fallthrough
	case TARGET_PARENT_COMMAND_LINE:
		fallthrough
	case TARGET_PWINDOW_TITLE:
		fallthrough
	case PWINDOW_TITLE:
		fallthrough
	case LOADED_MODULE:
		fallthrough
	case REG_VALUE_NAME:
		fallthrough
	case REG_KEY:
		fallthrough
	case REG_KEY_NEW:
		fallthrough

	case MM_FILE_SHARE:
		fallthrough
	case MM_FILE_EXTENSION:
		fallthrough
	case MM_FILE_STREAM:
		fallthrough
	case MM_FILE_FINAL_COMPONENT:
		fallthrough
	case ACCOUNT_NAME:
		fallthrough
	case ACCOUNT_DOMAIN:
		fallthrough
	case REG_DATA:
		message = ToUTF8(data)

	case MM_FILE_PARENT_DIR:
		fallthrough
	case PCWD:
		fallthrough
	case MM_FILE_NAME:
		fallthrough

	case PARENT_IMAGE_FILE_NAME:
		fallthrough
	case TARGET_IMAGE_FILE_NAME:
		fallthrough
	case TARGET_PARENT_IMAGE_FILE_NAME:
		fallthrough
	case TARGET_PCWD:
		fallthrough
	case REG_OLD_FILE_NAME:
		fallthrough
	case REG_NEW_FILE_NAME:
		fallthrough
	case MM_FILE_VOLUME:
		message = NtpathToDospath(ToUTF8(data))
	case MM_FILE_NEW_NAME:
		fallthrough
	case IMAGE_FILE_NAME:
		message = strings.TrimLeft(NtpathToDospath(ToUTF8(data)), `\??\`)
	case MATCH_ID:
		if datalen >= 4 {
			match_id := binary.LittleEndian.Uint32(data)
			message = fmt.Sprintf("[%d] %v", match_id, ResolveRuleIdToName(match_id))
		}
	case SID_DATA:

		message = convertSidToString(data)

	case PFLAGS:
		fallthrough
	case PHFLAGS:
		fallthrough
	case PHOPERATION:
		fallthrough
	case PHDESIRED_ACCESS:
		fallthrough
	case PHORIGINAL_DESIRED_ACCESS:
		fallthrough
	case CREATING_THREAD_ID:
		fallthrough
	case THREAD_ID:
		fallthrough
	case CREATION_STATUS:
		fallthrough
	case MM_NAMED_PIPE_TYPE:
		fallthrough
	case MM_NAMED_PIPE_CONFIG:
		fallthrough
	case MM_NAMED_PIPE_STATE:
		fallthrough
	case MM_NAMED_PIPE_END:
		fallthrough
	case MM_FILE_NETWORK_PROTOCOL:
		if datalen >= 4 {

			message = fmt.Sprintf("%#x", binary.LittleEndian.Uint32(data))
		}
	case PID:
		fallthrough
	case PARENT_PID:
		fallthrough
	case TARGET_PID:
		if datalen >= 8 {
			message = fmt.Sprintf("%d", binary.LittleEndian.Uint64(data))
		}
	case MM_FILE_CREATION_TIME:
		fallthrough

	case MM_FILE_CHANGE_TIME:
		if datalen >= 8 {
			ts := ToTime(binary.LittleEndian.Uint64(data))
			message = ts.UTC().Format(time.RFC3339Nano)
		}
	case MM_NET_INTERFACE:
		if datalen >= 8 {
			message = fmt.Sprintf("%#x", binary.LittleEndian.Uint64(data))
		}
	case MM_FILE_FLAGS:
		fallthrough
	case MM_FILE_ACCESS_FLAGS:
		fallthrough

	case MM_NET_INTERFACE_TYPE:
		if datalen >= 4 {
			message = fmt.Sprintf("%#x", binary.LittleEndian.Uint32(data))
		}
	case REG_DATA_TYPE:
		fallthrough
	case MM_FILE_NETWORK_PROTOCOL_VERSION_MAJOR:
		fallthrough
	case MM_FILE_NETWORK_PROTOCOL_VERSION_MINOR:
		fallthrough

	case MM_NET_LOCAL_PORT:
		fallthrough
	case MM_NET_REMOTE_PORT:
		if datalen >= 2 {
			message = fmt.Sprintf("%d", binary.LittleEndian.Uint16(data))
		}
	case MM_NET_DIRECTION:
		fallthrough
	case MM_NET_IP_PROTOCOL:
		fallthrough
	case MM_NET_ADDRESS_TYPE:
		fallthrough
	case MM_NET_PROMISCUOUS:
		if datalen >= 1 {
			message = fmt.Sprintf("%#x", data[:1])
		}
	case MM_NET_LOCAL_IPV4_ADDR:
		fallthrough
	case MM_NET_REMOTE_IPV4_ADDR:
		if datalen >= 4 {
			message = formatIpv4(data)
		}
	case MM_NET_LOCAL_IPV6_ADDR:
		fallthrough
	case MM_NET_REMOTE_IPV6_ADDR:
		if datalen >= 16 {
			message = formatIpv6(data)
		}
	case MM_FILE_OPERATION:
		if datalen >= 4 {
			fileop := int(binary.LittleEndian.Uint32(data))
			fileop_s, ok := FILEOPS[fileop]
			if !ok {
				message = "BAD_FILEOP_ID:" + strconv.Itoa(int(fileop))
			} else {
				message = fileop_s
			}
		} else {
			message = "UNKNOWN FILE OPERATION"
		}
	case REG_OP:
		if datalen >= 2 {
			regop := int(binary.LittleEndian.Uint16(data))
			regop_s, ok := REGOPS[regop]
			if !ok {
				message = "BAD_REGISTRYOP_ID:" + strconv.Itoa(int(regop))
			} else {
				message = regop_s
			}
		}
	case MM_WFP_LAYER:
		if datalen >= 2 {
			layer := int(binary.LittleEndian.Uint16(data))
			layer_s, ok := WFPLAYERS[layer]
			if !ok {
				message = "BAD_WFP_LAYER:" + strconv.Itoa(int(layer))
			} else {
				message = layer_s
			}
		}

	case MODULE_PROPERTIES:
		if datalen >= 4 {
			var properties = binary.LittleEndian.Uint32(data)
			var ImageInfo = make(map[string]uint32)
			ImageInfo["ImageAddressingMode"] = (properties >> 24)
			ImageInfo["SystemModeImage"] = ((properties << 8) >> 31)
			ImageInfo["ImageMappedToAllPids"] = ((properties << 9) >> 31)
			ImageInfo["ExtendedInfoPresent"] = ((properties << 10) >> 31)
			ImageInfo["MachineTypeMismatch"] = ((properties << 11) >> 31)
			ImageInfo["ImageSignatureLevel"] = ((properties << 12) >> 28)
			ImageInfo["ImageSignatureType"] = ((properties << 16) >> 29)
			ImageInfo["ImagePartialMap"] = ((properties << 19) >> 31)

			jsonData, err := json.Marshal(ImageInfo)
			if err != nil {
				fmt.Println("MODULE_PROPERTIES: Error marshaling JSON:", err)
				break
			}
			message = string(jsonData)
		}
	case MM_FILE_DELETION:
		message = "DELETED"
	case EVENT_TYPE:
		if datalen >= 2 {
			event_type := int(binary.LittleEndian.Uint16(data))
			event_s, ok := Events[event_type]
			if !ok {
				message = "BAD_EVENT_TYPE_ID:" + strconv.Itoa(int(event_type))
			} else {
				message = event_s
			}
		}

	default:
		message = hex.EncodeToString(data)
	}
	return message
}
