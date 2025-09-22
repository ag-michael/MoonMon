
param (
       [Parameter(Mandatory=$true)] [string]$Dist,
       [Parameter(Mandatory=$true)] [string]$Config,
       [Parameter(Mandatory=$true)] [string]$MoonMonBuildPath,
       [Parameter(Mandatory=$true)] [string]$ZipPath
    )

$GO = "go.exe" 
$NASM = "C:\nasm\nasm.exe"
$GOLINK = "C:\nasm\GoLink.exe"

if(-not ((test-path (gcm $GO).Source) -and (test-path (gcm $GO).Source) -and (test-path (gcm $GO).Source))){
     Write-Error "$GO , $NASM and $GOLINK need to be valid paths."
}

if((Test-Path $Dist) -and (Test-Path $Config)){
     Write-Host "Clearing distribution path $Dist"
     rm -force -recurse "$Dist\*" -Verbose 

     if(Test-Path $ZipPath){
         Write-Host "Removing existing zip package at $ZipPath"
         rm -force $ZipPath
     }
     Write-Host "Building Golang binaries.."
     &$Go build -v -o "$Dist\luna.exe" .\cmd\luna
     &$Go build -v -o "$Dist\tests.exe" .\cmd\tests
     &$Go build -v -o "$Dist\create_remote_thread.exe" .\cmd\tests\create_remote_thread
     
     Write-Host "Building smol & tiny helpers :)"
     &$NASM -f win64 -o tiny.obj tests\tiny.asm
     &$NASM -f win64 -o smol.obj tests\smol.asm
     &$GOLINK  /fo "$Dist\tiny.exe" /entry _mainCRTStartup  tiny.obj
     &$GOLINK  /fo "$Dist\smol.exe" /entry _mainCRTStartup  smol.obj
     rm tiny.obj
     rm smol.obj 
     
     Write-Host "Copying config files"
     copy -Recurse -Destination "$Dist\Config\"  -Path $Config -Verbose
     
     Write-Host "Copying MoonMon driver from it's build path $MooMonBuildPath"
     copy -Destination "$Dist\" -Path "$MoonMonBuildPath\*" -Verbose 
     
     Compress-Archive -Path "$Dist\*" -DestinationPath $ZipPath
     Write-Host "Done"
}else{
    Write-Error "Both $Dist and $Config need to be existing paths"
}