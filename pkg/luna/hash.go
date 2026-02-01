package MoonMon

import (
	"crypto/sha256"
	"encoding/hex"
	"io"
	"log"
	"os"
)

var HashCache [65535]string
var HashCacheLookup = make(map[string]int)
var writeIndex int

func SHA256(filePath string) string {
	if i, ok := HashCacheLookup[filePath]; ok {
		return HashCache[i]
	}
	file, err := os.Open(filePath)
	if err != nil {
		log.Printf("[SHA256] Error opening file: %v", err)
		return ""
	}
	defer file.Close()

	hasher := sha256.New()
	if _, err := io.Copy(hasher, file); err != nil {
		log.Printf("[SHA256] Error copying file to hasher: %v", err)
		return ""
	}

	hashInBytes := hasher.Sum(nil)
	hashString := hex.EncodeToString(hashInBytes)
	if writeIndex > 65535 {
		writeIndex = 0
	}
	HashCacheLookup[filePath] = writeIndex
	HashCache[writeIndex] = hashString
	writeIndex++
	return hashString
}
