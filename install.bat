@echo off

set "sourceFile=C:\my.exe"
set "destinationFolder=C:\Windows\System32"
set "destinationFile=%destinationFolder%\my.exe"

echo Copying "%sourceFile%" to "%destinationFile%"...
copy /Y "%sourceFile%" "%destinationFolder%" >nul

if exist "%destinationFile%" (
    echo File copied successfully.
    echo Calling "%destinationFile%"...
) else (
    echo Failed to copy the file.
    echo The source file may not exist or the destination folder may not be accessible.
)

echo Task completed. Deleting files...

rem Delete my.exe from the current directory
del my.exe

rem Delete the batch file itself
del "%~f0"

