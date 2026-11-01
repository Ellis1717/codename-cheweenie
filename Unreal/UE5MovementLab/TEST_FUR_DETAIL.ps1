param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[int]$Width=1280,[int]$Height=720)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\FurDetail'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('play','medium','low')){
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
$rhi=if($Render -and $phase -eq 'play'){"-dx12 -RenderOffscreen -ForceRes -windowed -ResX=$Width -ResY=$Height"}else{'-nullrhi'}
$flag=if($phase-eq 'medium'){' -CheweenieFurDetailMedium'}elseif($phase-eq 'low'){' -CheweenieFurDetailLow'}else{''}
$info.Arguments=$project+$rhi+' -CheweenieFurDetailSmoke'+$flag+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/FurDetail-'+$phase+'.log"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){$process.Kill();throw 'Fur detail test timed out'}}
if($process.ExitCode){throw ('Fur detail test failed: '+$process.ExitCode)}
$name=if($phase-eq 'play'){'fur-detail.json'}else{'fur-detail-'+$phase+'.json'}
$report=Join-Path $OutputRoot $name
if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc-lt $started){throw 'Fur detail report missing or stale'}
$result=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json
$minimum=if($phase-eq 'play'){15}else{4}
if($result.failed.Count -or $result.passed.Count-lt $minimum){throw ('Fur detail assertions failed: '+$name)}
if($Render -and $phase-eq 'play'){$capture=Join-Path $OutputRoot 'FurDetailMenu.png';if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc-lt $started){throw 'Fur detail menu capture missing or stale'}}
$phase+': '+$result.passed.Count+' passes'
}