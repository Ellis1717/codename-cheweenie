param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Hockey'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$rhi=if($Render){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '+$rhi+' -UserDir="'+$OutputRoot+'/User" -CheweenieHockeySmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -AudioMixer -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Hockey.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Hockey test timed out'}}
if($process.ExitCode){throw 'Hockey test failed; inspect Hockey.log'}
$path=Join-Path $OutputRoot 'hockey-smoke.json'
if(!(Test-Path $path) -or (Get-Item $path).LastWriteTimeUtc -lt $started){throw 'Hockey report missing/stale'}
$report=Get-Content $path -Raw|ConvertFrom-Json
if($report.failed.Count -or $report.passed.Count -lt 17){throw 'Hockey assertions failed'}
if($Render){foreach($name in @('HockeyTruck.png','HockeyDressingRoom.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
'Hockey: '+$report.passed.Count+' assertions passed'
