param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$CacheRoot='D:\CheweenieValidation\UE5Cache',[switch]$Render,[string]$GameExe,[string]$OutputRoot,[string]$MapFile)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved\Ratchet'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
if(Test-Path -LiteralPath (Join-Path $OutputRoot 'ratchet-memory.json')){Remove-Item -LiteralPath (Join-Path $OutputRoot 'ratchet-memory.json')}
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and !$MapFile -and $phase -eq 'resume'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieRatchetResume'}else{''}
    $info.Arguments=$project+$rhi+' -CheweenieRatchetSmoke'+$resume+' -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/Ratchet-'+$phase+'.log"'
    if($MapFile){$info.Arguments+=' -CheweenieNeighbourhood="'+[IO.Path]::GetFullPath($MapFile)+'"'};$info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(-not $process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 10){$process.Kill();throw 'Ratchet test timed out'}}
    if($process.ExitCode){throw ('Ratchet process failed: '+$phase)}
    $report=Join-Path $OutputRoot $(if($phase -eq 'resume'){'ratchet-return.json'}else{'ratchet-unlock.json'})
    if(!(Test-Path $report) -or (Get-Item $report).LastWriteTimeUtc -lt $started){throw 'Ratchet report missing/stale'}
    $result=Get-Content $report -Raw|ConvertFrom-Json;$minimum=if($phase -eq 'resume'){3}else{6}
    if($result.failed.Count -or $result.passed.Count -lt $minimum){throw ('Ratchet checks failed: '+$phase)}
    if($Render -and !$MapFile -and $phase -eq 'resume'){foreach($name in @('RatchetMemory.png')){if(!(Test-Path (Join-Path $OutputRoot $name))){throw ('Missing capture '+$name)}}}
    $phase+': '+$result.passed.Count+' passes'
}
