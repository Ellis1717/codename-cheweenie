@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Unreal\UE5MovementLab\PLAY_COOP.ps1" -Mode Join
if errorlevel 1 pause
