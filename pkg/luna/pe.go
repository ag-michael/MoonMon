package MoonMon

import (
	"log"

	"encoding/json"

	"github.com/bi-zone/go-fileversion"
	"github.com/saferwall/pe"
	peparser "github.com/saferwall/pe"
)

var PECache [65535]string
var PECacheLookup = make(map[string]int)
var PEwriteIndex int

type PEData struct {
	CompanyName      string           `json:"CompanyName,omitempty"`
	FileDescription  string           `json:"FileDescription,omitempty"`
	FileVersion      string           `json:"FileVersion,omitempty"`
	InternalName     string           `json:"InternalName,omitempty"`
	LegalCopyright   string           `json:"LegalCopyright,omitempty"`
	OriginalFilename string           `json:"OriginalFilename,omitempty"`
	ProductName      string           `json:"ProductName,omitempty"`
	ProductVersion   string           `json:"ProductVersion,omitempty"`
	Comments         string           `json:"Comments,omitempty"`
	LegalTrademarks  string           `json:"LegalTrademarks,omitempty"`
	PrivateBuild     string           `json:"PrivateBuild,omitempty"`
	SpecialBuild     string           `json:"SpecialBuild,omitempty"`
	Certificates     []pe.Certificate `json:"Certificates,omitempty"`
}

func PEEnrich(filePath string) string {
	if i, ok := PECacheLookup[filePath]; ok {
		return PECache[i]
	}
	f, err := fileversion.New(filePath)
	if err != nil {
		log.Printf("[PE] Error opening file for FileVersion enrichment:%v\n", err)
		return ""
	}
	certinfo := false
	pe, err := peparser.New(filePath, &peparser.Options{})
	if err != nil {
		log.Printf("[PE] Error while opening file: %s, reason: %v", filePath, err)

	} else {

		err = pe.Parse()
		if err != nil {
			log.Printf("[PE] Error while parsing file: %s, reason: %v", filePath, err)

		} else {
			certinfo = true
		}
	}
	var pedata PEData
	if certinfo {
		pedata = PEData{
			f.CompanyName(),
			f.FileDescription(),
			f.FileVersion(),
			f.InternalName(),
			f.LegalCopyright(),
			f.OriginalFilename(),
			f.ProductName(),
			f.ProductVersion(),
			f.Comments(),
			f.LegalTrademarks(),
			f.PrivateBuild(),
			f.SpecialBuild(),
			pe.Certificates.Certificates}
	} else {
		pedata = PEData{
			f.CompanyName(),
			f.FileDescription(),
			f.FileVersion(),
			f.InternalName(),
			f.LegalCopyright(),
			f.OriginalFilename(),
			f.ProductName(),
			f.ProductVersion(),
			f.Comments(),
			f.LegalTrademarks(),
			f.PrivateBuild(),
			f.SpecialBuild(),
			nil}
	}
	result, err := json.MarshalIndent(pedata, "", "  ")
	if err != nil {
		log.Printf("[PE] Error Marshaling PEData:%v", err)
		return ""
	}
	PEResult := string(result)
	PECacheLookup[filePath] = PEwriteIndex
	PECache[writeIndex] = PEResult
	PEwriteIndex++
	return PEResult
}
