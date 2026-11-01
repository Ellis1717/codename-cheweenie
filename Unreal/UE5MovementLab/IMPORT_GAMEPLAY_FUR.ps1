param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts\import_marty_gameplay_fur.py').Replace('\','/')
$log=Join-Path $PSScriptRoot 'Saved\ImportMartyGameplayFur.log'
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+$log+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Gameplay fur import timed out'}}
if($process.ExitCode){throw ('Gameplay fur import failed: '+$process.ExitCode)}
if(@(Select-String -LiteralPath $log -Pattern 'Log\w+: (Error|Fatal):|Failed to compile Material|Default Material will be used').Count){throw 'Import/shader errors; inspect ImportMartyGameplayFur.log'}
$report=Join-Path $PSScriptRoot 'Saved\marty-gameplay-fur-import.json'
if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Gameplay fur report missing/stale'}
$result=Get-Content $report -Raw|ConvertFrom-Json
if(!$result.ready -or $result.materials.Count -ne 4){throw 'Gameplay fur import incomplete'}
$result
