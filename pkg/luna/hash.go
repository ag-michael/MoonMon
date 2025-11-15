package MoonMon

import (
	"crypto/sha256"
	"encoding/hex"
	"io"
	"log"
	"os"
)

func SHA256(filePath string) string {

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
	return hashString
}
