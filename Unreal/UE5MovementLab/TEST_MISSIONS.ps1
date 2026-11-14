param([string]$EngineRoot='D:\Epic Games\UE_5.8',[string]$OutputRoot,[switch]$Render)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/OpenMissions'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
    $rhi=if($Render -and $phase -eq 'play'){'-dx12 -RenderOffscreen -ForceRes -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase -eq 'resume'){' -CheweenieMissionResume'}else{''}
    $info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '+$rhi+' -CheweenieMissionSmoke'+$resume+' -UserDir="'+$OutputRoot+'/User" -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -NoVSync -AbsLog="'+$OutputRoot+'/'+$phase+'.log"'
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache';$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes -gt 12){$process.Kill();throw 'Mission test timed out'}}
    $report=Join-Path $OutputRoot ('missions-'+$phase+'.json')
    if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $started){throw 'Mission report missing/stale'}
    $r=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json;$minimum=if($phase -eq 'play'){30}else{7}
    if($process.ExitCode -or $r.failed.Count -or $r.passed.Count -lt $minimum){throw ('Mission checks failed: '+$phase)}
    $phase+': '+$r.passed.Count+' passes'
}
