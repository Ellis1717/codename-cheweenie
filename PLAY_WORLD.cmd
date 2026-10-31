@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Unreal\UE5MovementLab\PLAY_WORLD.ps1"
if errorlevel 1 pause
