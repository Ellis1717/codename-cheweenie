param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/SpeciesGait'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess -nullrhi -CheweenieCompanionSmoke -CheweenieSpeciesGaitSmoke -UserDir="'+$OutputRoot+'/User" -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -AbsLog="'+$OutputRoot+'/Species.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache';$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Species gait test timed out'}}
$path=Join-Path $OutputRoot 'companions-smoke.json'
if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $started){throw 'Species report missing or stale'}
$r=Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
if($process.ExitCode -or $r.failed.Count -or $r.passed.Count -lt 14){throw 'Species gait assertions failed'}
'Species gait: '+$r.passed.Count+' passes'
