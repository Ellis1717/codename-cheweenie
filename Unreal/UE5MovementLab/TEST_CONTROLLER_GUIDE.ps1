param([string]$EngineRoot='D:/Epic Games/UE_5.8',[switch]$Render,[string]$CacheRoot='D:/CheweenieValidation/UE5Cache',[string]$GameExe,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$backend=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '}
$info.Arguments=$project+$backend+' -CheweenieTestOutput="'+$OutputRoot+'" -CheweenieControllerGuideSmoke -unattended -nosplash -nosound -AbsLog="'+(Join-Path $OutputRoot 'ControllerGuide.log')+'"'
# Keep synthetic checks independent of physical devices and player preferences.
$info.Arguments+=' -NoGamepad -UserDir="'+(Join-Path $OutputRoot 'User')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=$( $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'; [Diagnostics.Process]::Start($info) )
try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 12){throw 'Controller guide acceptance timed out.'}}}
finally{if(!$process.HasExited){$process.Kill()}}
$path=Join-Path $OutputRoot 'controller-guide-smoke.json'
if(!(Test-Path -LiteralPath $path)-or(Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Controller guide report missing or stale.'}
$result=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
if($process.ExitCode -or $result.failed.Count -or $result.passed.Count-lt 31){throw 'Controller guide checks failed; inspect Saved/ControllerGuide.log.'}
if($Render){foreach($name in @('Xbox','PlayStation','Keyboard','PauseXbox','PausePlayStation','PauseKeyboard','GymPlayStation','HelpXbox')){
    $capture=Join-Path $OutputRoot ('ControllerGuide'+$name+'.png')
    if(!(Test-Path -LiteralPath $capture)-or(Get-Item -LiteralPath $capture).LastWriteTimeUtc-lt $started){throw ('Missing or stale capture: '+$name)}
}}
$result
