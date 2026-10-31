@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Unreal\UE5MovementLab\PLAY_COOP.ps1" -Mode Host
if errorlevel 1 pause
