param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Costume'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and !$MapFile -and $phase -eq 'play'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieCostumeResume'}else{''}
    $info.Arguments=$project+$rhi+' -CheweenieCostumeSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Costume-'+$phase+'.log"'
    if($MapFile){$info.Arguments+=' -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'};$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Costume test timed out'}}
    if($process.ExitCode){throw ('Costume process failed: '+$phase)}
    $report=Join-Path $OutputRoot $(if($phase -eq 'resume'){'costumes-resume.json'}else{'costumes-smoke.json'})
    if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Costume report missing/stale'}
    $result=Get-Content $report -Raw|ConvertFrom-Json;$minimum=if($phase -eq 'resume'){2}else{18}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Costume checks failed: '+$phase)}
    if($Render -and !$MapFile -and $phase -eq 'play'){foreach($name in @('HalloweenMenu.png','HalloweenMarty.png','HalloweenLakitu.png','HalloweenRio.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
    $phase+': '+$result.passed.Count+' passes'
}
