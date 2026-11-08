param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$CacheRoot='D:/CheweenieValidation/UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts/import_crew_art.py').Replace('\','/')
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -Multiprocess -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved/ImportCrewArt.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 8){throw 'Crew illustration import timed out.'}}}
finally{if(!$process.HasExited){$process.Kill()}}
$report=Join-Path $PSScriptRoot 'Saved/crew-art-import.json'
if($process.ExitCode -or !(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc-lt $started){throw 'Crew illustration import failed; inspect Saved/ImportCrewArt.log.'}
Get-Content -LiteralPath $report
