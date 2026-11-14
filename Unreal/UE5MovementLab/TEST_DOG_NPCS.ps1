param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/DogNPCs'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
$rhi=if($Render){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '+$rhi+' -CheweenieOutsideSmoke -CheweenieDogSmoke -CheweenieTestOutput="'+$OutputRoot+'" -UserDir="'+$OutputRoot+'/User" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/DogNPCs.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Dog NPC acceptance timed out'}}
if($process.ExitCode){throw ('Dog NPC acceptance failed: '+$process.ExitCode)}
$report=Join-Path $OutputRoot 'dog-npcs.json'
if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $started){throw 'Dog NPC report missing or stale'}
$result=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
if($result.failed.Count -or $result.passed.Count -lt 32){throw 'Dog NPC assertions failed'}
if($Render){foreach($name in @('NeighbourDogs.png','NeighbourHank.png','NeighbourPotcake.png')){$capture=Join-Path $OutputRoot $name;if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $started){throw 'Dog NPC capture missing or stale'}}}
'Dog NPCs: '+$result.passed.Count+' passes'
