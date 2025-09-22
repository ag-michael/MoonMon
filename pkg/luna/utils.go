// This module contains utility and helper functions
package MoonMon

import (
	"archive/zip"
	"io"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"
	"unsafe"

	w "golang.org/x/sys/windows"
	"golang.org/x/text/encoding/unicode"
	"golang.org/x/text/transform"
)

// converts a windows utf16 string to a golang utf8 string
func ToUTF8(indata []byte) string {
	transformed := []byte("<utf8 decoding failure>")
	decoder := unicode.UTF16(unicode.LittleEndian, unicode.UseBOM).NewDecoder()
	transformed, _, err := transform.Bytes(decoder, indata)
	if err != nil {
		log.Fatal(err)
	}
	return string(transformed) // by default, go strings are utf8
}

// Looks up a string value in a map[int]string and returns the associated map key
func StringValueToKey(intStrMap map[int]string, value string) int {
	for k, v := range intStrMap {
		if strings.EqualFold(v, value) || strings.EqualFold(v, "mm_"+value) {
			return k
		}
	}

	return 0
}

// Takes a golang (utf8) string and returns a byte slice of
// utf16 bytes compatible with UNICODE_STRING
func StringToUTF16Bytes(str string) []byte {
	data, err := w.UTF16PtrFromString(str)
	if err != nil {
		log.Fatalf("Error converting data to UTF16: %v", err)
	}
	var ustr w.NTUnicodeString
	w.RtlInitUnicodeString(&ustr, data)

	data_bytes := unsafe.Slice((*byte)(unsafe.Pointer(ustr.Buffer)), uint32(ustr.Length))
	return data_bytes
}

// Takes a 64bit windows timestamp and returns a Unix timestamp Time struct
func ToTime(t uint64) time.Time {

	nanoseconds := (int64(t) - 116444736000000000) * 100

	return time.Unix(0, nanoseconds)
}

// Download a file from url and save it at path
// return value indicates success or failure of the download attempt
func DownloadFile(url string, path string) bool {
	fsave, err := os.Create(path)
	if err != nil {
		log.Printf("DownloadFile: Failed to create download path:%v\n", path)
		return false
	}
	defer fsave.Close()
	resp, err := http.Get(url)
	if err != nil {
		log.Printf("DownloadFile: Failed to download content from url %v , error:%v\n", err)
		return false
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		log.Printf("DownloadFile: Failed to download content from url %v , HTTP status cocde:%v\n", url)
		return false
	}
	_, err = io.Copy(fsave, resp.Body)
	if err != nil {
		log.Printf("DownloadFile: Failed to save downloaded content from %v to %v,error:%v\n", url, path, err)
	} else {
		return true
	}
	return false
}

