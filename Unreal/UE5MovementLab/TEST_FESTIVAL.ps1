param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Festival'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and !$MapFile -and $phase -eq 'play'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieFestivalResume -CheweenieHalloweenSpecial'}else{''}
    $info.Arguments=$project+$rhi+' -CheweenieFestivalSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Festival-'+$phase+'.log"'
    if($MapFile){$info.Arguments+=' -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'};$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Festival test timed out'}}
    if($process.ExitCode){throw ('Festival process failed: '+$phase)}
    $report=Join-Path $OutputRoot $(if($phase -eq 'resume'){'festival-resume.json'}else{'festival-smoke.json'})
    if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Festival report missing/stale'}
    $result=Get-Content $report -Raw|ConvertFrom-Json;$minimum=if($phase -eq 'resume'){6}else{40}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Festival checks failed: '+$phase)}
    if($Render -and !$MapFile -and $phase -eq 'play'){foreach($name in @('FestivalTitle.png','FestivalPorch.png','FestivalFinale.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
    $bad=@(Select-String -LiteralPath (Join-Path $OutputRoot ('Festival-'+$phase+'.log')) -Pattern 'Log\w+: (Error|Fatal):|Failed to compile Material|Default Material will be used')
    if($bad.Count){throw ('Festival runtime error or material fallback: '+$phase)}
    $phase+': '+$result.passed.Count+' passes'
}
