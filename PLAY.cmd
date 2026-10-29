@echo off
setlocal
set "CHEWEENIE_GAME=%~dp0builds\Current"
if not exist "%CHEWEENIE_GAME%\Cheweenie.exe" set "CHEWEENIE_GAME=%~dp0builds\Windows"
if not exist "%CHEWEENIE_GAME%\Cheweenie.exe" goto missing
if not exist "%CHEWEENIE_GAME%\UnityPlayer.dll" goto incomplete
if not exist "%CHEWEENIE_GAME%\Cheweenie_Data\Managed\CodenameCheweenie.dll" goto incomplete
start "" /D "%CHEWEENIE_GAME%" "%CHEWEENIE_GAME%\Cheweenie.exe"
exit /b 0
:missing
echo No packaged game was found. Open this project in Unity and use Cheweenie ^> Build Windows game.
echo The ready-to-play local release normally lives in builds\Current.
pause
exit /b 1
:incomplete
echo Game files are missing. Extract the entire portable ZIP, keeping its Data folder and DLLs together.
pause
exit /b 1
