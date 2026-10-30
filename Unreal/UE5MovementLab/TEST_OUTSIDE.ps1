param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Outside'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
$rhi=if($Render){'-dx12 -RenderOffscreen -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments=$project+$rhi+' -CheweenieOutsideSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Outside.log"'
if($MapFile){if($Render){throw 'Private map audit is headless; no private-layout screenshots are exported'};$info.Arguments+=' -CheweenieOutsideMapAudit -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'}
$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Outside test timed out'}}
if($process.ExitCode){throw ('Outside test failed: '+$process.ExitCode)}
$report=Join-Path $OutputRoot $(if($MapFile){'outside-map-audit.json'}else{'outside-smoke.json'})
if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Outside report missing/stale'}
$result=Get-Content $report -Raw|ConvertFrom-Json
$minimum=if($MapFile){4}else{12}
if($result.failed.Count -or $result.passed.Count -lt $minimum){throw 'Outside assertions failed; inspect Outside.log'}
if($Render){foreach($name in @('OutsideArrival.png','OutsidePark.png','OutsideSchool.png')){
    $capture=Join-Path $OutputRoot $name
    if(!(Test-Path $capture) -or (Get-Item $capture).LastWriteTimeUtc -lt $started){throw ('Missing/stale capture '+$name)}
}}
'Outside: '+$result.passed.Count+' passes'
