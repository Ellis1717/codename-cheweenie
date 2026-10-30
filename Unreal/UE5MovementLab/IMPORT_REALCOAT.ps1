param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts\import_marty_realcoat.py').Replace('\','/')
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+$script+'" -ini:Engine:[DevOptions.Shaders]:NumUnusedShaderCompilingThreads=15 -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\ImportMartyRealCoat.log')+'"'
$info.Arguments+=' -ExecCmds="Interchange.FeatureFlags.Import.FBX 0"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Marty import timed out'}}
if($process.ExitCode){throw ('Marty import failed: '+$process.ExitCode)}
$report=Join-Path $PSScriptRoot 'Saved\marty-realcoat-import.json'
if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Marty import report missing/stale; inspect Saved/ImportMartyRealCoat.log'}
Get-Content $report
