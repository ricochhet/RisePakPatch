New-Item -Path "./release/" -ItemType Directory
Copy-Item -Path "./scripts/CreatePak.bat" -Destination "./release/"
Copy-Item -Path "./scripts/ExtractPak.bat" -Destination "./release/"
Copy-Item -Path "./build/Release/RisePakPatch.exe" -Destination "./release/"