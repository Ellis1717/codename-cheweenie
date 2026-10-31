param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Navigation'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$phases=if($MapFile){@('private')}else{@('play','resume')}
foreach($phase in $phases){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and $phase -eq 'play'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieNavigationResume'}elseif($MapFile){' -CheweenieNavigationPrivateAudit -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'}else{''}
    $info.Arguments=$project+$rhi+' -CheweenieNavigationSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Navigation-'+$phase+'.log"'
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Navigation test timed out'}}
    if($process.ExitCode){throw ('Navigation process failed: '+$phase)}
    $report=Join-Path $OutputRoot $(if($MapFile){'navigation-private.json'}elseif($phase -eq 'resume'){'navigation-resume.json'}else{'navigation-smoke.json'})
    if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Navigation report missing/stale'}
    $result=Get-Content $report -Raw|ConvertFrom-Json;$minimum=if($MapFile){6}elseif($phase -eq 'resume'){4}else{59}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Navigation checks failed: '+$phase)}
    if($Render -and $phase -eq 'play'){foreach($name in @('NavigationIndoor.png','NavigationSwitches.png','NavigationOutside.png','NavigationArea.png','NavigationPetSwitches.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
    $phase+': '+$result.passed.Count+' passes'
}