// Unzips the src source zip file under the dest destinatin directory
func Unzip(src string, dest string) error {
	r, err := zip.OpenReader(src)
	if err != nil {
		return err
	}
	defer r.Close()

	for _, f := range r.File {
		fpath := filepath.Join(dest, f.Name)

		if f.FileInfo().IsDir() {
			os.MkdirAll(fpath, os.ModePerm)
			continue
		}

		if err = os.MkdirAll(filepath.Dir(fpath), os.ModePerm); err != nil {
			return err
		}

		outFile, err := os.OpenFile(fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
		if err != nil {
			return err
		}
		defer outFile.Close()

		rc, err := f.Open()
		if err != nil {
			return err
		}
		defer rc.Close()

		_, err = io.Copy(outFile, rc)
		if err != nil {
			return err
		}
	}
	return nil
}

// Takes the FieldType and associated ValidRanges
// And returns a string slice of values
// which describe to users the valid ranges of values to fields
// This is mainly to be used as part of the builtin field documentation features
func ParseRanges(FieldType string, ValidRanges []string) []string {
	var strRange []string
	for _, valid := range ValidRanges {
		if strings.EqualFold(valid, "uint32") {
			if strings.EqualFold(FieldType, "hex-uint32") {
				strRange = append(strRange, "0x00000000 - 0xffffffff")
			} else if strings.Contains(valid, "-") {
				strRange = append(strRange, strings.Replace(valid, "-", "...", -1))
			} else {
				strRange = append(strRange, valid)
			}
		} else if strings.EqualFold(valid, "uint16") {
			strRange = append(strRange, "0...65535")
		} else if strings.EqualFold(valid, "byte") {
			strRange = append(strRange, "0...255")
		} else if strings.EqualFold(valid, "unsupported") {
			strRange = append(strRange, "UNSUPPORTED FIELD")
		} else if strings.EqualFold(valid, "ipv4_string") {
			strRange = append(strRange, "A valid IPV4 string")
		} else if strings.EqualFold(valid, "ipv6_string") {
			strRange = append(strRange, "A valid IPV6 string")
		} else if strings.EqualFold(valid, "unicode_file_path_unc") {
			strRange = append(strRange, valid)
		} else if strings.EqualFold(valid, "unicode_registry_key") {
			strRange = append(strRange, "A fully-qualfied registry object path that starts with \\Registry\\. Length between 1 and 32767 characters")
		} else if strings.EqualFold(valid, "unicode_string") {
			strRange = append(strRange, "String. Length between 1 and 32767 characters")
		} else if strings.EqualFold(valid, "unicode_file_path") {
			strRange = append(strRange, "A valid windows file path. Length between 1 and 32767 characters")
		} else if strings.EqualFold(valid, "regops") {
			for _, regop := range REGOPS_SUPPORTED {
				strRange = append(strRange, regop)
			}
		} else if strings.EqualFold(valid, "wfplayers") {
			for _, layer := range WFPLAYERS_SUPPORTED {
				strRange = append(strRange, layer)
			}
		} else if strings.EqualFold(valid, "fileops") {
			for _, fileop := range FILEOPS {
				strRange = append(strRange, fileop)
			}
		} else {
			strRange = append(strRange, valid)
		}
	}
	//	strconv.ParseUint(data, 10, 16)
	return strRange
}

// Takes the  field parameter (field name) and the value parameter which
// contains the value associated with that field in the configuration and
// returns a bool indicating if the value is valid for the associated field.
func ValidateFieldValue(field string, value string) bool {
	var fi FieldInfo
	found := false
	for _, f := range FieldInformation {
		if strings.EqualFold(f.Name, field) {
			fi = f
			found = true
			break
		}
	}
	if !found {
		log.Println("Field not found")
		return false
	}
	ValidRanges := fi.ValidRanges
	FieldType := fi.FieldType
	for _, valid := range ValidRanges {
		if strings.EqualFold(valid, "uint32") || strings.EqualFold(FieldType, "hex-uint32") {
			if strings.EqualFold(FieldType, "hex-uint32") {
				val, err := strconv.ParseUint(strings.TrimLeft(value, "0x"), 16, 32)
				if err != nil {
					log.Printf("Error: hex-uint32 field parsing error for '%v', field %v\n", value, field)
					return false
				}
				if val >= 0x00000000 && val <= 0xffffffff {
					return true
				}
			} else if strings.Contains(valid, "-") {
				vals := strings.Split(valid, "-")
				lval := vals[0]
				rval := vals[1]
				val, err := strconv.ParseUint(value, 16, 32)
				if err != nil {
					log.Printf("Error: uint32 field parsing error for '%v', field %v\n", value, field)
					return false
				}
				ilval, err := strconv.ParseUint(lval, 16, 32)
				if err != nil {
					log.Printf("Error:uint32 field parsing error for '%v', field %v\n", lval, field)
					return false
				}
				irval, err := strconv.ParseUint(rval, 16, 32)
				if err != nil {
					log.Printf("Error: uint32 field parsing error for '%v', field %v\n", rval, field)
					return false
				}
				if val >= ilval && val <= irval {
					return true
				}
			} else {
				val, err := strconv.ParseUint(value, 16, 32)
				if err != nil {
					log.Printf("Error: uint32 field parsing error for '%v', field %v\n", value, field)
					return false
				}
				if strings.EqualFold(valid, "uint32") && val >= 0x00000000 && val <= 0xffffffff {
					return true
				} else {
					_valid, err := strconv.ParseUint(value, 16, 32)
					if err != nil {
						log.Printf("Error: uint32 field parsing error for '%v', field %v\n", valid, field)
						return false
					}
					if val == _valid {
						return true
					}
				}
			}
		} else if strings.EqualFold(valid, "uint16") || strings.EqualFold(FieldType, "uint16") {

			if strings.Contains(valid, "-") {
				vals := strings.Split(valid, "-")
				lval := vals[0]
				rval := vals[1]
				val, err := strconv.ParseUint(value, 10, 16)
				if err != nil {
					log.Printf("Error: uint32 field parsing error for '%v', field %v\n", value, field)
					return false
				}
				ilval, err := strconv.ParseUint(lval, 10, 16)
				if err != nil {
					log.Printf("Error:uint32 field parsing error for '%v', field %v\n", lval, field)
					return false
				}
				irval, err := strconv.ParseUint(rval, 10, 16)
				if err != nil {
					log.Printf("Error: uint32 field parsing error for '%v', field %v\n", rval, field)
					return false
				}
				if val >= ilval && val <= irval {
					return true
				}
			} else {
				val, err := strconv.ParseUint(value, 10, 16)
				if err != nil {
					log.Printf("Error: uint16 field parsing error for '%v', field %v\n", value, field)
					return false
				}

				if strings.EqualFold(valid, "uint16") && val >= 0 && val <= 65535 {
					return true
				} else {
					_valid, err := strconv.ParseUint(valid, 10, 16)
					if err != nil {
						log.Printf("Error: uint16 field parsing error for '%v', field %v\n", valid, field)
						return false
					}
					if val == _valid {
						return true
					}
				}
			}
		} else if strings.EqualFold(valid, "byte") || strings.EqualFold(FieldType, "decimal-byte") {
			intval, err := strconv.Atoi(value)
			if err != nil {
				log.Printf("Error: byte field parsing error for '%v', field %v\n", value, field)
				return false
			}
			if strings.EqualFold(valid, "byte") && intval >= 0 && intval <= 255 {
				return true
			} else {
				if strings.Contains(valid, "-") {
					vals := strings.Split(valid, "-")
					lval := vals[0]
					rval := vals[1]
					val, err := strconv.Atoi(value)
					if err != nil {
						log.Printf("Error: byte field parsing error for '%v', field %v\n", value, field)
						return false
					}
					ilval, err := strconv.Atoi(lval)
					if err != nil {
						log.Printf("Error:byte field parsing error for '%v', field %v\n", lval, field)
						return false
					}
					irval, err := strconv.Atoi(rval)
					if err != nil {
						log.Printf("Error: byte field parsing error for '%v', field %v\n", rval, field)
						return false
					}
					if val >= ilval && val <= irval {
						return true
					}
				} else {
					val, err := strconv.Atoi(value)
					if err != nil {
						log.Printf("Error: byte field parsing error for '%v', field %v\n", value, field)
						return false
					}
					_valid, err := strconv.Atoi(valid)
					if err != nil {
						log.Printf("Error: byte field parsing error for '%v', field %v\n", valid, field)
						return false
					}
					if val == _valid {
						return true
					}
				}
			}
		} else if strings.EqualFold(valid, "unsupported") {
			log.Printf("Unsupported field %v\n", field)
			return true
		} else if strings.EqualFold(valid, "ipv4_string") {
			if !VerifyIPv4(value) {
				log.Printf("Failed to validate IPv4 value:%v\n", value)
				return false
			} else {
				return true
			}
		} else if strings.EqualFold(valid, "ipv6_string") {
			if !VerifyIPv6(value) {
				log.Printf("Failed to validate IPv6 value:%v\n", value)
				return false
			} else {
				return true
			}
		} else if strings.EqualFold(valid, "unicode_file_path_unc") {
			if len(value) < 32767 {
				return true
			}
		} else if strings.EqualFold(valid, "unicode_registry_key") {
			if len(value) < 32767 {
				return true
			}
		} else if strings.EqualFold(valid, "unicode_string") {
			if len(value) < 32767 {
				return true
			}
		} else if strings.EqualFold(valid, "unicode_file_path") {
			if len(value) < 32767 {
				return true
			}
		} else if strings.EqualFold(valid, "regops") {
			for _, regop := range REGOPS {
				if strings.EqualFold(regop, value) {
					return true
				}
			}
		} else if strings.EqualFold(valid, "wfplayers") {
			for _, layer := range WFPLAYERS {
				if strings.EqualFold(layer, value) {
					return true
				}
			}
		} else if strings.EqualFold(valid, "fileops") {
			for _, fileop := range FILEOPS {
				if strings.EqualFold(fileop, value) {
					return true
				}
			}
		}
	}
	return false
}
