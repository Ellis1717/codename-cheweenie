param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render,[string]$CacheRoot='D:/CheweenieValidation/UE5Cache')
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/Overlays'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
foreach($phase in @('main','resume')){
    $root=Join-Path $OutputRoot $phase;New-Item -ItemType Directory -Path $root -Force|Out-Null
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){$GameExe}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $info.Arguments=$project+$rhi+' -CheweenieOverlaySmoke -CheweenieTestOutput="'+$root+'" -unattended -nosound -nosplash -AbsLog="'+$root+'/Overlays.log"'
    if($phase-eq 'resume'){$info.Arguments+=' -CheweenieOverlayResume'}
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true
    $info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
    $started=[DateTime]::UtcNow;$process=$( $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'; [Diagnostics.Process]::Start($info) )
    try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 8){throw 'Overlay acceptance timed out.'}}}
    finally{if(!$process.HasExited){$process.Kill()}}
    $path=Join-Path $root 'overlay-smoke.json'
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Overlay report missing or stale.'}
    $report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
    if($process.ExitCode -or $report.failed.Count -or $report.passed.Count-lt $(if($phase-eq 'main'){79}else{3})){throw ('Overlay acceptance failed: '+$phase)}
    if($Render -and $phase-eq 'main'){foreach($file in @('OverlaysDefault.png','OverlaysHidden.png','OverlaySettings.png')){
        $capture=Join-Path $root $file
        if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc-lt $started){throw ('Missing or stale overlay capture: '+$file)}
    }}
    $phase+': '+$report.passed.Count+' passes'
}
