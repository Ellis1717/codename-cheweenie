param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache', [switch]$Render,[string]$GameExe,[string]$OutputRoot)
$ErrorActionPreference='Stop'
if(!$GameExe -and -not(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Binaries\Win64\UnrealEditor-CheweenieUE5Lab.dll'))){throw 'Run BUILD.ps1 first.'}
$runtimeSaved=if($GameExe){Join-Path (Split-Path -Parent ([IO.Path]::GetFullPath($GameExe))) 'CheweenieUE5Lab/Saved'}else{Join-Path $PSScriptRoot 'Saved'}
if(!$OutputRoot){$OutputRoot=$runtimeSaved}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot);New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'}
$backend=if($Render){'-dx12 -RenderOffscreen -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
# The native build validates SDKs; runtime acceptance must not queue another build-tool
# SDK query behind a different checkout's compilation (TargetPlatformManager's supported flag).
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '}
$info.Arguments=$project+$backend+' -CheweenieSmoke -unattended -nosplash -nosound -NoVSync -AbsLog="'+(Join-Path $OutputRoot 'MovementSmoke.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$startedAt=[DateTime]::UtcNow
$process=$( $info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'; [Diagnostics.Process]::Start($info) )
while(-not $process.WaitForExit(1000)) {
    if(([DateTime]::UtcNow-$startedAt).TotalMinutes -gt 10){$process.Kill(); throw 'Native runtime smoke test timed out.'}
}
if($process.ExitCode -ne 0){throw ('Native runtime smoke test failed: '+$process.ExitCode+'. Inspect Saved/MovementSmoke.log.')}
$report=Join-Path $runtimeSaved 'movement-smoke.json'
if(-not(Test-Path -LiteralPath $report)){throw 'Native runtime report missing.'}
if((Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $startedAt){throw 'Native runtime report is stale.'}
$result=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
if($result.failed.Count -ne 0 -or $result.passed.Count -lt 16){throw 'Native acceptance checks did not all pass.'}
if($Render) {
    $capture=Join-Path $runtimeSaved 'MovementGameplay.png'
    if(-not(Test-Path -LiteralPath $capture)){throw 'Gameplay capture missing.'}
    if((Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $startedAt){throw 'Gameplay capture is stale.'}
}
if($OutputRoot.TrimEnd('\','/') -ne $runtimeSaved.TrimEnd('\','/')){Copy-Item -LiteralPath $report -Destination (Join-Path $OutputRoot 'movement-smoke.json');if($Render){Copy-Item -LiteralPath $capture -Destination (Join-Path $OutputRoot 'MovementGameplay.png')}}
$result
