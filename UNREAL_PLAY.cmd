@echo off
if exist "%~dp0builds\UnrealAdventure-0.4.0\Windows\CheweenieUE5Lab.exe" (
  start "" "%~dp0builds\UnrealAdventure-0.4.0\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1280 -ResY=720
) else (
  powershell -NoProfile -File "%~dp0Unreal\UE5MovementLab\OPEN.ps1"
  if errorlevel 1 pause
)
