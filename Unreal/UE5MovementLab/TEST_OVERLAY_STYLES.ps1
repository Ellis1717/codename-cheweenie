param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render,[switch]$ResumeOnly,[string]$CacheRoot='D:/CheweenieValidation/UE5Cache')
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/OverlayStyles'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
$phases=if($ResumeOnly){@('resume')}else{@('main','resume')}
foreach($phase in $phases){
    $root=Join-Path $OutputRoot $phase;New-Item -ItemType Directory -Path $root -Force|Out-Null
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){$GameExe}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and $phase-eq 'main'){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $info.Arguments=$project+$rhi+' -CheweenieOverlaySmoke -CheweenieOverlayStyleSmoke -CheweenieTestOutput="'+$root+'" -unattended -nosound -nosplash -AbsLog="'+$root+'/Overlays.log"'
    if($phase-eq 'resume'){$info.Arguments+=' -CheweenieOverlayStyleResume'}
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true
    $info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
    try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 8){throw 'Overlay style acceptance timed out.'}}}
    finally{if(!$process.HasExited){$process.Kill()}}
    $path=Join-Path $root 'overlay-styles.json'
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Style report missing or stale.'}
    $report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
    if($process.ExitCode -or $report.failed.Count -or $report.passed.Count-lt $(if($phase-eq 'main'){11}else{4})){throw ('Overlay style acceptance failed: '+$phase)}
    if($Render -and $phase-eq 'main'){foreach($file in @('Style0Game.png','Style0Menu.png','Style1Game.png','Style1Menu.png','Style2Game.png','Style2Menu.png','Style3Game.png','Style3Menu.png')){
        $capture=Join-Path $root $file
        if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc-lt $started){throw ('Missing or stale style capture: '+$file)}
    }}
    $phase+': '+$report.passed.Count+' passes'
}
