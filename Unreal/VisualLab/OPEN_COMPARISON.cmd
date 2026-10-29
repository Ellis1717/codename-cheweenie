@echo off
setlocal
if not defined CHEWEENIE_UNREAL_ENGINE set "CHEWEENIE_UNREAL_ENGINE=C:\Program Files\Epic Games\UE_4.27"
if not exist "%CHEWEENIE_UNREAL_ENGINE%\Engine\Binaries\Win64\UE4Editor.exe" goto missing
start "" "%CHEWEENIE_UNREAL_ENGINE%\Engine\Binaries\Win64\UE4Editor.exe" "%~dp0CheweenieVisualLab.uproject" /Game/VisualLab/Comparison -game -d3d11 -windowed -ResX=1280 -ResY=720
exit /b 0
:missing
echo Unreal 4.27 is required for this static comparison. Set CHEWEENIE_UNREAL_ENGINE to its folder.
pause
