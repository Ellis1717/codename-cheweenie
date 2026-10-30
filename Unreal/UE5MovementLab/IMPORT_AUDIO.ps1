param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts\import_adventure_audio.py').Replace('\','/')
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\ImportAdventureAudio.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$startedAt=[DateTime]::UtcNow
$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)) {
    if(([DateTime]::UtcNow-$startedAt).TotalMinutes -gt 10){$process.Kill();throw 'Audio import timed out.'}
}
if($process.ExitCode -ne 0){throw ('Audio import failed: '+$process.ExitCode)}
$report=Join-Path $PSScriptRoot 'Saved\adventure-audio.json'
if(-not(Test-Path -LiteralPath $report)){throw 'Audio report missing.'}
if((Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $startedAt){throw 'Audio report stale. Inspect Saved/ImportAdventureAudio.log.'}
Get-Content -LiteralPath $report
