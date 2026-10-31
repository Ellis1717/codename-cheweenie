@echo off
set "CHEWEENIE_LOCAL_ARGS=-CheweenieNeighbourhood="%~dp0LocalGameData\streets.json" -CheweenieDogs="%~dp0LocalGameData\NeighbourDogs.json""
if exist "%~dp0builds\UnrealAdventure-0.8.0\validated.json" (
  start "" "%~dp0builds\UnrealAdventure-0.8.0\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1920 -ResY=1080 %CHEWEENIE_LOCAL_ARGS%
) else if exist "%~dp0builds\UnrealAdventure-0.7.0\validated.json" (
  start "" "%~dp0builds\UnrealAdventure-0.7.0\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1920 -ResY=1080 %CHEWEENIE_LOCAL_ARGS%
) else if exist "%~dp0builds\UnrealAdventure-0.6.1\validated.json" (
  start "" "%~dp0builds\UnrealAdventure-0.6.1\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1280 -ResY=720 %CHEWEENIE_LOCAL_ARGS%
) else if exist "%~dp0builds\UnrealAdventure-0.6.0\validated.json" (
  start "" "%~dp0builds\UnrealAdventure-0.6.0\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1280 -ResY=720 %CHEWEENIE_LOCAL_ARGS%
) else if exist "%~dp0builds\UnrealAdventure-0.5.0\validated.json" (
  start "" "%~dp0builds\UnrealAdventure-0.5.0\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1280 -ResY=720 %CHEWEENIE_LOCAL_ARGS%
) else if exist "%~dp0builds\UnrealAdventure-0.4.0\Windows\CheweenieUE5Lab.exe" (
  start "" "%~dp0builds\UnrealAdventure-0.4.0\Windows\CheweenieUE5Lab.exe" -dx12 -windowed -ResX=1280 -ResY=720
) else (
  powershell -NoProfile -File "%~dp0Unreal\UE5MovementLab\OPEN.ps1"
  if errorlevel 1 pause
)
