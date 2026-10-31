param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$script=(Join-Path $PSScriptRoot 'Scripts\build_halloween_materials.py').Replace('\','/')
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -ExecutePythonScript="'+$script+'" -unattended -nop4 -nosplash -nullrhi -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\ImportHalloween.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$startedAt=[DateTime]::UtcNow
$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)) {
    if(([DateTime]::UtcNow-$startedAt).TotalMinutes -gt 10){$process.Kill();throw 'Halloween import timed out.'}
}
if($process.ExitCode -ne 0){throw ('Halloween import failed: '+$process.ExitCode)}
if(!(Select-String -LiteralPath (Join-Path $PSScriptRoot 'Saved\ImportHalloween.log') -Pattern 'HALLOWEEN_MATERIALS_READY' -Quiet)){throw 'Halloween material generation did not complete.'}
Write-Output 'Eight Halloween materials generated.'
