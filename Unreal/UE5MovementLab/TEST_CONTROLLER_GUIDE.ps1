param([string]$EngineRoot='D:/Epic Games/UE_5.8',[switch]$Render,[string]$CacheRoot='D:/CheweenieValidation/UE5Cache')
$ErrorActionPreference='Stop'
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
$backend=if($Render){'-dx12 -RenderOffscreen -windowed -ForceRes -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '+$backend+' -CheweenieControllerGuideSmoke -unattended -nosplash -nosound -AbsLog="'+(Join-Path $PSScriptRoot 'Saved/ControllerGuide.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 8){throw 'Controller guide acceptance timed out.'}}}
finally{if(!$process.HasExited){$process.Kill()}}
$path=Join-Path $PSScriptRoot 'Saved/controller-guide-smoke.json'
if(!(Test-Path -LiteralPath $path)-or(Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Controller guide report missing or stale.'}
$result=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
if($process.ExitCode -or $result.failed.Count -or $result.passed.Count-lt 14){throw 'Controller guide checks failed; inspect Saved/ControllerGuide.log.'}
if($Render){foreach($name in @('Xbox','PlayStation','Keyboard')){
    $capture=Join-Path $PSScriptRoot ('Saved/ControllerGuide'+$name+'.png')
    if(!(Test-Path -LiteralPath $capture)-or(Get-Item -LiteralPath $capture).LastWriteTimeUtc-lt $started){throw ('Missing or stale capture: '+$name)}
}}
$result
