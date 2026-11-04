param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/ControllerComfort'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess -MultiprocessSaveConfig '}
    $backend=if($Render-and $phase-eq 'play'){'-dx12 -RenderOffscreen -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $resume=if($phase-eq 'resume'){' -CheweenieControllerComfortResume'}else{''}
    $info.Arguments=$project+$backend+' -CheweenieControllerComfortSmoke'+$resume+' -ExecCmds="t.MaxFPS 60" -CheweenieTestOutput="'+$OutputRoot+'" -UserDir="'+(Join-Path $OutputRoot 'User')+'" -unattended -nosound -nosplash -AbsLog="'+(Join-Path $OutputRoot ($phase+'.log'))+'"'
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1';$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache'
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){throw 'Controller comfort audit timed out.'}}}finally{if(!$process.HasExited){$process.Kill()}}
    $name=if($phase-eq 'resume'){'controller-comfort-resume.json'}else{'controller-comfort.json'};$path=Join-Path $OutputRoot $name
    if(!(Test-Path -LiteralPath $path)-or(Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Controller comfort report missing/stale.'}
    $r=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json;$minimum=if($phase-eq 'resume'){9}else{45}
    if($process.ExitCode-or $r.failed.Count-or $r.passed.Count-lt $minimum){throw ('Controller comfort failed: '+($r.failed-join '; '))}
    $phase+': '+$r.passed.Count+' comfort checks passed'
}
if($Render){$capture=Join-Path $OutputRoot 'ControllerComfortSettings.png';if(!(Test-Path -LiteralPath $capture)){throw 'Controller comfort capture missing.'}}
