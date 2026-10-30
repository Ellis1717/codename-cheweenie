param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Tricks'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and $phase -eq 'play'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieTricksResume'}else{''}
    $info.Arguments=$project+$rhi+' -CheweenieTricksSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Tricks-'+$phase+'.log"'
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Pet life test timed out'}}
    if($process.ExitCode){throw ('Pet life process failed: '+$phase)}
    $report=Join-Path $OutputRoot $(if($phase -eq 'resume'){'tricks-resume.json'}else{'tricks-smoke.json'})
    if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Pet life report missing/stale'}
    $result=Get-Content $report -Raw|ConvertFrom-Json;$minimum=if($phase -eq 'resume'){5}else{29}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Pet life checks failed: '+$phase)}
    if($Render -and $phase -eq 'play'){foreach($name in @('MartyLying.png','MartyRoll.png','MartyShake.png','MartyBackpack.png','MartyBodySway.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
    $phase+': '+$result.passed.Count+' passes'
}
