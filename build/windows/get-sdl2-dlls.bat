@echo off
setlocal enabledelayedexpansion

set EXTRACT_COMMAND=7z x -y
set SDL2_VERSION=2.26.4
set SDL2_IMAGE_VERSION=2.8.1
set SDL2_MIXER_VERSION=2.6.3


set DESTDIR=%1
set BITS=%2
set DOWNLOAD_COMMAND=%~3

rem PLEASE NO SPACES IN SDL2_* VARIABLES

set SDL2_ARCHIVE=SDL2-%SDL2_VERSION%-win32-x%BITS%.zip
set SDL2_URL=https://www.libsdl.org/release/%SDL2_ARCHIVE%

set SDL2_IMAGE_ARCHIVE=SDL2_image-%SDL2_IMAGE_VERSION%-win32-x%BITS%.zip
set SDL2_IMAGE_URL=https://www.libsdl.org/projects/SDL_image/release/%SDL2_IMAGE_ARCHIVE%

rem ========================================================

if "!DESTDIR!" == "" (
	echo "Usage %0 destination_dir (32|64) [download_command]"
	echo Assume you have 7z in your PATH
	exit /b 1
)

if not exist !DESTDIR!\* (
	echo Directory "!DESTDIR!" doesn't exist. Creating it...
	md "!DESTDIR!"
) else (
	echo Directory "!DESTDIR!" exists...
)

echo cd into "!DESTDIR!"
cd "!DESTDIR!"

call :downloadIfNeeded !SDL2_URL!
call :downloadIfNeeded !SDL2_IMAGE_URL!

%EXTRACT_COMMAND% !SDL2_ARCHIVE!
%EXTRACT_COMMAND% !SDL2_IMAGE_ARCHIVE!

rem Copy optional DLLs within the optional folders
copy .\optional\*.dll .

exit /b
rem ========================================================


rem --------------------------------------------------------
rem Downloads file and places it as destination file IN CURRENT DIR
rem	%1 -- URL
rem 	%2 -- destination file
:download
	if "!DOWNLOAD_COMMAND!" == "" (
		rem TODO: bitsadmin is deprecated, but it is the only method to download file from pure cmd. 
		rem TODO: It will have to be fixed some day

		for /F %%i in ("%1") do bitsadmin.exe /transfer "Download  %%~ni%%~xi" %1  "%cd%/%%~ni%%~xi"
	) else (
		!DOWNLOAD_COMMAND! %1
	)
exit /b
rem --------------------------------------------------------


rem --------------------------------------------------------
rem Downloads file IF FILE IS MISSING IN CURRENT DIR
rem	%1 -- URL
:downloadIfNeeded
	for /F %%i in ("%1") do if exist "%%~ni%%~xi" (
			echo "%%~ni%%~xi" already exists. Skipping download...
		) else (
			echo Downloading "%%~ni%%~xi" ...
			call :download %1 
		)
exit /b
rem --------------------------------------------------------