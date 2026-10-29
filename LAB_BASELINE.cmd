@echo off
setlocal
set "LAB_GAME=%~dp0builds\VisualLab\CheweenieLab.exe"
if not exist "%LAB_GAME%" exit /b 1
start "" /D "%~dp0builds\VisualLab" "%LAB_GAME%" -force-d3d11
