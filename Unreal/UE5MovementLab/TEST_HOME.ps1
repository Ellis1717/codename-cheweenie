param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[string]$LegacySave,[switch]$Render)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/HomeProgression'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
foreach($phase in @('play','resume')){
    $info=[Diagnostics.ProcessStartInfo]::new();$info.FileName=if($GameExe){$GameExe}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
    $project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '}
    $rhi=if($Render -and $phase-eq 'play'){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
    $info.Arguments=$project+$rhi+' -CheweenieHomeSmoke -CheweenieTestOutput="'+$OutputRoot+'" -unattended -nosound -nosplash -AbsLog="'+$OutputRoot+'/'+$phase+'.log"'
    if($phase-eq 'resume'){$info.Arguments+=' -CheweenieHomeResume'}elseif($LegacySave){$info.Arguments+=' -CheweenieHomeLegacySave="'+[IO.Path]::GetFullPath($LegacySave)+'"'}
    $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache'
    $started=[DateTime]::UtcNow;$process=$( $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'; [Diagnostics.Process]::Start($info) )
    while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){$process.Kill();throw 'Home acceptance timed out.'}}
    $path=Join-Path $OutputRoot $(if($phase-eq 'resume'){'home-resume.json'}else{'home-progression.json'})
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Home report missing or stale.'}
    $r=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
    if($process.ExitCode -or $r.failed.Count -or $r.passed.Count-lt $(if($phase-eq 'resume'){5}else{34})){throw ('Home acceptance failed: '+$phase)}
    if($Render -and $phase-eq 'play'){foreach($file in @('HomeCatReaction.png','HomeBedRest.png','HomeArmorWorkshop.png','HomeGym.png')){if(!(Test-Path -LiteralPath (Join-Path $OutputRoot $file)) -or (Get-Item -LiteralPath (Join-Path $OutputRoot $file)).LastWriteTimeUtc-lt $started){throw ('Home capture missing/stale: '+$file)}}}
    $phase+': '+$r.passed.Count+' passes'
}
