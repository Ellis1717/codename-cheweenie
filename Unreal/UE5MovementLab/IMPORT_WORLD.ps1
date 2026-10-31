param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts/import_world_art.py').Replace('\','/')
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -ExecCmds="Interchange.FeatureFlags.Import.FBX 0" -AbsLog="'+(Join-Path $PSScriptRoot 'Saved/ImportWorld.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 15){$process.Kill();throw 'World art import timed out'}}
if($process.ExitCode){throw 'World art import failed; inspect Saved/ImportWorld.log'}
$report=Join-Path $PSScriptRoot 'Saved/world-art-import.json'
if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $started){throw 'World art report missing/stale'}
Get-Content -LiteralPath $report
