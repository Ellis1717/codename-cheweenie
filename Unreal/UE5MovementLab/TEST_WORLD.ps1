param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[switch]$ReadableBrick)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/World'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" '}
$rhi=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1920 -ResY=1080'}else{'-nullrhi'}
$info.Arguments=$project+' -game '+$rhi+' -CheweenieWorldSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosplash -nosound -AbsLog="'+$OutputRoot+'/World.log"'
if($ReadableBrick){$info.Arguments+=' -CheweenieReadableBrickSmoke'}
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'World test timed out'}}
if($process.ExitCode){throw 'World runtime failed; inspect report and World.log'}
$report=Join-Path $OutputRoot 'world-smoke.json'
if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $started){throw 'World report missing/stale'}
$result=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json
if($result.failed.Count -or $result.passed.Count -lt 14){throw 'World acceptance failed/incomplete'}
if($ReadableBrick -and $result.passed.Count -lt 16){throw 'Readable brick checks missing'}
if($ReadableBrick -and $Render){$capture=Join-Path $OutputRoot 'WorldBrickDogHeight.png';if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $started){throw 'Brick inspection capture missing/stale'}}
if($Render){foreach($name in @('WorldRoom.png','WorldDogView.png','WorldHome.png','WorldPark.png')){if(!(Test-Path -LiteralPath (Join-Path $OutputRoot $name)) -or (Get-Item -LiteralPath (Join-Path $OutputRoot $name)).LastWriteTimeUtc -lt $started){throw 'World capture missing/stale'}}}
$result
