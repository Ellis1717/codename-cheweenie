param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$editor=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
if(-not(Test-Path -LiteralPath $editor)){throw 'Unreal Engine 5 editor missing; supply -EngineRoot.'}
$project=Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject'
$script=Join-Path $PSScriptRoot 'Scripts\build_preview.py'
New-Item -ItemType Directory -Path (Join-Path $PSScriptRoot 'Saved') -Force | Out-Null
$info=[System.Diagnostics.ProcessStartInfo]::new()
$info.FileName=$editor
# The Python executor interprets backslash escapes inside its filename argument.
$info.Arguments='"'+$project+'" -ExecutePythonScript="'+$script.Replace('\','/')+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\BuildPreview.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$startedAt=[DateTime]::UtcNow
$process=[System.Diagnostics.Process]::Start($info)
$process.WaitForExit()
if($process.ExitCode -ne 0){throw ('UE5 preview generation failed: '+$process.ExitCode)}
$report=Join-Path $PSScriptRoot 'Saved\preview-report.json'
if(-not(Test-Path -LiteralPath $report)){throw 'UE5 did not produce a validation report. Inspect Saved/BuildPreview.log.'}
if((Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $startedAt){throw 'UE5 report is stale. Inspect Saved/BuildPreview.log.'}
Get-Content -LiteralPath $report
