param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$python=Join-Path $EngineRoot 'Engine/Binaries/ThirdParty/Python3/Win64/python.exe'
& $python (Join-Path $PSScriptRoot 'Scripts/build_readable_brick.py')
if($LASTEXITCODE){throw 'Brick map generation failed'}
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts/import_readable_brick.py').Replace('\','/')
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved/ImportReadableBrick.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 15){$process.Kill();throw 'Brick import timed out'}}
if($process.ExitCode){throw 'Brick import failed; inspect Saved/ImportReadableBrick.log'}
$report=Join-Path $PSScriptRoot 'Saved/readable-brick-import.json'
if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $started){throw 'Brick import report missing/stale'}
Get-Content -LiteralPath $report
