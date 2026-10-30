param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[int]$Width=1280,[int]$Height=720)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Gait'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
$rhi=if($Render){"-dx12 -RenderOffscreen -ForceRes -windowed -ResX=$Width -ResY=$Height"}else{'-nullrhi'}
$info.Arguments=$project+$rhi+' -CheweenieGaitSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Gait.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Gait test timed out'}}
if($process.ExitCode){throw ('Gait test failed: '+$process.ExitCode)}
$report=Join-Path $OutputRoot 'marty-gait.json'
if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Gait report missing/stale'}
$result=Get-Content $report -Raw|ConvertFrom-Json
if($result.failed.Count -or $result.passed.Count -lt 16){throw 'Gait assertions failed; inspect Gait.log'}
if($Render){foreach($name in @('MartyPortrait.png','MartyWalk.png','MartyTrot.png','MartyIdle.png')){
    $capture=Join-Path $OutputRoot $name
    if(!(Test-Path $capture)){throw ('Missing capture '+$name)}
    if((Get-Item $capture).LastWriteTimeUtc -lt $started){throw ('Stale capture '+$name)}
}}
'Gait: '+$result.passed.Count+' passes'
