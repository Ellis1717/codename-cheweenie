param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Jackets'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and !$MapFile -and $phase -eq 'play'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieJacketResume'}else{''}
    $info.Arguments=$project+$rhi+' -CheweenieJacketSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Jacket-'+$phase+'.log"'
    if($MapFile){$info.Arguments+=' -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'};$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    # Official engine runtime option: the native build has already validated this PC's compiler/SDK.
    $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Jacket test timed out'}}
    if($process.ExitCode){throw ('Jacket process failed: '+$phase)}
    $report=Join-Path $OutputRoot $(if($phase -eq 'resume'){'jackets-resume.json'}else{'jackets-smoke.json'})
    if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Jacket report missing/stale'}
    $result=Get-Content $report -Raw|ConvertFrom-Json;$minimum=if($phase -eq 'resume'){3}else{27}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Jacket checks failed: '+$phase)}
    if($Render -and !$MapFile -and $phase -eq 'play'){foreach($name in @('JacketYellow.png','JacketFleece.png','JacketNavy.png','JacketPink.png','JacketPinkLow.png','JacketLying.png','JacketRolling.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
    $phase+': '+$result.passed.Count+' passes'
}
