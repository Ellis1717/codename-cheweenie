@echo off
setlocal
set "LAB_GAME=%~dp0builds\VisualLab\CheweenieLab.exe"
if not exist "%LAB_GAME%" goto missing
start "" /D "%~dp0builds\VisualLab" "%LAB_GAME%" -force-d3d11 -cheweenie-visual-lab
exit /b 0
:missing
echo Build this separate experiment in Unity: Cheweenie ^> Visual Lab ^> Build separate Windows experiment.
pause
exit /b 1
