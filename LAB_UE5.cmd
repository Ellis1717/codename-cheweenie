@echo off
powershell -NoProfile -File "%~dp0Unreal\UE5VisualLab\OPEN_PREVIEW.ps1" -Play
if errorlevel 1 pause
