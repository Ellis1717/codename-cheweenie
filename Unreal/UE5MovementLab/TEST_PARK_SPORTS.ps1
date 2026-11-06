param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$OutputRoot,[string]$GameExe,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\ParkSports'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
$rhi=if($Render){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '}
$info.Arguments=$project+$rhi+' -CheweenieParkSportsSmoke -CheweenieTestOutput="'+$OutputRoot+'" -UserDir="'+$OutputRoot+'/User" -unattended -nosound -nosplash -NoVSync -ExecCmds="t.MaxFPS 60" -AbsLog="'+$OutputRoot+'/ParkSports.log"'
if($MapFile){if($Render){throw 'Private sports audit is headless; private-layout captures are prohibited'};$info.Arguments+=' -CheweenieParkPrivateAudit -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'}
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
# Editor startup can be slow on a cold asset cache; standalone acceptance retains its 10-minute cap.
$timeout=if($GameExe){10}else{15}
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt $timeout){$process.Kill();throw 'Park sports test timed out'}}
if($process.ExitCode){throw 'Park sports test failed; inspect ParkSports.log'}
$path=Join-Path $OutputRoot 'park-sports-smoke.json'
if(!(Test-Path $path) -or (Get-Item $path).LastWriteTimeUtc -lt $started){throw 'Park sports report missing/stale'}
$report=Get-Content $path -Raw|ConvertFrom-Json
if($report.failed.Count -or $report.passed.Count -lt 35){throw 'Park sports assertions failed'}
if($Render){foreach($name in @('ParkSoccer.png','ParkBaseball.png','ParkBasketball.png','ParkTerrain.png','ParkHillFollow.png')){if(!(Test-Path (Join-Path $OutputRoot $name)) -or (Get-Item (Join-Path $OutputRoot $name)).LastWriteTimeUtc -lt $started){throw ('Missing/stale capture '+$name)}}}
'Park sports: '+$report.passed.Count+' assertions passed'
