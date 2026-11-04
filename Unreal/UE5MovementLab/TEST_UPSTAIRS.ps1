param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$OutputRoot,[string]$GameExe)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Upstairs'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
$rhi=if($Render){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
$info.Arguments=$project+$rhi+' -CheweenieUpstairsSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Upstairs.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=$( $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'; [Diagnostics.Process]::Start($info) )
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Upstairs test timed out'}}
if($process.ExitCode){throw 'Upstairs test failed; inspect Upstairs.log'}
$path=Join-Path $OutputRoot 'upstairs-smoke.json'
if(!(Test-Path $path) -or (Get-Item $path).LastWriteTimeUtc -lt $started){throw 'Upstairs report missing/stale'}
$report=Get-Content $path -Raw|ConvertFrom-Json
if($report.failed.Count -or $report.passed.Count -lt 18){throw 'Upstairs assertions failed'}
if($Render){foreach($name in @('UpstairsOverview.png','UpstairsBedroom.png','UpstairsStairs.png','UpstairsGameplay.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
'Upstairs: '+$report.passed.Count+' assertions passed'
