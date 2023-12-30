package main

import (
	"crypto/md5"
	"encoding/hex"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

func calculateChecksum(filePath string) (string, error) {
	file, err := os.Open(filePath)
	if err != nil {
		return "", err
	}
	defer file.Close()

	hash := md5.New()
	_, err = io.Copy(hash, file)
	if err != nil {
		return "", err
	}

	return hex.EncodeToString(hash.Sum(nil)), nil
}

func compareFolders(folder1, folder2 string) error {
	fileChecksums1 := make(map[string]string)
	fileChecksums2 := make(map[string]string)

	err := filepath.Walk(folder1, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !info.IsDir() {
			relativePath, _ := filepath.Rel(folder1, path)
			checksum, err := calculateChecksum(path)
			if err != nil {
				return err
			}
			fileChecksums1[relativePath] = checksum
		}
		return nil
	})
	if err != nil {
		return err
	}

	err = filepath.Walk(folder2, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !info.IsDir() {
			relativePath, _ := filepath.Rel(folder2, path)
			checksum, err := calculateChecksum(path)
			if err != nil {
				return err
			}
			fileChecksums2[relativePath] = checksum
		}
		return nil
	})
	if err != nil {
		return err
	}

	for path1, checksum1 := range fileChecksums1 {
		checksum2, exists := fileChecksums2[path1]
		if !exists {
			fmt.Printf("File %s exists in folder1 but not in folder2\n", path1)
			continue
		}

		if checksum1 != checksum2 {
			fmt.Printf("Checksums for file %s do not match:\n", path1)
			fmt.Printf("  Folder1: %s\n", checksum1)
			fmt.Printf("  Folder2: %s\n", checksum2)
		}
	}

	for path2 := range fileChecksums2 {
		_, exists := fileChecksums1[path2]
		if !exists {
			fmt.Printf("File %s exists in folder2 but not in folder1\n", path2)
		}
	}

	return nil
}

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Usage: diff <folder1> <folder2>")
		os.Exit(1)
	}

	folder1 := os.Args[1]
	folder2 := os.Args[2]

	err := compareFolders(folder1, folder2)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
	}
}
