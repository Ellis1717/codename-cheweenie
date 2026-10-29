param([string]$EngineRoot='C:\Program Files\Epic Games\UE_4.27')
$ErrorActionPreference='Stop'
$editor=Join-Path $EngineRoot 'Engine\Binaries\Win64\UE4Editor-Cmd.exe'
if(-not(Test-Path -LiteralPath $editor)){throw 'Unreal 4.27 editor not found; supply -EngineRoot.'}
$project=Join-Path $PSScriptRoot 'CheweenieVisualLab.uproject'
$script=Join-Path $PSScriptRoot 'Scripts\build_comparison.py'
# The 4.27 Python commandlet misparses script filenames containing spaces.
# Run a local no-space helper; runpy preserves the real script's __file__.
$helperRoot=Join-Path $env:LOCALAPPDATA 'CheweenieVisualLab'
New-Item -ItemType Directory -Path $helperRoot -Force|Out-Null
$helper=Join-Path $helperRoot 'run_comparison.py'
if($helper.Contains(' ')){throw 'The 4.27 commandlet needs a no-space helper path.'}
$pythonPath=ConvertTo-Json -InputObject $script -Compress
[IO.File]::WriteAllText($helper,"import runpy`nrunpy.run_path("+$pythonPath+", run_name='__main__')`n")
$process=Start-Process -FilePath $editor -ArgumentList @(('"'+$project+'"'),('-ExecutePythonScript='+$helper.Replace('\','/')),'-unattended','-nop4','-SCCProvider=None','-nosplash','-nullrhi') -WindowStyle Hidden -PassThru
$process.WaitForExit()
if($process.ExitCode -ne 0){throw ('Unreal comparison failed: '+$process.ExitCode)}
