param([string]$EngineRoot='D:/Epic Games/UE_5.8',[string]$GameExe,[string]$OutputRoot,[switch]$Render)
$ErrorActionPreference='Stop'
if(!$OutputRoot){$OutputRoot=Join-Path $PSScriptRoot 'Saved/ControllerButtons'}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
$info=[Diagnostics.ProcessStartInfo]::new()
$info.FileName=if($GameExe){[IO.Path]::GetFullPath($GameExe)}else{Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'}
$project=if($GameExe){''}else{'"'+(Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject')+'" -game -Multiprocess '}
$backend=if($Render){'-dx12 -RenderOffscreen -windowed -ResX=1280 -ResY=720'}else{'-nullrhi'}
$info.Arguments=$project+$backend+' -CheweenieControllerButtonsSmoke -CheweenieTestOutput="'+$OutputRoot+'" -UserDir="'+(Join-Path $OutputRoot 'User')+'" -unattended -nosound -nosplash -AbsLog="'+(Join-Path $OutputRoot 'ControllerButtons.log')+'"'
$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables['UE_SKIP_UBT_SDK_SETUP']='1'
$info.EnvironmentVariables['UE-LocalDataCachePath']='D:/CheweenieValidation/UE5Cache'
$started=[DateTime]::UtcNow;$process=[Diagnostics.Process]::Start($info)
try{while(!$process.WaitForExit(1000)){if(([DateTime]::UtcNow-$started).TotalMinutes-gt 10){throw 'Controller button audit timed out.'}}}
finally{if(!$process.HasExited){$process.Kill()}}
$path=Join-Path $OutputRoot 'controller-buttons.json'
if(!(Test-Path -LiteralPath $path)-or(Get-Item -LiteralPath $path).LastWriteTimeUtc-lt $started){throw 'Controller button report missing or stale.'}
$report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
if($process.ExitCode -or $report.failed.Count -or $report.passed.Count-lt 50){throw ('Controller button audit failed: '+($report.failed -join '; '))}
if($Render){$capture=Join-Path $OutputRoot 'ControllerButtonsGameplay.png';if(!(Test-Path -LiteralPath $capture)-or(Get-Item -LiteralPath $capture).LastWriteTimeUtc-lt $started){throw 'Controller audit gameplay capture missing or stale.'}}
$report
