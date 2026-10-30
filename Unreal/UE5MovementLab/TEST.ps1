param([string]$EngineRoot='D:\Epic Games\UE_5.8', [string]$CacheRoot='D:\CheweenieValidation\UE5Cache', [switch]$Render)
$ErrorActionPreference='Stop'
if(-not(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Binaries\Win64\UnrealEditor-CheweenieUE5Lab.dll'))){throw 'Run BUILD.ps1 first.'}
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$backend=if($Render){'-dx12 -RenderOffscreen -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments='"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game '+$backend+' -CheweenieSmoke -unattended -nosplash -nosound -NoVSync -AbsLog="'+(Join-Path $PSScriptRoot 'Saved\MovementSmoke.log')+'"'
$info.UseShellExecute=$false
$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE-LocalDataCachePath']=$CacheRoot
$startedAt=[DateTime]::UtcNow
$process=[Diagnostics.Process]::Start($info)
while(-not $process.WaitForExit(1000)) {
    if(([DateTime]::UtcNow-$startedAt).TotalMinutes -gt 10){$process.Kill(); throw 'Native runtime smoke test timed out.'}
}
if($process.ExitCode -ne 0){throw ('Native runtime smoke test failed: '+$process.ExitCode+'. Inspect Saved/MovementSmoke.log.')}
$report=Join-Path $PSScriptRoot 'Saved\movement-smoke.json'
if(-not(Test-Path -LiteralPath $report)){throw 'Native runtime report missing.'}
if((Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $startedAt){throw 'Native runtime report is stale.'}
$result=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
if($result.failed.Count -ne 0 -or $result.passed.Count -lt 16){throw 'Native acceptance checks did not all pass.'}
if($Render) {
    $capture=Join-Path $PSScriptRoot 'Saved\MovementGameplay.png'
    if(-not(Test-Path -LiteralPath $capture)){throw 'Gameplay capture missing.'}
    if((Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $startedAt){throw 'Gameplay capture is stale.'}
}
$result
